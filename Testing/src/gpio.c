/**
 * @file    gpio_driver.c
 * @brief   Implementation of bare-metal GPIO output driver for MSPM0G3507.
 *
 * @warning See gpio_driver.h header for full verification status of each
 *          register/bitfield. Items marked TODO in the header are NOT
 *          confirmed against primary sources and must be resolved before
 *          this is trusted on real hardware.
 */

#include "../inc/gpio.h"
#include <stdint.h>

const gpio_port_t GPIOA = {(volatile uint32_t*)GPIOA_BASE};
const gpio_port_t GPIOB = {(volatile uint32_t*)GPIOB_BASE};

static inline volatile uint32_t* gpio_reg(const gpio_port_t* port,
                                          uint32_t offset)
{
	return (volatile uint32_t*)((uint8_t*)port->base + offset);
}

void gpio_port_init(const gpio_port_t* port)
{
	volatile uint32_t* rstctl = gpio_reg(port, GPIO_O_RSTCTL);
	volatile uint32_t* pwren = gpio_reg(port, GPIO_O_PWREN);

	/* Assert reset (keyed write, SLAU846D Table 9-15). */
	*rstctl = GPIO_RSTCTL_KEY | GPIO_RSTCTL_ASSERT;

	/* NOTE: no explicit "deassert" bit exists in RSTCTL — RESETASSERT is a
	 * pulsed/self-clearing action per the peripheral's own reset FSM.
	 * TODO: confirm timing/self-clear behavior against the SYSCTL /
	 * peripheral reset chapter before relying on immediate reuse of the
	 * peripheral right after this write. */

	/* Enable power (keyed write, SLAU846D Table 9-13). */
	*pwren = GPIO_PWREN_KEY | GPIO_PWREN_ENABLE;

	/* TODO: TRM does not specify a required delay between PWREN enable
	 * and first register access in this chapter. Verify against SYSCTL
	 * power-sequencing guidance before removing any settle delay. */
}

void gpio_output_config(const gpio_port_t* port, uint32_t pincm_idx,
                        uint32_t pin_bit)
{
	volatile uint32_t* pincm = &IOMUX_PINCM(pincm_idx);
	volatile uint32_t* doeset = gpio_reg(port, GPIO_O_DOESET31_0);

	/* Runtime reconfig sequence per SLAU846D 8.2.1:
	 * 1. Disable currently connected peripheral (n/a on cold config)
	 * 2. Clear PC + INENA
	 * 3. Clear PF
	 * 4. Write new PF (GPIO peripheral ID)
	 * 5. Set PC + INENA
	 * 6. Enable peripheral (GPIO side: set DOE bit) */

	*pincm &= ~((1UL << PINCM_PC_BIT) | (1UL << PINCM_INENA_BIT));
	*pincm &= ~PINCM_PF_MASK;
	*pincm |= (PINCM_PF_GPIO_PLACEHOLDER &
	           PINCM_PF_MASK); /* TODO: verify real PF value */
	*pincm |= (1UL << PINCM_PC_BIT) | (1UL << PINCM_INENA_BIT);

	/* Enable GPIO output driver on this bit (DOESET, no RMW needed). */
	*doeset = (1UL << pin_bit);
}

void gpio_set_pin(const gpio_port_t* port, uint32_t pin_bit)
{
	volatile uint32_t* doutset = gpio_reg(port, GPIO_O_DOUTSET31_0);
	*doutset = (1UL << pin_bit);
}

void gpio_clear_pin(const gpio_port_t* port, uint32_t pin_bit)
{
	volatile uint32_t* doutclr = gpio_reg(port, GPIO_O_DOUTCLR31_0);
	*doutclr = (1UL << pin_bit);
}

void gpio_toggle_pin(const gpio_port_t* port, uint32_t pin_bit)
{
	volatile uint32_t* douttgl = gpio_reg(port, GPIO_O_DOUTTGL31_0);
	*douttgl = (1UL << pin_bit);
}

void gpio_write_pin(const gpio_port_t* port, uint32_t pin_bit, bool level)
{
	if (level) {
		gpio_set_pin(port, pin_bit);
	}
	else {
		gpio_clear_pin(port, pin_bit);
	}
}
