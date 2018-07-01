#ifndef PIOS_BONE_H
#define PIOS_BONE_H

#include "pios.h"

typedef struct pios_bone_dev * pios_bone_dev_t;

int32_t PIOS_Bone_Init(pios_bone_dev_t *dev);

extern const struct pios_adc_driver  pios_bone_adc_driver;

extern const struct pios_rcvr_driver pios_bone_rcvr_driver;

#endif
