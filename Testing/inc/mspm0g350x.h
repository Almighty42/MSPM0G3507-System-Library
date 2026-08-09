#ifndef MSPM0G350X_HEADER
#define MSPM0G350X_HEADER

#include <stddef.h>
#include <stdint.h>

// NOTE: --- Utility macros ---

#define __R volatile const // Read only
#define __W volatile       // Write only
#define __RW volatile      // Read / Write
#define __RC volatile      // Read to Clear
#define __W1S volatile   // Write 1 to Set
#define __W1C volatile   // Write 1 to Clear

#define ENABLE 1
#define DISABLE 0
#define SET ENABLE
#define RESET DISABLE
#define GPIO_PIN_SET SET
#define GPIO_PIN_RESET RESET
#define USART_PIN_SET SET
#define USART_PIN_RESET RESET

#define READ_BIT(reg, bit) ((reg) & (1UL << (bit)))
#define IS_BIT_SET(reg, bit) (READ_BIT(reg, bit) != 0UL)

#define CLEAR_BIT(reg, bit) ((reg) &= ~(1UL << (bit)))
#define CLEAR_FIELD_2BIT(reg, pos) ((reg) &= ~(3U << (pos)))
#define CLEAR_FIELD_4BIT(reg, pos) ((reg) &= ~(0xF << (pos)))
#define CLEAR_BYTE(reg, pos) ((reg) &= ~(0xFF << (pos)))

#define WRITE_FIELD(reg, pos, width, val)                                    \
    do {                                                                     \
        uint32_t mask_ = (1UL << (width)) - 1UL;                             \
        (reg) = ((reg) & ~(mask_ << (pos))) |                                \
                (((uint32_t)(val) & mask_) << (pos));                        \
    } while (0)

#define WRITE_FIELD_2BIT(reg, pos, val) \
                                        do { \
                                        (reg) = ((reg) & ~(0x3U << (pos))) | (((uint32_t)(val) & 0x3U) << (pos)); \
                                        } while (0)

#define WRITE_FIELD_4BIT(reg, pos, val) \
                                        do { \
                                        (reg) = ((reg) & ~(0xFU << (pos))) | (((uint32_t)(val) & 0xFU) << (pos)); \
                                        } while (0)

#define SET_BIT(reg, bit) ((reg) |= (1UL << (bit)))
#define SET_BITS_BY_VAR(reg, val) ((reg) |= (val))
#define SET_BYTE(reg, byte_pos, val)                                \
                                                                    do {    \
                                                                    CLEAR_BYTE((reg), (byte_pos)); \
                                                                    (reg) |= (((uint32_t)(val) & 0xFFU) << (byte_pos)); \
                                                                    } while (0)
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1UL << (bit)))

// NOTE: --- Validation macros ---

#define VALIDATE_RANGE(val, min, max, err_code)                      \
                                                                    do { \
                                                                    if ((val) < (min) || (val) > (max)) {  \
                                                                    return (err_code); \
}                                                              \
} while (0)

#define VALIDATE_PTR(ptr, err_code)                                            \
do {                                                                   \
if ((ptr) == NULL) {                                           \
return (err_code);                                     \
}                                                              \
} while (0)

#define VALIDATE_EN_DI(EN_or_DI, err_code)                                     \
do {                                                                   \
if ((EN_or_DI) != ENABLE && (EN_or_DI) != DISABLE) {           \
return (err_code);                                     \
}                                                              \
} while (0)

#define VALIDATE_BIT_SET(reg, bit, err_code)                                   \
do {                                                                   \
if (!IS_BIT_SET((reg), (bit))) {                               \
return (err_code);                                     \
}                                                              \
} while (0)

#define VALIDATE_ENUM(val, max_val, err_code)                                  \
do {                                                                   \
if ((val) > (max_val)) {                                       \
return (err_code);                                     \
}                                                              \
} while (0)

#define VALIDATE_IN_RANGE(val, range, len, err_code)                           \
do {                                                                   \
uint8_t found = 0;                                             \
for (uint32_t i = 0; i < len; i++) {                           \
if ((val) == (range)[i]) {                             \
found = 1;                                     \
break;                                         \
}                                                      \
}                                                              \
if (!found)                                                    \
return (err_code);                                     \
} while (0)

#define VALIDATE_IRQ_NUMBER(irq, err_code)                                     \
VALIDATE_RANGE((irq), 0U, 83U, err_code)

// NOTE: --- Base Addresses --

#define FLASH_BASE_ADDR 0x00000000UL // Flash / Code region
#define SRAM_BASE_ADDR 0x20000000UL  // SRAM region

// System Control Space ( Core peripherals )

#define SCS_BASE_ADDR 0xE000E000UL
#define SYSTICK_BASE_ADDR (SCS_BASE_ADDR + 0x0010UL)
#define NVIC_BASE_ADDR (SCS_BASE_ADDR + 0x0100UL)
#define SCB_BASE_ADDR (SCS_BASE_ADDR + 0x0D00UL)
#define MPU_BASE_ADDR (SCS_BASE_ADDR + 0x0D90UL)

#define ACTLR_ADDR 0xE000E008UL
#define STIR_ADDR 0xE000EF00UL

// Base addresses of peripherals

#define COMP0_BASE_ADDR 0x40008000UL
#define COMP1_BASE_ADDR 0x4000A000UL
#define COMP2_BASE_ADDR 0x4000C000UL
#define DAC_OUT_BASE_ADDR 0x40018000UL
#define OPA0_BASE_ADDR 0x40020000UL
#define OPA1_BASE_ADDR 0x40022000UL
#define VREF_BASE_ADDR 0x40030000UL
#define WWDT0_BASE_ADDR 0x40080000UL
#define WWDT1_BASE_ADDR 0x40082000UL
#define TIMG0_BASE_ADDR 0x40084000UL
#define TIMG8_BASE_ADDR 0x40090000UL
#define RTC_BASE_ADDR 0x40094000UL
#define GPIO0_BASE_ADDR 0x400A0000UL
#define GPIO1_BASE_ADDR 0x400A2000UL
#define SYSCTL_BASE_ADDR 0x400AF000UL
#define DEBUGSS_BASE_ADDR 0x400C7000UL
#define EVENT_BASE_ADDR 0x400C9000UL
#define NVMNW_BASE_ADDR 0x400CD000UL
#define I2C0_BASE_ADDR 0x400F0000UL
#define I2C1_BASE_ADDR 0x400F2000UL
#define UART1_BASE_ADDR 0x40100000UL
#define UART2_BASE_ADDR 0x40102000UL
#define UART0_BASE_ADDR 0x40108000UL
#define MCPUSS_BASE_ADDR 0x40400000UL
#define MATHACL_BASE_ADDR 0x40410000UL
#define WUC_BASE_ADDR 0x40424000UL
#define IOMUX_BASE_ADDR 0x40428000UL
#define DMA_BASE_ADDR 0x4042A000UL
#define CRC_BASE_ADDR 0x40440000UL
#define AES_BASE_ADDR 0x40442000UL
#define TRNG_BASE_ADDR 0x40444000UL
#define SPI0_BASE_ADDR 0x40468000UL
#define SPI1_BASE_ADDR 0x4046A000UL
#define UART3_BASE_ADDR 0x40500000UL
#define CANFD_BASE_ADDR 0x40508000UL
#define ADC0_BASE_ADDR 0x40000000UL
#define ADC1_BASE_ADDR 0x40002000UL
#define ADC0_ALIAS_BASE_ADDR 0x40556000UL
#define ADC1_ALIAS_BASE_ADDR 0x40558000UL
#define TIMA0_BASE_ADDR 0x40860000UL
#define TIMA1_BASE_ADDR 0x40862000UL
#define TIMG6_BASE_ADDR 0x40868000UL
#define TIMG7_BASE_ADDR 0x4086A000UL
#define TIMG12_BASE_ADDR 0x40870000UL

// NOTE: --- Core Peripheral TypeDefs ---

typedef struct
{
__RW uint32_t CSR;
__RW uint32_t RVR;
__RW uint32_t CVR;
__R uint32_t CALIB;
} SysTick_Type;
#define SysTick ((SysTick_Type*)SYSTICK_BASE_ADDR)

typedef struct
{
__RW uint32_t ISER;
uint32_t res0[31];
__RW uint32_t ICER;
uint32_t res1[31];
__RW uint32_t ISPR;
uint32_t res2[31];
__RW uint32_t ICPR;
uint32_t res3[31];
uint32_t res4[64];
__RW uint32_t IPR[8];

} NVIC_Type;
#define NVIC ((NVIC_Type*)NVIC_BASE_ADDR)

typedef struct
{
__R uint32_t CPUID;
__RW uint32_t ICSR;
__RW uint32_t VTOR;
__RW uint32_t AIRCR;
__RW uint32_t SCR;
__R uint32_t CCR;
uint32_t res0;
__RW uint32_t SHPR2;
__RW uint32_t SHPR3;
} SCB_Type;
#define SCB ((SCB_Type*)SCB_BASE_ADDR)

typedef struct
{
__R uint32_t MPU_TYPE;
__RW uint32_t MPU_CTRL;
__RW uint32_t MPU_RNR;
__RW uint32_t MPU_RBAR;
__RW uint32_t MPU_RASR;
} MPU_Type;
#define MPU ((MPU_Type*)MPU_BASE_ADDR)

// NOTE: --- Other Peripheral TypeDefs ---

typedef struct
{
    uint32_t res0[1032];
    __R uint32_t IIDX;
    uint32_t res1;
    __RW uint32_t IMASK;
    uint32_t res2;
    __R uint32_t RIS;
    uint32_t res3;
    __R uint32_t MIS;
    uint32_t res4;
    __W1S uint32_t ISET;
    uint32_t res5;
    __W1C uint32_t ICLR;
    uint32_t res6;
    __R uint32_t NMIIIDX;
    uint32_t res7[3];
    __R uint32_t NMIRIS;
    uint32_t res8[3];
    __W1S uint32_t NMIISET;
    uint32_t res9;
    __W1C uint32_t NMIICLR;
    uint32_t res10[33];
    __RW uint32_t SYSOSCCFG;
    __RW uint32_t MCLKCFG;
    __RW uint32_t HSCLKEN;
    __RW uint32_t HSCLKCFG;
    __RW uint32_t HFCLKCLKCFG;
    __RW uint32_t LFCLKCFG;
    uint32_t res11[2];
    __RW uint32_t SYSPLLCFG0;
    __RW uint32_t SYSPLLCFG1;
    __RW uint32_t SYSPLLPARAM0;
    __RW uint32_t SYSPLLPARAM1;
    uint32_t res12[2];
    __RW uint32_t GENCLKCFG;
    __RW uint32_t GENCLKEN;
    __RW uint32_t PMODECFG;
    uint32_t res13[3];
    __R uint32_t FCC;
    uint32_t res14[7];
    __RW uint32_t SYSOSCTRIMUSER;
    uint32_t res15;
    __RW uint32_t SRAMBOUNDARY;
    __RW uint32_t SRAMBOUNDARYHIGH;
    __RW uint32_t SYSTEMCFG;
    __RW uint32_t SRAMCFG;
    uint32_t res16[30];
    __RW uint32_t WRITELOCK;
    __R uint32_t CLKSTATUS;
    __R uint32_t SYSSTATUS;
    __R uint32_t DEDERRADDR;
    uint32_t res17[4];
    __RC uint32_t RSTCAUSE;
    uint32_t res18[55];
    __RW uint32_t RESETLEVEL;
    __W uint32_t RESETCMD;
    __RW uint32_t BORTHRESHOLD;
    __W uint32_t BORCLRCMD;
    __W uint32_t SYSOSCFCLCTL;
    __W uint32_t LFXTCTL;
    __W uint32_t EXLFCTL;
    __W uint32_t SHDNIOREL;
    __W uint32_t EXRSTPIN;
    __W uint32_t SYSSTATUSCLR;
    __W uint32_t SWDCFG;
    __W uint32_t FCCCMD;
    uint32_t res19[52];
    __RW uint32_t SHUTDNSTORE0;
    __RW uint32_t SHUTDNSTORE1;
    __RW uint32_t SHUTDNSTORE2;
    __RW uint32_t SHUTDNSTORE3;
    uint32_t res20[780];
    __R uint32_t IDREG;
} SYSCTL_Type;
#define SYSCTL ((SYSCTL_Type*)SYSCTL_BASE_ADDR)


#define SYSCTL_MCLKCFG_MDIV                         0
#define SYSCTL_MCLKCFG_UDIV                         4
#define SYSCTL_MCLKCFG_FLASHWAIT                    8
#define SYSCTL_MCLKCFG_USEMFTICK                    12
#define SYSCTL_MCLKCFG_USEHSCLK                     16
#define SYSCTL_MCLKCFG_USELFCLK                     20
#define SYSCTL_MCLKCFG_STOPCLKSTBY                  21
#define SYSCTL_MCLKCFG_MCLKDEADCHK                  22

#define SYSCTL_HSCLKCFG_HSCLKSEL                    0

#define SYSCTL_SYSPLLPARAM0_STARTTIME               0
#define SYSCTL_SYSPLLPARAM0_STARTTIMELP             8
#define SYSCTL_SYSPLLPARAM0_CPCURRENT               16
#define SYSCTL_SYSPLLPARAM0_CAPBVAL                 24
#define SYSCTL_SYSPLLPARAM0_CAPBOVERRIDE            31

#define SYSCTL_SYSPLLPARAM1_LPFCAPA                 0
#define SYSCTL_SYSPLLPARAM1_LPFRESA                 8
#define SYSCTL_SYSPLLPARAM1_LPFRESC                 24

#define SYSCTL_HSCLKEN_HFXTEN                       0
#define SYSCTL_HSCLKEN_SYSPLLEN                     8
#define SYSCTL_HSCLKEN_USEEXTHFCLK                  16

#define SYSCTL_SYSPLLCFG1_PDIV                      0
#define SYSCTL_SYSPLLCFG1_QDIV                      8

#define SYSCTL_SYSPLLCFG0_SYSPLLREF                 0
#define SYSCTL_SYSPLLCFG0_MCLK2XVCO                 1
#define SYSCTL_SYSPLLCFG0_ENABLECLK0                4
#define SYSCTL_SYSPLLCFG0_ENABLECLK1                5
#define SYSCTL_SYSPLLCFG0_ENABLECLK2X               6
#define SYSCTL_SYSPLLCFG0_RDIVCLK0                  8
#define SYSCTL_SYSPLLCFG0_RDIVCLK1                  12
#define SYSCTL_SYSPLLCFG0_RDIVCLK2X                 16

#define SYSCTL_SYSPLLCFG1_PDIV                      0

#define SYSCTL_CLKSTATUS_SYSOSCFREQ                 0
#define SYSCTL_CLKSTATUS_HSCLKMUX                   4
#define SYSCTL_CLKSTATUS_LFCLKMUX                   6
#define SYSCTL_CLKSTATUS_HFCLKGOOD                  8
#define SYSCTL_CLKSTATUS_SYSPLLGOOD                 9
#define SYSCTL_CLKSTATUS_LFXTGOOD                   10
#define SYSCTL_CLKSTATUS_LFOSCGOOD                  11
#define SYSCTL_CLKSTATUS_HSCLKSOFF                  12
#define SYSCTL_CLKSTATUS_HFCLKOFF                   13
#define SYSCTL_CLKSTATUS_SYSPLLOFF                  14
#define SYSCTL_CLKSTATUS_CURHSCLKSEL                16
#define SYSCTL_CLKSTATUS_CURMCLKSEL                 17
#define SYSCTL_CLKSTATUS_HSCLKDEAD                  20
#define SYSCTL_CLKSTATUS_HSCLKGOOD                  21
#define SYSCTL_CLKSTATUS_LFCLKFAIL                  23
#define SYSCTL_CLKSTATUS_FCLMODE                    24
#define SYSCTL_CLKSTATUS_FCCDONE                    25
#define SYSCTL_CLKSTATUS_HFCLKBLKUPD                28
#define SYSCTL_CLKSTATUS_SYSPLLBLKUPD               29
#define SYSCTL_CLKSTATUS_OPAMPCLKERR                30
#define SYSCTL_CLKSTATUS_ANACLKERR                  31

#define HWREGW(x) (*((volatile uint32_t *)(x)))

// TODO:
// COMP, DAC, OPA, VREF, WWDT, TIM, RTC, GPIO, DEBUG, EVENT, NVMNW, I2C,
// UART, MCPUSS, MATHACL, WUC, IOMUX, DMA, CRC, AES, TRNG, SPI, CAN, ADC

// NOTE: --- Interrupt Definitions ---

typedef enum IRQn {
NonMaskableInt_IRQn = -14,/* 2  Non Maskable Interrupt */
HardFault_IRQn = -13,/* 3  Hard Fault Interrupt */
SVCall_IRQn = -5,/* 11 SV Call Interrupt */
PendSV_IRQn = -2,/* 14 Pend SV Interrupt */
SysTick_IRQn = -1,/* 15 System Tick Interrupt */
SYSCTL_INT_IRQn = 0,/* 16 SYSCTL_INT Interrupt */
WWDT1_INT_IRQn = 0,/* 16 WWDT1_INT Interrupt */
WWDT0_INT_IRQn = 0,/* 16 WWDT0_INT Interrupt */
FLASHCTL_INT_IRQn = 0,/* 16 FLASHCTL_INT Interrupt */
DEBUGSS_INT_IRQn = 0,/* 16 DEBUGSS_INT Interrupt */
GPIOB_INT_IRQn = 1,/* 17 GPIOB_INT Interrupt */
GPIOA_INT_IRQn = 1,/* 17 GPIOA_INT Interrupt */
TRNG_INT_IRQn = 1,/* 17 TRNG_INT Interrupt */
COMP0_INT_IRQn = 1,/* 17 COMP0_INT Interrupt */
COMP1_INT_IRQn = 1,/* 17 COMP1_INT Interrupt */
COMP2_INT_IRQn = 1,/* 17 COMP2_INT Interrupt */
TIMG8_INT_IRQn = 2,/* 18 TIMG8_INT Interrupt */
UART3_INT_IRQn = 3,/* 19 UART3_INT Interrupt */
ADC0_INT_IRQn = 4,/* 20 ADC0_INT Interrupt */
ADC1_INT_IRQn = 5,/* 21 ADC1_INT Interrupt */
CANFD0_INT_IRQn = 6,/* 22 CANFD0_INT Interrupt */
DAC0_INT_IRQn = 7,/* 23 DAC0_INT Interrupt */
SPI0_INT_IRQn = 9,/* 25 SPI0_INT Interrupt */
SPI1_INT_IRQn = 10,/* 26 SPI1_INT Interrupt */
UART1_INT_IRQn = 13,/* 29 UART1_INT Interrupt */
UART2_INT_IRQn = 14,/* 30 UART2_INT Interrupt */
UART0_INT_IRQn = 15,/* 31 UART0_INT Interrupt */
TIMG0_INT_IRQn = 16,/* 32 TIMG0_INT Interrupt */
TIMG6_INT_IRQn = 17,/* 33 TIMG6_INT Interrupt */
TIMA0_INT_IRQn = 18,/* 34 TIMA0_INT Interrupt */
TIMA1_INT_IRQn = 19,/* 35 TIMA1_INT Interrupt */
TIMG7_INT_IRQn = 20,/* 36 TIMG7_INT Interrupt */
TIMG12_INT_IRQn = 21,/* 37 TIMG12_INT Interrupt */
I2C0_INT_IRQn = 24,/* 40 I2C0_INT Interrupt */
I2C1_INT_IRQn = 25,/* 41 I2C1_INT Interrupt */
AES_INT_IRQn = 28,/* 44 AES_INT Interrupt */
RTC_INT_IRQn = 30,/* 46 RTC_INT Interrupt */
DMA_INT_IRQn = 31,/* 47 DMA_INT Interrupt */
} IRQn_Type;

#endif
