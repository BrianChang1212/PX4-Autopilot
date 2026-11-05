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
 * @file init.c
 *
 * Accton Godwit GFF4 specific early startup code. This file implements the
 * board_app_initialize() function that is called early by nsh during startup.
 *
 * Code here is run before the rcS script is invoked; it should start required
 * subsystems and perform board-specific initialization.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "board_config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/spi/spi.h>
#include <nuttx/mtd/mtd.h>
#include <nuttx/fs/fs.h>

#include <stm32.h>
#include <arch/board/board.h>

#include <drivers/drv_hrt.h>
#include <drivers/drv_board_led.h>

#include <systemlib/px4_macros.h>

#include <px4_arch/io_timer.h>
#include <px4_platform_common/init.h>
#include <px4_platform/board_dma_alloc.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
extern void led_init(void);
extern void led_on(int led);
extern void led_off(int led);
__END_DECLS

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/************************************************************************************
 * Name: board_peripheral_reset
 ************************************************************************************/
__EXPORT void board_peripheral_reset(int ms)
{
	UNUSED(ms);
}

/************************************************************************************
 * Name: board_on_reset
 *
 * Description:
 * Optionally provided function called on entry to board_system_reset
 * It should perform any house keeping prior to the rest.
 *
 * status - 1 if resetting to boot loader
 *          0 if just resetting
 *
 ************************************************************************************/
__EXPORT void board_on_reset(int status)
{
	/* configure the GPIO pins to outputs and keep them low */
	for (int i = 0; i < DIRECT_PWM_OUTPUT_CHANNELS; ++i) {
		px4_arch_configgpio(io_timer_channel_get_gpio_output(i));
	}

	/* On resets invoked from system (not boot) insure we establish a low
	 * output state (discharge the pins) on PWM pins before they become inputs.
	 */

	if (status >= 0) {
		up_mdelay(400);
	}
}

/************************************************************************************
 * Name: stm32_boardinitialize
 *
 * Description:
 *   All STM32 architectures must provide the following entry point.  This entry point
 *   is called early in the initialization -- after all memory has been configured
 *   and mapped but before any devices have been initialized.
 *
 ************************************************************************************/

__EXPORT void
stm32_boardinitialize(void)
{
	/* Reset all PWM to Low outputs */

	board_on_reset(-1);

	/* configure LEDs */
	board_autoled_initialize();

	/* configure the HW revision strap */
	stm32_configgpio(GPIO_HW_VER_SENSE);

	/* power the 9 V rail for the video transmitter (product ships with one) */
	stm32_configgpio(GPIO_VDD_9V_EN);

	/* configure SPI chip select / DRDY GPIOs for all buses */

	stm32_spiinitialize();

	/* configure USB interfaces */

	stm32_usbinitialize();
}

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Perform application specific initialization.  This function is never
 *   called directly from application code, but only indirectly via the
 *   (non-standard) boardctl() interface using the command BOARDIOC_INIT.
 *
 * Input Parameters:
 *   arg - The boardctl() argument is passed to the board_app_initialize()
 *         implementation without modification.  The argument has no
 *         meaning to NuttX; the meaning of the argument is a contract
 *         between the board-specific initalization logic and the the
 *         matching application logic.  The value cold be such things as a
 *         mode enumeration value, a set of DIP switch switch settings, a
 *         pointer to configuration data read from a file or serial FLASH,
 *         or whatever you would like to do with it.  Every implementation
 *         should accept zero/NULL as a default configuration.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure to indicate the nature of the failure.
 *
 ****************************************************************************/

__EXPORT int board_app_initialize(uintptr_t arg)
{
	px4_platform_init();

#if !defined(CONFIG_DEV_CONSOLE)
	/* Without a console NuttX leaves stdin/stdout/stderr (fd 0..2) closed.
	 * The first files nsh opens (rcS and the scripts it sources) would then
	 * take those numbers and every task started by rcS would inherit a script
	 * file as its stdio, and the MAVLink shell (which dup()s fd 0..2) cannot
	 * start at all. Point fd 0..2 at /dev/null so they are always valid.
	 */
	{
		int nullfd = open("/dev/null", O_RDWR);

		if (nullfd >= 0) {
			for (int i = 0; i < 3; i++) {
				if (nullfd != i) {
					dup2(nullfd, i);
				}
			}

			if (nullfd > 2) {
				close(nullfd);
			}
		}
	}
#endif

	/* configure the DMA allocator */

#if defined(BOARD_DMA_ALLOC_POOL_SIZE)
	if (board_dma_alloc_init() < 0) {
		syslog(LOG_ERR, "[boot] DMA alloc FAILED\n");
	}
#endif

#if defined(SERIAL_HAVE_RXDMA)
	// set up the serial DMA polling at 1ms intervals for received bytes that have not triggered a DMA event.
	static struct hrt_call serial_dma_call;
	hrt_call_every(&serial_dma_call, 1000, 1000, (hrt_callout)stm32_serial_dma_poll, NULL);
#endif

	/* initial LED state */
	drv_led_start();
	led_off(LED_RED);
	led_off(LED_GREEN);
	led_off(LED_BLUE);

	if (board_hardfault_init(2, true) != 0) {
		led_on(LED_RED);
	}

	/* Initialize the MX35LF2GE4AD NAND flash on SPI3 and mount littlefs on
	 * it as the blackbox log store -- this board has no SD card.
	 */

#if !defined(GFF4_DIAG_SKIP_NAND)
	struct spi_dev_s *spi3 = stm32_spibus_initialize(3);

	if (!spi3) {
		syslog(LOG_ERR, "[boot] FAILED to initialize SPI3 for MX35LF2GE4AD\n");
		led_on(LED_RED);

	} else {
		struct mtd_dev_s *mtd = mx35_initialize(spi3);

		if (!mtd) {
			syslog(LOG_ERR, "[boot] FAILED to initialize MX35LF2GE4AD MTD driver\n");
			led_on(LED_RED);

		} else {
			int ret = register_mtddriver("/dev/mtd0", mtd, 0755, NULL);

			if (ret < 0) {
				syslog(LOG_ERR, "[boot] FAILED to register MTD driver: %d\n", ret);
				led_on(LED_RED);

			} else {
				syslog(LOG_INFO, "[boot] MX35LF2GE4AD MTD registered at /dev/mtd0\n");

#ifdef CONFIG_FS_LITTLEFS
				ret = nx_mount("/dev/mtd0", CONFIG_BOARD_ROOT_PATH, "littlefs", 0, NULL);

				if (ret < 0) {
					/* Nothing mounted, so nothing to lose: a blank or corrupt
					 * volume gets formatted.
					 */
					ret = nx_mount("/dev/mtd0", CONFIG_BOARD_ROOT_PATH, "littlefs", 0, "forceformat");

				} else {
					/* Mounted. Check it is writable, but do NOT reformat if it
					 * is not: the parameter store lives on this volume, and a
					 * volume that mounts is very likely still readable (it may
					 * just be full). Losing the log space beats silently losing
					 * every parameter and calibration on the vehicle.
					 */
					int fd = open(CONFIG_BOARD_ROOT_PATH "/.mount_test", O_CREAT | O_WRONLY | O_TRUNC);

					if (fd >= 0) {
						close(fd);
						unlink(CONFIG_BOARD_ROOT_PATH "/.mount_test");

					} else {
						syslog(LOG_ERR,
						       "[boot] littlefs mounted read-only or full: logging disabled, parameters may not persist\n");
						led_on(LED_RED);
					}
				}

				if (ret < 0) {
					syslog(LOG_ERR, "[boot] FAILED to mount littlefs: %d\n", ret);
					led_on(LED_RED);

				} else {
					syslog(LOG_INFO, "[boot] LittleFS mounted at %s\n", CONFIG_BOARD_ROOT_PATH);
				}

#endif
			}
		}
	}

#else
	syslog(LOG_WARNING, "[boot] GFF4_DIAG_SKIP_NAND: NAND/littlefs init skipped\n");
#endif

	/* Parameters are a file on the NAND littlefs (CONFIG_BOARD_PARAM_FILE),
	 * so there is no flash parameter store to bring up here.
	 */

	/* Configure the HW based on the manifest */

	px4_platform_configure();

	return OK;
}
