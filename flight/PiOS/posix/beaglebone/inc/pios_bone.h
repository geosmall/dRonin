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
 
#ifndef PIOS_BONE_H
#define PIOS_BONE_H

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include "pios.h"

typedef struct pios_bone_dev * pios_bone_dev_t;

int32_t PIOS_Bone_Init(pios_bone_dev_t *dev);

extern const struct pios_adc_driver  pios_bone_adc_driver;

extern const struct pios_rcvr_driver pios_bone_rcvr_driver;

#endif

#endif

