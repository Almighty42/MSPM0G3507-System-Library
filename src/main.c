#include "../examples/example_i2c.h"
#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_startup.h"
#include "../inc/mspm0g350x_systick.h"
#include <stdint.h>

#define EEPROM_ADDR 0x50U
#define EEPROM_WRITE_CYCLE_MS                                                  \
	6U /* datasheet write cycle is 5 ms, plus margin */
#define TEST_TIMEOUT_MS 100U
#define PATTERN_LEN 3U

static const uint8_t k_pattern[PATTERN_LEN] = {0xDEU, 0xADU, 0xBEU};
static const uint8_t k_mem_addr[1] = {0x00U}; /* 2 bytes on 24C32 and larger */

static i2c_handle_t g_i2c1;
static uint8_t g_rx[PATTERN_LEN];

volatile i2c_status_t g_i2c1_status = I2C_ERROR_INVALID_STATE;
volatile i2c_status_t g_write_status = I2C_ERROR_INVALID_STATE;
volatile i2c_status_t g_wr_status = I2C_ERROR_INVALID_STATE;
volatile i2c_status_t g_rd_status = I2C_ERROR_INVALID_STATE;
volatile uint8_t g_readback_ok; /* bit0 = write_read ok, bit1 = read ok */

__attribute__((noinline)) static void test_checkpoint(void)
{
	__asm volatile("" ::: "memory");
}

static void wait_ms(uint32_t ms)
{
	const uint32_t t = sys_tick_get_ms();
	while ((uint32_t)(sys_tick_get_ms() - t) < ms) {
	}
}

static void rx_clear(void)
{
	for (uint32_t i = 0U; i < PATTERN_LEN; i++) {
		g_rx[i] = 0U;
	}
}

static uint8_t rx_matches_pattern(void)
{
	for (uint32_t i = 0U; i < PATTERN_LEN; i++) {
		if (g_rx[i] != k_pattern[i]) {
			return 0U;
		}
	}
	return 1U;
}

int main(void)
{
	sys_tick_init(40000000U);
	g_i2c1_status = i2c1_config(&g_i2c1);
	test_checkpoint();

	if (g_i2c1_status == I2C_OK) {
		/* 1: write 00 DE AD BE, then wait for the EEPROM write cycle */
		g_write_status = i2c1_example_write(&g_i2c1);
		wait_ms(EEPROM_WRITE_CYCLE_MS);
		test_checkpoint();

		/* 2: random read (write_read), expect DE AD BE */
		rx_clear();
		g_wr_status = i2c1_example_write_read(&g_i2c1, g_rx);
		if ((g_wr_status == I2C_OK) && rx_matches_pattern()) {
			g_readback_ok |= 0x01U;
		}
		test_checkpoint();

		/* 3: set pointer to 0x00, then current-address read, expect DE
		 * AD BE */
		(void)i2c_controller_write_pl(&g_i2c1, EEPROM_ADDR, k_mem_addr,
		                              1U, TEST_TIMEOUT_MS);
		rx_clear();
		g_rd_status = i2c1_example_read(&g_i2c1, g_rx);
		if ((g_rd_status == I2C_OK) && rx_matches_pattern()) {
			g_readback_ok |= 0x02U;
		}
		test_checkpoint();
	}

	while (1) {
	}
}
