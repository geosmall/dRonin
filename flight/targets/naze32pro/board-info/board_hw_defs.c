/**
 ******************************************************************************
 * @addtogroup Targets Target Boards
 * @{
 * @addtogroup AFRO Nazxe32Pro
 * @{
 *
 * @file       naze32pro/board-info/board_hw_defs.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2011.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2012-2015
 * @author     dRonin, http://dRonin.org/, Copyright (C) 2016
 * @brief      Defines board specific static initializers for hardware for the
 *             DTFc board.
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
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
 *
 * Additional note on redistribution: The copyright and license notices above
 * must be maintained in each individual source file that is a derivative work
 * of this source file; otherwise redistribution is prohibited.
 */

#include <pios_config.h>
#include <pios_board_info.h>

#if defined(PIOS_INCLUDE_ANNUNC)
#include <pios_annunc_priv.h>

// PB12 Naze32Pro LED
// PB10 Naze32Pro BUZZER

static const struct pios_annunc pios_annuncs[] = {
	[PIOS_LED_HEARTBEAT] = {
		.pin = {
			.gpio = GPIOB,
			.init = {
				.GPIO_Pin   = GPIO_Pin_12,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_NOPULL
			},
		},
		.active_high = false,
	},
	[PIOS_ANNUNCIATOR_BUZZER] = {
		.pin = {
			.gpio = GPIOB,
			.init = {
				.GPIO_Pin   = GPIO_Pin_10,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_NOPULL
			},
		},
		.active_high = true,
	},
};

static const struct pios_annunc_cfg pios_annunc_cfg = {
	.annunciators     = pios_annuncs,
	.num_annunciators = NELEMENTS(pios_annuncs),
};

const struct pios_annunc_cfg *PIOS_BOARD_HW_DEFS_GetLedCfg(uint32_t board_revision)
{
	return &pios_annunc_cfg;
}

#endif	/* PIOS_INCLUDE_ANNUNC */

#if defined(PIOS_INCLUDE_SPI)
#include <pios_spi_priv.h>

/*
 * SPI2 Interface
 * Used for MPU6000, HMC5983, MS5611, and SPI EEPROM communication
 */
static const struct pios_spi_cfg pios_spi_internal_cfg = {
	.regs = SPI2,
	.remap = GPIO_AF_5,
	.init = {
		.SPI_Mode              = SPI_Mode_Master,
		.SPI_Direction         = SPI_Direction_2Lines_FullDuplex,
		.SPI_DataSize          = SPI_DataSize_8b,
		.SPI_NSS               = SPI_NSS_Soft,
		.SPI_FirstBit          = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial     = 7,
		.SPI_CPOL              = SPI_CPOL_High,
		.SPI_CPHA              = SPI_CPHA_2Edge,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8,
	},
	.sclk = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_13,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
		.pin_source = GPIO_PinSource13,
	},
	.miso = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_14,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
		.pin_source = GPIO_PinSource14,
	},
	.mosi = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_15,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
		.pin_source = GPIO_PinSource15,
	},
	.slave_count = 4,
	.ssel = {
		{
			.gpio = GPIOC,  // MPU6000
			.init = {
				.GPIO_Pin   = GPIO_Pin_15,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
		},
		{
			.gpio = GPIOC,  // HMC5983
			.init = {
				.GPIO_Pin   = GPIO_Pin_14,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
		},
		{
			.gpio = GPIOC,  // MS5611
			.init = {
				.GPIO_Pin   = GPIO_Pin_13,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
		},
		{
			.gpio = GPIOB,  // SPI EEPROM
			.init = {
				.GPIO_Pin   = GPIO_Pin_2,
				.GPIO_Speed = GPIO_Speed_50MHz,
				.GPIO_Mode  = GPIO_Mode_OUT,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
		},
	},
};
#endif	/* PIOS_INCLUDE_SPI */

#if defined(PIOS_INCLUDE_I2C)

#include <pios_i2c_priv.h>

/*
 * I2C Adapter
 */

void PIOS_I2C_external_ev_irq_handler(void);
void PIOS_I2C_external_er_irq_handler(void);

void I2C1_EV_EXTI23_IRQHandler() __attribute__ ((alias ("PIOS_I2C_external_ev_irq_handler")));
void I2C1_ER_IRQHandler()        __attribute__ ((alias ("PIOS_I2C_external_er_irq_handler")));

static const struct pios_i2c_adapter_cfg pios_i2c_external_cfg = {
  .regs = I2C1,
  .remap = GPIO_AF_4,
  .init = {
    .I2C_Mode                = I2C_Mode_I2C,
    .I2C_OwnAddress1         = 0x00,
    .I2C_Ack                 = I2C_Ack_Enable,
    .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit,
    .I2C_DigitalFilter       = 0x00,
    .I2C_AnalogFilter        = I2C_AnalogFilter_Disable,
    .I2C_Timing              = 0x20400D29,  // Generated from I2C_Timing_Configuration_V1.0.1.xls
                                            // SYSCLK:          72 MHz
                                            // Fast Mode:      400 kHz
                                            // Rise Time:      100 nS
                                            // Fall Time:       10 nS
                                            // Analog Filter:  Disabled
                                            // Digital Filter:   0
  },
  .transfer_timeout_ms = 50,
  .scl = {
    .gpio = GPIOB,
    .init = {
			.GPIO_Pin   = GPIO_Pin_8,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_UP,
    },
	.pin_source = GPIO_PinSource8,
  },
  .sda = {
    .gpio = GPIOB,
    .init = {
			.GPIO_Pin   = GPIO_Pin_9,
            .GPIO_Mode  = GPIO_Mode_AF,
            .GPIO_Speed = GPIO_Speed_50MHz,
            .GPIO_OType = GPIO_OType_OD,
            .GPIO_PuPd  = GPIO_PuPd_UP,
    },
	.pin_source = GPIO_PinSource9,
  },
  .event = {
    .flags   = 0,		/* FIXME: check this */
    .init = {
			.NVIC_IRQChannel = I2C1_EV_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
    },
  },
  .error = {
    .flags   = 0,		/* FIXME: check this */
    .init = {
			.NVIC_IRQChannel = I2C1_ER_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
    },
  },
};

pios_i2c_t pios_i2c_external_id;
void PIOS_I2C_external_ev_irq_handler(void)
{
  /* Call into the generic code to handle the IRQ for this specific device */
  PIOS_I2C_EV_IRQ_Handler(pios_i2c_external_id);
}

void PIOS_I2C_external_er_irq_handler(void)
{
  /* Call into the generic code to handle the IRQ for this specific device */
  PIOS_I2C_ER_IRQ_Handler(pios_i2c_external_id);
}

#endif /* PIOS_INCLUDE_I2C */

#if defined(PIOS_INCLUDE_FLASH)
#include "pios_flashfs_logfs_priv.h"

static const struct flashfs_logfs_cfg flashfs_internal_settings_cfg = {
	.fs_magic      = 0x9ae1ee11,
	.arena_size    = 0x00002000,       /* 32 * slot size = 8K bytes = 4 sectors */
	.slot_size     = 0x00000100,       /* 256 bytes */
};

#include "pios_flash_internal_priv.h"

static const struct pios_flash_internal_cfg flash_internal_cfg = {
};

#include "pios_flash_priv.h"

static const struct pios_flash_sector_range stm32f3_sectors[] = {
	{
		.base_sector = 0,
		.last_sector = 127,
		.sector_size = FLASH_SECTOR_2KB,
	},
};

uintptr_t pios_internal_flash_id;
static const struct pios_flash_chip pios_flash_chip_internal = {
	.driver        = &pios_internal_flash_driver,
	.chip_id       = &pios_internal_flash_id,
	.page_size     = 16, /* 128-bit rows */
	.sector_blocks = stm32f3_sectors,
	.num_blocks    = NELEMENTS(stm32f3_sectors),
};

static const struct pios_flash_partition pios_flash_partition_table[] = {
	{
		.label        = FLASH_PARTITION_LABEL_BL,
		.chip_desc    = &pios_flash_chip_internal,
		.first_sector = 0,
		.last_sector  = 7,
		.chip_offset  = 0,
		.size         = (7 - 0 + 1) * FLASH_SECTOR_2KB,
	},

	{
		.label        = FLASH_PARTITION_LABEL_SETTINGS,
		.chip_desc    = &pios_flash_chip_internal,
		.first_sector = 8,
		.last_sector  = 15,
		.chip_offset  = (8 * FLASH_SECTOR_2KB),
		.size         = (15 - 8 + 1) * FLASH_SECTOR_2KB,
	},

	{
		.label        = FLASH_PARTITION_LABEL_AUTOTUNE,
		.chip_desc    = &pios_flash_chip_internal,
		.first_sector = 16,
		.last_sector  = 23,
		.chip_offset  = (16 * FLASH_SECTOR_2KB),
		.size         = (23 - 16 + 1) * FLASH_SECTOR_2KB,
	},

	{
		.label        = FLASH_PARTITION_LABEL_FW,
		.chip_desc    = &pios_flash_chip_internal,
		.first_sector = 24,
		.last_sector  = 127,
		.chip_offset  = (24 * FLASH_SECTOR_2KB),
		.size         = (127 - 24 + 1) * FLASH_SECTOR_2KB,
	},
};

const struct pios_flash_partition * PIOS_BOARD_HW_DEFS_GetPartitionTable (uint32_t board_revision, uint32_t * num_partitions)
{
	PIOS_Assert(num_partitions);

	*num_partitions = NELEMENTS(pios_flash_partition_table);
	return pios_flash_partition_table;
}

#endif	/* PIOS_INCLUDE_FLASH */

#if defined(PIOS_INCLUDE_USART)

#include "pios_usart_priv.h"

#if defined(PIOS_INCLUDE_DSM)
/*
 * Spektrum/JR DSM USART
 */
#include <pios_dsm_priv.h>

static const struct pios_dsm_cfg pios_uart1_dsm_bind_cfg = {
	.bind = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
	},
};

static const struct pios_dsm_cfg pios_uart2_dsm_bind_cfg = {
	.bind = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_15,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
	},
};

static const struct pios_dsm_cfg pios_rcvr_dsm_bind_cfg = {
	.bind = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_11,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL
		},
	},
};

#endif	/* PIOS_INCLUDE_DSM */

static const struct pios_usart_cfg pios_uart1_cfg = {
	.regs = USART1,
	.remap = GPIO_AF_7,
	.irq = {
		.init = {
			.NVIC_IRQChannel = USART1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority = 0,
			.NVIC_IRQChannelCmd = ENABLE,
		},
	},
	.rx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_10,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
		.pin_source = GPIO_PinSource10,
	},
	.tx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_9,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
		.pin_source = GPIO_PinSource9,
	},
};

static const struct pios_usart_cfg pios_uart2_cfg = {
	.regs = USART2,
	.remap = GPIO_AF_7,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = USART2_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.rx = {
		.gpio = GPIOA,
		.init = {
			.GPIO_Pin   = GPIO_Pin_15,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
		.pin_source = GPIO_PinSource15,
	},
	.tx = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_3,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
		.pin_source = GPIO_PinSource3,
	},
};

static const struct pios_usart_cfg pios_rcvr_usart_cfg = {
	.regs = USART3,
	.remap = GPIO_AF_7,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = USART3_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGHEST,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		  },
	},
	.rx = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_11,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_AF,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_UP
		},
		.pin_source = GPIO_PinSource11,
	},
};

#endif  /* PIOS_INCLUDE_USART */

#if defined(PIOS_INCLUDE_COM)

#include "pios_com_priv.h"

#endif	/* PIOS_INCLUDE_COM */

#if defined(PIOS_INCLUDE_RTC)
/*
 * Realtime Clock (RTC)
 */
#include <pios_rtc_priv.h>

void PIOS_RTC_IRQ_Handler (void);
void RTC_WKUP_IRQHandler() __attribute__ ((alias ("PIOS_RTC_IRQ_Handler")));
static const struct pios_rtc_cfg pios_rtc_main_cfg = {
	.clksrc = RCC_RTCCLKSource_HSE_Div32,
	//.prescaler = 25 - 1, //  8MHz / 32 / 16 / 25 == 625Hz
	.prescaler = 37 - 1, // 12MHz / 32 / 16 / 37 == 633Hz
	//.prescaler = 38 - 1, // 12MHz / 32 / 16 / 38 == 617Hz
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = RTC_WKUP_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

void PIOS_RTC_IRQ_Handler (void)
{
	PIOS_RTC_irq_handler ();
}

#endif

#include "pios_tim_priv.h"

static const TIM_TimeBaseInitTypeDef tim_1_8_15_16_17_time_base = {
	.TIM_Prescaler = (PIOS_PERIPHERAL_APB2_CLOCK / 1000000) - 1,
	.TIM_ClockDivision = TIM_CKD_DIV1,
	.TIM_CounterMode = TIM_CounterMode_Up,
	.TIM_Period = ((1000000 / PIOS_SERVO_UPDATE_HZ) - 1),
	.TIM_RepetitionCounter = 0x0000,
};

static const TIM_TimeBaseInitTypeDef tim_2_3_4_time_base = {
	.TIM_Prescaler = (PIOS_PERIPHERAL_APB1_CLOCK / 1000000 * 2) - 1,
	.TIM_ClockDivision = TIM_CKD_DIV1,
	.TIM_CounterMode = TIM_CounterMode_Up,
	.TIM_Period = ((1000000 / PIOS_SERVO_UPDATE_HZ) - 1),
	.TIM_RepetitionCounter = 0x0000,
};

/* Input Timers */

static const struct pios_tim_clock_cfg tim_1_cfg = {
	.timer = TIM1,
	.time_base_init = &tim_1_8_15_16_17_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM1_CC_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

static const struct pios_tim_clock_cfg tim_16_cfg = {
	.timer = TIM16,
	.time_base_init = &tim_1_8_15_16_17_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM1_UP_TIM16_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

/* Output Timers */

static const struct pios_tim_clock_cfg tim_2_cfg = {
	.timer = TIM2,
	.time_base_init = &tim_2_3_4_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM2_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

static const struct pios_tim_clock_cfg tim_15_cfg = {
	.timer = TIM15,
	.time_base_init = &tim_1_8_15_16_17_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM1_BRK_TIM15_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

static const struct pios_tim_clock_cfg tim_3_cfg = {
	.timer = TIM3,
	.time_base_init = &tim_2_3_4_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM3_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

static const struct pios_tim_clock_cfg tim_4_cfg = {
	.timer = TIM4,
	.time_base_init = &tim_2_3_4_time_base,
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = TIM4_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_MID,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
};

/**
 * Pios servo configuration structures
 */
/*
 * 	OUTPUTS
	1:  TIM2_CH1  (PA0)
	2:  TIM2_CH2  (PA1)
	3:  TIM15_CH1 (PA2)
	4:  TIM15_CH2 (PA3)
	5:  TIM3_CH1  (PA6)
	6:  TIM3_CH2  (PA7)
	7:  TIM4_CH1  (PB6)
	8:  TIM4_CH2  (PB7)
 */

static const struct pios_tim_channel pios_tim_output_pins[] = {
	{ // Ch1
		.timer = TIM2,
		.timer_chan = TIM_Channel_1,
		.remap = GPIO_AF_1,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_0,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource0,
		},
	},
	{ // Ch2
		.timer = TIM2,
		.timer_chan = TIM_Channel_2,
		.remap = GPIO_AF_1,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_1,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource1,
		},
	},
	{ // Ch3
		.timer = TIM15,
		.timer_chan = TIM_Channel_1,
		.remap = GPIO_AF_9,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_2,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource2,
		},
	},
	{ // Ch4
		.timer = TIM15,
		.timer_chan = TIM_Channel_2,
		.remap = GPIO_AF_9,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_3,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource3,
		},
	},
	{ // Ch5
		.timer = TIM3,
		.timer_chan = TIM_Channel_1,
		.remap = GPIO_AF_2,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_6,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource6,
		},
	},
	{ // Ch6
		.timer = TIM3,
		.timer_chan = TIM_Channel_2,
		.remap = GPIO_AF_2,
		.pin = {
			.gpio = GPIOA,
			.init = {
				.GPIO_Pin   = GPIO_Pin_7,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource7,
		},
	},
	{ // Ch7
		.timer = TIM4,
		.timer_chan = TIM_Channel_1,
		.remap = GPIO_AF_2,
		.pin = {
			.gpio = GPIOB,
			.init = {
				.GPIO_Pin   = GPIO_Pin_6,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource6,
		},
	},
		{ // Ch8
		.timer = TIM4,
		.timer_chan = TIM_Channel_2,
		.remap = GPIO_AF_2,
		.pin = {
			.gpio = GPIOB,
			.init = {
				.GPIO_Pin   = GPIO_Pin_7,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource7,
		},
	},
};

#if defined(PIOS_INCLUDE_SERVO) && defined(PIOS_INCLUDE_TIM)
/*
 * Servo outputs
 */
#include <pios_servo_priv.h>

struct pios_servo_cfg pios_servo_cfg = {
	.tim_oc_init = {
		.TIM_OCMode       = TIM_OCMode_PWM1,
		.TIM_OutputState  = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Disable,
		.TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
		.TIM_OCPolarity   = TIM_OCPolarity_High,
		.TIM_OCNPolarity  = TIM_OCPolarity_High,
		.TIM_OCIdleState  = TIM_OCIdleState_Reset,
		.TIM_OCNIdleState = TIM_OCNIdleState_Reset,
	},
	.channels = pios_tim_output_pins,
	.num_channels = NELEMENTS(pios_tim_output_pins),
};

struct pios_servo_cfg pios_servo_slow_cfg = {
	.tim_oc_init = {
		.TIM_OCMode       = TIM_OCMode_PWM1,
		.TIM_OutputState  = TIM_OutputState_Enable,
		.TIM_OutputNState = TIM_OutputNState_Disable,
		.TIM_Pulse        = PIOS_SERVOS_INITIAL_POSITION,
		.TIM_OCPolarity   = TIM_OCPolarity_High,
		.TIM_OCNPolarity  = TIM_OCPolarity_High,
		.TIM_OCIdleState  = TIM_OCIdleState_Reset,
		.TIM_OCNIdleState = TIM_OCNIdleState_Reset,
	},
	.force_1MHz = true,
	.channels = pios_tim_output_pins,
	.num_channels = NELEMENTS(pios_tim_output_pins),
};

#endif	/* PIOS_INCLUDE_SERVO && PIOS_INCLUDE_TIM */

#if defined(PIOS_INCLUDE_PPM)
#include <pios_pwm_priv.h>

/*
 * 	PPM INPUT (JST Connector)
	1:  TIM2_CH4  (PB11) - Note: Restricts PWM 1,2, and 9 to 50 Hz!!
 */
static const struct pios_tim_channel pios_tim_rcvrport_ppm[] = {
	{
		.timer = TIM2,
		.timer_chan = TIM_Channel_4,
		.remap = GPIO_AF_1,
		.pin = {
			.gpio = GPIOB,
			.init = {
				.GPIO_Pin   = GPIO_Pin_11,
				.GPIO_Speed = GPIO_Speed_2MHz,
				.GPIO_Mode  = GPIO_Mode_AF,
				.GPIO_OType = GPIO_OType_PP,
				.GPIO_PuPd  = GPIO_PuPd_UP
			},
			.pin_source = GPIO_PinSource11,
		},
	},
};

#endif

/*
 * PPM Input
 */
#if defined(PIOS_INCLUDE_PPM)
#include <pios_ppm_priv.h>
static const struct pios_ppm_cfg pios_ppm_cfg = {
	.tim_ic_init = {
		.TIM_ICPolarity  = TIM_ICPolarity_Rising,
		.TIM_ICSelection = TIM_ICSelection_DirectTI,
		.TIM_ICPrescaler = TIM_ICPSC_DIV1,
		.TIM_ICFilter    = 0x0,
		.TIM_Channel     = TIM_Channel_4,
	},
	.channels = pios_tim_rcvrport_ppm,
	.num_channels = 1,
};
#endif // PIOS_INCLUDE_PPM

#if defined(PIOS_INCLUDE_ADC)
#include "pios_adc_priv.h"
#include "pios_internal_adc_priv.h"

uintptr_t pios_internal_adc_id;

void DMA1_Channel1_IRQHandler(void) __attribute__((alias("PIOS_ADC_DMA_irq_handler")));

void PIOS_ADC_DMA_irq_handler(void)
{
	/* Call into the generic code to handle the IRQ for this specific device */
	PIOS_INTERNAL_ADC_DMA_Handler(pios_internal_adc_id);
}

/**
 * ADC0 : Vrefint        Internal ADC voltage reference
 * ADC1 : PA4 ADC2_IN1   Battery Voltage (On board 11K / 1K divider)
 * ADC2 : PA5 ADC2_IN2
 * ADC3 : PA1 ADC1_IN1
 */
static struct pios_internal_adc_cfg internal_adc_cfg = {
	.dma = {
		.irq = {
			.flags   = (DMA1_FLAG_TC1 | DMA1_FLAG_TE1 | DMA1_FLAG_HT1 | DMA1_FLAG_GL1),
			.init    = {
				.NVIC_IRQChannel                   = DMA1_Channel1_IRQn,
				.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
				.NVIC_IRQChannelSubPriority        = 0,
				.NVIC_IRQChannelCmd                = ENABLE,
			},
		},
		.rx = {
			.channel = DMA1_Channel1,
			.init    = {
				.DMA_Priority = DMA_Priority_High,
			},
		}
	},
	.half_flag = DMA1_IT_HT1,
	.full_flag = DMA1_IT_TC1,
	.oversampling = 4,
	.adc_pins = {
			{NULL,  0,          ADC_Channel_Vrefint, true},
			{GPIOA, GPIO_Pin_4, ADC_Channel_1,      false},
			{GPIOA, GPIO_Pin_5, ADC_Channel_2,      false},
			{GPIOA, GPIO_Pin_1, ADC_Channel_2,       true},
	},
	.adc_pin_count = 4,
	.adc_dev_master = ADC1,
	.adc_dev_slave  = ADC2,
};

#endif /* PIOS_INCLUDE_ADC */

#if defined(PIOS_INCLUDE_RCVR)
#include "pios_rcvr_priv.h"
#endif /* PIOS_INCLUDE_RCVR */

#if defined(PIOS_INCLUDE_USB)
#include "pios_usb_priv.h"

static const struct pios_usb_cfg pios_usb_main_cfg = {
	.irq = {
		.init    = {
			.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_LOW,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.vsense = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_5,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	}
};

const struct pios_usb_cfg * PIOS_BOARD_HW_DEFS_GetUsbCfg (uint32_t board_revision)
{
	return &pios_usb_main_cfg;
}

#include "pios_usb_board_data_priv.h"
#include "pios_usb_desc_hid_cdc_priv.h"
#include "pios_usb_desc_hid_only_priv.h"
#include "pios_usbhook.h"

#endif	/* PIOS_INCLUDE_USB */

#if defined(PIOS_INCLUDE_COM_MSG)

#include <pios_com_msg_priv.h>

#endif /* PIOS_INCLUDE_COM_MSG */

#if defined(PIOS_INCLUDE_USB_HID)
#include <pios_usb_hid_priv.h>

const struct pios_usb_hid_cfg pios_usb_hid_cfg = {
	.data_if = 0,
	.data_rx_ep = 1,
	.data_tx_ep = 1,
};
#endif /* PIOS_INCLUDE_USB_HID */

#if defined(PIOS_INCLUDE_USB_CDC)
#include <pios_usb_cdc_priv.h>

const struct pios_usb_cdc_cfg pios_usb_cdc_cfg = {
	.ctrl_if = 1,
	.ctrl_tx_ep = 2,

	.data_if = 2,
	.data_rx_ep = 3,
	.data_tx_ep = 3,
};
#endif	/* PIOS_INCLUDE_USB_CDC */

/**
 * Configuration for the MPU6000 chip
 */
#if defined(PIOS_INCLUDE_MPU)
#include "pios_mpu.h"
static const struct pios_exti_cfg pios_exti_mpu_cfg __exti_config = {
	.vector = PIOS_MPU_IRQHandler,
	.line = EXTI_Line1,
	.pin = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_1,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = EXTI1_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_HIGH,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.exti = {
		.init = {
			.EXTI_Line    = EXTI_Line1, // matches above GPIO pin
			.EXTI_Mode    = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising,
			.EXTI_LineCmd = ENABLE,
		},
	},
};

static const struct pios_mpu_cfg pios_mpu_cfg = {
	.exti_cfg           = &pios_exti_mpu_cfg,
	.default_samplerate = 500,
	.orientation        = PIOS_MPU_TOP_270DEG
};
#endif /* PIOS_INCLUDE_MPU */

/**
 * Configuration of the internal HMC5983 chip
 */
#if defined(PIOS_INCLUDE_HMC5983)
#include "pios_hmc5983.h"
static const struct pios_exti_cfg pios_exti_hmc5983_cfg __exti_config = {
	.vector = PIOS_HMC5983_SPI_IRQHandler,
	.line = EXTI_Line0,
	.pin = {
		.gpio = GPIOB,
		.init = {
			.GPIO_Pin   = GPIO_Pin_0,
			.GPIO_Speed = GPIO_Speed_50MHz,
			.GPIO_Mode  = GPIO_Mode_IN,
			.GPIO_OType = GPIO_OType_OD,
			.GPIO_PuPd  = GPIO_PuPd_NOPULL,
		},
	},
	.irq = {
		.init = {
			.NVIC_IRQChannel                   = EXTI0_IRQn,
			.NVIC_IRQChannelPreemptionPriority = PIOS_IRQ_PRIO_LOW,
			.NVIC_IRQChannelSubPriority        = 0,
			.NVIC_IRQChannelCmd                = ENABLE,
		},
	},
	.exti = {
		.init = {
			.EXTI_Line    = EXTI_Line0, // matches above GPIO pin
			.EXTI_Mode    = EXTI_Mode_Interrupt,
			.EXTI_Trigger = EXTI_Trigger_Rising,
			.EXTI_LineCmd = ENABLE,
		},
	},
};

static const struct pios_hmc5983_cfg pios_hmc5983_cfg = {
	.exti_cfg    = &pios_exti_hmc5983_cfg,
	.Averaging   = PIOS_HMC5983_AVERAGING_8,
	.M_ODR       = PIOS_HMC5983_ODR_75,
	.Meas_Conf   = PIOS_HMC5983_MEASCONF_NORMAL,
	.Gain        = PIOS_HMC5983_GAIN_1_9,
	.Mode        = PIOS_HMC5983_MODE_CONTINUOUS,
	.Orientation = PIOS_HMC5983_TOP_180DEG,
};

#endif /* PIOS_INCLUDE_HMC5983 */

/**
 * Configuration for the MS5611 chip
 */
#if defined(PIOS_INCLUDE_MS5611_SPI)
#include "pios_ms5611_priv.h"
static const struct pios_ms5611_cfg pios_ms5611_cfg = {
	.oversampling             = MS5611_OSR_4096,
	.temperature_interleaving = 1,
};
#endif /* PIOS_INCLUDE_MS5611_SPI */

/**
 * @}
 * @}
 */