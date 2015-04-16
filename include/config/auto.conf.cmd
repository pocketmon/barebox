deps_config := \
	crypto/Kconfig \
	lib/bootstrap/Kconfig \
	lib/gui/Kconfig \
	lib/lzo/Kconfig \
	lib/Kconfig \
	fs/fat/Kconfig \
	fs/ext4/Kconfig \
	fs/Kconfig \
	drivers/w1/slaves/Kconfig \
	drivers/w1/masters/Kconfig \
	drivers/w1/Kconfig \
	drivers/of/Kconfig \
	drivers/gpio/Kconfig \
	drivers/dma/Kconfig \
	drivers/pwm/Kconfig \
	drivers/watchdog/Kconfig \
	drivers/input/Kconfig \
	drivers/eeprom/Kconfig \
	drivers/led/Kconfig \
	drivers/misc/Kconfig \
	drivers/mfd/Kconfig \
	drivers/clocksource/Kconfig \
	drivers/clk/Kconfig \
	drivers/mci/Kconfig \
	drivers/video/Kconfig \
	drivers/usb/gadget/Kconfig \
	drivers/usb/storage/Kconfig \
	drivers/usb/otg/Kconfig \
	drivers/usb/host/Kconfig \
	drivers/usb/imx/Kconfig \
	drivers/usb/Kconfig \
	drivers/ata/Kconfig \
	drivers/mtd/ubi/Kconfig \
	drivers/mtd/nand/Kconfig \
	drivers/mtd/devices/Kconfig \
	drivers/mtd/Kconfig \
	drivers/nor/Kconfig \
	drivers/i2c/busses/Kconfig \
	drivers/i2c/algos/Kconfig \
	drivers/i2c/Kconfig \
	drivers/spi/Kconfig \
	drivers/net/usb/Kconfig \
	drivers/net/phy/Kconfig \
	drivers/net/Kconfig \
	drivers/serial/Kconfig \
	drivers/Kconfig \
	net/Kconfig \
	commands/Kconfig \
	pbl/Kconfig \
	common/Kconfig \
	arch/arm/boards/toshiba-ac100/Kconfig \
	arch/arm/mach-tegra/Kconfig \
	arch/arm/boards/versatile/Kconfig \
	arch/arm/mach-versatile/Kconfig \
	arch/arm/boards/friendlyarm-tiny6410/Kconfig \
	arch/arm/boards/friendlyarm-mini2440/Kconfig \
	arch/arm/mach-samsung/Kconfig \
	arch/arm/mach-pxa/Kconfig \
	arch/arm/mach-omap/Kconfig \
	arch/arm/mach-nomadik/Kconfig \
	arch/arm/mach-netx/Kconfig \
	arch/arm/mach-mxs/Kconfig \
	arch/arm/mach-imx/Kconfig \
	arch/arm/mach-ep93xx/Kconfig \
	arch/arm/mach-clps711x/Kconfig \
	arch/arm/mach-bcm2835/Kconfig \
	arch/arm/mach-at91/Kconfig \
	arch/arm/cpu/Kconfig \
	arch/arm/Kconfig \
	Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(KERNELVERSION)" "2013.02.0"
include/config/auto.conf: FORCE
endif
ifneq "$(ARCH)" "arm"
include/config/auto.conf: FORCE
endif
ifneq "$(SRCARCH)" "arm"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
