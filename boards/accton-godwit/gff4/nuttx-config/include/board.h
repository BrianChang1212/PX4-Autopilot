/************************************************************************************
 * nuttx-configs/accton_godwit-gff4/include/board.h
 * include/arch/board/board.h
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
 * 3. Neither the name NuttX nor the names of its contributors may be
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
 ************************************************************************************/

#ifndef __NUTTX_CONFIG_ACCTON_GODWIT_GFF4_INCLUDE_BOARD_H
#define __NUTTX_CONFIG_ACCTON_GODWIT_GFF4_INCLUDE_BOARD_H

/************************************************************************************
 * Included Files
 ************************************************************************************/
#include "board_dma_map.h"

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
# include <stdint.h>
#endif

#include "stm32_rcc.h"
#include "stm32.h"

/************************************************************************************
 * Definitions
 ************************************************************************************/

/* Clocking *************************************************************************/
/* The GFF4 board features a single 8MHz crystal (X1). This is the canonical
 * configuration, identical to other STM32F405RG PX4 boards clocked from 8MHz:
 *
 *   System Clock source           : PLL (HSE)
 *   SYSCLK(Hz)                    : 168000000    Determined by PLL configuration
 *   HCLK(Hz)                      : 168000000    (STM32_RCC_CFGR_HPRE)
 *   AHB Prescaler                 : 1            (STM32_RCC_CFGR_HPRE)
 *   APB1 Prescaler                : 4            (STM32_RCC_CFGR_PPRE1)
 *   APB2 Prescaler                : 2            (STM32_RCC_CFGR_PPRE2)
 *   HSE Frequency(Hz)             : 8000000      (STM32_BOARD_XTAL)
 *   PLLM                          : 8            (STM32_PLLCFG_PLLM)
 *   PLLN                          : 336          (STM32_PLLCFG_PLLN)
 *   PLLP                          : 2            (STM32_PLLCFG_PLLP)
 *   PLLQ                          : 7            (STM32_PLLCFG_PLLQ)
 *   Main regulator output voltage : Scale1 mode  Needed for high speed SYSCLK
 *   Flash Latency(WS)             : 5
 *   Require 48MHz for USB OTG FS  : Enabled
 */

/* HSI - 16 MHz RC factory-trimmed
 * LSI - 32 KHz RC
 * HSE - On-board crystal frequency is 8MHz
 * LSE - not fitted
 */

#define STM32_BOARD_XTAL        8000000ul

#define STM32_HSI_FREQUENCY     16000000ul
#define STM32_LSI_FREQUENCY     32000
#define STM32_HSE_FREQUENCY     STM32_BOARD_XTAL

/* Main PLL Configuration.
 *
 * PLL source is HSE
 * PLL_VCO = (STM32_HSE_FREQUENCY / PLLM) * PLLN
 *         = (8,000,000 / 8) * 336
 *         = 336,000,000
 * SYSCLK  = PLL_VCO / PLLP
 *         = 336,000,000 / 2 = 168,000,000
 * USB OTG FS Clock
 *         =  PLL_VCO / PLLQ
 *         = 48,000,000
 */

#define STM32_PLLCFG_PLLM       RCC_PLLCFG_PLLM(8)
#define STM32_PLLCFG_PLLN       RCC_PLLCFG_PLLN(336)
#define STM32_PLLCFG_PLLP       RCC_PLLCFG_PLLP_2
#define STM32_PLLCFG_PLLQ       RCC_PLLCFG_PLLQ(7)

#define STM32_SYSCLK_FREQUENCY  168000000ul

/* AHB clock (HCLK) is SYSCLK (168MHz) */

#define STM32_RCC_CFGR_HPRE     RCC_CFGR_HPRE_SYSCLK  /* HCLK  = SYSCLK / 1 */
#define STM32_HCLK_FREQUENCY    STM32_SYSCLK_FREQUENCY
#define STM32_BOARD_HCLK        STM32_HCLK_FREQUENCY  /* same as above, to satisfy compiler */

/* APB1 clock (PCLK1) is HCLK/4 (42MHz) */

#define STM32_RCC_CFGR_PPRE1    RCC_CFGR_PPRE1_HCLKd4     /* PCLK1 = HCLK / 4 */
#define STM32_PCLK1_FREQUENCY   (STM32_HCLK_FREQUENCY/4)

/* Timers driven from APB1 will be twice PCLK1 */

#define STM32_APB1_TIM2_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM3_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM4_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM5_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM6_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM7_CLKIN   (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM12_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM13_CLKIN  (2*STM32_PCLK1_FREQUENCY)
#define STM32_APB1_TIM14_CLKIN  (2*STM32_PCLK1_FREQUENCY)

/* APB2 clock (PCLK2) is HCLK/2 (84MHz) */

#define STM32_RCC_CFGR_PPRE2    RCC_CFGR_PPRE2_HCLKd2     /* PCLK2 = HCLK / 2 */
#define STM32_PCLK2_FREQUENCY   (STM32_HCLK_FREQUENCY/2)

/* Timers driven from APB2 will be twice PCLK2 */

#define STM32_APB2_TIM1_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM8_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM9_CLKIN   (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM10_CLKIN  (2*STM32_PCLK2_FREQUENCY)
#define STM32_APB2_TIM11_CLKIN  (2*STM32_PCLK2_FREQUENCY)

/* Timer Frequencies, if APBx is set to 1, frequency is same to APBx
 * otherwise frequency is 2xAPBx.
 * Note: TIM1,8-11 are on APB2, others on APB1
 */

#define BOARD_TIM1_FREQUENCY    STM32_APB2_TIM1_CLKIN
#define BOARD_TIM2_FREQUENCY    STM32_APB1_TIM2_CLKIN
#define BOARD_TIM3_FREQUENCY    STM32_APB1_TIM3_CLKIN
#define BOARD_TIM4_FREQUENCY    STM32_APB1_TIM4_CLKIN
#define BOARD_TIM5_FREQUENCY    STM32_APB1_TIM5_CLKIN
#define BOARD_TIM6_FREQUENCY    STM32_APB1_TIM6_CLKIN
#define BOARD_TIM7_FREQUENCY    STM32_APB1_TIM7_CLKIN
#define BOARD_TIM8_FREQUENCY    STM32_APB2_TIM8_CLKIN
#define BOARD_TIM9_FREQUENCY    STM32_APB2_TIM9_CLKIN
#define BOARD_TIM10_FREQUENCY   STM32_APB2_TIM10_CLKIN
#define BOARD_TIM11_FREQUENCY   STM32_APB2_TIM11_CLKIN
#define BOARD_TIM12_FREQUENCY   STM32_APB1_TIM12_CLKIN
#define BOARD_TIM13_FREQUENCY   STM32_APB1_TIM13_CLKIN
#define BOARD_TIM14_FREQUENCY   STM32_APB1_TIM14_CLKIN

/* LED definitions ********************************************************************/
/* board_autoled_on()/board_autoled_off() (led.c) are called unconditionally by NuttX
 * core code (e.g. stm32_allocateheap.c) regardless of CONFIG_ARCH_LEDS, so these
 * index constants must always be defined.
 */

#define LED_STARTED       0
#define LED_HEAPALLOCATE  1
#define LED_IRQSENABLED   2
#define LED_STACKCREATED  3
#define LED_INIRQ         4
#define LED_SIGNAL        5
#define LED_ASSERTION     6
#define LED_PANIC         7

/* Alternate function pin selections ************************************************/

/* USART1 (/dev/ttyS0): Main ESC connector J5 pin 6 (UART1_RX), the
 * Betaflight-defined ESC telemetry input. Not the RC input on R0C: SBUS moved
 * to UART5 (see below).
 *
 * PA10 (RX), PA9 (TX, test point TP7 only)
 */

#define GPIO_USART1_RX  GPIO_USART1_RX_1
#define GPIO_USART1_TX  GPIO_USART1_TX_1

/* USART2 (/dev/ttyS1, GPS1): GPS connector J4 (J4 also carries I2C1 for an
 * external compass).
 *
 * PA2 (TX), PA3 (RX)
 */

#define GPIO_USART2_RX  GPIO_USART2_RX_1
#define GPIO_USART2_TX  GPIO_USART2_TX_1

/* USART3 (/dev/ttyS2, TEL1): TELEM connector J3. The same pins are also routed
 * to the D-VTX / DJI connector J13 (pins 3/4), so only one of the two can be
 * used at a time.
 *
 * PB10 (TX), PB11 (RX)
 */

#define GPIO_USART3_RX  GPIO_USART3_RX_1
#define GPIO_USART3_TX  GPIO_USART3_TX_1

/* UART4 (/dev/ttyS3): ELRS connector J6 on R0C. The console and fpv builds use
 * it as the NSH console (57600 8N1), so an ELRS receiver cannot be attached
 * while that is the case.
 *
 * PA0 (TX), PA1 (RX)
 */

#define GPIO_UART4_RX   GPIO_UART4_RX_1
#define GPIO_UART4_TX   GPIO_UART4_TX_1

/* UART5: SBUS input on R0C. SBUS (J7 / J13 pin 6) is inverted by Q2 and lands on
 * UART5_RX (PD2). STM32F405 only has one UART5 pin option (PD2/PC12), so no _N
 * suffix exists to select; GPIO_UART5_RX already defaults to PD2. TX is not
 * routed out (PC12 is SPI3_MOSI), so override it to 0.
 */

#define GPIO_UART5_TX   0

/* SPI1: ICM-42688-P
 *  CS: PA4 -- configured in board_config.h
 *  CLK: PA5
 *  MISO: PA6
 *  MOSI: PA7
 */

#define GPIO_SPI1_SCK   GPIO_SPI1_SCK_1
#define GPIO_SPI1_MISO  GPIO_SPI1_MISO_1
#define GPIO_SPI1_MOSI  GPIO_SPI1_MOSI_1

/* SPI2: AT7456E OSD (DNP option)
 *  CS: PB12 -- configured in board_config.h
 *  CLK: PB13
 *  MISO: PC2
 *  MOSI: PC3
 */

#define GPIO_SPI2_SCK   GPIO_SPI2_SCK_2
#define GPIO_SPI2_MISO  GPIO_SPI2_MISO_2
#define GPIO_SPI2_MOSI  GPIO_SPI2_MOSI_2

/* SPI3: MX35LF2GE4AD SPI NAND (blackbox log store)
 *  CS: PA15 -- configured in board_config.h
 *  CLK: PC10
 *  MISO: PC11
 *  MOSI: PC12
 */

#define GPIO_SPI3_SCK   GPIO_SPI3_SCK_2
#define GPIO_SPI3_MISO  GPIO_SPI3_MISO_2
#define GPIO_SPI3_MOSI  GPIO_SPI3_MOSI_2

/* I2C1: DPS368 baro, IST8310 mag (DNP option), extension connector
 *
 * SCL: PB6
 * SDA: PB7
 */

#define GPIO_I2C1_SCL   GPIO_I2C1_SCL_1
#define GPIO_I2C1_SDA   GPIO_I2C1_SDA_1

/* CAN1: extension connector via TJA1051 transceiver
 *
 * RX: PB8
 * TX: PB9
 */

#define GPIO_CAN1_RX    GPIO_CAN1_RX_2
#define GPIO_CAN1_TX    GPIO_CAN1_TX_2

#endif  /* __NUTTX_CONFIG_ACCTON_GODWIT_GFF4_INCLUDE_BOARD_H */
