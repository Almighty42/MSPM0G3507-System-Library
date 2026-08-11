#include <stdint.h>
#include "../inc/gpio.h"
#include "../inc/mspm0g350x_startup.h"

#define GPIO_PWREN_KEY  0x26U

void gpio_enable_power(gpio_type *port)
{
    if (port == NULL) {
        return;
    }
    port->PWREN = (GPIO_PWREN_KEY << 24) | (1U << 0);
    for (volatile int i = 0; i < 10; i++) { /* let power domain settle */ }
}

void gpio_configure_pin(const gpio_pin_config_t *cfg)
{
    if (cfg == NULL || cfg->port == NULL) {
        return;
    }

    /* Use the SET/CLR alias registers, not a read-modify-write on DOE31_0 —
     * avoids a race if another bit in the same port is touched concurrently. */
    if (cfg->direction == GPIO_DIR_OUTPUT) {
        cfg->port->DOESET31_0 = (1UL << cfg->dio_bit);
    } else {
        cfg->port->DOECLR31_0 = (1UL << cfg->dio_bit);
    }
}

void gpio_write(gpio_type *port, uint8_t dio_bit, uint8_t level)
{
    if (port == NULL) {
        return;
    }

    if (level) {
        port->DOUTSET31_0 = (1UL << dio_bit);
    } else {
        port->DOUTCLR31_0 = (1UL << dio_bit);
    }
}

uint8_t gpio_read(gpio_type *port, uint8_t dio_bit)
{
    if (port == NULL) {
        return 0U;
    }

    return (uint8_t)((port->DIN31_0 >> dio_bit) & 0x1U);
}
