/*
 * (C) Copyright 2002-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/**
 * @file
 * @brief Main entry into the C part of barebox
 */
#include <common.h>
#include <init.h>
#include <command.h>
#include <malloc.h>
#include <debug_ll.h>
#include <fs.h>
#include <linux/stat.h>
#include <envfs.h>
#include <asm/sections.h>
#include <uncompress.h>
#include <custom.h>	//BT-8474

extern initcall_t __barebox_initcalls_start[], __barebox_early_initcalls_end[],
		  __barebox_initcalls_end[];

// <<390 PKM
#define SECURE_LIVE
#ifndef SECURE_SEED
#define SECURE_SEED(s)	(s)
#endif
#ifndef SEED_VAL
#define SEED_VAL	1
#endif
// 390 PKM>>


#ifdef CONFIG_DEFAULT_ENVIRONMENT
#include "barebox_default_env.h"

static int register_default_env(void)
{
	int ret;
	void *defaultenv;

	if (IS_ENABLED(CONFIG_DEFAULT_ENVIRONMENT_COMPRESSED)) {
		void *tmp = malloc(default_environment_size);

		if (!tmp)
			return -ENOMEM;

		memcpy(tmp, default_environment, default_environment_size);

		defaultenv = xzalloc(default_environment_uncompress_size);

		ret = uncompress(tmp, default_environment_size,
				NULL, NULL,
				defaultenv, NULL, uncompress_err_stdout);

		free(tmp);

		if (ret) {
			free(defaultenv);
			return ret;
		}
	} else {
		defaultenv = (void *)default_environment;
	}


	add_mem_device("defaultenv", (unsigned long)defaultenv,
		       default_environment_uncompress_size,
		       IORESOURCE_MEM_WRITEABLE);
	return 0;
}

device_initcall(register_default_env);
#endif

#if defined CONFIG_FS_RAMFS && defined CONFIG_FS_DEVFS
static int mount_root(void)
{
	mount("none", "ramfs", "/");
	mkdir("/dev", 0);
	mount("none", "devfs", "/dev");
	return 0;
}
fs_initcall(mount_root);
#endif

int (*barebox_main)(void);

void __noreturn start_barebox(void)
{
	initcall_t *initcall;
	int result;
	struct stat s;
	int abort = 0;
	int i = 0;

	if (!IS_ENABLED(CONFIG_SHELL_NONE))
		barebox_main = run_shell;

	for (initcall = __barebox_initcalls_start;
			initcall < __barebox_initcalls_end; initcall++) {
		debug("initcall-> %pS\n", *initcall);
		result = (*initcall)();
		if (result)
			pr_err("initcall %pS failed: %s\n", *initcall,
					strerror(-result));
	}

	debug("initcalls done\n");

	if (IS_ENABLED(CONFIG_ENV_HANDLING)) {
		int ret;

		ret = envfs_load(default_environment_path, "/env", 0);

		if (ret && IS_ENABLED(CONFIG_DEFAULT_ENVIRONMENT)) {
			printf("no valid environment found on %s. "
				"Using default environment\n",
				default_environment_path);
			envfs_load("/dev/defaultenv", "/env", 0);
		}
	}


// <<390 PKM port secure procedures to protect barebox/uboot setting
#ifdef SECURE_LIVE
	char ch = 0;
	// <<BT8474 remove unnecessary info at console, need to add delay to read input
	for (i=0; i<30; i++)
	{
		if (tstc()) {	/* we got a key press	*/
			ch = getc();  /* consume input	*/
			if (ch == '.')		//390 PKM some PCB detect key press during startup, need one more checking to fix this problem
			{
				puts ("\b\b\b 0");
				abort = 1;	/* don't auto boot	*/
				break;
			}
		}
		mdelay(1);
// KH Hack
//#ifdef URP2
//		puts ("\nURP2 => stop Watchdog\n");
//		stop_watchdog();
//#endif
	}
	// BT-8474>>
	// U-BOOT Secure Code,
	// Inherit from URP
	if ( abort ) {
		unsigned int seed=SEED_VAL, res, inp=0;
		int len = 0;
		char c[16];

		while ( len < 8 ) {
			if (tstc()) {   /* we got a key press   */
				ch = getc();
				if ( ch != res ) {
					len++;
					seed *= ch;
					res = ch;
					if ( (seed %3) == 0 )
						len--;
				}
			} else {
				seed ++;
			}
		}
		res = SECURE_SEED(seed);
		printf("%u.", seed);
		while ( 1 ) {
			if (tstc()) {   /* we got a key press   */
				ch = (char) getc();  /* consume input   */
				if ( ch=='e' )
					break;  /* skip preceeding returns */
			}
		}
		len = 0;
		while ( len < 4 ) {
			if (tstc()) {   /* we got a key press   */
				inp *= 10;
				inp += (getc()-'0');
				len++;
			}
		}
		if ( res != inp )
			while ( 1 );
	}
#endif
// 390 PKM>>


	if (IS_ENABLED(CONFIG_COMMAND_SUPPORT)) {
		// BT-8474 390 PKM remove unnecessary information in console
		//printf("running /env/bin/init...\n");

		if (!stat("/env/bin/init", &s)) {
			run_command("source /env/bin/init", 0);
		} else {
			printf("not found\n");
		}
	}

	if (abort == 0)
	{
		if (!stat("/env/bin/boot_board", &s)) {
			run_command("source /env/bin/boot_board", 0);
		} else {
			printf("boot_board not found\n");
		}
	}

	if (!barebox_main) {
		printf("No main function! aborting.\n");
		hang();
	}

	/* main_loop() can return to retry autoboot, if so just run it again. */
	for (;;)
		barebox_main();

	/* NOTREACHED - no way out of command loop except booting */
}

void __noreturn hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}

/* Everything needed to cleanly shutdown barebox.
 * Should be called before starting an OS to get
 * the devices into a clean state
 */
void shutdown_barebox(void)
{
	devices_shutdown();
#ifdef ARCH_SHUTDOWN
	arch_shutdown();
#endif
}
