/*
 * Copyright (C) 2012 Alexander Shiyan <shc_work@mail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <common.h>

extern void start(void);

void __noreturn reset_cpu(unsigned long addr)
{
	arch_shutdown();

	asm("mov pc, #0");

	hang();
}
