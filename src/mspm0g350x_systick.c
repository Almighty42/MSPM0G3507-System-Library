#include "mspm0g350x_systick.h"
#include "mspm0g350x_startup.h"

/********************************************************************************
 * @fn				- sys_delay_cpu_cycles
 *
 * @brief			- Busy-wait for at least the given number of
 * CPUCLK cycles using SysTick.
 *
 * @param[cpu_cycles]		- Number of CPU cycles to wait
 *
 * @return			- None
 *
 * @Note			- None
 *******************************************************************************/
void sys_delay_cpu_cycles(uint32_t cpu_cycles)
{
	SysTick->RVR = cpu_cycles - 1U;
	SysTick->CVR = 0U;
	SysTick->CSR = SYSTICK_CSR_CLKSOURCE | SYSTICK_CSR_ENABLE;

	while ((SysTick->CSR & SYSTICK_CSR_COUNTFLAG) == 0U) {
	}

	SysTick->CSR = 0U;
}

/********************************************************************************
 * @fn				- sys_clock_get_ulpclk_divider
 *
 * @brief			- Reads back the currently configured ULPCLK
 * divider from MCLKCFG.UDIV and returns it as a plain integer divisor
 *
 * @return			- 1U if ULPCLK = MCLK , 2U if ULPCLK = MCLK/2.
 *
 * @Note			- None
 *******************************************************************************/
uint32_t sys_clock_get_ulpclk_divider(void)
{
	uint32_t udiv_field = SYSCTL->MCLKCFG & SYSCTL_MCLKCFG_UDIV_MASK;

	switch (udiv_field) {
		case SYSCTL_MCLKCFG_UDIV_NODIVIDE:
			return 1U;
		case SYSCTL_MCLKCFG_UDIV_DIVIDE2:
			return 2U;
		default:
			return 1U;
	}
}
