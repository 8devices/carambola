/*
 *  CARAMBOLA board support
 *
 *  Copyright (C) 2011 Darius Augulis <darius@8devices.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/spi/spi.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>
#include <asm/sizes.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include "devices.h"

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition carambola_partitions[] = {
	{
		.name   = "u-boot",
		.offset = 0,
		.size   = SZ_128K + SZ_64K,
	}, {
		.name   = "u-boot-env",
		.offset = MTDPART_OFS_APPEND,
		.size   = SZ_64K,
	}, {
		.name   = "factory",
		.offset = MTDPART_OFS_APPEND,
		.size   = SZ_64K,
	}, {
		.name   = "kernel",
		.offset = SZ_256K + SZ_64K,
		.size   = SZ_1M,
	}, {
		.name   = "rootfs",
		.offset = SZ_256K + SZ_64K + SZ_1M,
		.size   = SZ_4M + SZ_2M,
	}, {
		.name   = "openwrt",
		.offset = SZ_256K + SZ_64K,
		.size   = SZ_4M + SZ_2M + SZ_1M,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data carambola_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(carambola_partitions),
	.parts		= carambola_partitions,
#endif
};


static int __init carambola_register_gpiodev(void)
{
       static struct resource res = {
               .start = 0xFFFFFFFF,
       };
       struct platform_device *pdev;

       pdev = platform_device_register_simple("GPIODEV", 0, &res, 1);
       if (!pdev) {
               printk(KERN_ERR "carambole: GPIODEV init failed\n");
               return -ENODEV;
       }

       return 0;
}

static struct i2c_gpio_platform_data carambola_i2c_gpio_data = {
	.sda_pin        = 1,
	.scl_pin        = 2,
};

static struct platform_device carambola_i2c_gpio = {
	.name           = "i2c-gpio",
	.id             = 0,
	.dev     = {
		.platform_data  = &carambola_i2c_gpio_data,
	},
};

static struct platform_device *carambola_devices[] __initdata = {
        &carambola_i2c_gpio
};

static struct spi_board_info carambola_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 0,
		.modalias	= "spidev",
	}
};

static void __init carambola_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) |
			 RT305X_GPIO_MODE_I2C);
	carambola_register_gpiodev();
	platform_add_devices(carambola_devices, ARRAY_SIZE(carambola_devices));
	rt305x_register_flash(0, &carambola_flash_data);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_spi(carambola_spi_info, ARRAY_SIZE(carambola_spi_info));
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_CARAMBOLA, "CARAMBOLA", "CARAMBOLA",
	     carambola_init);
