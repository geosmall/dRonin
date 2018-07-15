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
 
#ifndef PIOSBONEPRUINPUT_H
#define PIOSBONEPRUINPUT_H

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#if defined(PIOS_INCLUDE_BLUE)
	#define RCIN_PRUSS_RAM_BASE         0x4a303000
#elif defined(PIOS_INCLUDE_POCKET)
	#define RCIN_PRUSS_RAM_BASE         0x4a301000
#else
	#error "No Bone Target Defined!!"
#endif

#define NUM_RING_ENTRIES            300
#define TICK_PER_US                 200

#define MIN_NUM_CHANNELS            5
#define LINUX_RC_INPUT_NUM_CHANNELS 16

// shared ring buffer with the PRU which records pin transitions
struct ring_buffer {
	volatile uint16_t ring_head;   // owned by ARM CPU
	volatile uint16_t ring_tail;   // owned by the PRU
	struct {
		volatile uint32_t s1_t;    // 5ns per tick
		volatile uint32_t s0_t;    // 5ns per tick
	} buffer[NUM_RING_ENTRIES];
};

volatile struct ring_buffer *ring_buffer;

uint16_t pwm_values[LINUX_RC_INPUT_NUM_CHANNELS];
uint8_t  num_channels;
uint8_t rc_input_count;

// state of ppm decoder
struct {
	int8_t   channel_counter;
	uint16_t pulse_capt[LINUX_RC_INPUT_NUM_CHANNELS];
} ppm_state;

// state of SBUS bit decoder
struct {
	uint16_t bytes[25]; // including start bit, parity and stop bits
	uint16_t bit_ofs;
} sbus_state;

#endif

#endif

