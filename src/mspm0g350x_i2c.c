#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_startup.h"
#include "mspm0g350x_systick.h"

/********************************************************************************
 *
 * TODO: Finish work on functions and then test them one by one:
 * - i2c_controller_write_pl
 * - i2c_controller_read_pl
 * - i2c_controller_write_read_pl
 * - i2c_target_write_pl
 * - i2c_target_read_pl
 * - i2c_peri_control
 *
 * TODO:
 * Research how open drain works ( in particular i2c application )
 *
 * TODO: Implement Systick where it is necessary ( I2C )
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
	// i2c_controller_write_pl:
	//
	// Validate:
	// - Handle and controller role.
	// - Peripheral power and CCR.ACTIVE.
	// - TX buffer.
	// - len in the range 1..0xFFF.
	// - Address against the configured address mode.
	//
	// Create one absolute deadline for the complete transfer.
	//
	// Wait until CSR.IDLE is set.
	// Wait until CSR.BUSBSY is clear for this new transaction.
	//
	// Flush stale controller FIFO contents.
	//
	// Preload as many bytes as possible into CTXDATA.
	//
	// Build CSA:
	// - CSA.TADDR = target address.
	// - CSA.CMODE = configured address mode.
	// - CSA.DIR = transmit.
	//
	// Build one CCTR value:
	// - CCTR.CBLEN = len.
	// - CCTR.ACK = don't-care/cleared for transmit.
	// - CCTR.START = 1.
	// - CCTR.STOP = 1.
	// - CCTR.BURSTRUN = 1.
	//
	// Write CCTR once to launch the transaction.
	//
	// Apply the three-I2C-clock post-start guard.
	//
	// While bytes remain to be queued:
	// - Wait for TX FIFO space or the TX FIFO trigger condition.
	// - Check ADRACK, DATACK, ARBLST, hardware timeout, and software
	// deadline.
	// - Write further bytes to CTXDATA.
	//
	// Wait until the controller transaction finishes.
	//
	// Decode final status:
	// - CSR.ARBLST -> I2C_ERROR_ARBITRATION_LOST.
	// - CSR.ADRACK -> I2C_ERROR_NACK_ADDR.
	// - CSR.DATACK -> I2C_ERROR_NACK_DATA.
	// - Deadline expiry -> I2C_ERROR_TIMEOUT.
	//
	// Confirm the controller returned to idle.
	// Return I2C_OK.
	//
	// Source:
	// - TRM §25.2.4.1.1, Table 25-4: transmit from idle
	// - TRM §25.2.4.1.2: controller transmitter operation
	// - TRM §25.3.32: CSA
	// - TRM §25.3.33: CCTR
	// - TRM §25.3.34: CSR
	// - TRM §25.3.36: CTXDATA
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
	// i2c_controller_read_pl:
	//
	// Validate:
	// - Handle and controller role.
	// - Peripheral power and CCR.ACTIVE.
	// - RX buffer.
	// - len in the range 1..0xFFF.
	// - Target address.
	//
	// Create one absolute deadline.
	//
	// Wait for CSR.IDLE and a free bus.
	//
	// Flush stale RX FIFO contents.
	//
	// Build CSA:
	// - CSA.TADDR = target address.
	// - CSA.CMODE = configured address mode.
	// - CSA.DIR = receive.
	//
	// Build one CCTR value:
	// - CCTR.CBLEN = len.
	// - CCTR.CACKOEN = 0 for the first implementation.
	// - CCTR.ACK = 0 so the final received byte is NACKed.
	// - CCTR.START = 1.
	// - CCTR.STOP = 1.
	// - CCTR.BURSTRUN = 1.
	//
	// Launch the transfer with one CCTR write.
	//
	// Apply the three-I2C-clock post-start guard.
	//
	// Until len bytes have been copied:
	// - Wait for controller RX data.
	// - Check address NACK, arbitration loss, hardware timeout,
	//   and the software deadline.
	// - Read CRXDATA.VALUE into the caller's buffer.
	//
	// Wait for CSR.BUSY to clear and CSR.IDLE to set.
	//
	// Confirm exactly len bytes were received.
	// Decode final controller status.
	// Return I2C_OK.
	//
	// Source:
	// - TRM §25.2.4.1.1, Table 25-6: receive from idle
	// - TRM §25.2.4.1.2: controller receiver operation
	// - TRM §25.3.33: CCTR.ACK, STOP, START, BURSTRUN, CBLEN
	// - TRM §25.3.35: CRXDATA.VALUE
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
	// i2c_controller_write_read_pl:
	//
	// Validate both buffers, both lengths, address, controller role,
	// peripheral power, and CCR.ACTIVE.
	//
	// Restrict wlen and rlen independently to 1..0xFFF.
	//
	// Create one deadline shared by both phases.
	//
	// Wait for CSR.IDLE and CSR.BUSBSY == 0 only before the first phase.
	//
	// Flush stale controller FIFOs.
	//
	// WRITE PHASE:
	// - Preload controller TX FIFO.
	// - Program CSA for transmit.
	// - Program CCTR.CBLEN = wlen.
	// - Set START = 1.
	// - Set STOP = 0.
	// - Set BURSTRUN = 1.
	// - Start the write burst.
	// - Apply the post-start guard.
	// - Refill TX FIFO until all write bytes are queued.
	// - Wait for the write burst to finish.
	// - Check address NACK, data NACK, arbitration loss, and timeout.
	//
	// Do not wait for CSR.BUSBSY to clear.
	// The bus must remain owned between phases.
	//
	// READ PHASE:
	// - Program CSA for receive using the same target address.
	// - Program CCTR.CBLEN = rlen.
	// - Set ACK = 0 for the final byte.
	// - Set START = 1; because the bus is owned, this becomes repeated
	// START.
	// - Set STOP = 1.
	// - Set BURSTRUN = 1.
	// - Start the read burst.
	// - Apply the post-start guard.
	// - Drain CRXDATA until rlen bytes are received.
	// - Wait for completion and final STOP.
	// - Decode errors.
	//
	// Return success only if both phases completed and exactly rlen bytes
	// were copied.
	//
	// Source:
	// - TRM §25.2.3.6: repeated START
	// - TRM §25.2.4.1.1, Tables 25-8 and 25-9
	// - TRM §25.3.32: CSA.DIR
	// - TRM §25.3.33: CCTR.START, STOP, ACK, CBLEN, BURSTRUN
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
	// i2c_peri_control:
	//
	// Validate handle, peripheral, configured role, and EN_or_DI.
	//
	// Verify PWREN.ENABLE before attempting role activation.
	//
	// ENABLE, controller role:
	// - Require TCTR.ACTIVE == 0.
	// - Require a valid functional clock selection.
	// - Require CCR.ACTIVE == 0.
	// - Set CCR.ACTIVE once.
	// - Verify that it became active.
	// - Do not write ACTIVE=1 again while already active.
	//
	// ENABLE, target role:
	// - Require CCR.ACTIVE == 0.
	// - Require at least one enabled valid own address.
	// - Require TCTR.ACTIVE == 0.
	// - Set TCTR.ACTIVE once.
	// - Verify that it became active.
	//
	// DISABLE, controller role:
	// - Wait for CSR.BUSY == 0 and CSR.IDLE == 1.
	// - Clear CCR.ACTIVE.
	//
	// DISABLE, target role:
	// - Wait for TSR.BUSBSY == 0.
	// - Clear TCTR.ACTIVE.
	//
	// Source:
	// - TRM §25.3.38: CCR.ACTIVE
	// - TRM §25.3.44: TOAR.OAREN and OAR
	// - TRM §25.3.46: TCTR.ACTIVE
	// - TRM §25.3.47: TSR.BUSBSY
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
