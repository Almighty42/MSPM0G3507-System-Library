#include "../inc/mspm0g350x_gpio.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdbool.h>
#include <stdint.h>

/********************************************************************************
 *
 * TODO: Implement Systick where it is necessary ( GPIO )
 *
 *******************************************************************************/

/********************************************************************************
 *
 * INFO: Driver map
 * @CONFIGURATION
 * @POWER_CONTROL
 * @INPUT_OUTPUT
 *
 *******************************************************************************/

/********************************************************************************
 * - ( STATIC ) HELPER FUNCTIONS PROTOTYPES
 *******************************************************************************/

static bool gpio_dio_is_valid(gpio_type* p_gpio_x, gpio_dio_t dio_bit);

// NOTE: @CONFIGURATION

/********************************************************************************
 * @fn				- gpio_configure_pin
 *
 * @brief			- Configures gpio pin
 *
 * @param[gpio_config_t]	- GPIO configuration struct
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_configure_pin(const gpio_config_t* gpio_cfg)
{
	VALIDATE_PTR(gpio_cfg, GPIO_ERROR_NULL_PTR);
	VALIDATE_PTR(gpio_cfg->p_GPIOx, GPIO_ERROR_NULL_PTR);

	if (gpio_cfg->p_GPIOx != GPIO0 && gpio_cfg->p_GPIOx != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (!gpio_dio_is_valid(gpio_cfg->p_GPIOx, gpio_cfg->dio_bit))
		return GPIO_ERROR_INVALID_PIN;

	const uint32_t mask = 1UL << gpio_cfg->dio_bit;

	switch (gpio_cfg->direction) {
		case GPIO_DIR_OUTPUT:
			if (gpio_cfg->level != 0) {
				gpio_cfg->p_GPIOx->DOUTSET31_0 = mask;
			}
			else {
				gpio_cfg->p_GPIOx->DOUTCLR31_0 = mask;
			}
			gpio_cfg->p_GPIOx->DOESET31_0 = mask;
			break;
		case GPIO_DIR_INPUT:
			gpio_cfg->p_GPIOx->DOECLR31_0 = mask;
			break;
		default:
			return GPIO_ERROR_INVALID_MODE;
	}

	return GPIO_OK;
}

// NOTE: @POWER_CONTROL

/********************************************************************************
 * - GPIO PWREN KEY
 *******************************************************************************/

#define GPIO_PWREN_KEY 0x26U

/********************************************************************************
 * @fn				- gpio_enable_power
 *
 * @brief			- Turns GPIO ON
 *
 * @param[*p_gpio_x]		- Base address of the GPIO peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_enable_power(gpio_type* p_gpio_x)
{
	VALIDATE_PTR(p_gpio_x, GPIO_ERROR_NULL_PTR);

	if (p_gpio_x != GPIO0 && p_gpio_x != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	p_gpio_x->PWREN = (GPIO_PWREN_KEY << 24) | (1U << 0);

	for (volatile int i = 0; i < 10; i++) {
	}

	return GPIO_OK;
}

/********************************************************************************
 * @fn				- gpio_disable_power
 *
 * @brief			- Turns GPIO OFF
 *
 * @param[*p_gpio_x]		- Base address of the GPIO peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_disable_power(gpio_type* port)
{
	VALIDATE_PTR(port, GPIO_ERROR_NULL_PTR);

	if (port != GPIO0 && port != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	port->PWREN = (GPIO_PWREN_KEY << 24) | (0U << 0);

	return GPIO_OK;
}

// NOTE: @INPUT_OUTPUT

/********************************************************************************
 * @fn				- gpio_write
 *
 * @brief			- Writes data on a GPIO pin
 *
 * @param[*p_gpio_x]		- Base address of the GPIO peripheral
 * @param[dio_bit]		- GPIO pin to write to
 * @param[level]		- Write HIGH or LOW
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_write(gpio_type* p_gpio_x, uint8_t dio_bit,
                         gpio_level_t level)
{
	VALIDATE_PTR(p_gpio_x, GPIO_ERROR_NULL_PTR);

	if (p_gpio_x != GPIO0 && p_gpio_x != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (!gpio_dio_is_valid(p_gpio_x, dio_bit))
		return GPIO_ERROR_INVALID_PIN;

	if (level == GPIO_LEVEL_HIGH)
		p_gpio_x->DOUTSET31_0 = (1UL << dio_bit);
	else if (level == GPIO_LEVEL_LOW)
		p_gpio_x->DOUTCLR31_0 = (1UL << dio_bit);
	else
		return GPIO_ERROR_INVALID_LEVEL;

	return GPIO_OK;
}

/********************************************************************************
 * @fn				- gpio_read
 *
 * @brief			- Reads data from a GPIO pin
 *
 * @param[*p_gpio_x]		- Base address of the GPIO peripheral
 * @param[dio_bit]		- GPIO pin to write to
 * @param[*level]		- Reads GPIO level into a pointer
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_read(gpio_type* p_gpio_x, uint8_t dio_bit,
                        gpio_level_t* level)
{
	VALIDATE_PTR(p_gpio_x, GPIO_ERROR_NULL_PTR);
	VALIDATE_PTR(level, GPIO_ERROR_NULL_PTR);

	if (p_gpio_x != GPIO0 && p_gpio_x != GPIO1)
		return GPIO_ERROR_INVALID_PORT;

	if (!gpio_dio_is_valid(p_gpio_x, dio_bit))
		return GPIO_ERROR_INVALID_PIN;

	*level = ((p_gpio_x->DIN31_0 >> dio_bit) & 0x1U) ? GPIO_LEVEL_HIGH
	                                                 : GPIO_LEVEL_LOW;

	return GPIO_OK;
}

/********************************************************************************
 * @fn				- gpio_toggle
 *
 * @brief			- Toggles level of GPIO pin
 *
 * @param[*p_gpio_x]		- Base address of the GPIO peripheral
 * @param[dio_bit]		- GPIO pin to write to
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

gpio_status_t gpio_toggle(gpio_type* p_gpio_x, uint8_t dio_bit)
{
	VALIDATE_PTR(p_gpio_x, GPIO_ERROR_NULL_PTR);

	if (p_gpio_x != GPIO0 && p_gpio_x != GPIO1) {
		return GPIO_ERROR_INVALID_PORT;
	}

	if (!gpio_dio_is_valid(p_gpio_x, dio_bit)) {
		return GPIO_ERROR_INVALID_PIN;
	}

	p_gpio_x->DOUTTGL31_0 = (1UL << dio_bit);

	return GPIO_OK;
}

static bool gpio_dio_is_valid(gpio_type* p_gpio_x, gpio_dio_t dio_bit)
{
	uint32_t mask;

	if (p_gpio_x == GPIO0)
		mask = GPIO_PORTA_DIO_MASK;
	else if (p_gpio_x == GPIO1)
		mask = GPIO_PORTB_DIO_MASK;
	else
		return false;

	return (dio_bit <= GPIO_PORT_MAX_DIO) && ((mask >> dio_bit) & 1U);
}
