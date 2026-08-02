// src/main.c
#include <stdint.h>

volatile uint32_t tick_count = 0; // increments every 100ms
volatile uint32_t seconds = 0;

void SysTick_Handler(void)
{
	tick_count++;
	if (tick_count >= 10) {
		tick_count = 0;
		seconds++;
	}
}

int main(void)
{
	while (1) {
		// busy loop; tick_count increments once per second via SysTick
		// IRQ
	}
	return 0;
}
