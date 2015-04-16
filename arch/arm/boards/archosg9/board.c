/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <init.h>
#include <ns16550.h>
#include <asm/armlinux.h>
#include <generated/mach-types.h>
#include <mach/omap4-silicon.h>
#include <mach/omap4-devices.h>
#include <sizes.h>
#include <i2c/i2c.h>
#include <gpio.h>
#include "archos_features.h"

static int archosg9_console_init(void){
	if (IS_ENABLED(CONFIG_DRIVER_SERIAL_OMAP4_USBBOOT))
		add_generic_device("serial_omap4_usbboot", DEVICE_ID_DYNAMIC
			, NULL, 0, 0, 0, NULL);
	if (IS_ENABLED(CONFIG_DRIVER_SERIAL_NS16550)) {
		gpio_direction_output(41, 0); /* gps_disable */
		gpio_direction_output(34, 1); /* 1v8_pwron */
		omap44xx_add_uart1();
	}
	return 0;
}
console_initcall(archosg9_console_init);

static int archosg9_mem_init(void){
	omap_add_ram0(SZ_1G);
	return 0;
}
mem_initcall(archosg9_mem_init);

static struct i2c_board_info i2c_devices[] = {
	{ I2C_BOARD_INFO("twl6030", 0x48), },
};

static int archosg9_devices_init(void){
	i2c_register_board_info(0, i2c_devices, ARRAY_SIZE(i2c_devices));
	omap44xx_add_i2c1(NULL);
	omap44xx_add_mmc1(NULL);
	omap44xx_add_mmc2(NULL);

	armlinux_set_bootparams((void *)0x80000100);
	/*
	 * This should be:
	 * armlinux_set_architecture(MACH_TYPE_OMAP4_ARCHOSG9);
	 * But Archos has not registered it's board to arch/arm/tools/mach-types
	 * So here there is the hardcoded value
	 */
	armlinux_set_architecture(5032);
	armlinux_set_revision(5);
	armlinux_set_atag_appender(archos_append_atags);

	return 0;
}
device_initcall(archosg9_devices_init);
