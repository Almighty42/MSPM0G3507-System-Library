#ifndef MSPM0G3507_SPI_H
#define MSPM0G3507_SPI_H

#include "mspm0g350x_startup.h"
#include <stdint.h>

// NOTE: --- Structures for SPI ---

typedef struct
{
	uint8_t SPI_Device_Mode;					// Possible values from @SPI_DEVICE_MODE
	uint8_t SPI_Clock_Source;					// Possible values from @SPI_CLOCK_SOURCE
	uint8_t SPI_SCLK_Speed;						// Possible values from @SPI_SCLK_SPEED
	uint8_t SPI_Data_Width;						// Possible values from @SPI_DATA_WIDTH
	uint8_t SPI_CPOL;						// Possible values from @SPI_CPOL
	uint8_t SPI_CPHA;						// Possible values from @SPI_CPHA
	uint8_t SPI_CS_Select;						// Possible values from @SPI_CS_SELECT
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
	SPI_ERROR_INVALID_SCLK_SPEED,					// SCLK speed value out of range
	SPI_ERROR_INVALID_DATA_WIDTH,					// Data width value out of range
	SPI_ERROR_INVALID_CPOL,						// CPOL value out of range
	SPI_ERROR_INVALID_CPHA,						// CPHA value out of range
	SPI_ERROR_INVALID_CS,						// CS value out of range
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

// USAGE: --- @SPI_SCLK_SPEED ---

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

// WARNING: Hardware supports selecting 0-3, but this particular board only has SPI0 and SPI1, so for this reason the limitation is in place

#define SPI_CS_0				0
#define SPI_CS_1				1

// NOTE: --- SPI Validation macros ---

#define VALIDATE_SPI_PORT(port)		do { \
							if ((port) == NULL || \
							!((port) == SPI0 || (port) == SPI1 ) { \
								return SPI_ERROR_INVALID_PORT; \
							} \
						} while(0)
#define VALIDATE_SPI_DEVICE_MODE(mode)		VALIDATE_ENUM((mode), SPI_DEVICE_MODE_PERIPHERAL, SPI_ERROR_INVALID_MODE)
#define VALIDATE_SPI_CLOCK_SRC(src) 		VALIDATE_ENUM((src), SPI_CLOCK_SRC_LFCLK, SPI_ERROR_INVALID_CLOCK_SRC)
#define VALIDATE_SPI_SCLK_SPEED(wlen)		VALIDATE_ENUM((wlen), SPI_SCLK_SPEED_DIV_8, SPI_ERROR_INVALID_SCLK_SPEED)
#define VALIDATE_SPI_DATA_WIDTH(width, err_code) do { \
    if ((width) < SPI_DATA_WIDTH_7 || (width) > SPI_DATA_WIDTH_16) { \
        return (err_code); \
    } \
} while (0)
#define VALIDATE_SPI_CPOL(flow)			VALIDATE_ENUM((flow), SPI_CPOL_HIGH, SPI_ERROR_INVALID_CPOL)
#define VALIDATE_SPI_CPHA(flow)			VALIDATE_ENUM((flow), SPI_CPHA_HIGH, SPI_ERROR_INVALID_CPHA)
#define VALIDATE_SPI_ENABLED(port) 		VALIDATE_BIT_SET((port)->CTL1, SPI_CTL1_ENABLE, SPI_ERROR_NOT_ENABLED)

// NOTE: --- Bit position definitions SPI_STAT ---

#define SPI_SR_TFE_STATE   			0
#define SPI_SR_TNF_STATE   			1
#define SPI_SR_RFE_STATE   			2
#define SPI_SR_RNF_STATE   			3
#define SPI_SR_BUSY_STATE   			4

// NOTE: --- Bit position definitions SPI_STAT ( bit shifting ) ---

#define SPI_SR_TFE   				(1U << 0)
#define SPI_SR_TNF   				(1U << 1)
#define SPI_SR_RFE   				(1U << 2)
#define SPI_SR_RNF   				(1U << 3)
#define SPI_SR_BUSY   				(1U << 4)

// NOTE: --- Bit position definitions SPI_CTL0 ---

#define SPI_CTL0_CSCLR				14
#define SPI_CTL0_CSSEL				12
#define SPI_CTL0_SPH				9
#define SPI_CTL0_SPO				8
#define SPI_CTL0_PACKEN				7
#define SPI_CTL0_FRF				5
#define SPI_CTL0_DSS				0

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
