#ifndef EXAMPLE_SPI_H
#define EXAMPLE_SPI_H

#include "mspm0g350x_spi.h"
#include <stdint.h>

spi_status_t spi1_config(spi_handle_t *p_spi);
spi_status_t spi1_example_write(spi_handle_t *p_spi);
spi_status_t spi1_example_read(spi_handle_t *p_spi, uint8_t expected,
                               uint32_t *p_mismatches);

#endif
