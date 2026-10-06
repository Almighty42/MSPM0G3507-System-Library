#ifndef MSPM0G3507_SYSTICK_H
#define MSPM0G3507_SYSTICK_H

#include <stdint.h>

#define SYSTICK_MAX_RELOAD	    (0x00FFFFFFU)

#define SYSTICK_CSR_ENABLE_OFS      (0)
#define SYSTICK_CSR_ENABLE          (1U << SYSTICK_CSR_ENABLE_OFS)

#define SYSTICK_CSR_TICKINT_OFS     (1)
#define SYSTICK_CSR_TICKINT         (1U << SYSTICK_CSR_TICKINT_OFS)

#define SYSTICK_CSR_CLKSOURCE_OFS   (2)
#define SYSTICK_CSR_CLKSOURCE       (1U << SYSTICK_CSR_CLKSOURCE_OFS)

#define SYSTICK_CSR_COUNTFLAG_OFS   (16)
#define SYSTICK_CSR_COUNTFLAG       (1U << SYSTICK_CSR_COUNTFLAG_OFS)

#define SYSCTL_MCLKCFG_UDIV_OFS         (4U)
#define SYSCTL_MCLKCFG_UDIV_WIDTH       (2U)
#define SYSCTL_MCLKCFG_UDIV_MASK        ((uint32_t)0x00000030U)   /* bits 5:4 */

#define SYSCTL_MCLKCFG_UDIV_NODIVIDE    ((uint32_t)0x00000000U)   /* ULPCLK = MCLK */
#define SYSCTL_MCLKCFG_UDIV_DIVIDE2     ((uint32_t)0x00000010U)   /* ULPCLK = MCLK/2 */

void sys_tick_init(uint32_t cpu_hz);
uint32_t sys_tick_get_ms(void);

void sys_delay_cpu_cycles(uint32_t cpu_cycles);
uint32_t sys_clock_get_ulpclk_divider(void);

#endif
