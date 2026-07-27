#include <stdint.h>
#include "mspm0g350x.h"

#define CPUSS_START 0x40400000
#define CPUSS_IIDX_GROUP0 (*(volatile uint32_t*)(CPUSS_START + 0x1100))
#define CPUSS_IIDX_GROUP1 (*(volatile uint32_t*)(CPUSS_START + 0x1130))

/* Entry point for the application. */
extern void SystemInit(void);
extern int  main( void );

extern uint32_t __data_load__;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __ramfunct_load__;
extern uint32_t __ramfunct_start__;
extern uint32_t __ramfunct_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

typedef void( *pFunc )( void );

void Default_Handler(void);
extern void Reset_Handler       (void) __attribute__((weak));
extern void __libc_init_array(void);
extern void _init               (void) __attribute__((weak, alias("initStub")));
void initStub(void){;}

/* Processor Exceptions */
extern void NMI_Handler         (void) __attribute__((weak, alias("Default_Handler")));
extern void HardFault_Handler   (void) __attribute__((weak, alias("Default_Handler")));
extern void SVC_Handler         (void) __attribute__((weak, alias("Default_Handler")));
extern void PendSV_Handler      (void) __attribute__((weak, alias("Default_Handler")));
extern void SysTick_Handler     (void) __attribute__((weak, alias("Default_Handler")));

/* Device Specific Interrupt Handlers */
void WWDT0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void WWDT1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DEBUGSS_Handler(void) __attribute__((weak, alias("Default_Handler")));
void NVMNW_Handler(void) __attribute__((weak, alias("Default_Handler")));
void Event_Sub_Port0_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
void Event_Sub_Port1_Handler(void)
    __attribute__((weak, alias("Default_Handler")));
void SYSCTL_Handler(void) __attribute__((weak, alias("Default_Handler")));
void GPIO0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void GPIO1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void COMP2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TRNG_Handler(void) __attribute__((weak, alias("Default_Handler")));
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

void GROUP0_Handler(void)
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
void GROUP1_Handler(void)
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

/* Interrupt vector table.  Note that the proper constructs must be placed on this to */
/* ensure that it ends up at physical address 0x0000.0000 or at the start of          */
/* the program if located at a start address other than 0.                            */
void (* const interruptVectors[])(void) __attribute__ ((used)) __attribute__ ((section (".intvecs"))) = {
    (pFunc)&__StackTop,                    /* The initial stack pointer */
    Reset_Handler,                         /* The reset handler         */
    NMI_Handler,                           /* The NMI handler           */
    HardFault_Handler,                     /* The hard fault handler    */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    SVC_Handler,                           /* SVCall handler            */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    PendSV_Handler,                        /* The PendSV handler        */
    SysTick_Handler,                       /* SysTick handler           */
    GROUP0_Handler,                     /* GROUP0 interrupt handler  */
    GROUP1_Handler,                     /* GROUP1 interrupt handler  */
    TIMG8_Handler,                      /* TIMG8 interrupt handler   */
    UART3_Handler,                      /* UART3 interrupt handler   */
    ADC0_Handler,                       /* ADC0 interrupt handler    */
    ADC1_Handler,                       /* ADC1 interrupt handler    */
    CAN_FD_Handler,                     /* CANFD0 interrupt handler  */
    DAC_OUT_Handler,                       /* DAC0 interrupt handler    */
    0,                                     /* Reserved                  */
    SPI0_Handler,                       /* SPI0 interrupt handler    */
    SPI1_Handler,                       /* SPI1 interrupt handler    */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    UART1_Handler,                      /* UART1 interrupt handler   */
    UART2_Handler,                      /* UART2 interrupt handler   */
    UART0_Handler,                      /* UART0 interrupt handler   */
    TIMG0_Handler,                      /* TIMG0 interrupt handler   */
    TIMG6_Handler,                      /* TIMG6 interrupt handler   */
    TIMA0_Handler,                      /* TIMA0 interrupt handler   */
    TIMA1_Handler,                      /* TIMA1 interrupt handler   */
    TIMG7_Handler,                      /* TIMG7 interrupt handler   */
    TIMG12_Handler,                     /* TIMG12 interrupt handler  */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    I2C0_Handler,                       /* I2C0 interrupt handler    */
    I2C1_Handler,                       /* I2C1 interrupt handler    */
    0,                                     /* Reserved                  */
    0,                                     /* Reserved                  */
    AES_Handler,                        /* AES interrupt handler     */
    0,                                     /* Reserved                  */
    RTC_Handler,                        /* RTC interrupt handler     */
    DMA_Handler,                        /* DMA interrupt handler     */
};

/* Forward declaration of the default fault handlers. */
/* This is the code that gets called when the processor first starts execution */
/* following a reset event.  Only the absolutely necessary set is performed,   */
/* after which the application supplied entry() routine is called.  Any fancy  */
/* actions (such as making decisions based on the reset cause register, and    */
/* resetting the bits in that register) are left solely in the hands of the    */
/* application.                                                                */
void Reset_Handler(void)
{
	uint32_t *src, *dst;
    uint32_t *bs, *be;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
	src = &__data_load__;
    for(dst = &__data_start__; dst< &__data_end__; )
    {
        *dst++ = *src++;
    }

    //
    // Copy the ramfunct segment initializers from flash to SRAM.
    //
    src = &__ramfunct_load__;
    for(dst = &__ramfunct_start__; dst < &__ramfunct_end__; )
    {
        *dst++ = *src++;
    }

    // Initialize .bss to zero
    bs = &__bss_start__;
    be = &__bss_end__;
    while (bs < be)
    {
        *bs = 0;
        bs++;
    }

    /*
     * System initialization routine can be called here, but it's not
     * required for MSPM0.
     */
    SystemInit();

	//
	// Initialize virtual tables, along executing init, init_array, constructors
	// and preinit_array functions
	//
	__libc_init_array();

    //
    // Call the application's entry point.
    //
    main();

    //
    // If we ever return signal Error
    //
    HardFault_Handler();
}

/* This is the code that gets called when the processor receives an unexpected  */
/* interrupt.  This simply enters an infinite loop, preserving the system state */
/* for examination by a debugger.                                               */
void Default_Handler(void)
{
    /* Enter an infinite loop. */
    while(1)
    {
    }
}

void SystemInit(void) {
    // 0. FLASHWAIT state setup
    CLEAR_FIELD_2BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_FLASHWAIT);
    SYSCTL->MCLKCFG |= (2U << SYSCTL_MCLKCFG_FLASHWAIT);

	// 1. Verify that the SYSPLL is disabled (SYSPLLOFF is set in CLKSTATUS)
    if (!IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSPLLOFF)) {
        return;
    }

	// 2. Make sure that SYSOSC is running at base frequency (32MHz); this is a requirement for SYSPLL operation
	// even if HFCLK is used as the SYSPLL reference clock instead of SYSOSC
    if (IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSOSCFREQ)) {
        return;
    }

	// 3. Set SYSOSC as the SYSPLL reference (make sure that the SYSPLLREF bit in the SYSPLLCFG0 register is
	// cleared; this is the default state after reset)
    CLEAR_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_SYSPLLREF);

	// 4. Select a predivider PDIV to /2 (set SYSPLLCFG1.PDIV to 0x01), setting fLOOPIN to 16MHz ( 32 divided by 2 )
    WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_PDIV, 1);

	// 5. Load the PLL parameters into SYSPLLPARAM0 and SYSPLLPARAM1 to support fLOOPIN of 16MH
    SYSCTL->SYSPLLPARAM0 = HWREGW(0x41C4002C); // fLOOPIN = 16MHz bucket
    SYSCTL->SYSPLLPARAM1 = HWREGW(0x41C40030); // PARAM1 = PARAM0 base + 4

	// 6. Set the feedback divider QDIV to 5 (set SYSPLLCFG1.QDIV to 4), giving fVCO=80MHz ( 16MHz multiplied by 5 )
    WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG1, SYSCTL_SYSPLLCFG1_QDIV, 4);

	// 7. Set the SYSPLL output dividers for SYSPLLCLK1 and SYSPLLCLK2X to /2 (set SYSPLLCFG0.RDIVCLK1
	// to 0x0 and SYSPLLCFG0.RDIVCLK2X to 0x1) to get 40MHz and 80MHz at SYSPLLCLK1 and
	// SYSPLLCLK2X, respectively
    WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK1, 0);
    WRITE_FIELD_4BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_RDIVCLK2X, 1);

	// 8. Enable SYSPLLCLK1 and SYSPLLCLK2X outputs by setting the ENABLECLK1 and ENABLECLK2X bits in
	// the SYSPLLCFG0 register
    SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK1);
    SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_ENABLECLK2X);

	// 9. Select SYSPLLCLK2X as the PLL output to the HSCLK mux by setting MCLK2XVCO in the SYSPLLCFG0 register
    SET_BIT(SYSCTL->SYSPLLCFG0, SYSCTL_SYSPLLCFG0_MCLK2XVCO);

	// 10. With SYSOSC enabled and running at base frequency (32MHz, this is the default state out of reset), enable
	// the SYSPLL by setting SYSPLLEN in the HSCLKEN register
    SET_BIT(SYSCTL->HSCLKEN, SYSCTL_HSCLKEN_SYSPLLEN);

	// 11. Wait for the SYSPLLGOOD indication by testing SYSPLLGOOD in the CLKSTATUS register
    while (!IS_BIT_SET(SYSCTL->CLKSTATUS, SYSCTL_CLKSTATUS_SYSPLLGOOD)) {
        // Busy wait for PLL lock
    }

	// 12. Select the SYSPLL as the HSCLK source by ensuring that the HSCLKSEL bit is cleared in the HSCLKCFG
	// register (this is the default state)
    CLEAR_BIT(SYSCTL->HSCLKCFG, SYSCTL_HSCLKCFG_HSCLKSEL);

	// 13. elect the high-speed clock (HSCLK) as the source for MCLK by setting the USEHSCLK bit in the
	// MCLKCFG register. This will switch MCLK from SYSOSC to HSCLK. MCLK is now running from
	// SYSPLLCLK2X at 80MHz.
    SET_BIT(SYSCTL->MCLKCFG, SYSCTL_MCLKCFG_USEHSCLK);
}