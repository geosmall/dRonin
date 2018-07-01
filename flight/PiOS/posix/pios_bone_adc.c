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

#include "pios_bone_adc.h"

/**************************************************************/

#if __GNUC__ >= 3
#define unlikely(x)  __builtin_expect (!!(x), 0)
#else
#define unlikely(x)  (x)
#endif

volatile uint32_t   *map;  // pointer to /dev/mem
int mapped           = 0;  // boolean to check if mem mapped
int adc_initialized  = 0;

/**************************************************************
 * init_mmap
 **************************************************************/

// map /dev/mem if it hasn't been done already

int init_mmap(void)
{
	if (mapped) {
		return 0;
	}

	int fd = open("/dev/mem", O_RDWR);

	errno = 0;

	if (unlikely(fd == -1)) {
		printf("pios_bone_adc: Unable to open /dev/mem....\n");

		if (unlikely(errno == EPERM))
			printf("pios_bone_adc: Insufficient priviledges....\n");

		return -1;
	}

	map = (uint32_t*)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);

	if (map == MAP_FAILED) {
		close(fd);
		printf("pios_bone_adc: Unable to map /dev/mem....\n");
		return -1;
	}

	mapped = TRUE;

	return 0;
}

/**************************************************************
 * boneAdcInit
 **************************************************************/

// Initialize the Analog-Digital Converter
// each channel is set up in software one-shot mode for general purpose reading
// internal averagin set to 8 samples to reduce noise

int boneAdcInit(void)
{
	if (adc_initialized) {
		return 0;
	}

	if (init_mmap()) {
		return -1;
	}

	// enable the CM_WKUP_ADC_TSC_CLKCTRL with CM_WKUP_MODULEMODE_ENABLE
	map[(CM_WKUP + CM_WKUP_ADC_TSC_CLKCTRL - MMAP_OFFSET) / 4] |= MODULEMODE_ENABLE;

	// waiting for adc clock module to initialize
	while (!(map[(CM_WKUP + CM_WKUP_ADC_TSC_CLKCTRL - MMAP_OFFSET) / 4] & MODULEMODE_ENABLE)) {
		usleep(10);
	}

	// disable adc
	map[(ADC_CTRL - MMAP_OFFSET) / 4] &= !0x01;

	// make sure STEPCONFIG write protect is off
	map[(ADC_CTRL - MMAP_OFFSET) / 4] |= ADC_STEPCONFIG_WRITE_PROTECT_OFF;

	// setup each ADCSTEPCONFIG for each ain pin
	map[(ADCSTEPCONFIG1 - MMAP_OFFSET) / 4] = 0x00 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY1  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG2 - MMAP_OFFSET) / 4] = 0x01 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY2  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG3 - MMAP_OFFSET) / 4] = 0x02 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY3  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG4 - MMAP_OFFSET) / 4] = 0x03 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY4  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG5 - MMAP_OFFSET) / 4] = 0x04 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY5  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG6 - MMAP_OFFSET) / 4] = 0x05 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY6  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG7 - MMAP_OFFSET) / 4] = 0x06 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY7  - MMAP_OFFSET) / 4] =    0 << 24;
	map[(ADCSTEPCONFIG8 - MMAP_OFFSET) / 4] = 0x07 << 19 | ADC_AVG8 | ADC_SW_ONESHOT;
	map[(ADCSTEPDELAY8  - MMAP_OFFSET) / 4] =    0 << 24;

	// enable the ADC
	map[(ADC_CTRL - MMAP_OFFSET) / 4] |= 0x01;

	// clear the FIFO buffer
	int output;
	while (map[(FIFO0COUNT - MMAP_OFFSET) / 4] & FIFO_COUNT_MASK) {
		output = map[(ADC_FIFO0DATA - MMAP_OFFSET) / 4] & ADC_FIFO_MASK;
	}

	// just supress the warning about output not being used
	if (output) {}

	adc_initialized = 1;

	printf("pios_bone_adc: ADC Init Complete....\n");

	return 0;
}

/**************************************************************
 * boneAdcReadRaw
 **************************************************************/
int boneAdcReadRaw(int ch)
{
	// clear the FIFO buffer just in case it's not empty
	int output;
	while (map[(FIFO0COUNT - MMAP_OFFSET) / 4] & FIFO_COUNT_MASK) {
		output = map[(ADC_FIFO0DATA - MMAP_OFFSET) / 4] & ADC_FIFO_MASK;
	}

	// enable step for the right pin
	map[(ADC_STEPENABLE - MMAP_OFFSET) / 4] |= (0x01 << (ch + 1));

	// wait for sample to appear in the FIFO buffer
	while (!(map[(FIFO0COUNT - MMAP_OFFSET) / 4] & FIFO_COUNT_MASK)) {}

	// return the FIFO data register
	output = map[(ADC_FIFO0DATA - MMAP_OFFSET) / 4] & ADC_FIFO_MASK;

	return output;
}

#endif
