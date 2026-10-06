#include "../examples/example_i2c.h"
#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_startup.h"
#include "../inc/mspm0g350x_systick.h"
#include <stdint.h>

static i2c_handle_t g_i2c1;
volatile i2c_status_t g_i2c1_status = I2C_ERROR_INVALID_STATE;

__attribute__((noinline)) static void test_checkpoint(void)
{
	__asm volatile("" ::: "memory");
}

int main(void)
{
	sys_tick_init(40000000U);
	g_i2c1_status = i2c1_config(&g_i2c1);

	test_checkpoint();

	while (1) {
	}
}
