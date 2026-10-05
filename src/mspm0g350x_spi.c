#include "../inc/mspm0g350x_spi.h"
#include "../inc/mspm0g350x_startup.h"
#include "../inc/mspm0g350x_systick.h"
#include <stdint.h>

/********************************************************************************
 *
 * TODO: Do testing on a board, these functions in partiuclar:
 * - spi_peri_clk_control
 * - spi_init
 * - spi_de_init
 * - spi_write_data_pl
 * - spi_read_data_pl
 * - spi_peri_control
 * TODO: Implement Systick where it is necessary ( SPI )
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
	VALIDATE_EN_DI(EN_or_DI, SPI_ERROR_INVALID_STATE);
	VALIDATE_SPI_PORT(p_spi_x);

	spi_status_t status = SPI_ERROR_INVALID_PORT;
	uint32_t pwren = 0U;

	if (EN_or_DI == ENABLE) {
		// Enabling PWREN
		WRITE_FIELD(pwren, SPI_PWREN_KEY, SPI_PWREN_KEY_WIDTH,
		            SPI_PWREN_KEY_VAL_UNLOCK);
		WRITE_FIELD(pwren, SPI_PWREN_ENABLE, SPI_PWREN_ENABLE_WIDTH,
		            ENABLE);
		p_spi_x->PWREN = pwren;

		sys_delay_cpu_cycles(4U * sys_clock_get_ulpclk_divider());

		status = SPI_OK;
	}
	else {
		uint32_t i = 0;
		while (IS_BIT_SET(p_spi_x->STAT1, SPI_STAT1_BUSY)) {
			if (i++ >= SPI_SOFTWARE_TIMEOUT) {
				return SPI_BUSY;
			}
		}

		CLEAR_BIT(p_spi_x->CTL1, SPI_CTL1_ENABLE);

		// Clearing PWREN
		WRITE_FIELD(pwren, SPI_PWREN_KEY, SPI_PWREN_KEY_WIDTH,
		            SPI_PWREN_KEY_VAL_UNLOCK);
		WRITE_FIELD(pwren, SPI_PWREN_ENABLE, SPI_PWREN_ENABLE_WIDTH,
		            DISABLE);
		p_spi_x->PWREN = pwren;

		status = SPI_OK;
	}

	return status;
}

// NOTE: @INIT_DE-INIT

/********************************************************************************
 * - spi_int ( STATIC ) HELPER FUNCTIONS PROTOTYPES
 *******************************************************************************/

static inline void spi_set_device_mode(spi_type* port, uint8_t device_mode);
static inline void spi_set_clock_source(spi_type* port, uint8_t clock_source);
static inline void spi_set_clock_divide_ratio(spi_type* port, uint8_t ratio);
static inline void spi_set_clock_prescaler(spi_type* port, uint16_t scr);
static inline void spi_set_data_width(spi_type* port, uint8_t data_width);
static inline void spi_set_cpol(spi_type* port, uint8_t cpol);
static inline void spi_set_cpha(spi_type* port, uint8_t cpha);
static inline void spi_set_cs_selector(spi_type* port, uint8_t cs_selector);
static inline void spi_set_frame_format(spi_type* port,
                                        uint8_t frame_format_select);
static inline void spi_set_msb(spi_type* port, uint8_t msb);

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
	spi_type* port = p_spi_handle->p_SPIx;
	VALIDATE_SPI_PORT(port);

	// Device mode
	uint8_t device_mode = p_spi_handle->spi_config.SPI_Device_Mode;
	VALIDATE_SPI_DEVICE_MODE(device_mode);

	// Clock source
	uint8_t clock_source = p_spi_handle->spi_config.SPI_Clock_Source;
	VALIDATE_SPI_CLOCK_SRC(clock_source);

	// Clock divide ratio
	uint8_t divide_ratio = p_spi_handle->spi_config.SPI_Clock_Divide_Ratio;
	VALIDATE_SPI_CLOCK_DIVIDE_RATIO(divide_ratio);

	// Clock prescaler
	uint16_t prescaler = p_spi_handle->spi_config.SPI_Clock_Prescaler;
	VALIDATE_SPI_CLOCK_PRESCALER(prescaler);

	// Data width
	uint8_t data_width = p_spi_handle->spi_config.SPI_Data_Width;
	VALIDATE_SPI_DATA_WIDTH(data_width, SPI_ERROR_INVALID_DATA_WIDTH);

	// CPOL
	uint8_t cpol = p_spi_handle->spi_config.SPI_CPOL;
	VALIDATE_SPI_CPOL(cpol);

	// CPHA
	uint8_t cpha = p_spi_handle->spi_config.SPI_CPHA;
	VALIDATE_SPI_CPHA(cpha);

	// CS selector
	uint8_t cs_selector = p_spi_handle->spi_config.SPI_CS_Select;
	VALIDATE_SPI_CS_SELECTOR(cs_selector);

	// Frame format selector
	uint8_t frame_format = p_spi_handle->spi_config.SPI_Frame_Format_Select;
	VALIDATE_SPI_FRAME_FORMAT(frame_format);

	// MSB
	uint8_t msb = p_spi_handle->spi_config.SPI_MSB;
	VALIDATE_SPI_MSB(msb);

	spi_status_t clk_status = spi_peri_clk_control(port, ENABLE);
	if (clk_status != SPI_OK)
		return clk_status;

	if (IS_BIT_SET(port->CTL1, SPI_CTL1_ENABLE)) {
		CLEAR_BIT(port->CTL1, SPI_CTL1_ENABLE);
	}

	spi_set_device_mode(p_spi_handle->p_SPIx, device_mode);
	spi_set_clock_source(p_spi_handle->p_SPIx, clock_source);
	spi_set_clock_divide_ratio(p_spi_handle->p_SPIx, divide_ratio);
	spi_set_clock_prescaler(p_spi_handle->p_SPIx, prescaler);
	spi_set_data_width(p_spi_handle->p_SPIx, data_width);
	spi_set_cpol(p_spi_handle->p_SPIx, cpol);
	spi_set_cpha(p_spi_handle->p_SPIx, cpha);
	spi_set_cs_selector(p_spi_handle->p_SPIx, cs_selector);
	spi_set_frame_format(p_spi_handle->p_SPIx, frame_format);
	spi_set_msb(p_spi_handle->p_SPIx, msb);

	SET_BIT(port->CTL1, SPI_CTL1_ENABLE);

	return SPI_OK;
}

static inline void spi_set_device_mode(spi_type* port, uint8_t device_mode)
{
	uint32_t cp_bit = (device_mode == SPI_DEVICE_MODE_CONTROLLER) ? 1U : 0U;
	WRITE_FIELD(port->CTL1, SPI_CTL1_CP, SPI_CTL1_CP_WIDTH, cp_bit);
}

static inline void spi_set_clock_source(spi_type* port, uint8_t clock_source)
{
	uint32_t clksel = 0U;
	switch (clock_source) {
		case SPI_CLOCK_SRC_MFCLK:
			SET_BIT(clksel, SPI_CLKSEL_MFCLK_SEL);
			break;
		case SPI_CLOCK_SRC_LFCLK:
			SET_BIT(clksel, SPI_CLKSEL_LFCLK_SEL);
			break;
		case SPI_CLOCK_SRC_BUSCLK:
		default:
			SET_BIT(clksel, SPI_CLKSEL_SYSCLK_SEL);
			break;
	}
	port->CLKSEL = clksel;
}

static inline void spi_set_clock_divide_ratio(spi_type* port, uint8_t ratio)
{
	WRITE_FIELD(port->CLKDIV, SPI_CLKDIV_RATIO, SPI_CLKDIV_RATIO_WIDTH,
	            ratio);
}

static inline void spi_set_clock_prescaler(spi_type* port, uint16_t scr)
{
	WRITE_FIELD(port->CLKCTL, SPI_CLKCTL_SCR, SPI_CLKCTL_SCR_WIDTH, scr);
}

static inline void spi_set_data_width(spi_type* port, uint8_t data_width)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_DSS, SPI_CTL0_DSS_WIDTH, data_width);
}

static inline void spi_set_cpol(spi_type* port, uint8_t cpol)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_SPO, SPI_CTL0_SPO_WIDTH, cpol);
}
static inline void spi_set_cpha(spi_type* port, uint8_t cpha)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_SPH, SPI_CTL0_SPH_WIDTH, cpha);
}
static inline void spi_set_cs_selector(spi_type* port, uint8_t cs_selector)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_CSSEL, SPI_CTL0_CSSEL_WIDTH,
	            cs_selector);
}

static inline void spi_set_frame_format(spi_type* port,
                                        uint8_t frame_format_select)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_FRF, SPI_CTL0_FRF_WIDTH,
	            frame_format_select);
}

static inline void spi_set_msb(spi_type* port, uint8_t msb)
{
	WRITE_FIELD(port->CTL1, SPI_CTL1_MSB, SPI_CTL1_MSB_WIDTH, msb);
}

static spi_status_t spi_wait_idle(const spi_type* p_spi_x, uint32_t timeout);
static spi_status_t spi_reset(spi_type* p_spi_x);

/********************************************************************************
 * @fn				- spi_de_init
 *
 * @brief			- Resets a SPI peripheral
 *
 * @pre
 * WARNING: spi_de_init() may only be called
 * after a successful spi_init()
 *
 * @param[*p_SPI_x]		- Base address of the SPI peripheral
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_de_init(spi_type* p_spi_x)
{
	VALIDATE_PTR(p_spi_x, SPI_ERROR_NULL_PTR);
	VALIDATE_SPI_PORT(p_spi_x);

	spi_status_t st = spi_wait_idle(p_spi_x, SPI_SOFTWARE_TIMEOUT);
	if (st != SPI_OK) {
		return st;
	}

	CLEAR_BIT(p_spi_x->CTL1, SPI_CTL1_ENABLE);

	st = spi_reset(p_spi_x);
	if (st != SPI_OK) {
		return st;
	}

	uint32_t pwren = 0;
	WRITE_FIELD(pwren, SPI_PWREN_KEY, SPI_PWREN_KEY_WIDTH,
	            SPI_PWREN_KEY_VAL_UNLOCK);
	WRITE_FIELD(pwren, SPI_PWREN_ENABLE, SPI_PWREN_ENABLE_WIDTH, DISABLE);
	p_spi_x->PWREN = pwren;
	return SPI_OK;
}

static spi_status_t spi_wait_idle(const spi_type* p_spi_x, uint32_t timeout)
{
	while (READ_FIELD(p_spi_x->STAT1, SPI_STAT1_BUSY,
	                  SPI_STAT1_BUSY_WIDTH) != 0U) {
		if (timeout == 0) {
			return SPI_ERROR_TIMEOUT;
		}
		timeout--;
	}
	return SPI_OK;
}

static spi_status_t spi_reset(spi_type* p_spi_x)
{
	uint32_t rstctl = 0;
	WRITE_FIELD(rstctl, SPI_RSTCTL_KEY, SPI_RSTCTL_KEY_WIDTH,
	            SPI_RSTCTL_KEY_UNLOCK);
	WRITE_FIELD(rstctl, SPI_RSTCTL_RESETSTKYCLR,
	            SPI_RSTCTL_RESETSTKYCLR_WIDTH, ENABLE);
	WRITE_FIELD(rstctl, SPI_RSTCTL_RESETASSERT,
	            SPI_RSTCTL_RESETASSERT_WIDTH, ENABLE);
	p_spi_x->RSTCTL = rstctl;

	if (READ_FIELD(p_spi_x->STAT0, SPI_STAT0_RESETSTKY,
	               SPI_STAT0_RESETSTKY_WIDTH) == 0) {
		return SPI_ERROR_INVALID_STATE;
	}

	return SPI_OK;
}

// NOTE: @DATA_SEND_RECEIVE_POLLING

/********************************************************************************
 * - spi TX / RX ( STATIC ) HELPER FUNCTIONS PROTOTYPES
 *******************************************************************************/

static spi_status_t spi_wait_tx_ready(spi_type* p_spi_x, uint32_t timeout);
static spi_status_t spi_wait_rx_ready(spi_type* p_spi_x, uint32_t timeout);
// static spi_status_t spi_wait_idle(spi_type* p_spi_x, uint32_t timeout);
static spi_status_t spi_transceive_pl(spi_handle_t* p_spi_handle,
                                      const uint8_t* p_tx_buffer,
                                      uint8_t* p_rx_buffer,
                                      uint32_t frame_count, uint32_t timeout);

/********************************************************************************
 * @fn				- spi_write_data_pl
 *
 * @brief			- Write data over SPI using a polling
 *method
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[*p_tx_buffer]		- Transmission buffer ( pointer )
 * @param[frame_count]		- Number of bytes to send
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_write_data_pl(spi_handle_t* p_spi_handle,
                               const uint8_t* p_tx_buffer, uint32_t frame_count,
                               uint32_t timeout)
{
	//     Validate TX buffer and frame count.
	VALIDATE_PTR(p_spi_handle, SPI_ERROR_NULL_PTR);
	VALIDATE_SPI_PORT(p_spi_handle->p_SPIx);
	VALIDATE_PTR(p_tx_buffer, SPI_ERROR_NULL_PTR);
	if (frame_count == 0) {
		return SPI_ERROR_INVALID_FRAME_COUNT;
	}
	VALIDATE_SPI_ENABLED(p_spi_handle->p_SPIx);

	//     Confirm the configured frame width matches the buffer type.
	uint32_t dss = READ_FIELD(p_spi_handle->p_SPIx->CTL0, SPI_CTL0_DSS,
	                          SPI_CTL0_DSS_WIDTH);
	if (dss > (uint32_t)SPI_DATA_WIDTH_8) {
		return SPI_ERROR_INVALID_DATA_WIDTH;
	}

	//     Invoke the full-duplex transfer operation.
	//     Supply the user TX frames.
	//     Discard every simultaneously received frame.
	//     Wait until TX is empty and SPI is no longer busy.
	//     Return the transfer status.
	spi_status_t status = spi_transceive_pl(p_spi_handle, p_tx_buffer, NULL,
	                                        frame_count, timeout);

	return status;
}

static spi_status_t spi_wait_stat1_flag(spi_type* p_spi_x, uint32_t pos,
                                        uint32_t width, uint32_t expected,
                                        uint32_t timeout)
{
	while (READ_FIELD(p_spi_x->STAT1, pos, width) != expected) {
		if (timeout == 0) {
			return SPI_ERROR_TIMEOUT;
		}
		timeout--;
	}
	return SPI_OK;
}

static spi_status_t spi_wait_tx_ready(spi_type* p_spi_x, uint32_t timeout)
{
	return spi_wait_stat1_flag(p_spi_x, SPI_STAT1_TNF, SPI_STAT1_TNF_WIDTH,
	                           1, timeout);
}

static spi_status_t spi_wait_rx_ready(spi_type* p_spi_x, uint32_t timeout)
{
	return spi_wait_stat1_flag(p_spi_x, SPI_STAT1_RFE, SPI_STAT1_RFE_WIDTH,
	                           0, timeout);
}

static spi_status_t spi_wait_tx_complete(spi_type* p_spi_x, uint32_t timeout)
{
	spi_status_t st = spi_wait_stat1_flag(p_spi_x, SPI_STAT1_TFE,
	                                      SPI_STAT1_TFE_WIDTH, 1, timeout);
	if (st != SPI_OK) {
		return st;
	}
	return spi_wait_idle(p_spi_x, timeout);
}

static spi_status_t spi_transceive_pl(spi_handle_t* p_spi_handle,
                                      const uint8_t* p_tx_buffer,
                                      uint8_t* p_rx_buffer,
                                      uint32_t frame_count, uint32_t timeout)
{
	spi_type* p_spi_x = p_spi_handle->p_SPIx;
	spi_status_t status;

	for (uint32_t i = 0; i < frame_count; i++) {
		status = spi_wait_tx_ready(p_spi_x, timeout);
		if (status != SPI_OK) {
			return status;
		}

		p_spi_x->TXDATA = (p_tx_buffer != NULL)
		                      ? (uint32_t)p_tx_buffer[i]
		                      : SPI_DUMMY_BYTE;

		status = spi_wait_rx_ready(p_spi_x, timeout);
		if (status != SPI_OK) {
			return status;
		}

		uint8_t rx_frame = (uint8_t)p_spi_x->RXDATA;
		if (p_rx_buffer != NULL) {
			p_rx_buffer[i] = rx_frame;
		}
	}

	return spi_wait_tx_complete(p_spi_x, timeout);
}

/********************************************************************************
 * @fn				- spi_read_data_pl
 *
 * @brief			- Reads data over SPI using a polling
 *method
 *
 * @param[*p_spi_x]		- Base address of the SPI peripheral
 * @param[*p_rx_buffer]		- Receive buffer ( pointer )
 * @param[frame_count]		- Number of bytes to receive
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

spi_status_t spi_read_data_pl(spi_handle_t* p_spi_handle, uint8_t* p_rx_buffer,
                              uint32_t frame_count, uint32_t timeout)
{
	//     Validate RX buffer and frame count.
	VALIDATE_PTR(p_spi_handle, SPI_ERROR_NULL_PTR);
	VALIDATE_SPI_PORT(p_spi_handle->p_SPIx);
	VALIDATE_PTR(p_rx_buffer, SPI_ERROR_NULL_PTR);
	if (frame_count == 0) {
		return SPI_ERROR_INVALID_FRAME_COUNT;
	}
	VALIDATE_SPI_ENABLED(p_spi_handle->p_SPIx);

	spi_type* p_spi_x = p_spi_handle->p_SPIx;

	uint32_t dss =
	    READ_FIELD(p_spi_x->CTL0, SPI_CTL0_DSS, SPI_CTL0_DSS_WIDTH);
	if (dss > SPI_CTL0_DSS_VAL_BITS8) {
		return SPI_ERROR_INVALID_DATA_WIDTH;
	}
	if (p_spi_handle->spi_config.SPI_Device_Mode ==
	    SPI_DEVICE_MODE_CONTROLLER) {
		return spi_transceive_pl(p_spi_handle, NULL, p_rx_buffer,
		                         frame_count, timeout);
	}
	if (p_spi_handle->spi_config.SPI_Device_Mode !=
	    SPI_DEVICE_MODE_PERIPHERAL) {
		return SPI_ERROR_INVALID_MODE;
	}

	spi_status_t st = spi_wait_tx_ready(p_spi_x, timeout);
	if (st != SPI_OK) {
		return st;
	}
	p_spi_x->TXDATA = (uint32_t)SPI_DUMMY_BYTE;

	for (uint32_t i = 0; i < frame_count; i++) {
		st = spi_wait_rx_ready(p_spi_x, timeout);
		if (st != SPI_OK) {
			return st;
		}
		p_rx_buffer[i] = (uint8_t)p_spi_x->RXDATA;
		if ((i + 1) < frame_count) {
			st = spi_wait_tx_ready(p_spi_x, timeout);
			if (st != SPI_OK) {
				return st;
			}
			p_spi_x->TXDATA = (uint32_t)SPI_DUMMY_BYTE;
		}
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
	VALIDATE_SPI_PORT(p_spi_x);

	if (READ_FIELD(p_spi_x->PWREN, SPI_PWREN_ENABLE,
	               SPI_PWREN_ENABLE_WIDTH) == DISABLE) {
		return SPI_ERROR_NOT_ENABLED;
	}

	if (EN_or_DI == ENABLE) {
		uint32_t clksel =
		    READ_FIELD(p_spi_x->CLKSEL, SPI_CLKSEL_LFCLK_SEL,
		               SPI_CLKSEL_LFCLK_SEL_WIDTH) |
		    READ_FIELD(p_spi_x->CLKSEL, SPI_CLKSEL_MFCLK_SEL,
		               SPI_CLKSEL_MFCLK_SEL_WIDTH) |
		    READ_FIELD(p_spi_x->CLKSEL, SPI_CLKSEL_SYSCLK_SEL,
		               SPI_CLKSEL_SYSCLK_SEL_WIDTH);

		if (clksel == 0) {
			return SPI_ERROR_INVALID_STATE;
		}

		if (READ_FIELD(p_spi_x->CTL1, SPI_CTL1_ENABLE,
		               SPI_CTL1_ENABLE_WIDTH) != DISABLE) {
			return SPI_ERROR_INVALID_STATE;
		}

		WRITE_FIELD(p_spi_x->CTL1, SPI_CTL1_ENABLE,
		            SPI_CTL1_ENABLE_WIDTH, ENABLE);

		if (READ_FIELD(p_spi_x->CTL1, SPI_CTL1_ENABLE,
		               SPI_CTL1_ENABLE_WIDTH) != ENABLE) {
			return SPI_ERROR_INVALID_STATE;
		}

		return SPI_OK;
	}

	if (EN_or_DI == DISABLE) {
		if (spi_wait_idle(p_spi_x, SPI_SOFTWARE_TIMEOUT) != SPI_OK) {
			return SPI_BUSY;
		}
		WRITE_FIELD(p_spi_x->CTL1, SPI_CTL1_ENABLE,
		            SPI_CTL1_ENABLE_WIDTH, DISABLE);
		return SPI_OK;
	}

	return SPI_ERROR_INVALID_STATE;
}
