#include <stdint.h>

void Reset_Handler(void);

extern uint32_t _estack;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

#define CPUSS_START 0x40400000
#define CPUSS_IIDX_GROUP0 (*(volatile uint32_t*)(CPUSS_START + 0x1100))
#define CPUSS_IIDX_GROUP1 (*(volatile uint32_t*)(CPUSS_START + 0x1130))

int main(void);

void Default_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDT0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDT1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DEBUGSS_Handler(void) __attribute__((weak, alias("Default_Handler")));
void NVMNW_Handler(void) __attribute__((weak, alias("Default_Handler")));
void Event_Sub_Port0_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
void Event_Sub_Port1_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
void SYSCTL_Handler(void) __attribute__((weak, alias("Default_Handler")));
void GROUP0_IRQHandler(void)
{
	switch (CPUSS_IIDX_GROUP0) {
		case 1:
			WWDT0_Handler();
			break; /* was case 0 */
		case 2:
			WWDT1_Handler();
			break; /* was case 1 */
		case 3:
			DEBUGSS_Handler();
			break;
		case 4:
			NVMNW_Handler();
			break;
		case 5:
			Event_Sub_Port0_Handler();
			break;
		case 6:
			Event_Sub_Port1_Handler();
			break;
		case 7:
			SYSCTL_Handler();
			break;
		default:
			Default_Handler();
	}
}
void GPIO0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void GPIO1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TRNG_Handler(void) __attribute__((weak, alias("Default_Handler")));
void GROUP1_IRQHandler(void)
{
	switch (CPUSS_IIDX_GROUP1) {
		case 1:
			GPIO0_Handler();
			break;
		case 2:
			GPIO1_Handler();
			break;
		case 3:
			COMP0_Handler();
			break;
		case 4:
			COMP1_Handler();
			break;
		case 5:
			COMP2_Handler();
			break;
		case 6:
			TRNG_Handler();
			break;
		default:
			Default_Handler();
	}
}

void TIMG8_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void ADC0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void ADC1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN_FD_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DAC_OUT_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMG0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMG6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMA0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMA1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMG7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIMG12_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void AES_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t vectors[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&Reset_Handler,
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uint32_t)SVC_Handler,
    0,
    0,
    (uint32_t)PendSV_Handler,
    (uint32_t)SysTick_Handler,
    (uint32_t)GROUP0_IRQHandler,
    (uint32_t)GROUP1_IRQHandler,
    (uint32_t)TIMG8_Handler,
    (uint32_t)UART3_Handler,
    (uint32_t)ADC0_Handler,
    (uint32_t)ADC1_Handler,
    (uint32_t)CAN_FD_Handler,
    (uint32_t)DAC_OUT_Handler,
    0,
    (uint32_t)SPI0_Handler,
    (uint32_t)SPI1_Handler,
    0,
    0,
    (uint32_t)UART1_Handler,
    (uint32_t)UART2_Handler,
    (uint32_t)UART0_Handler,
    (uint32_t)TIMG0_Handler,
    (uint32_t)TIMG6_Handler,
    (uint32_t)TIMA0_Handler,
    (uint32_t)TIMA1_Handler,
    (uint32_t)TIMG7_Handler,
    (uint32_t)TIMG12_Handler,
    0,
    0,
    (uint32_t)I2C0_Handler,
    (uint32_t)I2C1_Handler,
    0,
    0,
    (uint32_t)AES_Handler,
    0,
    (uint32_t)RTC_Handler,
    (uint32_t)DMA_Handler,
};

void Default_Handler(void)
{
	for (;;)
		;
}

void Reset_Handler(void)
{
	uint32_t *src, *dst;

	src = &_etext;
	dst = &_sdata;
	while (dst < &_edata) {
		*dst++ = *src++;
	}

	dst = &_sbss;
	while (dst < &_ebss) {
		*dst++ = 0;
	}

	main();

	while (1) {
	}
}
