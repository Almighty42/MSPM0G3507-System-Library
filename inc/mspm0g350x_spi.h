#ifndef MSPM0G3507_SPI_H
#define MSPM0G3507_SPI_H

#include "mspm0g350x_startup.h"
#include <stdint.h>

// Polling / blocking value used in loops in functions used by the driver
#define SPI_SOFTWARE_TIMEOUT	1000000

// #define SPI_CTL1_ENABLE_OFS         (0U)
// #define SPI_CTL1_ENABLE_MASK        ((uint32_t)0x00000001U)  
//
// #define SPI_CTL1_ENABLE_DISABLE     ((uint32_t)0x00000000U)
// #define SPI_CTL1_ENABLE_ENABLE      ((uint32_t)0x00000001U)
//
// #define SPI_STAT_RFE_OFS            (2U)
// #define SPI_STAT_RFE_MASK           ((uint32_t)0x00000004U)   
//
// #define SPI_STAT_RFE_NOT_EMPTY      ((uint32_t)0x00000000U)
// #define SPI_STAT_RFE_EMPTY          ((uint32_t)0x00000004U)

// NOTE: --- Structures for SPI ---

typedef struct
{
	uint8_t SPI_Device_Mode;					// Possible values from @SPI_DEVICE_MODE
	uint8_t SPI_Clock_Source;					// Possible values from @SPI_CLOCK_SOURCE
	uint8_t SPI_Clock_Divide_Ratio;					// Possible values from @SPI_CLOCK_DIVIDE_RATIO
	uint16_t SPI_Clock_Prescaler;					// Possible values from 0..1023
	uint8_t SPI_Data_Width;						// Possible values from @SPI_DATA_WIDTH
	uint8_t SPI_CPOL;						// Possible values from @SPI_CPOL
	uint8_t SPI_CPHA;						// Possible values from @SPI_CPHA
	uint8_t SPI_CS_Select;						// Possible values from @SPI_CS_SELECT
	uint8_t SPI_MSB;						// Possible values from @SPI_MSB
} spi_config_t;

typedef struct
{
	spi_type* p_SPIx;						// Holds the base address of the SPI peripheral 
	spi_config_t spi_config;					// Holds SPI peripheral configuration settings
	uint8_t *p_tx_buffer;						// Stores application Tx buffer address
	uint8_t *p_rx_buffer;						// Stores application Rx buffer address
	uint32_t tx_len;						// Tx length
	uint32_t rx_len;						// Rx length
} spi_handle_t;

typedef enum {
	SPI_OK = 0,							// Success
	SPI_ERROR_INVALID_STATE,					// Invalid state of a argument
	SPI_ERROR_NULL_PTR,						// NULL pointer passed
	SPI_ERROR_INVALID_PORT,						// Invalid SPI port address
	SPI_ERROR_INVALID_IRQ,						// Invalid IRQ number
	SPI_ERROR_INVALID_MODE,						// Mode value out of range
	SPI_ERROR_INVALID_CLOCK_SRC,					// Clock source is incorrect
	SPI_ERROR_INVALID_CLK_DIV,					// Clock divide ratio value out of range
	SPI_ERROR_INVALID_CLK_PRESC,					// Clock prescaler value out of range
	SPI_ERROR_INVALID_DATA_WIDTH,					// Data width value out of range
	SPI_ERROR_INVALID_CPOL,						// CPOL value out of range
	SPI_ERROR_INVALID_CPHA,						// CPHA value out of range
	SPI_ERROR_INVALID_CS,						// CS value out of range
	SPI_ERROR_INVALID_MSB,						// MSB value out of range
	SPI_ERROR_TIMEOUT,						// SPI polling timeout
	SPI_ERROR_NOT_ENABLED,						// SPI not enabled, but has to be 
	SPI_ERROR_INVALID_LEN,						// Len is invalid
	SPI_ERROR_RX_OVERFLOW,						// RX FIFO overflow detected
	SPI_ERROR_TX_UNDERFLOW,						// TX FIFO underflow detected
	SPI_BUSY							// SPI Tx / Rx busy
} spi_status_t;

// USAGE: --- @SPI_DEVICE_MODE ---

#define SPI_DEVICE_MODE_PERIPHERAL		1
#define SPI_DEVICE_MODE_CONTROLLER		0

// USAGE: --- @SPI_CLOCK_SOURCE ---

#define SPI_CLOCK_SRC_BUSCLK 0U
#define SPI_CLOCK_SRC_MFCLK  1U
#define SPI_CLOCK_SRC_LFCLK  2U

// USAGE: --- @SPI_CLOCK_DIVIDE_RATIO ---

#define SPI_SCLK_SPEED_DIV_1			0
#define SPI_SCLK_SPEED_DIV_2			1
#define SPI_SCLK_SPEED_DIV_3			2
#define SPI_SCLK_SPEED_DIV_4			3
#define SPI_SCLK_SPEED_DIV_5			4
#define SPI_SCLK_SPEED_DIV_6			5
#define SPI_SCLK_SPEED_DIV_7			6
#define SPI_SCLK_SPEED_DIV_8			7

// USAGE: --- @SPI_DATA_WIDTH ---

typedef enum {
    SPI_DATA_WIDTH_7  = 0x6,   
    SPI_DATA_WIDTH_8  = 0x7,
    SPI_DATA_WIDTH_9  = 0x8,
    SPI_DATA_WIDTH_10 = 0x9,
    SPI_DATA_WIDTH_11 = 0xA,
    SPI_DATA_WIDTH_12 = 0xB,
    SPI_DATA_WIDTH_13 = 0xC,
    SPI_DATA_WIDTH_14 = 0xD,
    SPI_DATA_WIDTH_15 = 0xE,
    SPI_DATA_WIDTH_16 = 0xF 
} spi_data_width_t;

// USAGE: --- @SPI_CPOL ---

#define SPI_CPOL_HIGH				1
#define SPI_CPOL_LOW				0

// USAGE: --- @SPI_CPHA ---

#define SPI_CPHA_HIGH				1
#define SPI_CPHA_LOW				0

// USAGE: --- @SPI_CS_SELECT ---

#define SPI_CS_0				0
#define SPI_CS_1				1

// USAGE: --- @SPI_MSB ---

#define SPI_MSB_MSB				0
#define SPI_MSB_LSB				1

// NOTE: --- SPI Validation macros ---

#define VALIDATE_SPI_PORT(port) do { \
    if ((port) == NULL || \
        !((port) == SPI0 || (port) == SPI1)) { \
        return SPI_ERROR_INVALID_PORT; \
    } \
} while(0)
#define VALIDATE_SPI_DEVICE_MODE(mode)		VALIDATE_ENUM((mode), SPI_DEVICE_MODE_PERIPHERAL, SPI_ERROR_INVALID_MODE)
#define VALIDATE_SPI_CLOCK_SRC(src) 		VALIDATE_ENUM((src), SPI_CLOCK_SRC_LFCLK, SPI_ERROR_INVALID_CLOCK_SRC)
#define VALIDATE_SPI_CLOCK_DIVIDE_RATIO(ratio)	VALIDATE_ENUM((ratio), SPI_SCLK_SPEED_DIV_8, SPI_ERROR_INVALID_CLK_DIV)
#define VALIDATE_SPI_CLOCK_PRESCALER(prescaler)	VALIDATE_ENUM((prescaler), 1023, SPI_ERROR_INVALID_CLK_PRESC)
#define VALIDATE_SPI_DATA_WIDTH(width, err_code) do { \
    if ((width) < SPI_DATA_WIDTH_7 || (width) > SPI_DATA_WIDTH_16) { \
        return (err_code); \
    } \
} while (0)
#define VALIDATE_SPI_CPOL(flow)			VALIDATE_ENUM((flow), SPI_CPOL_HIGH, SPI_ERROR_INVALID_CPOL)
#define VALIDATE_SPI_CPHA(flow)			VALIDATE_ENUM((flow), SPI_CPHA_HIGH, SPI_ERROR_INVALID_CPHA)
#define VALIDATE_SPI_CS_SELECTOR(cs_sel)	VALIDATE_ENUM((cs_sel), SPI_CS_1, SPI_ERROR_INVALID_CS)
#define VALIDATE_SPI_MSB(msb)			VALIDATE_ENUM((msb), SPI_MSB_LSB, SPI_ERROR_INVALID_MSB)
#define VALIDATE_SPI_ENABLED(port) 		VALIDATE_BIT_SET((port)->CTL1, SPI_CTL1_ENABLE, SPI_ERROR_NOT_ENABLED)

// NOTE: --- SPI_PWREN bitfields and other ---

#define SPI_PWREN_KEY_OFS         (24)
#define SPI_PWREN_KEY_MASK        ((uint32_t)0xFF000000U)
#define SPI_PWREN_KEY_UNLOCK_W    ((uint32_t)0x26000000U)
#define SPI_PWREN_ENABLE_OFS      (0)
#define SPI_PWREN_ENABLE_MASK     ((uint32_t)0x00000001U)
#define SPI_PWREN_ENABLE_DISABLE  ((uint32_t)0x00000000U)
#define SPI_PWREN_ENABLE_ENABLE   ((uint32_t)0x00000001U)

// NOTE: --- SPI_CLKDIV bitfields and other ---

#define SPI_CLKDIV_RATIO_OFS      (0U)
#define SPI_CLKDIV_RATIO_MASK     ((uint32_t)0x00000007U)
#define SPI_CLKCTL_SCR_OFS    (0U)
#define SPI_CLKCTL_SCR_MASK   (0x000003FFU)

// NOTE: --- SPI_CLKSEL bitfields and other ---

#define SPI_CLKSEL_LFCLK_SEL_OFS   (1U)
#define SPI_CLKSEL_LFCLK_SEL_MASK  ((uint32_t)0x00000002U)
#define SPI_CLKSEL_MFCLK_SEL_OFS   (2U)
#define SPI_CLKSEL_MFCLK_SEL_MASK  ((uint32_t)0x00000004U)
#define SPI_CLKSEL_SYSCLK_SEL_OFS  (3U)
#define SPI_CLKSEL_SYSCLK_SEL_MASK ((uint32_t)0x00000008U)

// NOTE: --- SPI_RSTCTL bitfields and other ---

#define SPI_RSTCTL_KEY_UNLOCK_W          ((uint32_t)0xB1000000U)
#define SPI_RSTCTL_RESETASSERT_ASSERT    ((uint32_t)0x00000001U)
#define SPI_GPRCM_STAT_RESETSTKY_MASK    ((uint32_t)0x00000001U)
#define SPI_RSTCTL_RESETSTKYCLR_CLR      ((uint32_t)0x00000002U)

// NOTE: --- Bit position definitions SPI_STAT ---

#define SPI_SR_TFE_STATE   			0
#define SPI_SR_TNF_STATE   			1
#define SPI_SR_RFE_STATE   			2
#define SPI_SR_RNF_STATE   			3
#define SPI_SR_BUSY_STATE   			4

// NOTE: --- SPI_STAT bitfields ---

#define SPI_STAT_TFE_OFS         (0U)
#define SPI_STAT_TFE_MASK        ((uint32_t)0x00000001U)   

#define SPI_STAT_TNF_OFS         (1U)
#define SPI_STAT_TNF_MASK        ((uint32_t)0x00000002U)  

#define SPI_STAT_RFE_OFS         (2U)
#define SPI_STAT_RFE_MASK        ((uint32_t)0x00000004U) 

#define SPI_STAT_RNF_OFS         (3U)
#define SPI_STAT_RNF_MASK        ((uint32_t)0x00000008U)

#define SPI_STAT_BUSY_OFS        (4U)
#define SPI_STAT_BUSY_MASK       ((uint32_t)0x00000010U)

// NOTE: --- Bit position definitions SPI_CTL0 ---

#define SPI_CTL0_CSCLR				14
#define SPI_CTL0_CSSEL				12
#define SPI_CTL0_SPH				9
#define SPI_CTL0_SPO				8
#define SPI_CTL0_PACKEN				7
#define SPI_CTL0_FRF				5
#define SPI_CTL0_DSS				0

// NOTE: --- SPI_CTL0 bitfields ---

#define SPI_CTL0_DSS_OFS         (0U)
#define SPI_CTL0_DSS_WIDTH       (5U)
#define SPI_CTL0_DSS_MASK        ((uint32_t)0x0000001FU)

#define SPI_CTL0_FRF_OFS         (5U)
#define SPI_CTL0_FRF_WIDTH       (2U)                  
#define SPI_CTL0_FRF_MASK        ((uint32_t)0x00000060U)

#define SPI_CTL0_PACKEN_OFS      (7U)
#define SPI_CTL0_PACKEN_WIDTH    (1U)                  
#define SPI_CTL0_PACKEN_MASK     ((uint32_t)0x00000080U)

#define SPI_CTL0_SPO_OFS         (8U)
#define SPI_CTL0_SPO_MASK        ((uint32_t)0x00000100U)

#define SPI_CTL0_SPH_OFS         (9U)
#define SPI_CTL0_SPH_MASK        ((uint32_t)0x00000200U)   

#define SPI_CTL0_CSSEL_OFS       (12U)
#define SPI_CTL0_CSSEL_WIDTH     (2U)                     
#define SPI_CTL0_CSSEL_MASK      ((uint32_t)0x00003000U) 

#define SPI_CTL0_CSCLR_OFS       (14U)
#define SPI_CTL0_CSCLR_WIDTH     (1U)                   
#define SPI_CTL0_CSCLR_MASK      ((uint32_t)0x00004000U)



// NOTE: --- Bit position definitions SPI_CTL1 ---

#define SPI_CTL1_ENABLE				0
#define SPI_CTL1_LBM				1
#define SPI_CTL1_CP				2
#define SPI_CTL1_POD				3
#define SPI_CTL1_MSB				4
#define SPI_CTL1_PREN				5
#define SPI_CTL1_PES				6
#define SPI_CTL1_PTEN				8
#define SPI_CTL1_CDENABLE			11
#define SPI_CTL1_CDMODE				12
#define SPI_CTL1_REPEATTX			16
#define SPI_CTL1_RXTIMEOUT			24

// NOTE: --- SPI_CTL1 bitfields ---

#define SPI_CTL1_ENABLE_OFS      (0U)
#define SPI_CTL1_ENABLE_MASK     ((uint32_t)0x00000001U)   

#define SPI_CTL1_LBM_OFS         (1U)
#define SPI_CTL1_LBM_MASK        ((uint32_t)0x00000002U)  

#define SPI_CTL1_CP_OFS          (2U)
#define SPI_CTL1_CP_MASK         ((uint32_t)0x00000004U) 

#define SPI_CTL1_POD_OFS         (3U)
#define SPI_CTL1_POD_MASK        ((uint32_t)0x00000008U)

#define SPI_CTL1_MSB_OFS         (4U)
#define SPI_CTL1_MSB_MASK        ((uint32_t)0x00000010U)   

#define SPI_CTL1_PREN_OFS        (5U)
#define SPI_CTL1_PREN_MASK       ((uint32_t)0x00000020U)  

#define SPI_CTL1_PES_OFS         (6U)
#define SPI_CTL1_PES_MASK        ((uint32_t)0x00000040U) 

#define SPI_CTL1_PTEN_OFS        (8U)
#define SPI_CTL1_PTEN_MASK       ((uint32_t)0x00000100U) 

#define SPI_CTL1_CDENABLE_OFS    (11U)
#define SPI_CTL1_CDENABLE_MASK   ((uint32_t)0x00000800U)

#define SPI_CTL1_CDMODE_OFS      (12U)
#define SPI_CTL1_CDMODE_WIDTH    (1U)                  
#define SPI_CTL1_CDMODE_MASK     ((uint32_t)0x00001000U)

#define SPI_CTL1_REPEATTX_OFS    (16U)
#define SPI_CTL1_REPEATTX_MASK   ((uint32_t)0x00010000U)

#define SPI_CTL1_RXTIMEOUT_OFS   (24U)
#define SPI_CTL1_RXTIMEOUT_WIDTH (8U)                  
#define SPI_CTL1_RXTIMEOUT_MASK  ((uint32_t)0xFF000000U)

// Peripheral clock setup
spi_status_t spi_peri_clk_control(spi_type* p_spi_x, uint8_t EN_or_DI);

// Init / de-init
spi_status_t spi_init(spi_handle_t *p_spi_handle);
spi_status_t spi_de_init(spi_type* p_spi_x);

// Data send / receive ( polling ) 
spi_status_t spi_write_data_pl(spi_handle_t* p_spi_handle, const uint8_t* p_tx_buffer, uint32_t len, uint32_t timeout);
spi_status_t spi_read_data_pl(spi_handle_t* p_spi_handle, uint8_t* p_rx_buffer, uint32_t len, uint32_t timeout);

// Peripheral control API
spi_status_t spi_peri_control(spi_type* p_spi_x, uint8_t EN_or_DI);

#endif
