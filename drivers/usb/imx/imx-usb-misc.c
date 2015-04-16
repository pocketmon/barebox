/*
 * Copyright (c) 2009 Daniel Mack <daniel@caiaq.de>
 * Copyright (C) 2010 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <common.h>
#include <init.h>
#include <io.h>
#include <usb/chipidea-imx.h>

#define MX25_OTG_SIC_SHIFT	29
#define MX25_OTG_SIC_MASK	(0x3 << MX25_OTG_SIC_SHIFT)
#define MX25_OTG_PM_BIT		(1 << 24)
#define MX25_OTG_PP_BIT		(1 << 11)
#define MX25_OTG_OCPOL_BIT	(1 << 3)

#define MX25_H1_SIC_SHIFT	21
#define MX25_H1_SIC_MASK	(0x3 << MX25_H1_SIC_SHIFT)
#define MX25_H1_PP_BIT		(1 << 18)
#define MX25_H1_PM_BIT		(1 << 8)
#define MX25_H1_IPPUE_UP_BIT	(1 << 7)
#define MX25_H1_IPPUE_DOWN_BIT	(1 << 6)
#define MX25_H1_TLL_BIT		(1 << 5)
#define MX25_H1_USBTE_BIT	(1 << 4)
#define MX25_H1_OCPOL_BIT	(1 << 2)

static __maybe_unused int mx25_initialize_usb_hw(void __iomem *base, int port, unsigned int flags)
{
	unsigned int v;

	v = readl(base);

	switch (port) {
	case 0:	/* OTG port */
		v &= ~(MX25_OTG_SIC_MASK | MX25_OTG_PM_BIT | MX25_OTG_PP_BIT |
			MX25_OTG_OCPOL_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX25_OTG_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX25_OTG_PM_BIT;

		if (flags & MXC_EHCI_PWR_PIN_ACTIVE_HIGH)
			v |= MX25_OTG_PP_BIT;

		if (!(flags & MXC_EHCI_OC_PIN_ACTIVE_LOW))
			v |= MX25_OTG_OCPOL_BIT;

		break;
	case 1: /* H1 port */
		v &= ~(MX25_H1_SIC_MASK | MX25_H1_PM_BIT | MX25_H1_PP_BIT |
			MX25_H1_OCPOL_BIT | MX25_H1_TLL_BIT | MX25_H1_USBTE_BIT |
			MX25_H1_IPPUE_DOWN_BIT | MX25_H1_IPPUE_UP_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX25_H1_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX25_H1_PM_BIT;

		if (flags & MXC_EHCI_PWR_PIN_ACTIVE_HIGH)
			v |= MX25_H1_PP_BIT;

		if (!(flags & MXC_EHCI_OC_PIN_ACTIVE_LOW))
			v |= MX25_H1_OCPOL_BIT;

		if (!(flags & MXC_EHCI_TLL_ENABLED))
			v |= MX25_H1_TLL_BIT;

		if (flags & MXC_EHCI_INTERNAL_PHY)
			v |= MX25_H1_USBTE_BIT;

		if (flags & MXC_EHCI_IPPUE_DOWN)
			v |= MX25_H1_IPPUE_DOWN_BIT;

		if (flags & MXC_EHCI_IPPUE_UP)
			v |= MX25_H1_IPPUE_UP_BIT;

		break;
	default:
		return -EINVAL;
	}

	writel(v, base);

	return 0;
}

#define MX27_OTG_SIC_SHIFT	29
#define MX27_OTG_SIC_MASK	(0x3 << MX27_OTG_SIC_SHIFT)
#define MX27_OTG_PM_BIT		(1 << 24)

#define MX27_H2_SIC_SHIFT	21
#define MX27_H2_SIC_MASK	(0x3 << MX27_H2_SIC_SHIFT)
#define MX27_H2_PM_BIT		(1 << 16)
#define MX27_H2_DT_BIT		(1 << 5)

#define MX27_H1_SIC_SHIFT	13
#define MX27_H1_SIC_MASK	(0x3 << MX27_H1_SIC_SHIFT)
#define MX27_H1_PM_BIT		(1 << 8)
#define MX27_H1_DT_BIT		(1 << 4)

static __maybe_unused int mx27_mx31_initialize_usb_hw(void __iomem *base, int port, unsigned int flags)
{
	unsigned int v;

	v = readl(base);

	switch (port) {
	case 0:	/* OTG port */
		v &= ~(MX27_OTG_SIC_MASK | MX27_OTG_PM_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX27_OTG_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX27_OTG_PM_BIT;
		break;
	case 1: /* H1 port */
		v &= ~(MX27_H1_SIC_MASK | MX27_H1_PM_BIT | MX27_H1_DT_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX27_H1_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX27_H1_PM_BIT;

		if (!(flags & MXC_EHCI_TLL_ENABLED))
			v |= MX27_H1_DT_BIT;

		break;
	case 2:	/* H2 port */
		v &= ~(MX27_H2_SIC_MASK | MX27_H2_PM_BIT | MX27_H2_DT_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX27_H2_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX27_H2_PM_BIT;

		if (!(flags & MXC_EHCI_TLL_ENABLED))
			v |= MX27_H2_DT_BIT;

		break;
	default:
		return -EINVAL;
	}

	writel(v, base);

	return 0;
}

#define USBCTRL_OTGBASE_OFFSET	0x600

#define MX35_OTG_SIC_SHIFT	29
#define MX35_OTG_SIC_MASK	(0x3 << MX35_OTG_SIC_SHIFT)
#define MX35_OTG_PM_BIT		(1 << 24)
#define MX35_OTG_PP_BIT		(1 << 11)
#define MX35_OTG_OCPOL_BIT	(1 << 3)

#define MX35_H1_SIC_SHIFT	21
#define MX35_H1_SIC_MASK	(0x3 << MX35_H1_SIC_SHIFT)
#define MX35_H1_PP_BIT		(1 << 18)
#define MX35_H1_PM_BIT		(1 << 8)
#define MX35_H1_IPPUE_UP_BIT	(1 << 7)
#define MX35_H1_IPPUE_DOWN_BIT	(1 << 6)
#define MX35_H1_TLL_BIT		(1 << 5)
#define MX35_H1_USBTE_BIT	(1 << 4)
#define MX35_H1_OCPOL_BIT	(1 << 2)

static __maybe_unused int mx35_initialize_usb_hw(void __iomem *base, int port, unsigned int flags)
{
	unsigned int v;

	v = readl(base);

	switch (port) {
	case 0:	/* OTG port */
		v &= ~(MX35_OTG_SIC_MASK | MX35_OTG_PM_BIT | MX35_OTG_PP_BIT |
			MX35_OTG_OCPOL_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX35_OTG_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX35_OTG_PM_BIT;

		if (flags & MXC_EHCI_PWR_PIN_ACTIVE_HIGH)
			v |= MX35_OTG_PP_BIT;

		if (!(flags & MXC_EHCI_OC_PIN_ACTIVE_LOW))
			v |= MX35_OTG_OCPOL_BIT;

		break;
	case 1: /* H1 port */
		v &= ~(MX35_H1_SIC_MASK | MX35_H1_PM_BIT | MX35_H1_PP_BIT |
			MX35_H1_OCPOL_BIT | MX35_H1_TLL_BIT | MX35_H1_USBTE_BIT |
			MX35_H1_IPPUE_DOWN_BIT | MX35_H1_IPPUE_UP_BIT);
		v |= (flags & MXC_EHCI_INTERFACE_MASK) << MX35_H1_SIC_SHIFT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX35_H1_PM_BIT;

		if (flags & MXC_EHCI_PWR_PIN_ACTIVE_HIGH)
			v |= MX35_H1_PP_BIT;

		if (!(flags & MXC_EHCI_OC_PIN_ACTIVE_LOW))
			v |= MX35_H1_OCPOL_BIT;

		if (!(flags & MXC_EHCI_TLL_ENABLED))
			v |= MX35_H1_TLL_BIT;

		if (flags & MXC_EHCI_INTERNAL_PHY)
			v |= MX35_H1_USBTE_BIT;

		if (flags & MXC_EHCI_IPPUE_DOWN)
			v |= MX35_H1_IPPUE_DOWN_BIT;

		if (flags & MXC_EHCI_IPPUE_UP)
			v |= MX35_H1_IPPUE_UP_BIT;

		break;
	default:
		return -EINVAL;
	}

	writel(v, base);

	return 0;
}

/* USB_CTRL */
#define MX5_OTG_UCTRL_OWIE_BIT		(1 << 27)	/* OTG wakeup intr enable */
#define MX5_OTG_UCTRL_OPM_BIT		(1 << 24)	/* OTG power mask */
#define MX5_H1_UCTRL_H1UIE_BIT		(1 << 12)	/* Host1 ULPI interrupt enable */
#define MX5_H1_UCTRL_H1WIE_BIT		(1 << 11)	/* HOST1 wakeup intr enable */
#define MX5_H1_UCTRL_H1PM_BIT		(1 <<  8)	/* HOST1 power mask */

/* USB_PHY_CTRL_FUNC */
#define MX5_OTG_PHYCTRL_OC_POL_BIT	(1 << 9)	/* OTG Polarity of Overcurrent */
#define MX5_OTG_PHYCTRL_OC_DIS_BIT	(1 << 8)	/* OTG Disable Overcurrent Event */
#define MX5_H1_OC_POL_BIT		(1 << 6)	/* UH1 Polarity of Overcurrent */
#define MX5_H1_OC_DIS_BIT		(1 << 5)	/* UH1 Disable Overcurrent Event */
#define MX5_OTG_PHYCTRL_PWR_POL_BIT	(1 << 3)	/* OTG Power Pin Polarity */

/* USBH2CTRL */
#define MX5_H2_UCTRL_H2UIE_BIT		(1 << 8)
#define MX5_H2_UCTRL_H2WIE_BIT		(1 << 7)
#define MX5_H2_UCTRL_H2PM_BIT		(1 << 4)

#define MX5_USBCTRL_OFFSET		0x0
#define MX5_UTMI_PHY_CTRL_0		0x8
#define MX5_UTMI_PHY_CTRL_1		0xc
#define MX5_USBH2CTRL_OFFSET		0x14

static __maybe_unused int mx5_initialize_usb_hw(void __iomem *base, int port,
		unsigned int flags)
{
	unsigned int v;

	switch (port) {
	case 0:	/* OTG port */
		if (!(flags & MXC_EHCI_INTERNAL_PHY))
			return 0;

		/* Adjust UTMI PHY frequency to 24MHz */
		v = readl(base + MX5_UTMI_PHY_CTRL_1);
		v = (v & ~0x3) | 0x01;
		writel(v, base + MX5_UTMI_PHY_CTRL_1);

		v = readl(base + MX5_UTMI_PHY_CTRL_0);
		v &= ~MX5_OTG_PHYCTRL_OC_POL_BIT;
		v &= ~MX5_OTG_PHYCTRL_OC_DIS_BIT;
		v &= ~MX5_OTG_PHYCTRL_PWR_POL_BIT;

		if (flags & MXC_EHCI_OC_PIN_ACTIVE_LOW)
			v |= MX5_OTG_PHYCTRL_OC_POL_BIT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX5_OTG_PHYCTRL_OC_DIS_BIT;

		if (flags & MXC_EHCI_PWR_PIN_ACTIVE_HIGH)
			v |= MX5_OTG_PHYCTRL_PWR_POL_BIT;

		writel(v, base + MX5_UTMI_PHY_CTRL_0);

		v = readl(base + MX5_USBCTRL_OFFSET);
		v &= ~MX5_OTG_UCTRL_OWIE_BIT;
		v &= ~MX5_OTG_UCTRL_OPM_BIT;

		if (flags & MXC_EHCI_WAKEUP_ENABLED)
			v |= MX5_OTG_UCTRL_OWIE_BIT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX5_OTG_UCTRL_OPM_BIT;

		writel(v, base + MX5_USBCTRL_OFFSET);

		break;
	case 1:	/* H1 port */
		v = readl(base + MX5_USBCTRL_OFFSET);
		v &= ~MX5_H1_UCTRL_H1PM_BIT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX5_H1_UCTRL_H1PM_BIT;

		writel(v, base + MX5_USBCTRL_OFFSET);

		break;
	case 2: /* H2 port */
		v = readl(base + MX5_USBH2CTRL_OFFSET);
		v &= ~MX5_H2_UCTRL_H2PM_BIT;

		if (!(flags & MXC_EHCI_POWER_PINS_ENABLED))
			v |= MX5_H2_UCTRL_H2PM_BIT;

		writel(v, base + MX5_USBH2CTRL_OFFSET);

		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static __maybe_unused int mx6_initialize_usb_hw(void __iomem *base, int port,
		unsigned int flags)
{
	return 0;
}

static struct platform_device_id imx_usbmisc_ids[] = {
#ifdef CONFIG_ARCH_IMX25
	{
		.name = "imx25-usb-misc",
		.driver_data = (unsigned long)&mx25_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX27
	{
		.name = "imx27-usb-misc",
		.driver_data = (unsigned long)&mx27_mx31_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX31
	{
		.name = "imx31-usb-misc",
		.driver_data = (unsigned long)&mx27_mx31_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX35
	{
		.name = "imx35-usb-misc",
		.driver_data = (unsigned long)&mx35_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX51
	{
		.name = "imx51-usb-misc",
		.driver_data = (unsigned long)&mx5_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX53
	{
		.name = "imx53-usb-misc",
		.driver_data = (unsigned long)&mx5_initialize_usb_hw,
	},
#endif
#ifdef CONFIG_ARCH_IMX6
	{
		.name = "imx6-usb-misc",
		.driver_data = (unsigned long)&mx6_initialize_usb_hw,
	},
#endif
	{
                /* sentinel */
	},
};

static int (*__imx_usbmisc_port_init)(void __iomem *base, int port, unsigned flags);
static void __iomem *usbmisc_base;

int imx_usbmisc_port_init(int port, unsigned flags)
{
	if (!__imx_usbmisc_port_init)
		return -ENODEV;

	return __imx_usbmisc_port_init(usbmisc_base, port, flags);
}

static int imx_usbmisc_probe(struct device_d *dev)
{
	struct imx_serial_devtype_data *devtype;
	int ret;

	ret = dev_get_drvdata(dev, (unsigned long *)&devtype);
	if (ret)
		return ret;

	usbmisc_base = dev_request_mem_region(dev, 0);
	if (!usbmisc_base)
		return -ENOMEM;

	__imx_usbmisc_port_init = (void *)devtype;

	return 0;
}

static struct driver_d imx_usbmisc_driver = {
	.name   = "imx-usbmisc",
	.probe  = imx_usbmisc_probe,
	.id_table = imx_usbmisc_ids,
};

static int imx_usbmisc_init(void)
{
	platform_driver_register(&imx_usbmisc_driver);
	return 0;
}

coredevice_initcall(imx_usbmisc_init);
