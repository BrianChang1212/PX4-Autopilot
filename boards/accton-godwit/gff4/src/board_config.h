/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * Accton Godwit GFF4 internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/
/* Configuration ************************************************************************************/

/* LEDs - active low. Indexed per the standard PX4 drv_board_led.h scheme
 * (LED_BLUE=0, LED_RED=1, LED_SAFETY=2, LED_GREEN=3); this board has no
 * dedicated safety LED, so index 2 is left unpopulated in g_ledmap[].
 */

#define GPIO_LED_BLUE   (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTB|GPIO_PIN3)
/* R0C has no red LED: PC0 is VBAT_V (ADC3_IN10). 0 = not populated (led.c skips it). */
#define GPIO_LED_RED    0
/* R0C: LED_GREEN is PC4 (PB14 is ICM_CLKIN and must not be driven). */
#define GPIO_LED_GREEN  (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTC|GPIO_PIN4)

#define BOARD_OVERLOAD_LED     LED_BLUE

/* Parameters are a file on the NAND littlefs (CONFIG_BOARD_PARAM_FILE), with
 * PX4's backup copy alongside it. Internal flash sector 1 (16 KiB at
 * 0x08004000) is consequently unused; it stays inside the bootloader
 * reservation, so reclaiming it for the application would mean moving
 * APP_LOAD_ADDRESS and reflashing the bootloader on every unit.
 *
 * The trade-off of keeping parameters on the log volume is handled in init.c:
 * a mounted-but-unwritable filesystem is never reformatted automatically.
 */

/*
 * ADC channels
 *
 * These are the channel numbers of the ADCs of the microcontroller that
 * can be used by the PX4 Firmware in the adc driver.
 */

/* R0C: PC0 = VBAT_V (ADC3_IN10), PC1 = ESC_CURR (ADC3_IN11). PC0/PC1 are also
 * ADC1_IN10/IN11, and the PX4 ADC driver uses ADC1, so channels 10/11 apply. */
#define ADC_CHANNELS ((1 << 10) | (1 << 11))

#define ADC_BATTERY_VOLTAGE_CHANNEL  10  /* PC0, VBAT_V, divider 8.4 */
#define ADC_BATTERY_CURRENT_CHANNEL  11  /* PC1, ESC_CURR, scale to be calibrated on hardware */

/* Define Battery Voltage Divider and A per V */

#define BOARD_BATTERY1_V_DIV         (8.4f)
#define BOARD_BATTERY1_A_PER_V       (40.f)  /* placeholder: schematic marks this
                                               * "scale wait fine tune", calibrate
                                               * against a bench supply before use */

#define DIRECT_PWM_OUTPUT_CHANNELS      9
#define BOARD_HAS_PWM                   DIRECT_PWM_OUTPUT_CHANNELS
#define BOARD_NUM_IO_TIMERS             3  /* TIM8 (PWM1-4), TIM3 (PWM5-8), TIM1 (PWM9) */

/* High-resolution timer */

#define HRT_TIMER                    4  /* T4C1, free-running */
#define HRT_TIMER_CHANNEL            1  /* use capture/compare channel 1 */

/* No PPM support: RC input is SBUS-only. R0C: SBUS (J7 / J13 pin 6) is inverted
 * by Q2 and lands on UART5_RX (PD2) = /dev/ttyS4, not USART1. */

#define RC_SERIAL_PORT               "/dev/ttyS4"
#define BOARD_SUPPORTS_RC_SERIAL_PORT_OUTPUT

/* Tone alarm: buzzer on PB15 = TIM12_CH2 */

#define TONE_ALARM_TIMER             12
#define TONE_ALARM_CHANNEL           2
#define GPIO_TONE_ALARM_IDLE         (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTB|GPIO_PIN15)
#define GPIO_TONE_ALARM              (GPIO_ALT|GPIO_AF9|GPIO_SPEED_2MHz|GPIO_PUSHPULL|GPIO_PORTB|GPIO_PIN15)

/* HW revision strap: PC5 on R0C (schematic net HW_VER, pulled up). */

#define GPIO_HW_VER_SENSE            (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTC|GPIO_PIN5)

/* 9 V rail: PB2 = 9V_EN on R0C, the EN pin of U12 (TPS62933, VBAT -> VDD9V,
 * 9 V / 3 A). VDD9V feeds the D-VTX connector (J13) and the video in/out
 * connectors (J9, J11). R79 (10 kOhm) pulls 9V_EN low, so the rail stays off
 * until firmware drives PB2 high. PB2 is also BOOT1, which is only sampled at
 * reset, so driving it afterwards does not affect DFU entry.
 */

#define GPIO_VDD_9V_EN               (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTB|GPIO_PIN2)

/* R0C has no USB VBUS-sense pin, so GPIO_OTGFS_VBUS is deliberately NOT defined.
 * PC1 is ESC_CURR (ADC3_IN11): using it as VBUS made the USB port disappear
 * when the ESC board was powered. board_read_VBUS_state() lives in usb.c and
 * always reports VBUS present.
 */

/* No DMA pool: its only user is the FAT DMA allocator (fat_dma_alloc), and this
 * board stores files on littlefs/NAND, so the 5120 B static pool was 5 KB of
 * heap given away for nothing. init.c skips board_dma_alloc_init() without it. */
/* #define BOARD_DMA_ALLOC_POOL_SIZE 5120 */

#define BOARD_HAS_ON_RESET 1

/* #define BOARD_ENABLE_CONSOLE_BUFFER  -- removed 2026-10-19 (RAM, option B): no dmesg */
#define BOARD_CONSOLE_BUFFER_SIZE 512

__BEGIN_DECLS

/****************************************************************************************************
 * Public Types
 ****************************************************************************************************/

/****************************************************************************************************
 * Public data
 ****************************************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/****************************************************************************************************
 * Name: stm32_spiinitialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the PX4FMU board.
 *
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);

/****************************************************************************************************
 * Name: stm32_usbinitialize
 *
 * Description:
 *   Called to configure USB IO.
 *
 ****************************************************************************************************/

extern void stm32_usbinitialize(void);

extern void board_peripheral_reset(int ms);

#include <px4_platform_common/board_common.h>

#endif /* __ASSEMBLY__ */

__END_DECLS
