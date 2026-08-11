#include <stdint.h>
#include "mspm0g350x_startup.h"

#define DIO0                        0
#define DIO1                        1
#define DIO2                        2
#define DIO3                        3
#define DIO4                        4
#define DIO5                        5
#define DIO6                        6
#define DIO7                        7
#define DIO8                        8
#define DIO9                        9
#define DIO10                       10
#define DIO11                       11
#define DIO12                       12
#define DIO13                       13
#define DIO14                       14
#define DIO15                       15
#define DIO16                       16
#define DIO17                       17
#define DIO18                       18
#define DIO19                       19
#define DIO20                       20
#define DIO21                       21
#define DIO22                       22
#define DIO23                       23
#define DIO24                       24
#define DIO25                       25
#define DIO26                       26
#define DIO27                       27
#define DIO28                       28
#define DIO29                       29
#define DIO30                       30
#define DIO31                       31

#define GPIO_DIR_OUTPUT             0
#define GPIO_DIR_INPUT              1

typedef struct {
    gpio_type *port;                // GPIO0 or GPIO1
    uint8_t dio_bit;                // 0-31, the DIO bit number
    uint8_t direction;              // GPIO_DIR_OUPUT / INPUT
} gpio_pin_config_t;

void gpio_enable_power(gpio_type *port);

void gpio_configure_pin(const gpio_pin_config_t *cfg);
void gpio_write(gpio_type *port, uint8_t dio_bit, uint8_t level);
uint8_t gpio_read(gpio_type *port, uint8_t dio_bit);
