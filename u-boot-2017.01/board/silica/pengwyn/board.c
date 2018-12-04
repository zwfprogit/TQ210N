/*
 * board.c
 *
 * Copyright (C) 2013 Lothar Felten <lothar.felten@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <i2c.h>
#include <phy.h>
#include <cpsw.h>
#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

static struct ctrl_dev *cdev = (struct ctrl_dev *)CTRL_DEVICE_BASE;

#if defined(CONFIG_SPL_BUILD)

/* DDR3 RAM timings */
static const struct ddr_data ddr3_data = {
	.datardsratio0 = MT41K128MJT187E_RD_DQS,
	.datawdsratio0 = MT41K128MJT187E_WR_DQS,
	.datafwsratio0 = MT41K128MJT187E_PHY_FIFO_WE,
	.datawrsratio0 = MT41K128MJT187E_PHY_WR_DATA,
};

static const struct cmd_control ddr3_cmd_ctrl_data = {
	.cmd0csratio = MT41K128MJT187E_RATIO,
	.cmd0iclkout = MT41K128MJT187E_INVERT_CLKOUT,
	.cmd1csratio = MT41K128MJT187E_RATIO,
	.cmd1iclkout = MT41K128MJT187E_INVERT_CLKOUT,
	.cmd2csratio = MT41K128MJT187E_RATIO,
	.cmd2iclkout = MT41K128MJT187E_INVERT_CLKOUT,
};

static struct emif_regs ddr3_emif_reg_data = {
	.sdram_config = MT41K128MJT187E_EMIF_SDCFG,
	.ref_ctrl = MT41K128MJT187E_EMIF_SDREF,
	.sdram_tim1 = MT41K128MJT187E_EMIF_TIM1,
	.sdram_tim2 = MT41K128MJT187E_EMIF_TIM2,
	.sdram_tim3 = MT41K128MJT187E_EMIF_TIM3,
	.zq_config = MT41K128MJT187E_ZQ_CFG,
	.emif_ddr_phy_ctlr_1 = MT41K128MJT187E_EMIF_READ_LATENCY |
				PHY_EN_DYN_PWRDN,
};

const struct ctrl_ioregs ddr3_ioregs = {
	.cm0ioctl		= MT41K128MJT187E_IOCTRL_VALUE,
	.cm1ioctl		= MT41K128MJT187E_IOCTRL_VALUE,
	.cm2ioctl		= MT41K128MJT187E_IOCTRL_VALUE,
	.dt0ioctl		= MT41K128MJT187E_IOCTRL_VALUE,
	.dt1ioctl		= MT41K128MJT187E_IOCTRL_VALUE,
};

#ifdef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	/* break into full u-boot on 'c' */
	return serial_tstc() && serial_getc() == 'c';
}
#endif

#define OSC	(V_OSCK/1000000)
const struct dpll_params dpll_ddr_266 = {
		266, OSC-1, 1, -1, -1, -1, -1};
const struct dpll_params dpll_ddr_303 = {
		303, OSC-1, 1, -1, -1, -1, -1};
const struct dpll_params dpll_ddr_400 = {
		400, OSC-1, 1, -1, -1, -1, -1};

void am33xx_spl_board_init(void)
{
	/*
	 * The pengwyn board uses the TPS650250 PMIC  without I2C
	 * interface and will output the following fixed voltages:
	 * DCDC1=3V3 (IO) DCDC2=1V5 (DDR) DCDC3=1V26 (Vmpu)
	 * VLDO1=1V8 (IO) VLDO2=1V8(IO)
	 * Vcore=1V1 is fixed, generated by TPS62231
	 */

	/* Get the frequency */
	dpll_mpu_opp100.m = am335x_get_efuse_mpu_max_freq(cdev);

	/* Set CORE Frequencies to OPP100 */
	do_setup_dpll(&dpll_core_regs, &dpll_core_opp100);

	/* 720MHz cpu, this might change on newer board revisions */
	dpll_mpu_opp100.m = MPUPLL_M_720;
	do_setup_dpll(&dpll_mpu_regs, &dpll_mpu_opp100);
}

const struct dpll_params *get_dpll_ddr_params(void)
{
	/* future configs can return other clock settings */
	return &dpll_ddr_303;
}

void set_uart_mux_conf(void)
{
	enable_uart0_pin_mux();
}

void set_mux_conf_regs(void)
{
	enable_board_pin_mux();
}

void sdram_init(void)
{
	config_ddr(303, &ddr3_ioregs, &ddr3_data,
		   &ddr3_cmd_ctrl_data, &ddr3_emif_reg_data, 0);
}
#endif /* if CONFIG_SPL_BUILD */

/*
 * Basic board specific setup.  Pinmux has been handled already.
 */
int board_init(void)
{
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	gpmc_init();
	return 0;
}

#ifdef CONFIG_DRIVER_TI_CPSW
static void cpsw_control(int enabled)
{
	/* VTP can be added here */
	return;
}

static struct cpsw_slave_data cpsw_slaves[] = {
	{
		.slave_reg_ofs	= 0x208,
		.sliver_reg_ofs	= 0xd80,
		.phy_addr	= 1,
		.phy_if		= PHY_INTERFACE_MODE_MII,
	},
};

static struct cpsw_platform_data cpsw_data = {
	.mdio_base		= CPSW_MDIO_BASE,
	.cpsw_base		= CPSW_BASE,
	.mdio_div		= 0xff,
	.channels		= 8,
	.cpdma_reg_ofs		= 0x800,
	.slaves			= 1,
	.slave_data		= cpsw_slaves,
	.ale_reg_ofs		= 0xd00,
	.ale_entries		= 1024,
	.host_port_reg_ofs	= 0x108,
	.hw_stats_reg_ofs	= 0x900,
	.bd_ram_ofs		= 0x2000,
	.mac_control		= (1 << 5),
	.control		= cpsw_control,
	.host_port_num		= 0,
	.version		= CPSW_CTRL_VERSION_2,
};

int board_eth_init(bd_t *bis)
{
	int rv, n = 0;
	uint8_t mac_addr[6];
	uint32_t mac_hi, mac_lo;

	if (!eth_getenv_enetaddr("ethaddr", mac_addr)) {
		printf("<ethaddr> not set. Reading from E-fuse\n");
		/* try reading mac address from efuse */
		mac_lo = readl(&cdev->macid0l);
		mac_hi = readl(&cdev->macid0h);
		mac_addr[0] = mac_hi & 0xFF;
		mac_addr[1] = (mac_hi & 0xFF00) >> 8;
		mac_addr[2] = (mac_hi & 0xFF0000) >> 16;
		mac_addr[3] = (mac_hi & 0xFF000000) >> 24;
		mac_addr[4] = mac_lo & 0xFF;
		mac_addr[5] = (mac_lo & 0xFF00) >> 8;

		if (is_valid_ethaddr(mac_addr))
			eth_setenv_enetaddr("ethaddr", mac_addr);
		else
			return n;
	}

	writel(MII_MODE_ENABLE, &cdev->miisel);

	rv = cpsw_register(&cpsw_data);
	if (rv < 0)
		printf("Error %d registering CPSW switch\n", rv);
	else
		n += rv;
	return n;
}
#endif /* if CONFIG_DRIVER_TI_CPSW */
