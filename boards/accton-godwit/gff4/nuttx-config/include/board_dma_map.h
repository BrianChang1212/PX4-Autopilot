/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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

#pragma once

/*
 * DMA1
 *   Stream 0, Channel 0: SPI3_RX   (NAND flash read, DMACHAN_SPI3_RX)
 *   Stream 2, Channel 5: TIM3_UP   (DSHOT burst, PWM5-8 header)
 *   Stream 5, Channel 0: SPI3_TX   (NAND flash write, DMACHAN_SPI3_TX)
 *
 * DMA2
 *   Stream 0, Channel 3: SPI1_RX   (IMU, DMACHAN_SPI1_RX)
 *   Stream 1, Channel 7: TIM8_UP   (DSHOT burst, PWM1-4)
 *   Stream 2, Channel 4: USART1_RX (SBUS, DMAMAP_USART1_RX)
 *   Stream 3, Channel 3: SPI1_TX   (IMU, DMACHAN_SPI1_TX)
 *
 * SPI2 (OSD, low bandwidth) and the remaining UARTs are not DMA-driven.
 */

#define DMACHAN_SPI3_RX    DMAMAP_SPI3_RX_1   /* DMA1, Stream 0, Channel 0 */
#define DMACHAN_SPI3_TX    DMAMAP_SPI3_TX_1   /* DMA1, Stream 5, Channel 0 */

#define DMACHAN_SPI1_RX    DMAMAP_SPI1_RX_1   /* DMA2, Stream 0, Channel 3 */
#define DMAMAP_USART1_RX   DMAMAP_USART1_RX_1 /* DMA2, Stream 2, Channel 4 */
#define DMACHAN_SPI1_TX    DMAMAP_SPI1_TX_1   /* DMA2, Stream 3, Channel 3 */
