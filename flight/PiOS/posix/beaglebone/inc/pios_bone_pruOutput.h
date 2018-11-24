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
 
#ifndef PIOSBONEPRUOUTPUT
#define PIOSBONEPRUOUTPUT

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include <stdint.h>
#include <stdbool.h>

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BLUE)
	#define RCOUT_PRUSS_RAM_BASE  0x4A302000
	#define RCOUT_PRUSS_CTRL_BASE 0x4A324000
	#define RCOUT_PRUSS_IRAM_BASE 0x4A338000

	#define CHANNEL_COUNT_BONE_PWM_OUTPUT  8
	#define BANK_COUNT_BONE_PWM_OUTPUT     6
#elif defined(PIOS_INCLUDE_POCKET)
	#define RCOUT_PRUSS_RAM_BASE  0x4A300000
	#define RCOUT_PRUSS_CTRL_BASE 0x4A322000
	#define RCOUT_PRUSS_IRAM_BASE 0x4A334000

	#define CHANNEL_COUNT_BONE_PWM_OUTPUT  6
	#define BANK_COUNT_BONE_PWM_OUTPUT     4
#else
	#error "No Bone Target Defined!!"
#endif

void     pios_bone_rcOutput_init(void);
void     pios_bone_rcOutput_set_freq(uint32_t chmask, uint16_t freq_hz);
uint16_t pios_bone_rcOutput_get_freq(uint8_t ch);
void     pios_bone_rcOutput_enable_ch(uint8_t ch);
void     pios_bone_rcOutput_disable_ch(uint8_t ch);
void     pios_bone_rcOutput_write(uint8_t ch, uint16_t period_us);
uint16_t pios_bone_rcOutput_read_ch(uint8_t ch);
void     pios_bone_rcOutput_read_chs(uint16_t* period_us, uint8_t len);
void     pios_bone_rcOutput_cork(void);
void     pios_bone_rcOutput_push(void);

static const uint32_t tick_per_us = 200;
static const uint32_t tick_per_s  = 200000000;

struct pwm {
	volatile uint32_t channel_enable;
	struct {
		volatile uint32_t time_high;
		volatile uint32_t time_t;
	} channel[CHANNEL_COUNT_BONE_PWM_OUTPUT];
};

volatile struct pwm *pwm;
uint16_t pending[CHANNEL_COUNT_BONE_PWM_OUTPUT];
uint32_t pending_mask;
bool corked;

#endif

#endif

