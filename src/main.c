#include "../inc/mspm0g350x_gpio.h"
#include "../inc/mspm0g350x_iomux.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdint.h>

void SysTick_Handler(void)
{
}

void led_pin_init(void);

int main(void)
{
	led_pin_init();
	gpio_write(GPIO1, GPIO_DIO22, 1);
	while (1) {
		gpio_toggle(GPIO1, GPIO_DIO22);
		for (volatile uint32_t i = 0; i < 500000U; i++) {
		}
	}
	return 0;
}

void led_pin_init(void)
{
	gpio_enable_power(GPIO1);

	iomux_pin_config_t iomux_cfg = {
	    .pincm_index = IOMUX_PIN_PB22,
	    .pf = IOMUX_PIN_PB22_PF_GPIO,
	    .pull = IOMUX_PULL_NONE,
	    .input_enable = IOMUX_STATE_DISABLE,
	};

	iomux_configure_pin(&iomux_cfg);

	gpio_write(GPIO1, GPIO_DIO22, 0);

	gpio_pin_config_t gpio_cfg = {
	    .port = GPIO1,
	    .dio_bit = GPIO_DIO22,
	    .direction = GPIO_DIR_OUTPUT,
	};
	gpio_configure_pin(&gpio_cfg);
}
