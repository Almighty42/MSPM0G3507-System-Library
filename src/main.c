#include "../examples/example_spi.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdint.h>

#ifndef SPI_EXPECTED_BYTE
#define SPI_EXPECTED_BYTE 0xFFU
#endif

static spi_handle_t spi1;

int main(void)
{
	static volatile spi_status_t config_status;
	static volatile spi_status_t write_status;
	static volatile spi_status_t read_status;
	static volatile uint32_t read_mismatches;
	uint32_t mismatches = 0U;

	config_status = spi1_config(&spi1);
	if (config_status != SPI_OK) {
		while (1) {
		}
	}

	write_status = spi1_example_write(&spi1);
	read_status = spi1_example_read(&spi1, SPI_EXPECTED_BYTE, &mismatches);
	read_mismatches = mismatches;

	while (1) {
	}
}
