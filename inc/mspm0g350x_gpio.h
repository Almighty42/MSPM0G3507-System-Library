#ifndef MSPM0G3507_GPIO_DRIVER_H
#define MSPM0G3507_GPIO_DRIVER_H

#include <stdint.h>
#include "mspm0g350x_startup.h"

typedef struct
{
    gpio_type *port;            // @GPIO_PORT
    uint8_t dio_bit;            // @DIO_NUM
    gpio_direction_t direction; // @GPIO_DIRECTION
} gpio_pin_config_t;

// Function return status

typedef enum
{
    GPIO_OK = 0,              // Success
    GPIO_ERROR_INVALID_STATE, // Invalid state of a argument
    GPIO_ERROR_NULL_PTR,      // NULL pointer passed
    GPIO_ERROR_INVALID_PIN,   // Pin number > 15
    GPIO_ERROR_INVALID_PORT,  // Invalid GPIO port address
    GPIO_ERROR_INVALID_MODE,  // Mode value out of range
    GPIO_ERROR_INVALID_IRQ,   // Invalid IRQ number
    GPIO_ERROR_LOCK_FAILED,   // Failed to lock GPIO configuration
} gpio_status_t;

// USAGE: --- @GPIO_PORT ---

#define GPIO_PORT_0 0
#define GPIO_PORT_1 1

// USAGE: --- @DIO_NUM ---

#define GPIO_DIO0 0
#define GPIO_DIO1 1
#define GPIO_DIO2 2
#define GPIO_DIO3 3
#define GPIO_DIO4 4
#define GPIO_DIO5 5
#define GPIO_DIO6 6
#define GPIO_DIO7 7
#define GPIO_DIO8 8
#define GPIO_DIO9 9
#define GPIO_DIO10 10
#define GPIO_DIO11 11
#define GPIO_DIO12 12
#define GPIO_DIO13 13
#define GPIO_DIO14 14
#define GPIO_DIO15 15
#define GPIO_DIO16 16
#define GPIO_DIO17 17
#define GPIO_DIO18 18
#define GPIO_DIO19 19
#define GPIO_DIO20 20
#define GPIO_DIO21 21
#define GPIO_DIO22 22
#define GPIO_DIO23 23
#define GPIO_DIO24 24
#define GPIO_DIO25 25
#define GPIO_DIO26 26
#define GPIO_DIO27 27
#define GPIO_DIO28 28
#define GPIO_DIO29 29
#define GPIO_DIO30 30
#define GPIO_DIO31 31

// USAGE: --- @GPIO_DIRECTION ---

typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT = 1,
} gpio_direction_t;


gpio_status_t gpio_enable_power(gpio_type *port);
gpio_status_t gpio_disable_power(gpio_type *port);
gpio_status_t gpio_toggle(gpio_type *port, uint8_t dio_bit);
gpio_status_t gpio_configure_pin(const gpio_pin_config_t *cfg);
gpio_status_t gpio_write(gpio_type *port, uint8_t dio_bit, uint8_t level);
gpio_status_t gpio_read(gpio_type *port, uint8_t dio_bit);

#endif