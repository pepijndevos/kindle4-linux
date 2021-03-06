/*
 * STMP RTC Register Definitions
 *
 * Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2008 Embedded Alley Solutions, Inc All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * This file is created by xml file. Don't Edit it.
 */

#ifndef __ARCH_ARM___RTC_H
#define __ARCH_ARM___RTC_H  1

#define REGS_RTC_BASE (STMP3XXX_REGS_BASE + 0x5c000)
#define REGS_RTC_PHYS (0x8005C000)
#define REGS_RTC_SIZE 0x00002000

#define HW_RTC_CTRL	(0x00000000)
#define HW_RTC_CTRL_SET	(0x00000004)
#define HW_RTC_CTRL_CLR	(0x00000008)
#define HW_RTC_CTRL_TOG	(0x0000000c)
#define HW_RTC_CTRL_ADDR  \
		(REGS_RTC_BASE + HW_RTC_CTRL)
#define HW_RTC_CTRL_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_CTRL_SET)
#define HW_RTC_CTRL_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_CTRL_CLR)
#define HW_RTC_CTRL_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_CTRL_TOG)

#define BM_RTC_CTRL_SFTRST	0x80000000
#define BM_RTC_CTRL_CLKGATE	0x40000000
#define BP_RTC_CTRL_RSVD0	7
#define BM_RTC_CTRL_RSVD0	0x3FFFFF80
#define BF_RTC_CTRL_RSVD0(v)  \
		(((v) << 7) & BM_RTC_CTRL_RSVD0)
#define BM_RTC_CTRL_SUPPRESS_COPY2ANALOG	0x00000040
#define BM_RTC_CTRL_FORCE_UPDATE	0x00000020
#define BM_RTC_CTRL_WATCHDOGEN	0x00000010
#define BM_RTC_CTRL_ONEMSEC_IRQ	0x00000008
#define BM_RTC_CTRL_ALARM_IRQ	0x00000004
#define BM_RTC_CTRL_ONEMSEC_IRQ_EN	0x00000002
#define BM_RTC_CTRL_ALARM_IRQ_EN	0x00000001

#define HW_RTC_STAT	(0x00000010)
#define HW_RTC_STAT_SET	(0x00000014)
#define HW_RTC_STAT_CLR	(0x00000018)
#define HW_RTC_STAT_TOG	(0x0000001c)
#define HW_RTC_STAT_ADDR  \
		(REGS_RTC_BASE + HW_RTC_STAT)
#define HW_RTC_STAT_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_STAT_SET)
#define HW_RTC_STAT_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_STAT_CLR)
#define HW_RTC_STAT_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_STAT_TOG)

#define BM_RTC_STAT_RTC_PRESENT	0x80000000
#define BM_RTC_STAT_ALARM_PRESENT	0x40000000
#define BM_RTC_STAT_WATCHDOG_PRESENT	0x20000000
#define BM_RTC_STAT_XTAL32000_PRESENT	0x10000000
#define BM_RTC_STAT_XTAL32768_PRESENT	0x08000000
#define BP_RTC_STAT_RSVD1	24
#define BM_RTC_STAT_RSVD1	0x07000000
#define BF_RTC_STAT_RSVD1(v)  \
		(((v) << 24) & BM_RTC_STAT_RSVD1)
#define BP_RTC_STAT_STALE_REGS	16
#define BM_RTC_STAT_STALE_REGS	0x00FF0000
#define BF_RTC_STAT_STALE_REGS(v)  \
		(((v) << 16) & BM_RTC_STAT_STALE_REGS)
#define BP_RTC_STAT_NEW_REGS	8
#define BM_RTC_STAT_NEW_REGS	0x0000FF00
#define BF_RTC_STAT_NEW_REGS(v)  \
		(((v) << 8) & BM_RTC_STAT_NEW_REGS)
#define BP_RTC_STAT_RSVD0	0
#define BM_RTC_STAT_RSVD0	0x000000FF
#define BF_RTC_STAT_RSVD0(v)  \
		(((v) << 0) & BM_RTC_STAT_RSVD0)

#define HW_RTC_MILLISECONDS	(0x00000020)
#define HW_RTC_MILLISECONDS_SET	(0x00000024)
#define HW_RTC_MILLISECONDS_CLR	(0x00000028)
#define HW_RTC_MILLISECONDS_TOG	(0x0000002c)
#define HW_RTC_MILLISECONDS_ADDR  \
		(REGS_RTC_BASE + HW_RTC_MILLISECONDS)
#define HW_RTC_MILLISECONDS_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_MILLISECONDS_SET)
#define HW_RTC_MILLISECONDS_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_MILLISECONDS_CLR)
#define HW_RTC_MILLISECONDS_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_MILLISECONDS_TOG)

#define BP_RTC_MILLISECONDS_COUNT	0
#define BM_RTC_MILLISECONDS_COUNT	0xFFFFFFFF
#define BF_RTC_MILLISECONDS_COUNT(v)	(v)

#define HW_RTC_SECONDS	(0x00000030)
#define HW_RTC_SECONDS_SET	(0x00000034)
#define HW_RTC_SECONDS_CLR	(0x00000038)
#define HW_RTC_SECONDS_TOG	(0x0000003c)
#define HW_RTC_SECONDS_ADDR  \
		(REGS_RTC_BASE + HW_RTC_SECONDS)
#define HW_RTC_SECONDS_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_SECONDS_SET)
#define HW_RTC_SECONDS_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_SECONDS_CLR)
#define HW_RTC_SECONDS_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_SECONDS_TOG)

#define BP_RTC_SECONDS_COUNT	0
#define BM_RTC_SECONDS_COUNT	0xFFFFFFFF
#define BF_RTC_SECONDS_COUNT(v)	(v)

#define HW_RTC_ALARM	(0x00000040)
#define HW_RTC_ALARM_SET	(0x00000044)
#define HW_RTC_ALARM_CLR	(0x00000048)
#define HW_RTC_ALARM_TOG	(0x0000004c)
#define HW_RTC_ALARM_ADDR  \
		(REGS_RTC_BASE + HW_RTC_ALARM)
#define HW_RTC_ALARM_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_ALARM_SET)
#define HW_RTC_ALARM_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_ALARM_CLR)
#define HW_RTC_ALARM_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_ALARM_TOG)

#define BP_RTC_ALARM_VALUE	0
#define BM_RTC_ALARM_VALUE	0xFFFFFFFF
#define BF_RTC_ALARM_VALUE(v)	(v)

#define HW_RTC_WATCHDOG	(0x00000050)
#define HW_RTC_WATCHDOG_SET	(0x00000054)
#define HW_RTC_WATCHDOG_CLR	(0x00000058)
#define HW_RTC_WATCHDOG_TOG	(0x0000005c)
#define HW_RTC_WATCHDOG_ADDR  \
		(REGS_RTC_BASE + HW_RTC_WATCHDOG)
#define HW_RTC_WATCHDOG_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_WATCHDOG_SET)
#define HW_RTC_WATCHDOG_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_WATCHDOG_CLR)
#define HW_RTC_WATCHDOG_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_WATCHDOG_TOG)

#define BP_RTC_WATCHDOG_COUNT	0
#define BM_RTC_WATCHDOG_COUNT	0xFFFFFFFF
#define BF_RTC_WATCHDOG_COUNT(v)	(v)

#define HW_RTC_PERSISTENT0	(0x00000060)
#define HW_RTC_PERSISTENT0_SET	(0x00000064)
#define HW_RTC_PERSISTENT0_CLR	(0x00000068)
#define HW_RTC_PERSISTENT0_TOG	(0x0000006c)
#define HW_RTC_PERSISTENT0_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT0)
#define HW_RTC_PERSISTENT0_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT0_SET)
#define HW_RTC_PERSISTENT0_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT0_CLR)
#define HW_RTC_PERSISTENT0_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT0_TOG)

#define BP_RTC_PERSISTENT0_SPARE_ANALOG	18
#define BM_RTC_PERSISTENT0_SPARE_ANALOG	0xFFFC0000
#define BF_RTC_PERSISTENT0_SPARE_ANALOG(v) \
		(((v) << 18) & BM_RTC_PERSISTENT0_SPARE_ANALOG)
#define BM_RTC_PERSISTENT0_AUTO_RESTART	0x00020000
#define BM_RTC_PERSISTENT0_DISABLE_PSWITCH	0x00010000
#define BP_RTC_PERSISTENT0_LOWERBIAS	14
#define BM_RTC_PERSISTENT0_LOWERBIAS	0x0000C000
#define BF_RTC_PERSISTENT0_LOWERBIAS(v)  \
		(((v) << 14) & BM_RTC_PERSISTENT0_LOWERBIAS)
#define BM_RTC_PERSISTENT0_DISABLE_XTALOK	0x00002000
#define BP_RTC_PERSISTENT0_MSEC_RES	8
#define BM_RTC_PERSISTENT0_MSEC_RES	0x00001F00
#define BF_RTC_PERSISTENT0_MSEC_RES(v)  \
		(((v) << 8) & BM_RTC_PERSISTENT0_MSEC_RES)
#define BM_RTC_PERSISTENT0_ALARM_WAKE	0x00000080
#define BM_RTC_PERSISTENT0_XTAL32_FREQ	0x00000040
#define BM_RTC_PERSISTENT0_XTAL32KHZ_PWRUP	0x00000020
#define BM_RTC_PERSISTENT0_XTAL24MHZ_PWRUP	0x00000010
#define BM_RTC_PERSISTENT0_LCK_SECS	0x00000008
#define BM_RTC_PERSISTENT0_ALARM_EN	0x00000004
#define BM_RTC_PERSISTENT0_ALARM_WAKE_EN	0x00000002
#define BM_RTC_PERSISTENT0_CLOCKSOURCE	0x00000001

#define HW_RTC_PERSISTENT1	(0x00000070)
#define HW_RTC_PERSISTENT1_SET	(0x00000074)
#define HW_RTC_PERSISTENT1_CLR	(0x00000078)
#define HW_RTC_PERSISTENT1_TOG	(0x0000007c)
#define HW_RTC_PERSISTENT1_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT1)
#define HW_RTC_PERSISTENT1_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT1_SET)
#define HW_RTC_PERSISTENT1_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT1_CLR)
#define HW_RTC_PERSISTENT1_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT1_TOG)

#define BP_RTC_PERSISTENT1_GENERAL	0
#define BM_RTC_PERSISTENT1_GENERAL	0xFFFFFFFF
#define BF_RTC_PERSISTENT1_GENERAL(v)	(v)
#define BV_RTC_PERSISTENT1_GENERAL__ENUMERATE_500MA_TWICE 0x1000
#define BV_RTC_PERSISTENT1_GENERAL__USB_BOOT_PLAYER_MODE  0x0800
#define BV_RTC_PERSISTENT1_GENERAL__SKIP_CHECKDISK        0x0400
#define BV_RTC_PERSISTENT1_GENERAL__USB_LOW_POWER_MODE    0x0200
#define BV_RTC_PERSISTENT1_GENERAL__OTG_HNP_BIT           0x0100
#define BV_RTC_PERSISTENT1_GENERAL__OTG_ATL_ROLE_BIT      0x0080

#define HW_RTC_PERSISTENT2	(0x00000080)
#define HW_RTC_PERSISTENT2_SET	(0x00000084)
#define HW_RTC_PERSISTENT2_CLR	(0x00000088)
#define HW_RTC_PERSISTENT2_TOG	(0x0000008c)
#define HW_RTC_PERSISTENT2_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT2)
#define HW_RTC_PERSISTENT2_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT2_SET)
#define HW_RTC_PERSISTENT2_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT2_CLR)
#define HW_RTC_PERSISTENT2_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT2_TOG)

#define BP_RTC_PERSISTENT2_GENERAL	0
#define BM_RTC_PERSISTENT2_GENERAL	0xFFFFFFFF
#define BF_RTC_PERSISTENT2_GENERAL(v)	(v)

#define HW_RTC_PERSISTENT3	(0x00000090)
#define HW_RTC_PERSISTENT3_SET	(0x00000094)
#define HW_RTC_PERSISTENT3_CLR	(0x00000098)
#define HW_RTC_PERSISTENT3_TOG	(0x0000009c)
#define HW_RTC_PERSISTENT3_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT3)
#define HW_RTC_PERSISTENT3_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT3_SET)
#define HW_RTC_PERSISTENT3_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT3_CLR)
#define HW_RTC_PERSISTENT3_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT3_TOG)

#define BP_RTC_PERSISTENT3_GENERAL	0
#define BM_RTC_PERSISTENT3_GENERAL	0xFFFFFFFF
#define BF_RTC_PERSISTENT3_GENERAL(v)	(v)

#define HW_RTC_PERSISTENT4	(0x000000a0)
#define HW_RTC_PERSISTENT4_SET	(0x000000a4)
#define HW_RTC_PERSISTENT4_CLR	(0x000000a8)
#define HW_RTC_PERSISTENT4_TOG	(0x000000ac)
#define HW_RTC_PERSISTENT4_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT4)
#define HW_RTC_PERSISTENT4_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT4_SET)
#define HW_RTC_PERSISTENT4_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT4_CLR)
#define HW_RTC_PERSISTENT4_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT4_TOG)

#define BP_RTC_PERSISTENT4_GENERAL	0
#define BM_RTC_PERSISTENT4_GENERAL	0xFFFFFFFF
#define BF_RTC_PERSISTENT4_GENERAL(v)	(v)

#define HW_RTC_PERSISTENT5	(0x000000b0)
#define HW_RTC_PERSISTENT5_SET	(0x000000b4)
#define HW_RTC_PERSISTENT5_CLR	(0x000000b8)
#define HW_RTC_PERSISTENT5_TOG	(0x000000bc)
#define HW_RTC_PERSISTENT5_ADDR  \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT5)
#define HW_RTC_PERSISTENT5_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT5_SET)
#define HW_RTC_PERSISTENT5_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT5_CLR)
#define HW_RTC_PERSISTENT5_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_PERSISTENT5_TOG)

#define BP_RTC_PERSISTENT5_GENERAL	0
#define BM_RTC_PERSISTENT5_GENERAL	0xFFFFFFFF
#define BF_RTC_PERSISTENT5_GENERAL(v)	(v)

#define HW_RTC_DEBUG	(0x000000c0)
#define HW_RTC_DEBUG_SET	(0x000000c4)
#define HW_RTC_DEBUG_CLR	(0x000000c8)
#define HW_RTC_DEBUG_TOG	(0x000000cc)
#define HW_RTC_DEBUG_ADDR  \
		(REGS_RTC_BASE + HW_RTC_DEBUG)
#define HW_RTC_DEBUG_SET_ADDR \
		(REGS_RTC_BASE + HW_RTC_DEBUG_SET)
#define HW_RTC_DEBUG_CLR_ADDR \
		(REGS_RTC_BASE + HW_RTC_DEBUG_CLR)
#define HW_RTC_DEBUG_TOG_ADDR \
		(REGS_RTC_BASE + HW_RTC_DEBUG_TOG)

#define BP_RTC_DEBUG_RSVD0	2
#define BM_RTC_DEBUG_RSVD0	0xFFFFFFFC
#define BF_RTC_DEBUG_RSVD0(v) \
		(((v) << 2) & BM_RTC_DEBUG_RSVD0)
#define BM_RTC_DEBUG_WATCHDOG_RESET_MASK	0x00000002
#define BM_RTC_DEBUG_WATCHDOG_RESET	0x00000001

#define HW_RTC_VERSION	(0x000000d0)
#define HW_RTC_VERSION_ADDR \
		(REGS_RTC_BASE + HW_RTC_VERSION)

#define BP_RTC_VERSION_MAJOR	24
#define BM_RTC_VERSION_MAJOR	0xFF000000
#define BF_RTC_VERSION_MAJOR(v) \
		(((v) << 24) & BM_RTC_VERSION_MAJOR)
#define BP_RTC_VERSION_MINOR	16
#define BM_RTC_VERSION_MINOR	0x00FF0000
#define BF_RTC_VERSION_MINOR(v)  \
		(((v) << 16) & BM_RTC_VERSION_MINOR)
#define BP_RTC_VERSION_STEP	0
#define BM_RTC_VERSION_STEP	0x0000FFFF
#define BF_RTC_VERSION_STEP(v)  \
		(((v) << 0) & BM_RTC_VERSION_STEP)
#endif /* __ARCH_ARM___RTC_H */
