// src/system_init.c
#include "../inc/mspm0g350x.h"
#include <stdint.h>

#define SYSCTL_BASE 0x400AF000UL

#define SYSCTL_WRITELOCK_OFFSET 0x1200UL
#define SYSCTL_MCLKCFG_OFFSET 0x1104UL
#define SYSCTL_SYSPLLCFG0_OFFSET 0x1120UL
#define SYSCTL_SYSPLLCFG1_OFFSET 0x1124UL
#define SYSCTL_SYSPLLPARAM0_OFFSET 0x1128UL
#define SYSCTL_SYSPLLPARAM1_OFFSET 0x112CUL
#define SYSCTL_HSCLKEN_OFFSET 0x1108UL
#define SYSCTL_HSCLKCFG_OFFSET 0x110CUL
#define SYSCTL_CLKSTATUS_OFFSET 0x1204UL

#define SYSCTL_WRITELOCK                                                       \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_WRITELOCK_OFFSET))
#define SYSCTL_MCLKCFG                                                         \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_MCLKCFG_OFFSET))
#define SYSCTL_SYSPLLCFG0                                                      \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_SYSPLLCFG0_OFFSET))
#define SYSCTL_SYSPLLCFG1                                                      \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_SYSPLLCFG1_OFFSET))
#define SYSCTL_SYSPLLPARAM0                                                    \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_SYSPLLPARAM0_OFFSET))
#define SYSCTL_SYSPLLPARAM1                                                    \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_SYSPLLPARAM1_OFFSET))
#define SYSCTL_HSCLKEN                                                         \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_HSCLKEN_OFFSET))
#define SYSCTL_HSCLKCFG                                                        \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_HSCLKCFG_OFFSET))
#define SYSCTL_CLKSTATUS                                                       \
	(*(volatile uint32_t*)(SYSCTL_BASE + SYSCTL_CLKSTATUS_OFFSET))

// void SystemInit(void)
// {
// 0. FLASHWAIT state setup and Writelock unlocked
// CLEAR_BIT(SYSCTL->WRITELOCK, 0);
// CLEAR_FIELD_2BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_FLASHWAIT);
// SYSCTL->MCLKCFG |= (2U << SYSCTL_MCLKCFG_FLASHWAIT);

// 1. Verify that the SYSPLL is disabled (SYSPLLOFF is set in CLKSTATUS)
// if (!IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSPLLOFF)) {
//     return;
// }

// 2. Make sure that SYSOSC is running at base frequency (32MHz); this is a
// requirement for SYSPLL operation even if HFCLK is used as the SYSPLL
// reference clock instead of SYSOSC
// if (IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSOSCFREQ)) {
//     return;
// }

// 3. Set SYSOSC as the SYSPLL reference (make sure that the SYSPLLREF bit in
// the SYSPLLCFG0 register is cleared; this is the default state after reset)
// CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_SYSPLLREF);

// 4. Select a predivider PDIV to /2 (set SYSPLLCFG1.PDIV to 0x01), setting
// fLOOPIN to 16MHz ( 32 divided by 2 )
// WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_PDIV, 1);

// 5. Load the PLL parameters into SYSPLLPARAM0 and SYSPLLPARAM1 to support
// fLOOPIN of 16MH
// SYSCTL->SYSPLLPARAM0 = HWREGW(0x41C4002C); // fLOOPIN = 16MHz bucket
// SYSCTL->SYSPLLPARAM1 = HWREGW(0x41C40030); // PARAM1 = PARAM0 base + 4

// 6. Set the feedback divider QDIV to 5 (set SYSPLLCFG1.QDIV to 4), giving
// fVCO=80MHz ( 16MHz multiplied by 5 )
// WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_QDIV, 4);

// 7. Set the SYSPLL output dividers for SYSPLLCLK1 and SYSPLLCLK2X to /2 (set
// SYSPLLCFG0.RDIVCLK1 to 0x0 and SYSPLLCFG0.RDIVCLK2X to 0x1) to get 40MHz and
// 80MHz at SYSPLLCLK1 and SYSPLLCLK2X, respectively
// WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK1, 0);
// WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK2X, 1);

// 8. Enable SYSPLLCLK1 and SYSPLLCLK2X outputs by setting the ENABLECLK1 and
// ENABLECLK2X bits in the SYSPLLCFG0 register
// SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK1);
// SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK2X);

// 9. Select SYSPLLCLK2X as the PLL output to the HSCLK mux by setting MCLK2XVCO
// in the SYSPLLCFG0 register
// SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_MCLK2XVCO);

// 10. With SYSOSC enabled and running at base frequency (32MHz, this is the
// default state out of reset), enable the SYSPLL by setting SYSPLLEN in the
// HSCLKEN register
// SET_BIT(SYSCTL->HSCLKEN, SYSCTL_HSCLKEN_SYSPLLEN);

// 11. Wait for the SYSPLLGOOD indication by testing SYSPLLGOOD in the CLKSTATUS
// register
// while (!IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSPLLGOOD)) {
// Busy wait for PLL lock
// }

// 12. Select the SYSPLL as the HSCLK source by ensuring that the HSCLKSEL bit
// is cleared in the HSCLKCFG register (this is the default state)
// CLEAR_BIT(SYSCTL->HSCLKCFG, SYSCTL_HSCLKCFG_HSCLKSEL);

// 13. elect the high-speed clock (HSCLK) as the source for MCLK by setting the
// USEHSCLK bit in the MCLKCFG register. This will switch MCLK from SYSOSC to
// HSCLK. MCLK is now running from SYSPLLCLK2X at 80MHz.
// SET_BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_USEHSCLK);
// }

void SystemInit(void)
{
	// 0. Unlock SYSCTL config space, then set flash wait states
	// SYSCTL_WRITELOCK &= ~(1UL << 0);

	// 0. Set flash wait states for 80MHz operation, before switching MCLK
	// source
	SYSCTL_MCLKCFG &= ~(0xFU << SYSCTL_MCLKCFG_FLASHWAIT);
	SYSCTL_MCLKCFG |= (2U << SYSCTL_MCLKCFG_FLASHWAIT);

	// SYSCTL_MCLKCFG &= ~(0xFU << SYSCTL_MCLKCFG_FLASHWAIT);   // correct
	// 4-bit clear SYSCTL_MCLKCFG |= (2U << SYSCTL_MCLKCFG_FLASHWAIT); //
	// harmless now per TRM, but correct going forward

	// Step 1 removed — SYSPLLOFF defaults to 0h at reset (startup-failure
	// indicator, not an "is disabled" flag); no meaningful precondition
	// here. See TRM SYSCTL CLKSTATUS bit 14 description.

	// 2. Make sure that SYSOSC is running at base frequency (32MHz)
	if (SYSCTL_CLKSTATUS & (1UL << SYSCTL_CLKSTATUS_SYSOSCFREQ)) {
		return;
	}

	// 3. Set SYSOSC as the SYSPLL reference (clear SYSPLLREF in SYSPLLCFG0)
	SYSCTL_SYSPLLCFG0 &= ~(1UL << SYSCTL_SYSPLLCFG0_SYSPLLREF);

	// 4. Set predivider PDIV to /2 (SYSPLLCFG1.PDIV = 1)
	SYSCTL_SYSPLLCFG1 =
	    (SYSCTL_SYSPLLCFG1 & ~(0xFU << SYSCTL_SYSPLLCFG1_PDIV)) |
	    (1U << SYSCTL_SYSPLLCFG1_PDIV);

	// 5. Load PLL parameters for fLOOPIN = 16MHz
	SYSCTL_SYSPLLPARAM0 = HWREGW(0x41C4002C);
	SYSCTL_SYSPLLPARAM1 = HWREGW(0x41C40030);

	// 6. Set feedback divider QDIV to 5 (SYSPLLCFG1.QDIV = 4)
	SYSCTL_SYSPLLCFG1 =
	    (SYSCTL_SYSPLLCFG1 & ~(0xFU << SYSCTL_SYSPLLCFG1_QDIV)) |
	    (4U << SYSCTL_SYSPLLCFG1_QDIV);

	// 7. Set output dividers: RDIVCLK1 = 0, RDIVCLK2X = 1
	SYSCTL_SYSPLLCFG0 =
	    (SYSCTL_SYSPLLCFG0 & ~(0xFU << SYSCTL_SYSPLLCFG0_RDIVCLK1)) |
	    (0U << SYSCTL_SYSPLLCFG0_RDIVCLK1);
	SYSCTL_SYSPLLCFG0 =
	    (SYSCTL_SYSPLLCFG0 & ~(0xFU << SYSCTL_SYSPLLCFG0_RDIVCLK2X)) |
	    (1U << SYSCTL_SYSPLLCFG0_RDIVCLK2X);

	// 8. Enable SYSPLLCLK1 and SYSPLLCLK2X outputs
	SYSCTL_SYSPLLCFG0 |= (1UL << SYSCTL_SYSPLLCFG0_ENABLECLK1);
	SYSCTL_SYSPLLCFG0 |= (1UL << SYSCTL_SYSPLLCFG0_ENABLECLK2X);

	// 9. Select SYSPLLCLK2X as PLL output to HSCLK mux
	SYSCTL_SYSPLLCFG0 |= (1UL << SYSCTL_SYSPLLCFG0_MCLK2XVCO);

	// 10. Enable SYSPLL
	SYSCTL_HSCLKEN |= (1UL << SYSCTL_HSCLKEN_SYSPLLEN);

	// 11. Wait for SYSPLLGOOD
	while (!(SYSCTL_CLKSTATUS & (1UL << SYSCTL_CLKSTATUS_SYSPLLGOOD))) {
		// busy wait
	}

	// 12. Select SYSPLL as HSCLK source (clear HSCLKSEL)
	SYSCTL_HSCLKCFG &= ~(1UL << SYSCTL_HSCLKCFG_HSCLKSEL);

	// 13. Select HSCLK as MCLK source
	SYSCTL_MCLKCFG |= (1UL << SYSCTL_MCLKCFG_USEHSCLK);
}
