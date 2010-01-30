/*
 * Samsung Laptop Backlight driver
 *
 * Copyright (C) 2009 Greg Kroah-Hartman (gregkh@suse.de)
 * Copyright (C) 2009 Novell Inc.
 * Modified 2010 Kobelkov Sergey (sergeyko81@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/backlight.h>
#include <linux/fb.h>
#include <linux/dmi.h>

#define MAX_BRIGHT	0xff
#define OFFSET		0xf4

static int offset = OFFSET;
module_param(offset, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(offset, "The offset into the PCI device for the brightness control");

static struct pci_dev *pci_device;
static struct backlight_device *backlight_device;

static u8 read_brightness(void)
{
	u8 brightness;

	pci_read_config_byte(pci_device, offset, &brightness);
	return brightness;
}

static void set_brightness(u8 brightness)
{
	pci_write_config_byte(pci_device, offset, brightness);
}

static int get_brightness(struct backlight_device *bd)
{
	return bd->props.brightness;
}

static int update_status(struct backlight_device *bd)
{
	set_brightness(bd->props.brightness);
	return 0;
}

static struct backlight_ops backlight_ops = {
	.get_brightness	= get_brightness,
	.update_status	= update_status,
};

static int __init dmi_check_cb(const struct dmi_system_id *id)
{
	printk(KERN_INFO KBUILD_MODNAME ": found laptop model '%s'\n",
		id->ident);
	return 0;
}

static struct dmi_system_id __initdata samsung_dmi_table[] = {
	{
		.ident = "N120",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "N120"),
			DMI_MATCH(DMI_BOARD_NAME, "N120"),
		},
		.callback = dmi_check_cb,
	},
	{
		.ident = "N130",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "N130"),
			DMI_MATCH(DMI_BOARD_NAME, "N130"),
		},
		.callback = dmi_check_cb,
	},
	{
		.ident = "NC10",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "NC10"),
			DMI_MATCH(DMI_BOARD_NAME, "NC10"),
		},
		.callback = dmi_check_cb,
	},
	{
		.ident = "X360",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "X360"),
			DMI_MATCH(DMI_BOARD_NAME, "X360"),
		},
		.callback = dmi_check_cb,
	},
	{
		.ident = "R518",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "R518"),
			DMI_MATCH(DMI_BOARD_NAME, "R518"),
		},
		.callback = dmi_check_cb,
	},
	{
		.ident = "NP-Q45",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "SAMSUNG ELECTRONICS CO., LTD."),
			DMI_MATCH(DMI_PRODUCT_NAME, "SQ45S70S"),
			DMI_MATCH(DMI_BOARD_NAME, "SQ45S70S"),
		},
		.callback = dmi_check_cb,
	},
	{ },
};

static int __init samsung_init(void)
{
	if (!dmi_check_system(samsung_dmi_table))
		return -ENODEV;

	/*
	 * The Samsung N120, N130, and NC10 use pci device id 0x27ae, while the
	 * NP-Q45 uses 0x2a02.  Odds are we might need to add more to the
	 * list over time...
	 */
	pci_device = pci_get_device(PCI_VENDOR_ID_INTEL, 0x27ae, NULL);
        if(!pci_device) 
	  pci_device = pci_get_device(PCI_VENDOR_ID_INTEL, 0x2a02, NULL); // NP-Q45
        if(!pci_device) 
	  pci_device = pci_get_device(PCI_VENDOR_ID_INTEL, 0x2a42, NULL); // samsung X360, R518
	if (!pci_device)
          return -ENODEV;
 
        /* create a backlight device to talk to this one */
	backlight_device = backlight_device_register("samsung",
						     &pci_device->dev,
						     NULL, &backlight_ops);
	if (IS_ERR(backlight_device)) {
		pci_dev_put(pci_device);
		return PTR_ERR(backlight_device);
	}

	backlight_device->props.max_brightness = MAX_BRIGHT;
	backlight_device->props.brightness = read_brightness();
	backlight_device->props.power = FB_BLANK_UNBLANK;
	backlight_update_status(backlight_device);

	return 0;
}

static void __exit samsung_exit(void)
{
	backlight_device_unregister(backlight_device);

	/* we are done with the PCI device, put it back */
	pci_dev_put(pci_device);
}

module_init(samsung_init);
module_exit(samsung_exit);

MODULE_AUTHOR("Greg Kroah-Hartman <gregkh@suse.de>");
MODULE_DESCRIPTION("Samsung Backlight driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("dmi:*:svnSAMSUNGELECTRONICSCO.,LTD.:pnN120:*:rnN120:*");
MODULE_ALIAS("dmi:*:svnSAMSUNGELECTRONICSCO.,LTD.:pnN130:*:rnN130:*");
MODULE_ALIAS("dmi:*:svnSAMSUNGELECTRONICSCO.,LTD.:pnX360:*:rnX360:*");
MODULE_ALIAS("dmi:*:svnSAMSUNGELECTRONICSCO.,LTD.:pnNC10:*:rnNC10:*");
MODULE_ALIAS("dmi:*:svnSAMSUNGELECTRONICSCO.,LTD.:pnSQ45S70S:*:rnSQ45S70S:*");
