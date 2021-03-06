/*
 * Copyright (C) 2009-2013 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 *
 * Under GPLv2
  */

#define __LOWLEVEL_INIT__

#include <common.h>
#include <asm/system.h>
#include <asm/barebox-arm.h>
#include <asm/barebox-arm-head.h>
#include <mach/hardware.h>
#include <mach/io.h>
#include <mach/at91sam9_ddrsdr.h>
#include <init.h>
#include <sizes.h>

void __naked __bare_init reset(void)
{
	common_reset();

	arm_setup_stack(AT91SAM9G45_SRAM_BASE + AT91SAM9G45_SRAM_SIZE - 16);

	barebox_arm_entry(AT91_CHIPSELECT_6, at91sam9g45_get_ddram_size(0), 0);	//390 PKM new platfrom
}
