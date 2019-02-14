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
 
#ifndef PIOS_SBUS_PRIV_H
#define PIOS_SBUS_PRIV_H

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include <pios.h>

#include <pios_stm32.h>
#include <pios_usart_priv.h>

/*
 * S.Bus serial port settings:
 *  100000bps inverted serial stream, 8 bits, even parity, 2 stop bits
 *  frame period is 7ms (HS) or 14ms (FS)
 *
 * Frame structure:
 *  1 byte  - 0x0f (start of frame byte)
 * 22 bytes - channel data (11 bit/channel, 16 channels, LSB first)
 *  1 byte  - bit flags:
 *                   0x01 - discrete channel 1,
 *                   0x02 - discrete channel 2,
 *                   0x04 - lost frame flag,
 *                   0x08 - failsafe flag,
 *                   0xf0 - reserved
 *
 * The R7008SB receiver has four different end of frame bytes, which rotates in order:
 *    00000100
 *    00010100
 *    00100100
 *    00110100 *  1 byte  - 0x00 (end of frame byte)
 */

#define SBUS_FRAME_LENGTH		(1+22+1+1)
#define SBUS_SOF_BYTE			0x0f
#define SBUS_EOF_BYTE			0x00
#define SBUS_FLAG_DC1			0x01
#define SBUS_FLAG_DC2			0x02
#define SBUS_FLAG_FL			0x04
#define SBUS_FLAG_FS			0x08

#define SBUS_R7008SB_EOF_COUNTER_MASK 0xCF
#define SBUS_R7008SB_EOF_BYTE         0x04

/* Discrete channels represented as bits, provide values for them */
#define	SBUS_VALUE_MIN			352
#define	SBUS_VALUE_MAX			1696

/*
 * S.Bus configuration programmable invertor
 */
struct pios_sbus_cfg {
	struct stm32_gpio inv;
	void (*gpio_clk_func)(uint32_t periph, FunctionalState state);
	uint32_t gpio_clk_periph;
	BitAction gpio_inv_enable;
	BitAction gpio_inv_disable;
};

/*
 * S.Bus protocol provides 16 proportional and 2 discrete channels.
 * Do not change unless driver code is updated accordingly.
 */

#ifdef PIOS_INCLUDE_SBUS
#if (PIOS_SBUS_NUM_INPUTS != (16+2))
#error "S.Bus protocol provides 16 proportional and 2 discrete channels"
#endif
#endif

extern const struct pios_rcvr_driver pios_sbus_rcvr_driver;

extern int32_t PIOS_SBus_Init(uintptr_t *sbus_id,
			      const struct pios_com_driver *driver,
			      uintptr_t lower_id);


#endif

#endif