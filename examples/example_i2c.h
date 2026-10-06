#ifndef EXAMPLE_I2C_H
#define EXAMPLE_I2C_H

#include "mspm0g350x_i2c.h"
#include <stdint.h>

i2c_status_t i2c1_config(i2c_handle_t* p_i2c);
i2c_status_t i2c1_example_write(i2c_handle_t* p_i2c);
i2c_status_t i2c1_example_nack(i2c_handle_t* p_i2c);
i2c_status_t i2c1_example_write_read(i2c_handle_t* p_i2c, uint8_t* p_rx);
i2c_status_t i2c1_example_read(i2c_handle_t* p_i2c, uint8_t* p_rx);

#endif
