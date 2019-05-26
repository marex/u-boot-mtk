// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 MediaTek Inc.
 */

#include <common.h>
#include <asm/io.h>

#define WDOG_MODE		0x10007000

DECLARE_GLOBAL_DATA_PTR;

#define FUSB301_I2C_ADDR	0x25

static u8 gemini_fusb_301_set_host_read(void *base, u8 reg)
{
	writel(0x38, base + 0x10);
	writel(FUSB301_I2C_ADDR << 1, base + 0x04);
	writel(0xffffffff, base + 0x0c);
	writel(1, base + 0x38);
	writel(0xf8, base + 0x08);
	writel(0x1, base + 0x14);	/* 1 B outbound xfer */
	writel(0x1, base + 0x6c);	/* 1 B inbound xfer */
	writel(0x2, base + 0x18);	/* 2 xfers in total */
	writel(reg, base + 0x00);
	writel(0x1, base + 0x24);
	mdelay(10);
	readl(base + 0x30);
	return readl(base + 0x00) & 0xff;
}

static void gemini_fusb_301_set_host_write(void *base, u8 reg, u8 val)
{
	writel(0x38, base + 0x10);
	writel(FUSB301_I2C_ADDR << 1, base + 0x04);
	writel(0xffffffff, base + 0x0c);
	writel(1, base + 0x38);
	writel(0xf8, base + 0x08);
	writel(0x2, base + 0x14);	/* 2 B outbound xfer */
	writel(0x0, base + 0x6c);	/* 0 B inbound xfer */
	writel(0x1, base + 0x18);	/* 1 xfers in total */
	writel(reg, base + 0x00);
	writel(val, base + 0x00);
	writel(0x1, base + 0x24);
	mdelay(10);
	readl(base + 0x30);
}

static void gemini_fusb_301_dump_regs(void *i2c)
{
	printf("  TUSB301 (%p) Device ID = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x1));
	printf("  TUSB301 (%p) Modes     = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x2));
	printf("  TUSB301 (%p) Manual    = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x3));
	printf("  TUSB301 (%p) Reset     = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x4));
	printf("  TUSB301 (%p) Mask      = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x10));
	printf("  TUSB301 (%p) Status    = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x11));
	printf("  TUSB301 (%p) Type      = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x12));
	printf("  TUSB301 (%p) Interrupt = %02x\n", i2c, gemini_fusb_301_set_host_read(i2c, 0x13));
}

static void gemini_fusb_301_set_host(void)
{
	void *i2c0 = (void *)0x11007000;
	void *i2c1 = (void *)0x11008000;

	/*
	 * Fiddle with I2C0 and I2C1 registers, send I2C write to
	 * FUSB301 controlling the USB-C plug on both sides of the
	 * device, set the FUSB301 to Source mode instead of Sink
	 * mode and unblock USB host access.
	 */
	gemini_fusb_301_set_host_write(i2c0, 0x2, 0x1);
	gemini_fusb_301_dump_regs(i2c0);
	gemini_fusb_301_set_host_write(i2c1, 0x2, 0x1);
	gemini_fusb_301_dump_regs(i2c1);
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	/* Stop the WDT */
	writel(0x2200005c, WDOG_MODE);

	/* FIXME: Implement I2C driver and FUSB301 driver */
	gemini_fusb_301_set_host();

	return 0;
}
