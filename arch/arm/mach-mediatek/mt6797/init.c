// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Marek Vasut <marek.vasut@gmail.com>
 */

#include <common.h>
#include <clk.h>
#include <fdtdec.h>
#include <asm/armv8/mmu.h>
#include <asm/sections.h>
#include <linux/io.h>
#include <dt-bindings/clock/mt6797-clk.h>

DECLARE_GLOBAL_DATA_PTR;

#define WDOG_RESTART		0x10007008
#define WDOG_RESTART_KEY	0x1971
#define WDOG_SWRST		0x10007014
#define WDOG_SWRST_KEY		0x1209

int dram_init(void)
{
	int ret;

	ret = fdtdec_setup_memory_banksize();
	if (ret)
		return ret;

	return fdtdec_setup_mem_size_base();
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = gd->ram_base;
	gd->bd->bi_dram[0].size = gd->ram_size;

	return 0;
}

int mtk_soc_early_init(void)
{
	return 0;
}

void reset_cpu(ulong addr)
{
	writel(WDOG_RESTART_KEY, WDOG_RESTART);
	writel(WDOG_SWRST_KEY, WDOG_SWRST);
	hang();
}

int print_cpuinfo(void)
{
	printf("CPU:   MediaTek MT6797\n");
	return 0;
}

static struct mm_region mt6797_mem_map[] = {
	{
		/* DDR */
		.virt = 0x40000000UL,
		.phys = 0x40000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE,
	}, {
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x20000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		0,
	}
};
struct mm_region *mem_map = mt6797_mem_map;
