#include "../inc/mspm0g350x_iomux.h"
#include "../inc/mspm0g350x_spi.h"
#include "example_spi.h"
#include <stdint.h>

#define SPI_EXAMPLE_FRAMES 8U

/********************************************************************************
 * @fn      - spi1_config
 *
 * @brief   - Configures SPI1 as a controller and initialises it
 *
 * @param[*p_spi] - Handle structure of the SPI1 peripheral
 *
 * @return  - Success / Failure status of the function
 *
 * @Note    - Configuration: controller mode, BUSCLK source, divide ratio 1,
 *            prescaler 19, 8-bit frames, mode 3 (CPOL=1, CPHA=1), 4-wire
 *            frame format, CS0, MSB first.
 *            With a 40 MHz clock, SCLK should be about 1 MHz if the TRM
 *            formula is SCLK = SPI clock / (2 x (1 + SCR)). Verify the
 *            formula in the TRM and the result with the logic analyzer.
 *            Pins: PB9 = SCK, PB8 = PICO, PB7 = POCI, PB6 = CS0.
 *            The caller must have run the system clock setup first.
 *******************************************************************************/
spi_status_t spi1_config(spi_handle_t* p_spi)
{
	*p_spi = (spi_handle_t){0};

	p_spi->p_SPIx = SPI1;
	p_spi->spi_config.SPI_Device_Mode = SPI_DEVICE_MODE_CONTROLLER;
	p_spi->spi_config.SPI_Clock_Source = SPI_CLOCK_SRC_BUSCLK;
	p_spi->spi_config.SPI_Clock_Divide_Ratio = SPI_SCLK_SPEED_DIV_1;
	p_spi->spi_config.SPI_Clock_Prescaler = 19U;
	p_spi->spi_config.SPI_Data_Width = SPI_DATA_WIDTH_8;
	p_spi->spi_config.SPI_CPOL = SPI_CPOL_HIGH;
	p_spi->spi_config.SPI_CPHA = SPI_CPHA_HIGH;
	p_spi->spi_config.SPI_CS_Select = SPI_CS_0;
	p_spi->spi_config.SPI_Frame_Format_Select = SPI_FRAMEFORMAT_4WIRE;
	p_spi->spi_config.SPI_MSB = SPI_MSB_MSB;

	static const iomux_config_t spi1_sck_cfg = {
	    .pincm_index = IOMUX_PIN_PB9,
	    .pf = IOMUX_PIN_PB9_PF_SPI1_SCK,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
	};

	static const iomux_config_t spi1_pico_cfg = {
	    .pincm_index = IOMUX_PIN_PB8,
	    .pf = IOMUX_PIN_PB8_PF_SPI1_PICO,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
	};

	static const iomux_config_t spi1_poci_cfg = {
	    .pincm_index = IOMUX_PIN_PB7,
	    .pf = IOMUX_PIN_PB7_PF_SPI1_POCI,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_ENABLE,
	};

	static const iomux_config_t spi1_cs0_cfg = {
	    .pincm_index = IOMUX_PIN_PB6,
	    .pf = IOMUX_PIN_PB6_PF_SPI1_CS0,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
	};

	iomux_configure_pin(&spi1_sck_cfg);
	iomux_configure_pin(&spi1_pico_cfg);
	iomux_configure_pin(&spi1_poci_cfg);
	iomux_configure_pin(&spi1_cs0_cfg);

	return spi_init(p_spi);
}

/********************************************************************************
 * @fn				- spi1_example_write
 *
 * @brief			- Writes data into the SPI1 MOSI
 *
 * @param[*p_spi_handle]        - Handle structure of a SPI peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note    - Test setup: logic analyzer on SCK (PB9), PICO (PB8), POCI (PB7)
 *            and CS0 (PB6). SPI decoder: mode 3 (CPOL=1, CPHA=1), MSB first,
 *            8 bits.
 *            PASS: PICO decodes 55 A5 0F F0 00 FF 01 80 in that order, and
 *            the function returns SPI_OK.
 *******************************************************************************/
spi_status_t spi1_example_write(spi_handle_t* p_spi)
{
	static const uint8_t pattern[SPI_EXAMPLE_FRAMES] = {
	    0x55U, 0xA5U, 0x0FU, 0xF0U, 0x00U, 0xFFU, 0x01U, 0x80U};

	return spi_write_data_pl(p_spi, pattern, SPI_EXAMPLE_FRAMES,
	                         SPI_SOFTWARE_TIMEOUT);
}

/********************************************************************************
 * @fn				- spi1_example_read
 *
 * @brief			- Reads data from the SPI1 MISO
 *
 * @param[*p_spi_handle]        - Handle structure of a SPI peripheral
 * @param[expected]       - Value every received byte should have
 * @param[*p_mismatches]  - Out: number of bytes that differ from expected
 *
 * @return			- Success / Failure status of the function
 *
 * @Note    - In controller mode the driver sends dummy 0xFF bytes on PICO.
 *            Test A: jumper PB8 (PICO) to PB7 (POCI), expected = 0xFF.
 *            Test B: remove the jumper, tie PB7 to GND, expected = 0x00.
 *            PASS: function returns SPI_OK and *p_mismatches == 0 in both
 *            tests. The buffer is pre-filled with 0xAA, so an unwritten
 *            byte is counted as a mismatch.
 *******************************************************************************/
spi_status_t spi1_example_read(spi_handle_t* p_spi, uint8_t expected,
                               uint32_t* p_mismatches)
{
	static uint8_t rx[SPI_EXAMPLE_FRAMES];

	for (uint32_t i = 0U; i < SPI_EXAMPLE_FRAMES; i++) {
		rx[i] = 0xAAU;
	}

	spi_status_t status = spi_read_data_pl(p_spi, rx, SPI_EXAMPLE_FRAMES,
	                                       SPI_SOFTWARE_TIMEOUT);

	*p_mismatches = 0U;
	for (uint32_t i = 0U; i < SPI_EXAMPLE_FRAMES; i++) {
		if (rx[i] != expected) {
			(*p_mismatches)++;
		}
	}

	return status;
}
