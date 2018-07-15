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

#ifdef __linux__
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "openpilot.h"
#include "modulesettings.h"
#include "pios_thread.h"
#include "pios_queue.h"
#include "pios_mutex.h"
#include "uavobjectmanager.h"
#include "pios_modules.h"

#include "pios_bone_pruInput.h"

#include "pios_sbus.h"

// Private constants
#define STACK_SIZE_BYTES 512
#define TASK_PRIORITY PIOS_THREAD_PRIO_HIGH

#define BONE_PRU_RCINPUT_PERIOD_MS   1  // 1000 Hz
#define BONE_PRU_RCINPUT_QUEUE_SIZE 64

// Private types

// Private variables
static struct pios_thread          * bonePruRcInputTaskHandle;
static bool                          module_enabled;
struct pios_queue                  * bonePruRcInput_queue;
static struct pios_recursive_mutex * mutex;

// Private functions
static void bonePruRcInputTask(void *parameters);

void rcInputBonePruInit(void);
void rcInputBonePruTimerTick(void);

void process_rc_pulse(uint16_t width_s0, uint16_t width_s1);

void process_ppmsum_pulse(uint16_t width_usec);
void process_sbus_pulse(uint16_t width_s0, uint16_t width_s1);
void process_dsm_pulse(uint16_t width_s0, uint16_t width_s1);

// Local variables

/**
 * Initialise the Bone PRU RC Input module
 * \return -1 if initialisation failed
 * \return 0 on success
 */
int32_t BonePruRcInputInitialize(void)
{
	module_enabled = PIOS_Modules_IsEnabled(PIOS_MODULE_BONEPRURCINPUT);

	if (!module_enabled)
		return -1;

	rcInputBonePruInit();

	return 0;
}

/**
 * Start the Bone PRU RC Input module
 * \return -1 if initialisation failed
 * \return 0 on success
 */
int32_t BonePruRcInputStart(void)
{
	//Check if module is enabled or not
	if (module_enabled == false) {
		return -1;
	}

	// create Bone PRU RC Input queue
	bonePruRcInput_queue = PIOS_Queue_Create(BONE_PRU_RCINPUT_QUEUE_SIZE, sizeof(UAVObjEvent));
	if (!bonePruRcInput_queue){
		return -1;
	}

	// Create mutex
	mutex = PIOS_Recursive_Mutex_Create();
	if (mutex == NULL){
		return -2;
	}

	// Start BONE PRU RC Input task
	bonePruRcInputTaskHandle = PIOS_Thread_Create(bonePruRcInputTask, "BONEPruRcInput", STACK_SIZE_BYTES, NULL, TASK_PRIORITY);

	TaskMonitorAdd(TASKINFO_RUNNING_BONEPRURCINPUT, bonePruRcInputTaskHandle);

	return 0;
}

MODULE_INITCALL(BonePruRcInputInitialize, BonePruRcInputStart);

static void bonePruRcInputTask(void *parameters)
{
	uint32_t now = PIOS_Thread_Systime();

	// Loop forever
	while (1)
	{
		PIOS_Thread_Sleep_Until(&now, BONE_PRU_RCINPUT_PERIOD_MS);

		rcInputBonePruTimerTick();
	}
}

void rcInputBonePruInit(void)
{
#ifdef __linux__
	int mem_fd = open("/dev/mem", O_RDWR|O_SYNC|O_CLOEXEC);

	if (mem_fd == -1) {
		printf("pios_bone_pruInput: Unable to open /dev/mem");
	}

	ring_buffer = (volatile struct ring_buffer*) mmap(0, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, RCIN_PRUSS_RAM_BASE);

	close(mem_fd);
#endif
	ring_buffer->ring_head = 0;
}

/*
  called at 1kHz to check for new pulse capture data from the PRU
 */
void rcInputBonePruTimerTick(void)
{
	while (ring_buffer->ring_head != ring_buffer->ring_tail) {
		if (ring_buffer->ring_tail >= NUM_RING_ENTRIES) {
			// invalid ring_tail from PRU - ignore RC input
			return;
		}

		process_rc_pulse((ring_buffer->buffer[ring_buffer->ring_head].s1_t) / TICK_PER_US,
		                 (ring_buffer->buffer[ring_buffer->ring_head].s0_t) / TICK_PER_US);

		// move to the next ring buffer entry
		ring_buffer->ring_head = (ring_buffer->ring_head + 1) % NUM_RING_ENTRIES;
	}
}

/*
  process a RC input pulse of the given width
 */
void process_rc_pulse(uint16_t width_s0, uint16_t width_s1)
{
#if 0
	// useful for debugging
	static FILE *rclog;
	if (rclog == NULL) {
		rclog = fopen("/tmp/rcin.log", "w");
	}
	if (rclog) {
		fprintf(rclog, "%u %u\n", (unsigned)width_s0, (unsigned)width_s1);
	}
#endif
	// treat as PPM-sum
	process_ppmsum_pulse(width_s0 + width_s1);

	// treat as SBUS
	process_sbus_pulse(width_s0, width_s1);
}

/*
  process a PPM-sum pulse of the given width
 */
void process_ppmsum_pulse(uint16_t width_usec)
{
	if (width_usec >= 2700) {
		// a long pulse indicates the end of a frame. Reset the
		// channel counter so next pulse is channel 0
		if (ppm_state.channel_counter >= MIN_NUM_CHANNELS) {
			for (uint8_t i = 0; i < ppm_state.channel_counter; i++) {
				pwm_values[i] = ppm_state.pulse_capt[i];
			}
			num_channels = ppm_state.channel_counter;
			rc_input_count++;
		}
		ppm_state.channel_counter = 0;
		return;
	}

	if (ppm_state.channel_counter == -1) {
		// we are not synchronised
		return;
	}

	/*
	  we limit inputs to between 700usec and 2300usec. This allows us
	  to decode SBUS on the same pin, as SBUS will have a maximum
	  pulse width of 100usec
	 */
	if (width_usec > 700 && width_usec < 2300) {
		// take a reading for the current channel
		// buffer these
		ppm_state.pulse_capt[ppm_state.channel_counter] = width_usec;

		// move to next channel
		ppm_state.channel_counter++;
	}

	// if we have reached the maximum supported channels then
	// mark as unsynchronised, so we wait for a wide pulse
	if (ppm_state.channel_counter >= LINUX_RC_INPUT_NUM_CHANNELS) {
		for (uint8_t i = 0; i < ppm_state.channel_counter; i++) {
			pwm_values[i] = ppm_state.pulse_capt[i];
		}

		num_channels = ppm_state.channel_counter;

		rc_input_count++;

		ppm_state.channel_counter = -1;
	}
}

/*
  process a SBUS input pulse of the given width
 */
void process_sbus_pulse(uint16_t width_s0, uint16_t width_s1)
{
	// convert to bit widths, allowing for up to 1usec error, assuming 100000 bps
	uint16_t bits_s0 = (width_s0 + 1) / 10;
	uint16_t bits_s1 = (width_s1 + 1) / 10;
	uint16_t nlow;

	uint8_t byte_ofs = sbus_state.bit_ofs / 12;
	uint8_t bit_ofs  = sbus_state.bit_ofs % 12;

	if (bits_s0 == 0 || bits_s1 == 0) {
		// invalid data
		goto reset;
	}

	if (bits_s0 + bit_ofs > 10) {
		// invalid data as last two bits must be stop bits
		goto reset;
	}

	// pull in the high bits
	sbus_state.bytes[byte_ofs] |= ((1U<<bits_s0)-1) << bit_ofs;
	sbus_state.bit_ofs += bits_s0;
	bit_ofs += bits_s0;

	// pull in the low bits
	nlow = bits_s1;
	if (nlow + bit_ofs > 12) {
		nlow = 12 - bit_ofs;
	}

	bits_s1 -= nlow;
	sbus_state.bit_ofs += nlow;

	if (sbus_state.bit_ofs == 25*12 && bits_s1 > 12) {
		// we have a full frame
		uint8_t bytes[25];
		uint8_t i;

		for (i = 0; i < 25; i++) {
			// get inverted data
			uint16_t v = ~sbus_state.bytes[i];
			// check start bit
			if ((v & 1) != 0) {
				goto reset;
			}

			// check stop bits
			if ((v & 0xC00) != 0xC00) {
				goto reset;
			}

			// check parity
			uint8_t parity = 0, j;
			for (j = 1; j <= 8; j++) {
				parity ^= (v & (1U<<j))?1:0;
			}

			if (parity != (v&0x200)>>9) {
				goto reset;
			}

			bytes[i] = ((v>>1) & 0xFF);
		}

		uint16_t values[LINUX_RC_INPUT_NUM_CHANNELS];
		uint16_t num_values=0;

		bool sbus_failsafe = false, sbus_frame_drop = false;

		if (sbus_decode(bytes, values, &num_values, &sbus_failsafe, &sbus_frame_drop, LINUX_RC_INPUT_NUM_CHANNELS) &&
			num_values >= MIN_NUM_CHANNELS) {

			for (i  =0; i < num_values; i++) {
				pwm_values[i] = values[i];
			}

			num_channels = num_values;
			if (!sbus_failsafe) {
				rc_input_count++;
			}
		}

		goto reset;

	} else if (bits_s1 > 12) {
		// break
		goto reset;
	}

	return;

reset:
	memset(&sbus_state, 0, sizeof(sbus_state));
}

#endif

