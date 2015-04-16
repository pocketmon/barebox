#include <common.h>
#include <sizes.h>
#include <asm/barebox-arm-head.h>
#include <asm/barebox-arm.h>

void __naked reset(void)
{
	common_reset();
	barebox_arm_entry(0xa0000000, SZ_64M, 0);
}
