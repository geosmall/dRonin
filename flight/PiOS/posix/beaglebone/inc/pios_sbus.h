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
 
#ifndef PIOSSBUS
#define PIOSSBUS

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include <stdbool.h>

bool sbus_decode(const uint8_t frame[25], uint16_t *values, uint16_t *num_values, 
                 bool *sbus_failsafe, bool *sbus_frame_drop, uint16_t max_values);

#endif

#endif

