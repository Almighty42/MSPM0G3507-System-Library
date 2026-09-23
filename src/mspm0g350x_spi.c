#include "mspm0g350x_spi.h"
#include "mspm0g350x_gpio.h"

/********************************************************************************
 *
 *
 *******************************************************************************/

/********************************************************************************
 *
 * INFO: Driver map
 * @PERIPHERAL_CLOCK_SETUP
 * @INIT_DE-INIT
 * @DATA_SEND_RECEIVE_POLLING
 * @PERIPHERAL_CONTROL_API
 *
 *******************************************************************************/

// NOTE: @PERIPHERAL_CLOCK_SETUP

/********************************************************************************
 * @fn				- spi_peri_clk_control
 *
 * @brief			- Enables or disables peripheral clock for SPI
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[EN_or_DI]		- ENABLE or DISABLE macros
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_peri_clk_control(spi_type* p_spi_x, uint8_t EN_or_DI)
{
	VALIDATE_PTR(p_spi_x, SPI_ERROR_NULL_PTR);
	VALIDATE_EN_DI(EN_or_DI, SPI_ERROR_INVALID_MODE);

	if (EN_or_DI == ENABLE) {
		p_spi_x->PWREN =
		    SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_ENABLE;

		volatile uint32_t startup_delay = 16U;
		while (startup_delay--) {
			__asm__ volatile("nop");
		}
	}
	else {
		p_spi_x->PWREN = SPI_PWREN_KEY_UNLOCK_W;
	}

	return SPI_OK;
}

// NOTE: @INIT_DE-INIT

/********************************************************************************
 * @fn				- spi_init
 *
 * @brief			- Initializes a SPI peripheral
 *
 * @param[*p_spi_handle]	- Handle structure of a SPI peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_init(spi_handle_t* p_spi_handle)
{
	VALIDATE_PTR(p_spi_handle, SPI_ERROR_NULL_PTR);
	VALIDATE_PTR(p_spi_handle->p_SPIx, SPI_ERROR_NULL_PTR);

	spi_type* spi = p_spi_handle->p_SPIx;
	spi_config_t* cfg = &p_spi_handle->spi_config;

	spi_status_t status = spi_de_init(spi);
	if (status != SPI_OK) {
		return status;
	}

	status = spi_peri_clk_control(spi, ENABLE);
	if (status != SPI_OK) {
		return status;
	}

	/* CTL1.ENABLE must be clear before any config write */
	WRITE_BIT(spi->CTL1, SPI_CTL1_ENABLE, DISABLE);

	switch (cfg->SPI_Clock_Source) {
		case SPI_CLOCK_SRC_MFCLK:
			spi->CLKSEL = (1U << SPI_CLKSEL_MFCLK_SEL_OFS);
			break;
		case SPI_CLOCK_SRC_LFCLK:
			spi->CLKSEL = (1U << SPI_CLKSEL_LFCLK_SEL_OFS);
			break;
		case SPI_CLOCK_SRC_BUSCLK:
		default:
			spi->CLKSEL = (1U << SPI_CLKSEL_SYSCLK_SEL_OFS);
			break;
	}

	spi->CLKDIV = cfg->SPI_SCLK_Speed;

	// Device mode
	WRITE_BIT(spi->CTL1, SPI_CTL1_CP,
	          cfg->SPI_Device_Mode == SPI_DEVICE_MODE_CONTROLLER);
	// Frame format select
	WRITE_FIELD(spi->CTL0, SPI_CTL0_FRF, SPI_CTL0_FRF_WIDTH,
	            SPI_CTL0_FRF_MOTOROLA_4WIRE);
	// Data width
	WRITE_FIELD(spi->CTL0, SPI_CTL0_DSS, SPI_CTL0_DSS_WIDTH,
	            cfg->SPI_Data_Width);
	// CPOL
	WRITE_BIT(spi->CTL0, SPI_CTL0_SPO, cfg->SPI_CPOL);
	// CPHA
	WRITE_BIT(spi->CTL0, SPI_CTL0_SPH, cfg->SPI_CPHA);
	// CS select
	WRITE_FIELD(spi->CTL0, SPI_CTL0_CSSEL, SPI_CTL0_CSSEL_WIDTH,
	            cfg->SPI_CS_Select);
	// MSB first select
	WRITE_BIT(spi->CTL1, SPI_CTL1_MSB, ENABLE);
	// SPI enable
	WRITE_BIT(spi->CTL1, SPI_CTL1_ENABLE, ENABLE);

	return SPI_OK;
}

/********************************************************************************
 * @fn				- spi_de_init
 *
 * @brief			- Resets a SPI peripheral
 *
 * @param[*p_SPI_x]		- Base address of the SPI peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_de_init(spi_type* p_spi_x)
{
	VALIDATE_PTR(p_spi_x, SPI_ERROR_NULL_PTR);

	p_spi_x->RSTCTL = SPI_RSTCTL_KEY_UNLOCK_W | SPI_RSTCTL_RESETASSERT;

	return SPI_OK;
}

// NOTE: @DATA_SEND_RECEIVE_POLLING

/********************************************************************************
 * @fn				- spi_write_data_pl
 *
 * @brief			- Write data over SPI using a polling method
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[*p_tx_buffer]		- Transmission buffer ( pointer )
 * @param[len]			- Number of bytes to send
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_write_data_pl(spi_handle_t* p_spi_handle,
                               const uint8_t* p_tx_buffer, uint32_t len,
                               uint32_t timeout)
{
	if (p_spi_handle == NULL || p_spi_handle->p_SPIx == NULL ||
	    p_tx_buffer == NULL) {
		return SPI_ERROR_NULL_PTR;
	}
	if (len == 0U) {
		return SPI_ERROR_INVALID_LEN;
	}
	VALIDATE_SPI_ENABLED(p_spi_handle->p_SPIx);

	for (uint32_t i = 0U; i < len; i++) {
		uint32_t wait_count = timeout;
		while ((p_spi_handle->p_SPIx->STAT & SPI_SR_TNF) == 0U) {
			if (wait_count == 0U) {
				return SPI_ERROR_TIMEOUT;
			}
			wait_count--;
		}
		p_spi_handle->p_SPIx->TXDATA = p_tx_buffer[i];
	}

	return SPI_OK;
}

/********************************************************************************
 * @fn				- spi_read_data_pl
 *
 * @brief			- Reads data over SPI using a polling
 *method
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[*p_rx_buffer]		- Receive buffer ( pointer )
 * @param[len]			- Number of bytes to receive
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_read_data_pl(spi_handle_t* p_spi_handle, uint8_t* p_rx_buffer,
                              uint32_t len, uint32_t timeout)
{
	if (p_spi_handle == NULL || p_spi_handle->p_SPIx == NULL ||
	    p_rx_buffer == NULL) {
		return SPI_ERROR_NULL_PTR;
	}
	if (len == 0U) {
		return SPI_ERROR_INVALID_LEN;
	}
	VALIDATE_SPI_ENABLED(p_spi_handle->p_SPIx);

	for (uint32_t i = 0U; i < len; i++) {
		uint32_t wait_count = timeout;

		while ((p_spi_handle->p_SPIx->STAT & SPI_SR_RFE) != 0U) {
			if (wait_count == 0U) {
				return SPI_ERROR_TIMEOUT;
			}
			wait_count--;
		}

		p_rx_buffer[i] = (uint8_t)p_spi_handle->p_SPIx->RXDATA;
	}

	return SPI_OK;
}

// NOTE: @PERIPHERAL_CONTROL_API

/********************************************************************************
 * @fn				- spi_peri_control
 *
 * @brief			- Sets SPI peripheral control
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[EN_or_DI]		- ENABLE or DISABLE macros
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_peri_control(spi_type* p_spi_x, uint8_t EN_or_DI)
{
	VALIDATE_PTR(p_spi_x, SPI_ERROR_NULL_PTR);
	VALIDATE_EN_DI(EN_or_DI, SPI_ERROR_INVALID_MODE);

	WRITE_BIT(p_spi_x->CTL1, SPI_CTL1_ENABLE, EN_or_DI);

	return SPI_OK;
}

// void spi_reset(void)
// {
//
// 	SPI->GPRCM.RSTCTL = (SPI_RSTCTL_KEY_UNLOCK_W |
// SPI_RSTCTL_RESETASSERT);
// }
//
// void spi_enable_power(void)
// {
//
// 	SPI->GPRCM.PWREN = (PWREN_KEY_UNLOCK_W | ENABLE);
// }
//
// void spi_disable_power(void)
// {
//
// 	SPI->GPRCM.PWREN = (PWREN_KEY_UNLOCK_W | DISABLE);
// }
//
// void spi_enable(void)
// {
//
// 	write_reg_bit(&SPI->CTL1, SPI_CTL1_ENABLE_OFS, ENABLE);
// }
//
// void spi_disable(void)
// {
//
// 	write_reg_bit(&SPI->CTL1, SPI_CTL1_ENABLE_OFS, DISABLE);
// }
//
// void spi_set_clock_configuration(spi_clock_t clock, spi_clock_div_t
// divider)
// {
//
// 	// Disable previous clock configs if there are any
// 	write_reg_bit(&SPI->CLKSEL, SPI_CLKSEL_LFCLK_OFS, DISABLE);
// 	write_reg_bit(&SPI->CLKSEL, SPI_CLKSEL_MFCLK_OFS, DISABLE);
// 	write_reg_bit(&SPI->CLKSEL, SPI_CLKSEL_SYSCLK_OFS, DISABLE);
//
// 	switch (clock) {
// 		case SPI_CLKSEL_SYSCLK:
// 			write_reg_bit(&SPI->CLKSEL,
// SPI_CLKSEL_SYSCLK_OFS, 			              ENABLE);
// break;
//
// 		case SPI_CLKSEL_MFCLK:
// 			write_reg_bit(&SPI->CLKSEL,
// SPI_CLKSEL_MFCLK_OFS, 			              ENABLE);
// break;
//
// 		case SPI_CLKSEL_LFCLK:
// 		default:
// 			write_reg_bit(&SPI->CLKSEL,
// SPI_CLKSEL_LFCLK_OFS, 			              ENABLE);
// break;
// 	}
//
// 	write_reg_field(&SPI->CLKDIV, SPI_CLKDIV_RATIO_OFS,
// 	                SPI_CLKDIV_RATIO_WIDTH, divider);
// }
//
// void spi_set_clock_prescaler(uint32_t scr)
// {
//
// 	if (scr > 0x3FF) {
// 		scr = 0x3FF;
// 	}
//
// 	write_reg_field(&SPI->CLKCTL, SPI_CLKCTL_SCR_OFS,
// SPI_CLKCTL_SCR_WIDTH, 	                scr);
// }
//
// void spi_set_mode(spi_mode_t mode)
// {
//
// 	if (mode == SPI_CONTROLLER_MODE) {
// 		write_reg_bit(&SPI->CTL1, SPI_CTL1_CP_OFS, ENABLE);
// 	}
// 	if (mode == SPI_PERIPHERAL_MODE) {
// 		write_reg_bit(&SPI->CTL1, SPI_CTL1_CP_OFS, DISABLE);
// 	}
// }
//
// void spi_set_transfer_mode(spi_transfer_mode_t transfer_mode)
// {
//
// 	uint32_t current_frf =
// 	    read_reg_field(&SPI->CTL0, SPI_CTL0_FRF_OFS,
// SPI_CTL0_FRF_WIDTH);
//
// 	// SPO and SPH cant be configured outside of Motorola FRF
// 	if (current_frf != SPI_FRF_MOTOROLA_3WIRE &&
// 	    current_frf != SPI_FRF_MOTOROLA_4WIRE) {
// 		return;
// 	}
//
// 	uint32_t spo = (transfer_mode >> 1) & 0x1;
// 	uint32_t sph = transfer_mode & 0x1;
//
// 	write_reg_bit(&SPI->CTL0, SPI_CTL0_SPO_OFS, spo);
// 	write_reg_bit(&SPI->CTL0, SPI_CTL0_SPH_OFS, sph);
// }
//
// void spi_set_frame_format(spi_frame_format_t frame_format)
// {
// 	write_reg_field(&SPI->CTL0, SPI_CTL0_FRF_OFS,
// SPI_CTL0_FRF_WIDTH, 	                frame_format);
// }
//
// void spi_set_data_size(spi_data_size_t size)
// {
// 	write_reg_field(&SPI->CTL0, SPI_CTL0_DSS_OFS,
// SPI_CTL0_DSS_WIDTH, size);
// }
//
// void spi_set_bit_order(spi_bit_order_t order)
// {
// 	write_reg_bit(&SPI->CTL1, SPI_CTL1_MSB_OFS, order);
// }
//
// void spi_cs_enable(uint32_t pin)
// {
// 	gpio_write(pin, 0);
// }
//
// void spi_cs_disable(uint32_t pin)
// {
// 	gpio_write(pin, 1);
// }
//
// uint8_t spi_transfer_byte(uint8_t data)
// {
//
// 	// 1. Wait until TX FIFO isnt full
// 	while (read_reg_bit(&SPI->STAT, SPI_STAT_TNF_OFS) == 0) {
// 	}
//
// 	write_reg_field(&SPI->TXDATA, 0, 8, data);
//
// 	// 2. Wait until RX FIFO is not empty
// 	while (read_reg_bit(&SPI->STAT, SPI_STAT_RFE_OFS) == 1) {
// 	}
//
// 	uint8_t received = (uint8_t)read_reg_field(&SPI->RXDATA, 0, 8);
//
// 	return received;
// }
//
// void spi_write_buffer(uint32_t address, const uint8_t* data, uint32_t
// length,
//                       uint8_t WRITE_CMD)
// {
//
// 	spi_transfer_byte(WRITE_CMD);
// 	spi_transfer_byte((address >> 16) & 0xFF);
// 	spi_transfer_byte((address >> 8) & 0xFF);
// 	spi_transfer_byte(address & 0xFF);
//
// 	for (uint32_t i = 0; i < length; i++) {
// 		spi_transfer_byte(data[i]);
// 	}
// }
// void spi_read_buffer(uint32_t address, uint8_t* data, uint32_t
// length,
//                      uint8_t READ_CMD)
// {
//
// 	spi_transfer_byte(READ_CMD);
// 	spi_transfer_byte((address >> 16) & 0xFF);
// 	spi_transfer_byte((address >> 8) & 0xFF);
// 	spi_transfer_byte(address & 0xFF);
//
// 	for (uint32_t i = 0; i < length; i++) {
// 		data[i] = spi_transfer_byte(0xFF);
// 	}
// }
