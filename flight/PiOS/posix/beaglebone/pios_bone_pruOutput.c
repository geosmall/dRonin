/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 */
 
#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/mman.h>
#endif

#include "pios_bone_pruOutput.h"

#if defined(PIOS_INCLUDE_BLUE)
	#include "RcAioPRU_BBBLUE_bin.h"
#elif defined(PIOS_INCLUDE_POCKET)
	#include "RcAioPRU_POCKET_bin.h"
#else
	#error "No Bone Target Defined!!"
#endif

bool corked = false;

#ifdef __linux
static void catch_sigbus(int sig)
{
    printf("pios_bone_pruOutput: SIGBUS error generated....\n");
}
#endif

void pios_bone_rcOutput_init(void)
{
#ifdef __linux__
   uint32_t mem_fd;
   uint32_t *iram;
   uint32_t *ctrl;

   signal(SIGBUS, catch_sigbus);

   mem_fd = open("/dev/mem", O_RDWR|O_SYNC|O_CLOEXEC);

   pwm  = (struct pwm*) mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, RCOUT_PRUSS_RAM_BASE);
   iram = (uint32_t*)   mmap(0, 0x2000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, RCOUT_PRUSS_IRAM_BASE);
   ctrl = (uint32_t*)   mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, RCOUT_PRUSS_CTRL_BASE);

   close(mem_fd);

   // Reset PRU 1
   *ctrl = 0;

   // Load firmware
   memcpy(iram, PRUcode, sizeof(PRUcode));

   // Start PRU 1
   *ctrl |= 2;

   // all outputs default to 50Hz, the top level vehicle code
   // overrides this when necessary
   pios_bone_rcOutput_set_freq(0xFFFFFFFF, 50);
#endif
}

void pios_bone_rcOutput_set_freq(uint32_t chmask, uint16_t freq_hz)
{
   uint8_t i;
   uint32_t tick = tick_per_s / freq_hz;

   for(i = 0; i < CHANNEL_COUNT_BONE_PWM_OUTPUT; i++) {
      if(chmask & (1U << i)) {
         pwm->channel[i].time_t = tick;
      }
   }
}

uint16_t pios_bone_rcOutput_get_freq(uint8_t ch)
{
   uint16_t ret = 0;

   if(ch < CHANNEL_COUNT_BONE_PWM_OUTPUT) {
      ret = tick_per_s / pwm->channel[ch].time_t;
   }

   return ret;
}

void pios_bone_rcOutput_enable_ch(uint8_t ch)
{
   if(ch < CHANNEL_COUNT_BONE_PWM_OUTPUT) {
      pwm->channel_enable |= 1U << ch;
   }
}

void pios_bone_rcOutput_disable_ch(uint8_t ch)
{
   if(ch < CHANNEL_COUNT_BONE_PWM_OUTPUT) {
      pwm->channel_enable &= !(1U << ch);
   }
}

void pios_bone_rcOutput_write(uint8_t ch, uint16_t period_us)
{
   if(ch < CHANNEL_COUNT_BONE_PWM_OUTPUT) {
       if (corked) {
           pending_mask |= (1U << ch);
           pending[ch] = period_us;
       } else {
           pwm->channel[ch].time_high = tick_per_us * period_us;
       }
   }
}

uint16_t pios_bone_rcOutput_read_ch(uint8_t ch)
{
   uint16_t ret = 0;

   if(ch < CHANNEL_COUNT_BONE_PWM_OUTPUT) {
      ret = (pwm->channel[ch].time_high / tick_per_us);
   }

   return ret;
}

void pios_bone_rcOutput_read_chs(uint16_t* period_us, uint8_t len)
{
   uint8_t i;

   if(len > CHANNEL_COUNT_BONE_PWM_OUTPUT) {
      len = CHANNEL_COUNT_BONE_PWM_OUTPUT;
   }

   for(i = 0; i < len; i++) {
      period_us[i] = pwm->channel[i].time_high / tick_per_us;
   }
}

void pios_bone_rcOutput_cork(void)
{
    corked = true;
}

void pios_bone_rcOutput_push(void)
{
    if (!corked) {
        return;
    }

	corked = false;

	for (uint8_t i=0; i<CHANNEL_COUNT_BONE_PWM_OUTPUT; i++) {
        if (pending_mask & (1U<<i)) {
            pios_bone_rcOutput_write(i, pending[i]);
        }
    }

	pending_mask = 0;
}

#endif

