// src/main.c
/**
 * @brief Solid-ON LED test to isolate whether the GPIO/IOMUX register
 *        sequence works, independent of SysTick/clock correctness.
 *
 * @warning Pin identity is now CONFIRMED, not a placeholder:
 *   LED1 (red) on the LP-MSPM0G3507 LaunchPad is wired to PA0 via jumper
 *   J4, per the LaunchPad User's Guide (SLAU873). PA0 = PINCMx 1 in the
 *   device datasheet's Pin Attributes table, which is array index 0 in
 *   this driver's 0-indexed IOMUX_PINCM() macro. GPIOA bit 0.
 *
 * This test deliberately does NOT touch SysTick or depend on MCLK
 * frequency being correct -- it isolates the GPIO/IOMUX register
 * sequence (the thing we just fixed: PC=bit7, PF=bits[5:0]) from the
 * still-open clock bring-up questions in clock_init_worksheet.md.
 */

#include "../inc/gpio.h"
#include <stdint.h>

#define LED_PORT GPIOA   /* CONFIRMED: LED1 red is on GPIOA */
#define LED_PINCM_IDX 0U /* CONFIRMED: PA0 = PINCMx 1 = array idx 0 */
#define LED_PIN_BIT 0U   /* CONFIRMED: PA0 = GPIOA bit 0 */

int main(void)
{
	/* Power+reset the GPIO peripheral (keyed PWREN/RSTCTL writes,
	 * SLAU846D Table 9-13 / 9-15), then IOMUX+DOE config for PA0. */
	gpio_port_init(&LED_PORT);
	gpio_output_config(&LED_PORT, LED_PINCM_IDX, LED_PIN_BIT);

	/* Drive the pin high and leave it -- solid ON, no blink, no SysTick.
	 * If PC/PF are now correct, the LED should light immediately and
	 * stay lit. If it doesn't light, the bug is still in the GPIO/IOMUX
	 * sequence, not in clock timing. */
	gpio_set_pin(&LED_PORT, LED_PIN_BIT);

	while (1) {
		/* Nothing to do -- pin stays driven high. */
	}

	return 0;
}
