#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_startup.h"
#include "mspm0g350x_systick.h"
#include <stdbool.h>

/********************************************************************************
 *
 * TODO: Finish work on functions and then test them one by one:
 * - i2c_controller_read_pl
 * - i2c_controller_write_read_pl
 * - i2c_target_write_pl
 * - i2c_target_read_pl
 * - i2c_peri_control
 *
 *******************************************************************************/

/********************************************************************************
 *
 * INFO: Driver map
 * @STATIC_HELPER_PROTOTYPES
 * @PERIPHERAL_CLOCK_SETUP
 * @INIT_DE-INIT
 * @DATA_SEND_RECEIVE_POLLING_CONTROLLER
 * @DATA_SEND_RECEIVE_POLLING_TARGET
 * @PERIPHERAL_CONTROL_API
 * @STATIC_HELPERS
 *
 *******************************************************************************/

// NOTE: @STATIC_HELPER_PROTOTYPES

static i2c_status_t i2c_wait_controller_idle(const i2c_type* p_i2c_x,
                                             uint32_t timeout_ms);
static i2c_status_t i2c_wait_target_idle(const i2c_type* p_i2c_x,
                                         uint32_t timeout_ms);
static i2c_status_t i2c_deactivate(i2c_type* p_i2c_x, uint32_t timeout_ms);
static i2c_status_t i2c_reset(i2c_type* p_i2c_x);
static inline void i2c_set_clock_source(i2c_type* port, uint8_t clock_source);
static inline void i2c_set_clock_divider(i2c_type* port, uint8_t ratio);
static inline void i2c_set_glitch_filter(i2c_type* port, uint8_t enable);
static inline void i2c_config_controller(i2c_type* port, uint16_t tpr,
                                         uint8_t clk_stretch);
static inline void i2c_config_target(i2c_type* port, uint16_t own_addr,
                                     uint8_t addr_mode, uint8_t clk_stretch);
static uint32_t i2c_remaining_ms(uint32_t start_ms, uint32_t timeout_ms);
static i2c_status_t i2c_decode_error(const i2c_type* p_i2c_x);
static uint32_t i2c_tx_fifo_space(const i2c_type* p_i2c_x);
static i2c_status_t i2c_flush_fifos(i2c_type* p_i2c_x);
static i2c_status_t i2c_wait_bus_free(const i2c_type* p_i2c_x,
                                      uint32_t start_ms, uint32_t timeout_ms);
static void i2c_post_start_guard(const i2c_handle_t* p_h);
static i2c_status_t i2c_ctrl_validate(const i2c_handle_t* p_h, uint16_t addr);
static i2c_status_t i2c_ctrl_begin(i2c_type* p, uint32_t start_ms,
                                   uint32_t timeout);
static void i2c_ctrl_abort(i2c_type* p);
static void i2c_write_csa(i2c_type* p, uint16_t addr, uint8_t addr_mode,
                          uint32_t dir);
static i2c_status_t i2c_drain_rx(i2c_type* p, uint8_t* buf, uint32_t len,
                                 uint32_t start_ms, uint32_t timeout);
static i2c_status_t i2c_wait_done(const i2c_type* p, uint32_t start_ms,
                                  uint32_t timeout);

// NOTE: @PERIPHERAL_CLOCK_SETUP

/********************************************************************************
 * @fn				- i2c_peri_clk_control
 *
 * @brief			- Enables or disables peripheral clock for I2C
 *
 * @param[*p_i2c_x]		- Base address of the I2C peripheral
 * @param[EN_or_DI]		- ENABLE or DISABLE macros
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_peri_clk_control(i2c_type* p_i2c_x, uint8_t EN_or_DI)
{
	VALIDATE_PTR(p_i2c_x, I2C_ERROR_NULL_PTR);
	VALIDATE_I2C_PORT(p_i2c_x);

	uint32_t pwren = 0U;

	if (EN_or_DI == ENABLE) {
		if (IS_BIT_SET(p_i2c_x->PWREN, I2C_PWREN_ENABLE)) {
			return I2C_OK;
		}

		WRITE_FIELD(pwren, I2C_PWREN_KEY, I2C_PWREN_KEY_WIDTH,
		            I2C_PWREN_KEY_VAL_UNLOCK);
		WRITE_FIELD(pwren, I2C_PWREN_ENABLE, I2C_PWREN_ENABLE_WIDTH,
		            ENABLE);
		p_i2c_x->PWREN = pwren;

		sys_delay_cpu_cycles(I2C_PWREN_STARTUP_ULPCLK_CYCLES *
		                     sys_clock_get_ulpclk_divider());

		if (!IS_BIT_SET(p_i2c_x->PWREN, I2C_PWREN_ENABLE)) {
			return I2C_ERROR_NOT_ENABLED;
		}

		return I2C_OK;
	}
	if (EN_or_DI == DISABLE) {
		if (!IS_BIT_SET(p_i2c_x->PWREN, I2C_PWREN_ENABLE)) {
			return I2C_OK;
		}

		if (IS_BIT_SET(p_i2c_x->CCR, I2C_CCR_ACTIVE) ||
		    IS_BIT_SET(p_i2c_x->TCTR, I2C_TCTR_ACTIVE)) {
			return I2C_ERROR_INVALID_STATE;
		}

		i2c_status_t st =
		    i2c_wait_controller_idle(p_i2c_x, I2C_DEFAULT_TIMEOUT_MS);
		if (st != I2C_OK) {
			return st;
		}

		WRITE_FIELD(pwren, I2C_PWREN_KEY, I2C_PWREN_KEY_WIDTH,
		            I2C_PWREN_KEY_VAL_UNLOCK);
		WRITE_FIELD(pwren, I2C_PWREN_ENABLE, I2C_PWREN_ENABLE_WIDTH,
		            DISABLE);
		p_i2c_x->PWREN = pwren;

		return I2C_OK;
	}
	return I2C_ERROR_INVALID_STATE;
}

// NOTE: @INIT_DE-INIT

/********************************************************************************
 * @fn				- i2c_init
 *
 * @brief			- Initializes a I2C peripheral
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_init(i2c_handle_t* p_i2c_handle)
{
	VALIDATE_PTR(p_i2c_handle, I2C_ERROR_NULL_PTR);
	i2c_type* port = p_i2c_handle->p_I2Cx;
	VALIDATE_I2C_PORT(port);

	// Device mode
	uint8_t device_mode = p_i2c_handle->i2c_config.I2C_Device_Mode;
	VALIDATE_I2C_DEVICE_MODE(device_mode);

	// Clock source
	uint8_t clock_source = p_i2c_handle->i2c_config.I2C_Clock_Source;
	VALIDATE_I2C_CLOCK_SOURCE(clock_source);

	// Clock divider
	uint8_t divider = p_i2c_handle->i2c_config.I2C_Clock_Divider;
	VALIDATE_I2C_CLOCK_PRESCALE(divider);

	// Glitch filter
	uint8_t glitch = p_i2c_handle->i2c_config.I2C_Enable_Glitch_Filter;
	VALIDATE_ENUM(glitch, I2C_GLITCH_FILTER_ENABLE,
	              I2C_ERROR_INVALID_STATE);

	// Clock stretching
	uint8_t stretch = p_i2c_handle->i2c_config.I2C_Clock_Stretch;
	VALIDATE_ENUM(stretch, I2C_CLOCK_STRETCH_ENABLE,
	              I2C_ERROR_INVALID_STATE);

	// Role-specific configuration
	uint16_t tpr = p_i2c_handle->i2c_config.I2C_Timer_Period;
	uint8_t addr_mode = p_i2c_handle->i2c_config.I2C_Addressing_Mode;
	uint16_t own_addr = p_i2c_handle->i2c_config.I2C_Own_Address;

	if (device_mode == I2C_DEVICE_MODE_CONTROLLER) {
		VALIDATE_I2C_TIMER_PERIOD(tpr);
		VALIDATE_RANGE(tpr, I2C_TIMER_PERIOD_MIN, I2C_TIMER_PERIOD_MAX,
		               I2C_ERROR_INVALID_SPEED);
	}
	else {
		VALIDATE_I2C_ADDRESSING_MODE(addr_mode);
		VALIDATE_I2C_ADDRESS(own_addr, addr_mode);
	}

	// Power on, reset to a known state, then make sure power is still on.
	i2c_status_t st = i2c_peri_clk_control(port, ENABLE);
	if (st != I2C_OK) {
		return st;
	}

	// Stop any role left active by a previous run (warm restart, re-init).
	st = i2c_deactivate(port, I2C_DEFAULT_TIMEOUT_MS);
	if (st != I2C_OK) {
		return st;
	}

	st = i2c_reset(port);
	if (st != I2C_OK) {
		return st;
	}

	st = i2c_peri_clk_control(port, ENABLE);
	if (st != I2C_OK) {
		return st;
	}

	// Common configuration
	i2c_set_clock_source(port, clock_source);
	i2c_set_clock_divider(port, divider);
	i2c_set_glitch_filter(port, glitch);

	// Role configuration + activation (ACTIVE is set last)
	if (device_mode == I2C_DEVICE_MODE_CONTROLLER) {
		i2c_config_controller(port, tpr, stretch);
		SET_BIT(port->CCR, I2C_CCR_ACTIVE);
	}
	else {
		i2c_config_target(port, own_addr, addr_mode, stretch);
		SET_BIT(port->TCTR, I2C_TCTR_ACTIVE);
	}

	return I2C_OK;
}

/********************************************************************************
 * @fn				- i2c_de_init
 *
 * @brief			- Resets a I2C peripheral
 *
 * @param[*p_i2c_x]		- Base address of the I2C peripheral
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_de_init(i2c_type* p_i2c_x)
{
	VALIDATE_PTR(p_i2c_x, I2C_ERROR_NULL_PTR);
	VALIDATE_I2C_PORT(p_i2c_x);

	// Peripheral is unpowered: registers are not accessible, nothing to
	// undo.
	if (!IS_BIT_SET(p_i2c_x->PWREN, I2C_PWREN_ENABLE)) {
		return I2C_OK;
	}

	// Wait for the bus, then clear CCR.ACTIVE / TCTR.ACTIVE if set.
	// Returns I2C_BUSY rather than forcing a reset on a live bus.
	i2c_status_t st = i2c_deactivate(p_i2c_x, I2C_DEFAULT_TIMEOUT_MS);
	if (st != I2C_OK) {
		return st;
	}

	// Reset to a known state while powered, then wait for RESETSTKY.
	st = i2c_reset(p_i2c_x);
	if (st != I2C_OK) {
		return st;
	}

	// Both roles are now inactive, so the DISABLE path accepts this call.
	return i2c_peri_clk_control(p_i2c_x, DISABLE);
}

// NOTE: @DATA_SEND_RECEIVE_POLLING_CONTROLLER

/********************************************************************************
 * @fn				- i2c_controller_write_pl
 *
 * @brief			- Write data over I2C controller using a polling
 * method
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 * @param[addr]			- Address to send data to
 * @param[*p_tx_buffer]		- Transmission buffer ( pointer )
 * @param[len]			- Number of bytes to send
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_controller_write_pl(i2c_handle_t* p_i2c_handle, uint16_t addr,
                                     const uint8_t* p_tx_buffer, uint32_t len,
                                     uint32_t timeout)
{
	// Validate handle, role, power, and ACTIVE.
	VALIDATE_PTR(p_i2c_handle, I2C_ERROR_NULL_PTR);
	i2c_type* port = p_i2c_handle->p_I2Cx;
	VALIDATE_I2C_PORT(port);

	if (len > 8U) {
		return I2C_ERROR_INVALID_LEN;
	}

	if (p_i2c_handle->i2c_config.I2C_Device_Mode !=
	    I2C_DEVICE_MODE_CONTROLLER) {
		return I2C_ERROR_INVALID_MODE;
	}
	if (!IS_BIT_SET(port->PWREN, I2C_PWREN_ENABLE) ||
	    !IS_BIT_SET(port->CCR, I2C_CCR_ACTIVE)) {
		return I2C_ERROR_NOT_ENABLED;
	}

	VALIDATE_PTR(p_tx_buffer, I2C_ERROR_NULL_PTR);
	VALIDATE_I2C_LEN(len);
	if (len > I2C_CCTR_CBLEN_VAL_MAX) {
		return I2C_ERROR_INVALID_LEN;
	}

	const uint8_t addr_mode = p_i2c_handle->i2c_config.I2C_Addressing_Mode;
	VALIDATE_I2C_ADDRESS(addr, addr_mode);

	// One absolute deadline for the whole transfer.
	const uint32_t start_ms = sys_tick_get_ms();

	// Controller idle and bus free before a new transaction.
	i2c_status_t st =
	    i2c_wait_controller_idle(port, i2c_remaining_ms(start_ms, timeout));
	if (st != I2C_OK) {
		return st;
	}
	st = i2c_wait_bus_free(port, start_ms, timeout);
	if (st != I2C_OK) {
		return st;
	}

	// Drop stale FIFO contents, then preload as much data as fits.
	st = i2c_flush_fifos(port);
	if (st != I2C_OK) {
		return st;
	}

	uint32_t queued = 0U;
	while ((queued < len) && (i2c_tx_fifo_space(port) > 0U)) {
		port->CTXDATA = (uint32_t)p_tx_buffer[queued];
		queued++;
	}
	//
	// Target address, address mode, direction.
	uint32_t csa = 0U;
	WRITE_FIELD(csa, I2C_CSA_TADDR, I2C_CSA_TADDR_WIDTH, addr);
	WRITE_FIELD(csa, I2C_CSA_CMODE, I2C_CSA_CMODE_WIDTH, addr_mode);
	WRITE_FIELD(csa, I2C_CSA_DIR, I2C_CSA_DIR_WIDTH, DISABLE);
	port->CSA = csa;

	// One CCTR write launches START + address + data + STOP.
	uint32_t cctr = 0U;
	WRITE_FIELD(cctr, I2C_CCTR_CBLEN, I2C_CCTR_CBLEN_WIDTH, len);
	WRITE_FIELD(cctr, I2C_CCTR_START, I2C_CCTR_START_WIDTH, ENABLE);
	WRITE_FIELD(cctr, I2C_CCTR_STOP, I2C_CCTR_STOP_WIDTH, ENABLE);
	WRITE_FIELD(cctr, I2C_CCTR_BURSTRUN, I2C_CCTR_BURSTRUN_WIDTH, ENABLE);
	port->CCTR = cctr;

	i2c_post_start_guard(p_i2c_handle);

	// Refill TX FIFO while bytes remain, watching for errors.
	st = I2C_OK;
	while ((queued < len) && (st == I2C_OK)) {
		st = i2c_decode_error(port);
		if (st != I2C_OK) {
			break;
		}
		if (i2c_remaining_ms(start_ms, timeout) == 0U) {
			st = I2C_ERROR_TIMEOUT;
			break;
		}
		if (i2c_tx_fifo_space(port) > 0U) {
			port->CTXDATA = (uint32_t)p_tx_buffer[queued];
			queued++;
		}
	}

	// Wait for the controller to finish, still checking for errors.
	st = I2C_OK;
	while (st == I2C_OK) {
		st = i2c_decode_error(port);
		if (st != I2C_OK) {
			break;
		}
		const uint32_t csr = port->CSR;
		if (IS_BIT_SET(csr, I2C_CSR_IDLE) &&
		    !IS_BIT_SET(csr, I2C_CSR_BUSY)) {
			break;
		}
		if (i2c_remaining_ms(start_ms, timeout) == 0U) {
			st = I2C_ERROR_TIMEOUT;
		}
	}

	// Final decode: errors can latch right as BUSY clears.
	if (st == I2C_OK) {
		st = i2c_decode_error(port);
	}

	if (st != I2C_OK) {
		// Let the controller settle and discard unsent bytes.
		// A bounded wait: ignore its result, we already have an error.
		(void)i2c_wait_controller_idle(port, I2C_DEFAULT_TIMEOUT_MS);
		i2c_flush_fifos(port);
		return st;
	}

	return I2C_OK;
}

/********************************************************************************
 * @fn				- i2c_read_data_pl
 *
 * @brief			- Reads data over I2C controller using a polling
 * method
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 * @param[addr]			- Address to send data to
 * @param[*p_rx_buffer]		- Receive buffer ( pointer )
 * @param[len]			- Number of bytes to read
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_controller_read_pl(i2c_handle_t* p_i2c_handle, uint16_t addr,
                                    uint8_t* p_rx_buffer, uint32_t len,
                                    uint32_t timeout)
{
	i2c_status_t st = i2c_ctrl_validate(p_i2c_handle, addr);
	if (st != I2C_OK) {
		return st;
	}
	VALIDATE_PTR(p_rx_buffer, I2C_ERROR_NULL_PTR);
	VALIDATE_I2C_LEN(len);
	if (len > I2C_CCTR_CBLEN_VAL_MAX) {
		return I2C_ERROR_INVALID_LEN;
	}

	i2c_type* port = p_i2c_handle->p_I2Cx;
	const uint32_t start_ms = sys_tick_get_ms();

	st = i2c_ctrl_begin(port, start_ms, timeout);
	if (st != I2C_OK) {
		return st;
	}

	i2c_write_csa(port, addr, p_i2c_handle->i2c_config.I2C_Addressing_Mode,
	              I2C_CSA_DIR_VAL_RECEIVE);

	/* CACKOEN = 0, ACK = 0: hardware NACKs the last byte, then STOP */
	uint32_t cctr = 0U;
	WRITE_FIELD(cctr, I2C_CCTR_CBLEN, I2C_CCTR_CBLEN_WIDTH, len);
	WRITE_FIELD(cctr, I2C_CCTR_START, I2C_CCTR_START_WIDTH, ENABLE);
	WRITE_FIELD(cctr, I2C_CCTR_STOP, I2C_CCTR_STOP_WIDTH, ENABLE);
	WRITE_FIELD(cctr, I2C_CCTR_BURSTRUN, I2C_CCTR_BURSTRUN_WIDTH, ENABLE);
	port->CCTR = cctr;

	i2c_post_start_guard(p_i2c_handle);

	st = i2c_drain_rx(port, p_rx_buffer, len, start_ms, timeout);
	if (st == I2C_OK) {
		st = i2c_wait_done(port, start_ms, timeout);
	}
	if (st != I2C_OK) {
		i2c_ctrl_abort(port);
	}
	return st;
}

/********************************************************************************
 * @fn				- i2c_controller_write_read_pl
 *
 * @brief			- Reads data over I2C controller using a polling
 * method
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 * @param[addr]			- Address to send data to
 * @param[*write_buffer]	- Write buffer
 * @param[wlen]			- Number of bytes to write
 * @param[*receive_buffer]	- Receiving buffer
 * @param[rlen]			- Number of bytes to receive
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_controller_write_read_pl(
    i2c_handle_t* p_i2c_handle, uint16_t addr, const uint8_t* write_buffer,
    uint32_t wlen, uint8_t* receive_buffer, uint32_t rlen, uint32_t timeout)
{
}

// NOTE: @DATA_SEND_RECEIVE_POLLING_TARGET

/********************************************************************************
 * @fn				- i2c_target_write_pl
 *
 * @brief			- Write data over I2C target using a polling
 * method
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 * @param[*p_tx_buffer]		- Receiver buffer ( pointer )
 * @param[len]			- Number of bytes to send
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_target_write_pl(i2c_handle_t* p_i2c_handle,
                                 const uint8_t* p_tx_buffer, uint32_t len,
                                 uint32_t timeout)
{
	// i2c_target_write_pl / target_transmit:
	//
	// Validate target role, TCTR.ACTIVE, source buffer, and len.
	//
	// Create an absolute deadline.
	//
	// Flush stale target TX FIFO data left by an earlier short transaction.
	//
	// Preload as many target bytes as fit into TTXDATA.
	//
	// Wait for the target to be addressed in transmit direction:
	// - TSR.TXMODE identifies target transmitter mode.
	// - TSR.TREQ indicates that the controller is waiting while the
	//   target stretches SCL for more TX data.
	//
	// While the controller continues reading:
	// - Refill TTXDATA when FIFO space or TREQ indicates data is needed.
	// - Stop on target STOP or completion/NACK event.
	// - Check the transaction deadline.
	//
	// If the controller terminates before len bytes:
	// - Flush remaining stale TX FIFO data.
	// - Return the number of bytes consumed, or a documented short-transfer
	// status.
	//
	// Source:
	// - TRM §25.2.4.2.1: target transmitter operation
	// - TRM §25.3.46: TCTR target clock-stretch and stale-TX controls
	// - TRM §25.3.47: TSR.TXMODE, TREQ, STALE_TXFIFO
	// - TRM §25.3.49: TTXDATA.VALUE
	// - TRM §25.3.51 and §25.3.52: target FIFO control/status
}

/********************************************************************************
 * @fn				- i2c_target_read_pl
 *
 * @brief			- Reads data over I2C target using a polling
 * method
 *
 * @param[*p_i2c_handle]	- Handle structure of a I2C peripheral
 * @param[*p_rx_buffer]		- Receiver buffer ( pointer )
 * @param[len]			- Number of bytes to read
 * @param[timeout]		- Polling timeout in seconds
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_target_read_pl(i2c_handle_t* p_i2c_handle,
                                uint8_t* p_rx_buffer, uint32_t len,
                                uint32_t timeout)
{
	// i2c_target_read_pl / target_receive:
	//
	// Validate target role, TCTR.ACTIVE, destination buffer, and capacity.
	//
	// Create an absolute deadline.
	//
	// Ensure the target RX FIFO is empty before accepting a new packet.
	//
	// Wait for an address match in receive direction:
	// - TSR.RXMODE identifies target receiver mode.
	// - TSR.ADDRMATCH can be inspected for diagnostics.
	// - A target START event can be monitored through CPU_INT.RIS.TSTART.
	//
	// While no STOP has been detected:
	// - If target RX data is available, read TRXDATA.VALUE.
	// - Store the byte if capacity remains.
	// - If capacity is exhausted, apply the documented overflow policy:
	//   return overflow, NACK further input if supported, or continue
	//   draining.
	// - Check the software deadline.
	//
	// End on CPU_INT.RIS.TSTOP, not only when the supplied capacity is
	// full.
	//
	// Return the actual byte count through an output parameter.
	//
	// Source:
	// - TRM §25.2.4.2.1: target receiver operation
	// - TRM §25.3.47: TSR.RXMODE, RREQ, ADDRMATCH
	// - TRM §25.3.48: TRXDATA.VALUE
	// - TRM §25.3.10 and §25.3.13: RIS/ICLR target START and STOP
}

// NOTE: @PERIPHERAL_CONTROL_API

/********************************************************************************
 * @fn				- i2c_peri_control
 *
 * @brief			- Sets I2C peripheral control
 *
 * @param[*p_i2c_x]		- Base address of the I2C peripheral
 * @param[EN_or_DI]		- ENABLE or DISABLE macros
 *
 * @return			- Success / Failure status of the
 *function
 *
 * @Note			- None
 *******************************************************************************/

i2c_status_t i2c_peri_control(i2c_type* p_i2c_x, uint8_t EN_or_DI)
{
	VALIDATE_PTR(p_i2c_x, I2C_ERROR_NULL_PTR);
	VALIDATE_I2C_PORT(p_i2c_x);

	if ((EN_or_DI != ENABLE) && (EN_or_DI != DISABLE)) {
		return I2C_ERROR_INVALID_STATE;
	}

	/* Registers are not accessible while PWREN.ENABLE == 0 */
	if (!IS_BIT_SET(p_i2c_x->PWREN, I2C_PWREN_ENABLE)) {
		return I2C_ERROR_NOT_ENABLED;
	}

	const bool ctrl_active = IS_BIT_SET(p_i2c_x->CCR, I2C_CCR_ACTIVE);
	const bool tgt_active = IS_BIT_SET(p_i2c_x->TCTR, I2C_TCTR_ACTIVE);

	if (EN_or_DI == ENABLE) {
		/* Both roles active at once is never valid */
		if (ctrl_active && tgt_active) {
			return I2C_ERROR_INVALID_STATE;
		}
		/* Role is already running: nothing to do, never write ACTIVE
		 * twice */
		if (ctrl_active || tgt_active) {
			return I2C_OK;
		}

		/* Role selection: i2c_config_target() sets TOAR.OAREN, the
		 * controller path never does, and i2c_reset() clears it. */
		const uint32_t toar = p_i2c_x->TOAR;

		if (IS_BIT_SET(toar, I2C_TOAR_OAREN)) {
			/* ---- Target ---- */
			const uint32_t oar =
			    READ_FIELD(toar, I2C_TOAR_OAR, I2C_TOAR_OAR_WIDTH);
			const uint32_t tmod = READ_FIELD(toar, I2C_TOAR_TMODE,
			                                 I2C_TOAR_TMODE_WIDTH);
			VALIDATE_I2C_ADDRESS(oar, tmod);

			SET_BIT(p_i2c_x->TCTR, I2C_TCTR_ACTIVE);
			if (!IS_BIT_SET(p_i2c_x->TCTR, I2C_TCTR_ACTIVE)) {
				return I2C_ERROR_NOT_ENABLED;
			}
			return I2C_OK;
		}

		/* ---- Controller ---- */
		const uint32_t clk_sel =
		    READ_BIT(p_i2c_x->CLKSEL, I2C_CLKSEL_MFCLK_SEL) +
		    READ_BIT(p_i2c_x->CLKSEL, I2C_CLKSEL_BUSCLK_SEL);
		if (clk_sel != 1U) {
			return I2C_ERROR_INVALID_CLOCK_SRC;
		}

		SET_BIT(p_i2c_x->CCR, I2C_CCR_ACTIVE);
		if (!IS_BIT_SET(p_i2c_x->CCR, I2C_CCR_ACTIVE)) {
			return I2C_ERROR_NOT_ENABLED;
		}
		return I2C_OK;
	}

	/* ---- DISABLE ---- */
	if (ctrl_active) {
		const uint32_t start = sys_tick_get_ms();
		for (;;) {
			const uint32_t csr = p_i2c_x->CSR;
			if (!IS_BIT_SET(csr, I2C_CSR_BUSY) &&
			    IS_BIT_SET(csr, I2C_CSR_IDLE)) {
				break;
			}
			if ((uint32_t)(sys_tick_get_ms() - start) >=
			    I2C_DEFAULT_TIMEOUT_MS) {
				return I2C_BUSY;
			}
		}
		CLEAR_BIT(p_i2c_x->CCR, I2C_CCR_ACTIVE);
	}

	if (tgt_active) {
		const i2c_status_t st =
		    i2c_wait_target_idle(p_i2c_x, I2C_DEFAULT_TIMEOUT_MS);
		if (st != I2C_OK) {
			return st;
		}
		CLEAR_BIT(p_i2c_x->TCTR, I2C_TCTR_ACTIVE);
	}

	return I2C_OK;
}

// NOTE: @STATIC_HELPERS

static i2c_status_t i2c_wait_controller_idle(const i2c_type* p_i2c_x,
                                             uint32_t timeout_ms)
{
	const uint32_t start = sys_tick_get_ms();

	while (IS_BIT_SET(p_i2c_x->CSR, I2C_CSR_BUSY)) {
		if ((uint32_t)(sys_tick_get_ms() - start) >= timeout_ms) {
			return I2C_BUSY;
		}
	}

	return I2C_OK;
}

static i2c_status_t i2c_wait_target_idle(const i2c_type* p_i2c_x,
                                         uint32_t timeout_ms)
{
	const uint32_t start = sys_tick_get_ms();

	while (IS_BIT_SET(p_i2c_x->TSR, I2C_TSR_BUSBSY)) {
		if ((uint32_t)(sys_tick_get_ms() - start) >= timeout_ms) {
			return I2C_BUSY;
		}
	}
	return I2C_OK;
}

static i2c_status_t i2c_deactivate(i2c_type* p_i2c_x, uint32_t timeout_ms)
{
	i2c_status_t st;

	if (IS_BIT_SET(p_i2c_x->CCR, I2C_CCR_ACTIVE)) {
		st = i2c_wait_controller_idle(p_i2c_x, timeout_ms);
		if (st != I2C_OK) {
			return st;
		}
		CLEAR_BIT(p_i2c_x->CCR, I2C_CCR_ACTIVE);
	}

	if (IS_BIT_SET(p_i2c_x->TCTR, I2C_TCTR_ACTIVE)) {
		st = i2c_wait_target_idle(p_i2c_x, timeout_ms);
		if (st != I2C_OK) {
			return st;
		}
		CLEAR_BIT(p_i2c_x->TCTR, I2C_TCTR_ACTIVE);
	}

	return I2C_OK;
}

static i2c_status_t i2c_reset(i2c_type* p_i2c_x)
{
	uint32_t rstctl = 0U;
	WRITE_FIELD(rstctl, I2C_RSTCTL_KEY, I2C_RSTCTL_KEY_WIDTH,
	            I2C_RSTCTL_KEY_VAL_UNLOCK);
	WRITE_FIELD(rstctl, I2C_RSTCTL_RESETSTKYCLR,
	            I2C_RSTCTL_RESETSTKYCLR_WIDTH, ENABLE);
	WRITE_FIELD(rstctl, I2C_RSTCTL_RESETASSERT,
	            I2C_RSTCTL_RESETASSERT_WIDTH, ENABLE);
	p_i2c_x->RSTCTL = rstctl;

	const uint32_t start = sys_tick_get_ms();
	while (READ_FIELD(p_i2c_x->STAT0, I2C_STAT0_RESETSTKY,
	                  I2C_STAT0_RESETSTKY_WIDTH) == 0U) {
		if ((uint32_t)(sys_tick_get_ms() - start) >=
		    I2C_DEFAULT_TIMEOUT_MS) {
			return I2C_ERROR_TIMEOUT;
		}
	}

	return I2C_OK;
}
static inline void i2c_set_clock_source(i2c_type* port, uint8_t clock_source)
{
	uint32_t clksel = 0U;
	if (clock_source == I2C_CLOCK_SRC_MFCLK) {
		SET_BIT(clksel, I2C_CLKSEL_MFCLK_SEL);
	}
	else {
		SET_BIT(clksel, I2C_CLKSEL_BUSCLK_SEL);
	}
	port->CLKSEL = clksel;
}

static inline void i2c_set_clock_divider(i2c_type* port, uint8_t ratio)
{
	WRITE_FIELD(port->CLKDIV, I2C_CLKDIV_RATIO, I2C_CLKDIV_RATIO_WIDTH,
	            ratio);
}

static inline void i2c_set_glitch_filter(i2c_type* port, uint8_t enable)
{
	WRITE_FIELD(port->GFCTL, I2C_GFCTL_AGFEN, I2C_GFCTL_AGFEN_WIDTH,
	            enable);
}

static inline void i2c_config_controller(i2c_type* port, uint16_t tpr,
                                         uint8_t clk_stretch)
{
	port->CCTR = 0U;
	WRITE_FIELD(port->CTPR, I2C_CTPR_TPR, I2C_CTPR_TPR_WIDTH, tpr);
	WRITE_FIELD(port->CCR, I2C_CCR_CLKSTRETCH, I2C_CCR_CLKSTRETCH_WIDTH,
	            clk_stretch);
	CLEAR_BIT(port->CCR, I2C_CCR_MCTL);
	CLEAR_BIT(port->CCR, I2C_CCR_LPBK);
}

static inline void i2c_config_target(i2c_type* port, uint16_t own_addr,
                                     uint8_t addr_mode, uint8_t clk_stretch)
{
	uint32_t toar = 0U;
	WRITE_FIELD(toar, I2C_TOAR_OAR, I2C_TOAR_OAR_WIDTH, own_addr);
	WRITE_FIELD(toar, I2C_TOAR_TMODE, I2C_TOAR_TMODE_WIDTH, addr_mode);
	WRITE_FIELD(toar, I2C_TOAR_OAREN, I2C_TOAR_OAREN_WIDTH, ENABLE);
	port->TOAR = toar;

	WRITE_FIELD(port->TCTR, I2C_TCTR_TCLKSTRETCH,
	            I2C_TCTR_TCLKSTRETCH_WIDTH, clk_stretch);
	CLEAR_BIT(port->TCTR, I2C_TCTR_GENCALL);
}

static uint32_t i2c_remaining_ms(uint32_t start_ms, uint32_t timeout_ms)
{
	const uint32_t elapsed = (uint32_t)(sys_tick_get_ms() - start_ms);
	return (elapsed >= timeout_ms) ? 0U : (timeout_ms - elapsed);
}

static i2c_status_t i2c_decode_error(const i2c_type* p_i2c_x)
{
	const uint32_t csr = p_i2c_x->CSR;

	if (IS_BIT_SET(csr, I2C_CSR_ARBLST)) {
		return I2C_ERROR_ARBITRATION_LOST;
	}
	if (IS_BIT_SET(csr, I2C_CSR_ADRACK)) {
		return I2C_ERROR_NACK_ADDR;
	}
	if (IS_BIT_SET(csr, I2C_CSR_DATACK)) {
		return I2C_ERROR_NACK_DATA;
	}
	if (IS_BIT_SET(csr, I2C_CSR_ERR)) {
		return I2C_ERROR_BUS_ERROR;
	}
	return I2C_OK;
}
static uint32_t i2c_tx_fifo_space(const i2c_type* p_i2c_x)
{
	return READ_FIELD(p_i2c_x->CFIFOSR, I2C_CFIFOSR_TXFIFOCNT,
	                  I2C_CFIFOSR_TXFIFOCNT_WIDTH);
}

static i2c_status_t i2c_flush_fifos(i2c_type* p_i2c_x)
{
	// Only call while CSR.BUSY == 0 (CFIFOSR is only valid then).
	SET_BIT(p_i2c_x->CFIFOCTL, I2C_CFIFOCTL_TXFLUSH);
	SET_BIT(p_i2c_x->CFIFOCTL, I2C_CFIFOCTL_RXFLUSH);

	i2c_status_t st = I2C_OK;
	const uint32_t start = sys_tick_get_ms();
	while ((READ_FIELD(p_i2c_x->CFIFOSR, I2C_CFIFOSR_TXFIFOCNT,
	                   I2C_CFIFOSR_TXFIFOCNT_WIDTH) != I2C_FIFO_DEPTH) ||
	       (READ_FIELD(p_i2c_x->CFIFOSR, I2C_CFIFOSR_RXFIFOCNT,
	                   I2C_CFIFOSR_RXFIFOCNT_WIDTH) != 0U)) {
		if ((uint32_t)(sys_tick_get_ms() - start) >=
		    I2C_DEFAULT_TIMEOUT_MS) {
			st = I2C_ERROR_TIMEOUT;
			break;
		}
	}

	CLEAR_BIT(p_i2c_x->CFIFOCTL, I2C_CFIFOCTL_TXFLUSH);
	CLEAR_BIT(p_i2c_x->CFIFOCTL, I2C_CFIFOCTL_RXFLUSH);
	return st;
}

static i2c_status_t i2c_wait_bus_free(const i2c_type* p_i2c_x,
                                      uint32_t start_ms, uint32_t timeout_ms)
{
	while (IS_BIT_SET(p_i2c_x->CSR, I2C_CSR_BUSBSY)) {
		if (i2c_remaining_ms(start_ms, timeout_ms) == 0U) {
			return I2C_ERROR_BUS_BUSY;
		}
	}
	return I2C_OK;
}

static void i2c_post_start_guard(const i2c_handle_t* p_h)
{
	// Wait at least three I2C functional-clock cycles before trusting
	// CSR.BUSY (SDK errata workaround, see release notes).
	// Exact for BUSCLK only; MFCLK needs its own CPU-cycle ratio.
	const uint32_t ratio = (uint32_t)p_h->i2c_config.I2C_Clock_Divider + 1U;
	sys_delay_cpu_cycles(3U * ratio * sys_clock_get_ulpclk_divider());
}

static i2c_status_t i2c_ctrl_validate(const i2c_handle_t* p_h, uint16_t addr)
{
	VALIDATE_PTR(p_h, I2C_ERROR_NULL_PTR);
	const i2c_type* port = p_h->p_I2Cx;
	VALIDATE_I2C_PORT(port);

	if (p_h->i2c_config.I2C_Device_Mode != I2C_DEVICE_MODE_CONTROLLER) {
		return I2C_ERROR_INVALID_MODE;
	}
	if (!IS_BIT_SET(port->PWREN, I2C_PWREN_ENABLE) ||
	    !IS_BIT_SET(port->CCR, I2C_CCR_ACTIVE)) {
		return I2C_ERROR_NOT_ENABLED;
	}
	const uint8_t addr_mode = p_h->i2c_config.I2C_Addressing_Mode;
	VALIDATE_I2C_ADDRESS(addr, addr_mode);
	return I2C_OK;
}

static i2c_status_t i2c_ctrl_begin(i2c_type* p, uint32_t start_ms,
                                   uint32_t timeout)
{
	i2c_status_t st =
	    i2c_wait_controller_idle(p, i2c_remaining_ms(start_ms, timeout));
	if (st != I2C_OK) {
		return st;
	}
	st = i2c_wait_bus_free(p, start_ms, timeout);
	if (st != I2C_OK) {
		return st;
	}
	return i2c_flush_fifos(p);
}

static void i2c_ctrl_abort(i2c_type* p)
{
	(void)i2c_wait_controller_idle(p, I2C_DEFAULT_TIMEOUT_MS);
	(void)i2c_flush_fifos(p);
}

static void i2c_write_csa(i2c_type* p, uint16_t addr, uint8_t addr_mode,
                          uint32_t dir)
{
	uint32_t csa = 0U;
	WRITE_FIELD(csa, I2C_CSA_TADDR, I2C_CSA_TADDR_WIDTH, addr);
	WRITE_FIELD(csa, I2C_CSA_CMODE, I2C_CSA_CMODE_WIDTH, addr_mode);
	WRITE_FIELD(csa, I2C_CSA_DIR, I2C_CSA_DIR_WIDTH, dir);
	p->CSA = csa;
}

static i2c_status_t i2c_drain_rx(i2c_type* p, uint8_t* buf, uint32_t len,
                                 uint32_t start_ms, uint32_t timeout)
{
	uint32_t n = 0U;
	while (n < len) {
		const i2c_status_t st = i2c_decode_error(p);
		if (st != I2C_OK) {
			return st;
		}
		if (READ_FIELD(p->CFIFOSR, I2C_CFIFOSR_RXFIFOCNT,
		               I2C_CFIFOSR_RXFIFOCNT_WIDTH) != 0U) {
			buf[n] = (uint8_t)(p->CRXDATA &
			                   0xFFU); /* CRXDATA.VALUE [7:0] */
			n++;
			continue;
		}
		if (i2c_remaining_ms(start_ms, timeout) == 0U) {
			return I2C_ERROR_TIMEOUT;
		}
	}
	return I2C_OK;
}

static i2c_status_t i2c_wait_done(const i2c_type* p, uint32_t start_ms,
                                  uint32_t timeout)
{
	for (;;) {
		const i2c_status_t st = i2c_decode_error(p);
		if (st != I2C_OK) {
			return st;
		}
		const uint32_t csr = p->CSR;
		if (IS_BIT_SET(csr, I2C_CSR_IDLE) &&
		    !IS_BIT_SET(csr, I2C_CSR_BUSY)) {
			break;
		}
		if (i2c_remaining_ms(start_ms, timeout) == 0U) {
			return I2C_ERROR_TIMEOUT;
		}
	}
	return i2c_decode_error(p);
}
