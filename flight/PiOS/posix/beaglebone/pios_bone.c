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

#include "openpilot.h"
#include "pios.h"

#include "pios_thread.h"
#include "pios_queue.h"
#include "taskmonitor.h"

#include "pios_bone.h"
#include "pios_bone_adc.h"
#include "pios_bone_pruInput.h"
#include "pios_bone_pruOutput.h"

/**
 * Magic byte sequence used to validate the device state struct.
 * Should be unique amongst all PiOS drivers!
 */
enum pios_bone_dev_magic {
	PIOS_BONE_DEV_MAGIC = 0x656E6F42 /**< Unique byte sequence 'Bone' */
};

static void    PIOS_Bone_ActuatorUpdate(void);
static int     PIOS_Bone_ActuatorSetMode(const uint16_t *out_rate, const int banks, const uint16_t *channel_max, const uint16_t *channel_min);
static void    PIOS_Bone_ActuatorSet(uint8_t servo, float position);

const struct pios_servo_callbacks bone_callbacks = {
	.update   = PIOS_Bone_ActuatorUpdate,
	.set_mode = PIOS_Bone_ActuatorSetMode,
	.set      = PIOS_Bone_ActuatorSet
};

static int32_t PIOS_BONE_ADC_PinGet(uintptr_t dev_int, uint32_t pin);
static uint8_t PIOS_BONE_ADC_NumberOfChannels(uintptr_t dev_int);
static float   PIOS_BONE_ADC_LSB_Voltage(uintptr_t dev_int);

const struct pios_adc_driver pios_bone_adc_driver = {
		.get_pin            = PIOS_BONE_ADC_PinGet,
		.number_of_channels = PIOS_BONE_ADC_NumberOfChannels,
		.lsb_voltage        = PIOS_BONE_ADC_LSB_Voltage,
};

static int32_t PIOS_BONE_Receiver_Get(uintptr_t dev_int, uint8_t channel);

const struct pios_rcvr_driver pios_bone_rcvr_driver = {
	.read = PIOS_BONE_Receiver_Get,
};

/**
 * @brief The device state struct
 */
struct pios_bone_dev {
	enum pios_bone_dev_magic magic;    /**< Magic bytes to validate the struct contents */

	volatile uint16_t adc_value[BONE_MAX_ADCCHANS];
};

//! Global structure for this device device
static struct pios_bone_dev *bone_dev;

static float channel_values[CHANNEL_COUNT_BONE_PWM_OUTPUT];

//! Private functions
/**
 * @brief Allocate a new device
 */
static struct pios_bone_dev *PIOS_Bone_Alloc();

static struct pios_bone_dev *PIOS_Bone_Alloc()
{
	struct pios_bone_dev *dev;

	dev = (struct pios_bone_dev *)PIOS_malloc(sizeof(*bone_dev));
	if (!dev)
		return NULL;

	dev->magic = PIOS_BONE_DEV_MAGIC;

	PIOS_Servo_SetCallbacks(&bone_callbacks);

	return dev;
}

static int32_t PIOS_Bone_Validate(struct pios_bone_dev *dev)
{
	if (dev == NULL)
		return -1;
	if (dev->magic != PIOS_BONE_DEV_MAGIC)
		return -2;
	return 0;
}

int32_t PIOS_Bone_Init(pios_bone_dev_t *dev)
{
	bone_dev = PIOS_Bone_Alloc();
	if (bone_dev == NULL)
		return -1;
	*dev = bone_dev;

	for (int channel = 0; channel < CHANNEL_COUNT_BONE_PWM_OUTPUT; channel++)
		pios_bone_rcOutput_enable_ch(channel);

	return 0;
}

static void PIOS_Bone_ActuatorUpdate(void)
{
	PIOS_Assert(!PIOS_Bone_Validate(bone_dev));

	corked = false;

	for (uint8_t channel = 0; channel < CHANNEL_COUNT_BONE_PWM_OUTPUT; channel++)
		pios_bone_rcOutput_write(channel, (uint16_t)channel_values[channel]);
}

static int PIOS_Bone_ActuatorSetMode(const uint16_t *out_rate, const int banks, const uint16_t *channel_max, const uint16_t *channel_min)
{
	PIOS_Assert(banks >= BANK_COUNT_BONE_PWM_OUTPUT);

	int rate;

	for (int bank = 0; bank < BANK_COUNT_BONE_PWM_OUTPUT; bank++) {
		if (out_rate[bank] < 50) rate = 50;

		else if (out_rate[bank] > 1000) rate = 1000;

		else rate = out_rate[bank];

		if      (bank == 0) {
			pios_bone_rcOutput_set_freq(0b00000011, rate);
		}
		else if (bank == 1) {
			pios_bone_rcOutput_set_freq(0b00000100, rate);
		}
		else if (bank == 2) {
			pios_bone_rcOutput_set_freq(0b00001000, rate);
		}
		else if (bank == 3) {
			pios_bone_rcOutput_set_freq(0b00010000, rate);
		}
                else if (bank == 4) {
                        pios_bone_rcOutput_set_freq(0b00100000, rate);
		}
#if defined(PIOS_INCLUDE_BLUE)
		else if (bank == 5) {
			pios_bone_rcOutput_set_freq(0b11000000, rate);
		}
#elif defined(PIOS_INCLUDE_POCKET

#else
	#error "No Bone Target Defined!!"
#endif

	}

	return 0;
}

static void PIOS_Bone_ActuatorSet(uint8_t servo, float position)
{
//	PIOS_Assert(servo >= CHANNEL_COUNT_BONE_PWM_OUTPUT);

	if (servo < CHANNEL_COUNT_BONE_PWM_OUTPUT)
		channel_values[servo] = position;

	return;
}

static int32_t PIOS_BONE_ADC_PinGet(uintptr_t dev_int, uint32_t pin) {
	if (pin >= BONE_MAX_ADCCHANS) {
		return -1;
	}

	return boneAdcReadRaw(pin);
}

static uint8_t PIOS_BONE_ADC_NumberOfChannels(uintptr_t dev_int) {
	return BONE_MAX_ADCCHANS;
}

#define VREF_PLUS 1.8f

static float PIOS_BONE_ADC_LSB_Voltage(uintptr_t dev_int) {
	// Value is expected to be a fraction of 1.8v
        return VREF_PLUS / (((uint32_t)1 << 12) - 1);
}

static int32_t PIOS_BONE_Receiver_Get(uintptr_t dev_int, uint8_t channel)
{
	if (channel >= LINUX_RC_INPUT_NUM_CHANNELS) {
		return PIOS_RCVR_INVALID;
	}

	return pwm_values[channel];
}

#endif
