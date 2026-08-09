#include "../inc/mspm0g350x.h"
#include "../inc/mspm0g350x_clock_configuration.h"
#include <stdint.h>

void SystemInit(void)
{
	// 1. Set flash wait states for 80MHz before raising MCLK
	// TRM: running at target frequency with too few flash wait states is a
	// classic hard-fault/lock-up cause. Single atomic read-modify-write do
	// NOT split into clear-then-set (that briefly writes FLASHWAIT=0, which
	// is unsafe if MCLK is already running fast from a prior session).
	WRITE_FIELD_4BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_FLASHWAIT,
	                 MSPM0_FLASHWAIT_VALUE);

	// 2. Confirm SYSOSC is running at its base frequency (32MHz)
	// SYSPLL requires this regardless of which reference clock feeds it.
	if (IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSOSCFREQ)) {
		return;
	}

	// 3. Select SYSOSC as the SYSPLL reference (default state)
	if (MSPM0_SYSPLL_REF_SEL) {
		SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_SYSPLLREF);
	}
	else {
		CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_SYSPLLREF);
	}

	// 4. Predivider PDIV = /2 -> fLOOPIN = 32MHz / 2 = 16MHz
	WRITE_FIELD_2BIT(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_PDIV,
	                 MSPM0_SYSPLL_PDIV);

	// 5. Load PLL loop-filter parameters for fLOOPIN = 16MHz bucket
	// TODO(verify): confirm these two source addresses against the TRM's
	// SYSPLL parameter table / NONMAIN factory config region - do not trust
	// blindly, especially after the earlier board-independent reset bug.
	SYSCTL->SYSPLLPARAM0 = HWREGW(MSPM0_SYSPLLPARAM0_ADDR);
	SYSCTL->SYSPLLPARAM1 = HWREGW(MSPM0_SYSPLLPARAM1_ADDR);

	// 6. Feedback divider QDIV = 4 (*5) -> fVCO = 16MHz * 5 = 80MHz
	WRITE_FIELD(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_QDIV, 7,
	            MSPM0_SYSPLL_QDIV);

	// 7. Output dividers: CLK1 = /2 (40MHz), CLK2X = /1 (80MHz)
	WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK1,
	                 MSPM0_SYSPLL_RDIVCLK1);
	WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK2X,
	                 MSPM0_SYSPLL_RDIVCLK2X);

	// 8. Enable SYSPLLCLK1 and SYSPLLCLK2X outputs
	if (MSPM0_SYSPLL_ENABLE_CLK1) {
		SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK1);
	}
	else {
		CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK1);
	}

	if (MSPM0_SYSPLL_ENABLE_CLK2X) {
		SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK2X);
	}
	else {
		CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK2X);
	}

	// 9. Route SYSPLLCLK2X (80MHz) into the HSCLK mux
	// TODO(verify): confirm MCLK2XVCO is sufficient on its own to select
	// the
	// 2x tap, or whether a separate HSCLK mux-select field is also required
	// — this is the prime suspect for the measured 40MHz-instead-of-80MHz
	// bug.
	if (MSPM0_HSCLK_USE_MCLK2XVCO) {
		SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_MCLK2XVCO);
	}
	else {
		CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_MCLK2XVCO);
	}

	// 10. Enable SYSPLL
	if (MSPM0_SYSPLL_STATE) {
		SET_BIT(SYSCTL->HSCLKEN, SYSCTL_HSCLKEN_SYSPLLEN);
	}
	else {
		CLEAR_BIT(SYSCTL->HSCLKEN, SYSCTL_HSCLKEN_SYSPLLEN);
	}

	// 11. Wait for SYSPLLGOOD (poll, do not blind-delay)
	while (!IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSPLLGOOD)) {
	}

	// 12. Select SYSPLL as the HSCLK source (default state)
	CLEAR_BIT(SYSCTL->HSCLKCFG, SYSCTL_HSCLKCFG_HSCLKSEL);

	// 13. Confirm/force CPUCLK & ULPCLK dividers = /1
	// MCLKCFG.MDIV and MCLKCFG.UDIV both live in this same register.
	// TODO(verify): confirm 0 = "/1 (no divide)" for both fields in the TRM
	// MCLKCFG bitfield table - this is the other prime suspect for the
	// measured 40MHz-instead-of-80MHz result if it defaults to /2.
	WRITE_FIELD_4BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_MDIV, 0U);
	WRITE_FIELD_4BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_UDIV, 0U);

	// 14. Select HSCLK as the MCLK source -> MCLK = SYSPLLCLK2X = 80MHz
	if (MSPM0_MCLK_USE_HSCLK) {
		SET_BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_USEHSCLK);
	}
	else {
		CLEAR_BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_USEHSCLK);
	}
}
