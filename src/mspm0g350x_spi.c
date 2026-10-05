#include "../inc/mspm0g350x_spi.h"
#include "../inc/mspm0g350x_startup.h"
#include "../inc/mspm0g350x_systick.h"

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

	if (EN_or_DI == ENABLE) {
		if (p_spi_x == SPI1) {
		}
		else if (p_spi_x == SPI0) {
		}

		// Enabling PWREN
		p_spi_x->PWREN =
		    SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_ENABLE;

		sys_delay_cpu_cycles(4U * sys_clock_get_ulpclk_divider());

		status = SPI_OK;
	}
	else {
		uint32_t i = 0;
		while (IS_BIT_SET(p_spi_x->STAT1, SPI_SR_BUSY_STATE)) {
			if (i < SPI_SOFTWARE_TIMEOUT)
				i++;
			else
				return SPI_BUSY;
		}

		CLEAR_BIT(p_spi_x->CTL1, SPI_CTL1_ENABLE);

		// Clearing PWREN
		p_spi_x->PWREN =
		    SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_DISABLE;

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

	// MSB
	uint8_t msb = p_spi_handle->spi_config.SPI_MSB;
	VALIDATE_SPI_MSB(msb);

	spi_status_t clk_status = spi_peri_clk_control(port, ENABLE);
	if (clk_status != SPI_OK)
		return clk_status;

	if (IS_BIT_SET(port->CTL1, SPI_CTL1_ENABLE_MASK)) {
		CLEAR_BIT(port->CTL1, SPI_CTL1_ENABLE_MASK);
	}

	spi_set_device_mode(p_spi_handle->p_SPIx, device_mode);
	spi_set_clock_source(p_spi_handle->p_SPIx, clock_source);
	spi_set_clock_divide_ratio(p_spi_handle->p_SPIx, divide_ratio);
	spi_set_clock_prescaler(p_spi_handle->p_SPIx, prescaler);
	spi_set_data_width(p_spi_handle->p_SPIx, data_width);
	spi_set_cpol(p_spi_handle->p_SPIx, cpol);
	spi_set_cpha(p_spi_handle->p_SPIx, cpha);
	spi_set_cs_selector(p_spi_handle->p_SPIx, cs_selector);
	spi_set_msb(p_spi_handle->p_SPIx, msb);

	SET_BIT(port->CTL1, SPI_CTL1_ENABLE_MASK);

	return SPI_OK;
}

static inline void spi_set_device_mode(spi_type* port, uint8_t device_mode)
{
	uint32_t cp_bit = (device_mode == SPI_DEVICE_MODE_CONTROLLER) ? 1U : 0U;
	WRITE_FIELD(port->CTL1, SPI_CTL1_CP_OFS, 1U, cp_bit);
}

static inline void spi_set_clock_source(spi_type* port, uint8_t clock_source)
{
	uint32_t sel_mask;
	switch (clock_source) {
		case SPI_CLOCK_SRC_MFCLK:
			sel_mask = SPI_CLKSEL_MFCLK_SEL_MASK;
			break;
		case SPI_CLOCK_SRC_LFCLK:
			sel_mask = SPI_CLKSEL_LFCLK_SEL_MASK;
			break;
		case SPI_CLOCK_SRC_BUSCLK:
		default:
			sel_mask = SPI_CLKSEL_SYSCLK_SEL_MASK;
			break;
	}
	port->CLKSEL = sel_mask;
}

static inline void spi_set_clock_divide_ratio(spi_type* port, uint8_t ratio)
{
	port->CLKDIV = (uint32_t)ratio & SPI_CLKDIV_RATIO_MASK;
}

static inline void spi_set_clock_prescaler(spi_type* port, uint16_t scr)
{
	port->CLKCTL = (port->CLKCTL & ~SPI_CLKCTL_SCR_MASK) |
	               ((uint32_t)scr & SPI_CLKCTL_SCR_MASK);
}

static inline void spi_set_data_width(spi_type* port, uint8_t data_width)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_DSS_OFS, 5U, data_width);
}

static inline void spi_set_cpol(spi_type* port, uint8_t cpol)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_SPO_OFS, 1U, cpol);
}
static inline void spi_set_cpha(spi_type* port, uint8_t cpha)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_SPH_OFS, 1U, cpha);
}
static inline void spi_set_cs_selector(spi_type* port, uint8_t cs_selector)
{
	WRITE_FIELD(port->CTL0, SPI_CTL0_CSSEL_OFS, SPI_CTL0_CSSEL_WIDTH,
	            cs_selector);
}

static inline void spi_set_msb(spi_type* port, uint8_t msb)
{
	WRITE_FIELD(port->CTL1, SPI_CTL1_MSB_OFS, 1U, msb);
}

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

	volatile uint32_t* ctl1 = &p_spi_x->CTL1;

	// Wait for any possible transfer operations on the peripheral to end
	uint32_t timeout = SPI_SOFTWARE_TIMEOUT;
	while (IS_BIT_SET(p_spi_x->STAT1, SPI_STAT_BUSY_OFS)) {
		if (timeout == 0U) {
			return SPI_BUSY;
		}
		timeout--;
	}

	// Disable the functional SPI block
	CLEAR_BIT(*ctl1, SPI_CTL1_ENABLE_OFS);

	// Assert peripheral reset
	p_spi_x->RSTCTL = SPI_RSTCTL_KEY_UNLOCK_W |
	                  SPI_RSTCTL_RESETSTKYCLR_CLR |
	                  SPI_RSTCTL_RESETASSERT_ASSERT;

	// Confirm the rest took place
	if ((p_spi_x->STAT0 & SPI_GPRCM_STAT_RESETSTKY_MASK) == 0U) {
		return SPI_ERROR_INVALID_STATE;
	}

	// Disable peripheral power last
	p_spi_x->PWREN = SPI_PWREN_KEY_UNLOCK_W | SPI_PWREN_ENABLE_DISABLE;

	return SPI_OK;
}

// NOTE: @DATA_SEND_RECEIVE_POLLING

/********************************************************************************
 * - spi TX / RX ( STATIC ) HELPER FUNCTIONS PROTOTYPES
 *******************************************************************************/

static spi_status_t spi_wait_tx_ready(spi_type* p_spi_x, uint32_t timeout);
static spi_status_t spi_wait_rx_ready(spi_type* p_spi_x, uint32_t timeout);
static spi_status_t spi_wait_idle(spi_type* p_spi_x, uint32_t timeout);
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
	VALIDATE_PTR(p_spi_handle->p_SPIx, SPI_ERROR_INVALID_PORT);
	VALIDATE_PTR(p_tx_buffer, SPI_ERROR_NULL_PTR);
	if (frame_count == 0) {
		return SPI_ERROR_INVALID_FRAME_COUNT;
	}

	//     Confirm the configured frame width matches the buffer type.
	uint32_t dss = READ_FIELD(p_spi_handle->p_SPIx->CTL0, SPI_CTL0_DSS_OFS,
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

static spi_status_t spi_wait_tx_ready(spi_type* p_spi_x, uint32_t timeout)
{
	uint32_t elapsed = 0;
	while ((p_spi_x->STAT0 & SPI_STAT_TNF_MASK) == 0) // TX FIFO full
	{
		if (++elapsed >= timeout) {
			return SPI_ERROR_TIMEOUT;
		}
	}
	return SPI_OK;
}

static spi_status_t spi_wait_rx_ready(spi_type* p_spi_x, uint32_t timeout)
{
	uint32_t elapsed = 0;
	while ((p_spi_x->STAT0 & SPI_STAT_RFE_MASK) != 0U) /* RX FIFO empty */
	{
		if (++elapsed >= timeout) {
			return SPI_ERROR_TIMEOUT;
		}
	}
	return SPI_OK;
}
static spi_status_t spi_wait_idle(spi_type* p_spi_x, uint32_t timeout)
{
	uint32_t elapsed = 0U;
	while (((p_spi_x->STAT0 & SPI_STAT_TFE_MASK) == 0) ||
	       ((p_spi_x->STAT0 & SPI_STAT_BUSY_MASK) != 0)) {
		if (++elapsed >= timeout) {
			return SPI_ERROR_TIMEOUT;
		}
	}
	return SPI_OK;
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

		p_spi_x->TXDATA =
		    (p_tx_buffer != NULL) ? (uint32_t)p_tx_buffer[i] : 0xFF;

		status = spi_wait_rx_ready(p_spi_x, timeout);
		if (status != SPI_OK) {
			return status;
		}

		uint32_t rx_frame = (uint8_t)p_spi_x->RXDATA;
		if (p_rx_buffer != NULL) {
			p_rx_buffer[i] = rx_frame;
		}
	}

	return spi_wait_idle(p_spi_x, timeout);
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
	// TODO: Check if function is correctly written

	//     Validate RX buffer and frame count.
	VALIDATE_PTR(p_spi_handle, SPI_ERROR_NULL_PTR);
	VALIDATE_PTR(p_spi_handle->p_SPIx, SPI_ERROR_INVALID_PORT);
	VALIDATE_PTR(p_rx_buffer, SPI_ERROR_NULL_PTR);
	if (frame_count == 0) {
		return SPI_ERROR_INVALID_FRAME_COUNT;
	}
	//     Confirm that the peripheral is initialized and enabled.
	if ((p_spi_handle->p_SPIx->CTL1 & SPI_CTL1_ENABLE_MASK) == 0) {
		return SPI_ERROR_NOT_ENABLED;
	}

	if (p_spi_handle->spi_config.SPI_Device_Mode ==
	    SPI_DEVICE_MODE_CONTROLLER) {
		//     For every requested frame, transmit one dummy frame.
		spi_status_t status = spi_transceive_pl(
		    p_spi_handle, NULL, p_rx_buffer, frame_count, timeout);

		return status;
	}
	else if (p_spi_handle->spi_config.SPI_Device_Mode ==
	         SPI_DEVICE_MODE_PERIPHERAL) {

		//     If the peripheral must respond during reception, preload
		//     TX data before the external controller starts clocking.
		p_spi_handle->p_SPIx->TXDATA = 0xFF;

		//     Poll STAT.RFE.
		uint32_t elapsed = 0;
		for (uint32_t frame = 0; frame < frame_count; frame++) {
			while ((p_spi_handle->p_SPIx->STAT0 &
			        SPI_STAT_RFE_MASK) != 0) {
				if (++elapsed >= timeout) {
					return SPI_ERROR_TIMEOUT;
				}
			}
			p_rx_buffer[frame] =
			    (uint8_t)p_spi_handle->p_SPIx->RXDATA;
		}

		//     Check RX overflow and TX underflow events where
		//     applicable.
		// if ((p_spi_handle->p_SPIx->RIS0 & SPI_RIS))
		// DO NOT HAVE SPI_RIS MASKS

		return SPI_OK;
	}
	else {
		return SPI_ERROR_INVALID_MODE;
	}
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
	// TODO: Finish work on the function

	if (EN_or_DI == ENABLE) {
		//     Validate the pointer and SPI instance.

		//     Verify peripheral power is enabled.

		//     Verify configuration has been completed.

		//     Verify that the SPI is currently disabled.

		//     Set CTL1.ENABLE.

		//     Optionally read back CTL1.ENABLE.

		// Return
	}
	else {
		//     Validate pointer and instance.

		//     Inspect STAT.BUSY.

		//     If active, wait with a timeout or return SPI_BUSY.

		//     Once idle, clear CTL1.ENABLE.

		// Return
	}

	/*
	 *Find it in:
	 TRM 26.2.1, Clock Control.
	 TRM 26.2.5, Initialization.
	 TRM 26.3.29, CTL1.ENABLE and CTL1.CP.
	 TRM 26.3.32, STAT.BUSY.
	 */

	return SPI_OK;
}
