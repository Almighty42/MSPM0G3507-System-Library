#include "../inc/mspm0g350x_gpio.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdint.h>

#define GPIO_PWREN_KEY 0x26U

gpio_status_t gpio_enable_power(gpio_type* port)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	port->PWREN = (GPIO_PWREN_KEY << 24) | (1U << 0);

	for (volatile int i = 0; i < 10; i++) { /* let power domain settle */
	}

	return GPIO_OK;
}

gpio_status_t gpio_disable_power(gpio_type* port)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	port->PWREN = (GPIO_PWREN_KEY << 24) | (0U << 0);

	return GPIO_OK;
}

gpio_status_t gpio_toggle(gpio_type* port, uint8_t dio_bit)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1) {
		return GPIO_ERROR_INVALID_PORT;
	}

	if (dio_bit > 31U) {
		return GPIO_ERROR_INVALID_PIN;
	}

	port->DOUTTGL31_0 = (1UL << dio_bit);

	return GPIO_OK;
}

gpio_status_t gpio_configure_pin(const gpio_pin_config_t* cfg)
{
	VALIDATE_PTR(cfg, GPIO_ERROR_NULL_PTR);
	VALIDATE_PTR(cfg->port, GPIO_ERROR_NULL_PTR);

	if (cfg->port != GPIO0 && cfg->port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (cfg->dio_bit > 31U)
		return GPIO_ERROR_INVALID_PIN;

	switch (cfg->direction) {
		case GPIO_DIR_OUTPUT:
			cfg->port->DOESET31_0 = (1UL << cfg->dio_bit);
			break;
		case GPIO_DIR_INPUT:
			cfg->port->DOECLR31_0 = (1UL << cfg->dio_bit);
			break;
		default:
			return GPIO_ERROR_INVALID_MODE;
	}

	return GPIO_OK;
}

gpio_status_t gpio_write(gpio_type* port, uint8_t dio_bit, uint8_t level)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (dio_bit > 31U)
		return GPIO_ERROR_INVALID_PIN;

	if (level)
		port->DOUTSET31_0 = (1UL << dio_bit);
	else
		port->DOUTCLR31_0 = (1UL << dio_bit);

	return GPIO_OK;
}

gpio_status_t gpio_read(gpio_type* port, uint8_t dio_bit, uint8_t* level)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);
	VALIDATE_PTR(level, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (dio_bit > 31U)
		return GPIO_ERROR_INVALID_PIN;

	*level = (uint8_t)((port->DIN31_0 >> dio_bit) & 0x1U);

	return GPIO_OK;
}
