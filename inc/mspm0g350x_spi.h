#ifndef MSPM0G3507_SPI_H
#define MSPM0G3507_SPI_H

#include "mspm0g350x_startup.h"
#include <stdint.h>

// // ============================================================================
// // SPI0 base 0x40468000U
// // ============================================================================
//
// typedef struct {
//     __I  uint32_t IIDX;                     // (@ 0x00001080) Interrupt index register
//          uint32_t RESERVED0;
//     __IO uint32_t IMASK;                    // (@ 0x00001088) Interrupt mask
//          uint32_t RESERVED1;
//     __I  uint32_t RIS;                      // (@ 0x00001090) Raw interrupt status
//          uint32_t RESERVED2;
//     __I  uint32_t MIS;                      // (@ 0x00001098) Masked interrupt status
//          uint32_t RESERVED3;
//     __O  uint32_t ISET;                     // (@ 0x000010A0) Interrupt set
//          uint32_t RESERVED4;
//     __O  uint32_t ICLR;                     // (@ 0x000010A8) Interrupt clear
// } SPI_DMA_TRIG_TX_Regs;
//
// typedef struct {
//     __I  uint32_t IIDX;                     // (@ 0x00001050) Interrupt index register
//          uint32_t RESERVED0;
//     __IO uint32_t IMASK;                    // (@ 0x00001058) Interrupt mask
//          uint32_t RESERVED1;
//     __I  uint32_t RIS;                      // (@ 0x00001060) Raw interrupt status
//          uint32_t RESERVED2;
//     __I  uint32_t MIS;                      // (@ 0x00001068) Masked interrupt status
//          uint32_t RESERVED3;
//     __O  uint32_t ISET;                     // (@ 0x00001070) Interrupt set
//          uint32_t RESERVED4;
//     __O  uint32_t ICLR;                     // (@ 0x00001078) Interrupt clear
// } SPI_DMA_TRIG_RX_Regs;
//
// typedef struct{
//     __I  uint32_t IIDX;                     // (@ 0x00001020) Interrupt index register
//          uint32_t RESERVED0;
//     __IO uint32_t IMASK;                    // (@ 0x00001028) Interrupt mask
//          uint32_t RESERVED1;
//     __I  uint32_t RIS;                      // (@ 0x00001030) Raw interrupt status
//          uint32_t RESERVED2;
//     __I  uint32_t MIS;                      // (@ 0x00001038) Masked interrupt status
//          uint32_t RESERVED3;
//     __O  uint32_t ISET;                     // (@ 0x00001040) Interrupt set
//          uint32_t RESERVED4;
//     __O  uint32_t ICLR;                     // (@ 0x00001048) Interrupt clear
// } SPI_CPU_INT_Regs;
//
// typedef struct{
//     __IO uint32_t PWREN;                    // (@ 0x00000800) Power enable
//     __O  uint32_t RSTCTL;                   // (@ 0x00000804) Reset control
//     __IO uint32_t CLKCFG;                   // (@ 0x00000808) Peripheral clock configuration register
//          uint32_t RESERVED0[2];
//     __I  uint32_t STAT;                     // (@ 0x00000814) Status register
// } SPI_GPRCM_Regs;
//
// typedef struct{
//          uint32_t RESERVED0[512];
//     SPI_GPRCM_Regs GPRCM;                   // (@ 0x00000800) 
//          uint32_t RESERVED1[506];
//     __IO uint32_t CLKDIV;                   // (@ 0x00001000) Clock Divider
//     __IO uint32_t CLKSEL;                   // (@ 0x00001004) Clock Select for Ultra Low Power Peripherals
//          uint32_t RESERVED2[4];
//     __IO uint32_t PDBGCTL;                  // (@ 0x00001018) Peripheral Debug Control
//          uint32_t RESERVED3;
//     SPI_CPU_INT_Regs CPU_INT;               // (@ 0x00001020)
//          uint32_t RESERVED4;
//     SPI_DMA_TRIG_RX_Regs DMA_TRIG_RX;       // (@ 0x00001050)
//          uint32_t RESERVED5;
//     SPI_DMA_TRIG_TX_Regs DMA_TRIG_TX;       // (@ 0x00001080)
//          uint32_t RESERVED6[13];
//     __IO uint32_t EVT_MODE;                 // (@ 0x000010E0) Event mode
//     __IO uint32_t INTCTL;                   // (@ 0x000010E4) Interrupt Control register
//          uint32_t RESERVED7[6];
//     __IO uint32_t CTL0;                     // (@ 0x00001100) SPI Control register 0
//     __IO uint32_t CTL1;                     // (@ 0x00001104) SPI Control register 1
//     __IO uint32_t CLKCTL;                   // (@ 0x00001108) Clock prescaler and divider register
//     __IO uint32_t IFLS;                     // (@ 0x0000110C) Interrupt FIFO Level Select Register
//     __I  uint32_t STAT;                     // (@ 0x00001110) Status register
//          uint32_t RESERVED8[7];
//     __I  uint32_t RXDATA;                   // (@ 0x00001130) RXDATA Register
//          uint32_t RESERVED9[3];
//     __IO uint32_t TXDATA;                   // (@ 0x00001140) TXDATA Register
// } SPI_Regs;
//
// #define SPI ((SPI_Regs *) SPI0_BASE)

// USAGE: --- @SPI_CLKDIV ---

typedef enum{
     SPI_CLKDIV_NO_DIV = 0U,
     SPI_CLKDIV_2_DIV = 1U,
     SPI_CLKDIV_3_DIV = 2U,
     SPI_CLKDIV_4_DIV = 3U,
     SPI_CLKDIV_5_DIV = 4U,
     SPI_CLKDIV_6_DIV = 5U,
     SPI_CLKDIV_7_DIV = 6U,
     SPI_CLKDIV_8_DIV = 7U,
} spi_clock_div_t;

#define SPI_CLKDIV_RATIO_OFS          (0U)
#define SPI_CLKDIV_RATIO_WIDTH        (3U)

// USAGE: --- @SPI_CLKSEL ---

typedef enum{
     SPI_CLKSEL_LFCLK  = 1,
     SPI_CLKSEL_MFCLK  = 2,
     SPI_CLKSEL_SYSCLK = 3,
} spi_clock_t;

#define SPI_CLKSEL_LFCLK_OFS          (1U)
#define SPI_CLKSEL_MFCLK_OFS          (2U)
#define SPI_CLKSEL_SYSCLK_OFS         (3U)


// USAGE: --- @SPI_DSS ---

typedef enum{
     SPI_DSS_4B = 3U,
     SPI_DSS_5B = 4U,
     SPI_DSS_6B = 5U,
     SPI_DSS_7B = 6U,
     SPI_DSS_8B = 7U,
     SPI_DSS_9B = 8U,
     SPI_DSS_10B = 9U,
     SPI_DSS_11B = 10U,
     SPI_DSS_12B = 11U,
     SPI_DSS_13B = 12U,
     SPI_DSS_14B = 13U,
     SPI_DSS_15B = 14U,
     SPI_DSS_16B = 15U,
} spi_data_size_t;

// USAGE: --- @SPI_MODE ---

typedef enum{
     SPI_PERIPHERAL_MODE = 0U,
     SPI_CONTROLLER_MODE = 1U,
} spi_mode_t;

// USAGE: --- @SPI_TRANSFER_MODE ---

typedef enum{
     SPI_MODE_0 = 0U,    // SPO = 0, SPH = 0
     SPI_MODE_1 = 1U,    // SPO = 0, SPH = 1
     SPI_MODE_2 = 2U,    // SPO = 1, SPH = 0
     SPI_MODE_3 = 3U,    // SPO = 1, SPH = 1
} spi_transfer_mode_t;

// USAGE: --- @SPI_FRAME_FORMAT_MODE ---

typedef enum{
     SPI_FRF_MOTOROLA_3WIRE        = 0U,
     SPI_FRF_MOTOROLA_4WIRE        = 1U,
     SPI_FRF_TI_SYNC_SERIAL_FF     = 2U,
     SPI_FRF_NATIONAL_MICROWIRE_FF = 3U,
} spi_frame_format_t;

// USAGE: --- @SPI_BIT_ORDER ---

typedef enum{
     SPI_BIT_ORDER_LSB_FIRST = 0U,
     SPI_BIT_ORDER_MSB_FIRST = 1U,
} spi_bit_order_t;

// USAGE: --- @SPI_CS_LINE ---

typedef enum{
     SPI_CS_LINE_0 = 0U,
     SPI_CS_LINE_1 = 1U,
     SPI_CS_LINE_2 = 2U,
     SPI_CS_LINE_3 = 3U,
} spi_cs_line_t;

// RSTCTL Macros

#define SPI_RSTCTL_KEY_UNLOCK_W       (0xB1000000U)
#define SPI_RSTCTL_RESETASSERT        (1U)


// CTL0 Macros

#define SPI_CTL0_DSS_OFS              (0U)
#define SPI_CTL0_DSS_WIDTH            (5U)
#define SPI_CTL0_FRF_OFS              (5U)
#define SPI_CTL0_FRF_WIDTH            (2U)
#define SPI_CTL0_SPO_OFS              (8U)
#define SPI_CTL0_SPH_OFS              (9U)
#define SPI_CTL0_CSSEL_OFS            (12U)
#define SPI_CTL0_CSSEL_WIDTH          (2U)

// CTL1 Macros

#define SPI_CTL1_ENABLE_OFS           (0U)
#define SPI_CTL1_CP_OFS               (2U)
#define SPI_CTL1_MSB_OFS              (4U)

// CLKCTL Macros

#define SPI_CLKCTL_SCR_OFS            (0U)
#define SPI_CLKCTL_SCR_WIDTH          (10U)
#define SPI_CLKCTL_DSAMPLE_OFS        (28U)
#define SPI_CLKCTL_DSAMPLE_WIDTH      (4U)

// STAT Macros

#define SPI_STAT_TFE_OFS              (0U)
#define SPI_STAT_TNF_OFS              (1U)
#define SPI_STAT_RFE_OFS              (2U)
#define SPI_STAT_RNF_OFS              (3U)
#define SPI_STAT_BUSY_OFS             (4U)


// ========================== Configuration Functions ===========================
void spi_reset                     (void);
void spi_enable_power              (void);
void spi_disable_power             (void);
void spi_set_clock_configuration   (spi_clock_t clock, spi_clock_div_t divider);
void spi_set_clock_prescaler       (uint32_t scr);
void spi_set_cs_line               (spi_cs_line_t line);
void spi_set_mode                  (spi_mode_t mode);
void spi_set_transfer_mode         (spi_transfer_mode_t transfer_mode);
void spi_set_frame_format          (spi_frame_format_t frame_format);
void spi_set_data_size             (spi_data_size_t size);
void spi_set_bit_order             (spi_bit_order_t order);
void spi_enable                    (void);
void spi_disable                   (void);

void spi_cs_enable                 (uint32_t pin);
void spi_cs_disable                (uint32_t pin);
uint8_t spi_transfer_byte              (uint8_t data);
void spi_write_buffer              (uint32_t address, const uint8_t *data, uint32_t length, uint8_t WRITE_CMD);
void spi_read_buffer               (uint32_t address, uint8_t *data, uint32_t length, uint8_t READ_CMD);


#endif
