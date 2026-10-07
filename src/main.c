#include "../examples/example_spi.h"
#include "../inc/mspm0g350x_spi.h"
#include <stdint.h>

static spi_handle_t g_spi1;

volatile spi_status_t g_init_status;
volatile spi_status_t g_write_status;
volatile spi_status_t g_read_status;
volatile uint32_t g_mismatches;

static void delay(volatile uint32_t n)
{
	while (n--) {
	}
}

int main(void)
{
	g_init_status = spi1_config(&g_spi1);

	g_write_status = spi1_example_write(&g_spi1);
	delay(2000000U);

	g_read_status =
	    spi1_example_read(&g_spi1, 0xFFU, (uint32_t*)&g_mismatches);
	delay(2000000U);
}
