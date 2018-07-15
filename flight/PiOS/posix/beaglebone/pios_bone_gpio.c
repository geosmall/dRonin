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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __linux__

#include <poll.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "pios_bone_gpio.h"

bool use_bone_gpio = false;

/**************************************************************
 * boneGpioInit
 **************************************************************/
void boneGpioInit(void)
{
	int mem_fd;
	// Enable all GPIO banks
	// Without this, access to deactivated banks (i.e. those with no clock source set up) will (logically) fail with SIGBUS
	// Idea taken from https://groups.google.com/forum/#!msg/beagleboard/OYFp4EXawiI/Mq6s3sg14HoJ

	uint8_t bank_enable[3] = { 5, 65, 105 };

	int export_fd = open("/sys/class/gpio/export", O_WRONLY | O_CLOEXEC);

	if (export_fd == -1) {
		printf("pios_bone_gpio: Unable to open /sys/class/gpio/export....\n");
	}

	for (uint8_t i = 0; i < 3; i++) {
		dprintf(export_fd, "%u\n", (unsigned)bank_enable[i]);
	}

	close(export_fd);

	// open /dev/mem

	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0) {
		printf("pios_bone_gpio: Can't open /dev/mem....\n");
		exit(-1);
	}

	// mmap GPIO

	off_t offsets[LINUX_GPIO_NUM_BANKS] = { GPIO0_BASE, GPIO1_BASE, GPIO2_BASE, GPIO3_BASE };

	for (uint8_t i = 0; i < LINUX_GPIO_NUM_BANKS; i++) {
		gpio_bank[i].base = (volatile unsigned *)mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,  mem_fd, offsets[i]);

		if ((char *)gpio_bank[i].base == MAP_FAILED) {
			printf("pios_bone_gpio: Unable to map GPIO bank....");
		}

		gpio_bank[i].oe  = gpio_bank[i].base + GPIO_OE;
		gpio_bank[i].in  = gpio_bank[i].base + GPIO_IN;
		gpio_bank[i].out = gpio_bank[i].base + GPIO_OUT;
	}

	close(mem_fd);

	printf("pios_bone_gpio: GPIO Init Complete....\n");
}

/**************************************************************
 * boneGpioPinMode
 **************************************************************/
void boneGpioPinMode(uint8_t pin, uint8_t output)
{
	uint8_t bank = pin / 32;

	uint8_t bank_pin = pin & 0x1F;

	if (bank >= LINUX_GPIO_NUM_BANKS) {
		return;
	}

	if (output == BONE_GPIO_INPUT) {
		*gpio_bank[bank].oe |=  (1U << bank_pin);
	} else {
		*gpio_bank[bank].oe &= ~(1U << bank_pin);
	}
}

/**************************************************************
 * boneGpioRead
 **************************************************************/
uint8_t boneGpioRead(uint8_t pin)
{
	uint8_t bank = pin /32;

	uint8_t bank_pin = pin & 0x1F;

	if (bank >= LINUX_GPIO_NUM_BANKS) {
		return 0;
	}

	return *gpio_bank[bank].in & (1U << bank_pin) ? BONE_GPIO_HIGH : BONE_GPIO_LOW;
}

/**************************************************************
 * boneGpioWrite
 **************************************************************/
void boneGpioWrite(uint8_t pin, uint8_t value)
{
	uint8_t bank = pin / 32;

	uint8_t bank_pin = pin & 0x1F;

	if (bank >= LINUX_GPIO_NUM_BANKS) {
		return;
	}

	if (value == BONE_GPIO_LOW) {
		*gpio_bank[bank].out &= ~(1U << bank_pin);
	} else {
		*gpio_bank[bank].out |=  (1U << bank_pin);
	}
}

/**************************************************************
 * boneGpioToggle
 **************************************************************/
void boneGpioToggle(uint8_t pin)
{
	boneGpioWrite(pin, !boneGpioRead(pin));
}

#endif

#endif

