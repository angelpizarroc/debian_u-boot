/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <i2c_eeprom.h>
#include <netdev.h>
#include <power/regulator.h>


static int get_ethaddr_from_eeprom(u8 *addr)
{
	int ret;
	struct udevice *dev;

	ret = uclass_first_device_err(UCLASS_I2C_EEPROM, &dev);
	if (ret)
		return ret;

	return i2c_eeprom_read(dev, 0, addr, 6);
}

int rk_board_late_init(void)
{
	u8 ethaddr[6];

	if (get_ethaddr_from_eeprom(ethaddr))
		return 0;

	if (is_valid_ethaddr(ethaddr))
		eth_setenv_enetaddr("ethaddr", ethaddr);

	return 0;
}


#ifdef CONFIG_DM_PMIC
static int rockchip_set_regulator_on(const char *name, uint uv)
{
	struct udevice *dev;
	int ret;

	ret = regulator_get_by_platname(name, &dev);
	if (ret) {
		debug("%s: Cannot find regulator %s\n", __func__, name);
		return ret;
	}
	ret = regulator_set_value(dev, uv);
	if (ret) {
		debug("%s: Cannot set regulator %s\n", __func__, name);
		return ret;
	}
	ret = regulator_set_enable(dev, 1);
	if (ret) {
		debug("%s: Cannot enable regulator %s\n", __func__, name);
		return ret;
	}

	return 0;
}

int power_init_board(void)
{
	int ret = rockchip_set_regulator_on("vcc33_mipi", 3300000);
	if (ret)
		return ret;

	return 0;
}
#endif