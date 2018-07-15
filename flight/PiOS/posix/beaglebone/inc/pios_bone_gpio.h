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
 
#ifndef PIOSBONEGPIO_H
#define PIOSBONEGPIO_H

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BONE)

#include <stdint.h>
#include <stdbool.h>

#include "pios_config.h"

#if defined(PIOS_INCLUDE_BLUE)
// BeagleBone Blue GPIO mappings
#define BLUE_PAUSE_BTN          69  // GPIO 2.5
#define BLUE_MODE_BTN           68  // GPIO 2.4

#define BLUE_IMU_INTERRUPT_PIN 117  // GPIO 3.21

#define BLUE_RED_LED            66  // GPIO 2.2
#define BLUE_GRN_LED            67  // GPIO 2.3
#define BLUE_SERVO_PWR          80  // GPIO 2.16

#define BLUE_GP1_PIN_4          97  // GPIO 3.1
#define BLUE_GP1_PIN_3          98  // GPIO 3.2

#define BLUE_GP0_PIN_6         113  // GPIO 3.17
#define BLUE_GP0_PIN_5         116  // GPIO 3.20
#define BLUE_GP0_PIN_4          49  // GPIO 1.17
#define BLUE_GP0_PIN_3          57  // GPIO 1.25

#elif defined(PIOS_INCLUDE_POCKET)
// Pocket Beagle GPIO mappings
#define POCKET_RED_LED            59
#define POCKET_BLUE_LED           58
#define POCKET_AMBER_LED          57

#define POCKET_BUZZER             50

#else
#error "No Bone Target Defined!!"
#endif

#define BONE_GPIO_LOW  0
#define BONE_GPIO_HIGH 1

#define BONE_GPIO_INPUT  0
#define BONE_GPIO_OUTPUT 1

/////////////////////////////////////////////////////////

#define SYSFS_GPIO_DIR "/sys/class/gpio"

#define GPIO0_BASE 0x44E07000
#define GPIO1_BASE 0x4804C000
#define GPIO2_BASE 0x481AC000
#define GPIO3_BASE 0x481AE000

#define GPIO_SIZE  0x00000FFF

// OE: 0 is output, 1 is input

#define GPIO_OE  0x14D
#define GPIO_IN  0x14E
#define GPIO_OUT 0x14F

#define LINUX_GPIO_NUM_BANKS 4

/////////////////////////////////////////////////////////

struct GPIO {
	volatile uint32_t *base;
	volatile uint32_t *oe;
	volatile uint32_t *in;
	volatile uint32_t *out;
} gpio_bank[LINUX_GPIO_NUM_BANKS];

bool    use_bone_gpio;

void    boneGpioInit(void);
void    boneGpioPinMode(uint8_t pin, uint8_t output);
uint8_t boneGpioRead(uint8_t pin);
void    boneGpioWrite(uint8_t pin, uint8_t value);
void    boneGpioToggle(uint8_t pin);

#endif

#endif

