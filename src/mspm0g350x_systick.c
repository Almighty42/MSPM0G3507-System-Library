#include "../inc/mspm0g350x_systick.h"
#include "../inc/mspm0g350x_startup.h"

/********************************************************************************
 *
 *******************************************************************************/

/********************************************************************************
 * INFO: SysTick service module
 * Owns the SysTick timer. Provides a 1 ms time base for driver timeouts and
 * a busy-wait delay measured in CPU cycles. No other module may configure
 * SysTick directly.
 *******************************************************************************/

static volatile uint32_t s_ms_ticks;

/********************************************************************************
 * @fn				- SysTick_Handler
 *
 * @brief			- SysTick interrupt handler. Increments the
 *millisecond counter once per tick.
 *
 * @return 0;			- None
 *
 * @Note			- Requires sys_tick_init() to have been called.
 *Defined here so the vector table entry overrides the weak default handler.
 *******************************************************************************/
void SysTick_Handler(void)
{
	s_ms_ticks++;
}

/********************************************************************************
 * @fn				- sys_tick_init
 *
 * @brief			- Configures SysTick as a free-running 1 ms time
 *base driven by the CPU clock and enables its interrupt
 *
 * @param[cpu_hz]		- CPU clock frequency in Hz (for example
 *40000000U)
 *
 * @return			- None
 *
 * @Note			- Call once after the system clock setup
 *and before any driver that uses timeouts. The reload value is (cpu_hz / 1000)
 *- 1 and must fit in 24 bits. Resets the millisecond counter to 0.
 *******************************************************************************/
void sys_tick_init(uint32_t cpu_hz)
{
	SysTick->CSR = 0U;
	SysTick->RVR = (cpu_hz / 1000U) - 1U; /* 1 ms period */
	SysTick->CVR = 0U;
	s_ms_ticks = 0U;

	SET_BIT(SysTick->CSR, SYSTICK_CSR_CLKSOURCE_OFS);
	SET_BIT(SysTick->CSR, SYSTICK_CSR_TICKINT_OFS);
	SET_BIT(SysTick->CSR, SYSTICK_CSR_ENABLE_OFS);
}

/********************************************************************************
 * @fn				- sys_tick_get_ms
 *
 * @brief			- Returns the number of milliseconds since
 *sys_tick_init()
 *
 * @return			- Millisecond counter value (wraps after
 *about 49.7 days)
 *
 * @Note			- Compute elapsed time with unsigned
 *subtraction, for example (uint32_t)(sys_tick_get_ms() - start), so counter
 *wrap is handled. The value does not advance while interrupts are masked.
 *******************************************************************************/
uint32_t sys_tick_get_ms(void)
{
	return s_ms_ticks;
}

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
	if ((SysTick->CSR & SYSTICK_CSR_ENABLE) != 0U) {
		uint32_t period = SysTick->RVR + 1U;
		uint32_t prev = SysTick->CVR;
		uint32_t elapsed = 0U;

		while (elapsed < cpu_cycles) {
			uint32_t now = SysTick->CVR;
			elapsed += (prev >= now) ? (prev - now)
			                         : (prev + period - now);
			prev = now;
		}
		return;
	}
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
