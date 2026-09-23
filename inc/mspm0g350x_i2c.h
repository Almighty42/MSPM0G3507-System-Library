#ifndef MSPM0G3507_I2C_DRIVER_H
#define MSPM0G3507_I2C_DRIVER_H

#include "mspm0g350x_startup.h"
#include <stdint.h>

// NOTE: --- Structures for I2C ---

typedef struct
{
	uint8_t I2C_Device_Mode;					// Possible values from @I2C_DEVICE_MODE
	uint8_t I2C_Clock_Source;					// Possible values from @I2C_CLOCK_SOURCE
	uint8_t I2C_Clock_Prescale;					// Possible values from @I2C_CLOCK_PRESCALE
	uint16_t I2C_Timer_Period;					// Possible values from @I2C_TIMER_PERIOD
	uint8_t I2C_Addressing_Mode;					// Possible values from @I2C_ADDRESSING_MODE
	uint16_t  I2C_Own_Address;					// Possible values from @I2C_OWN_ADDRESS
	uint8_t I2C_Enable_Glitch_Filter;				// Possible values from @I2C_ENABLE_GLITCH_FILTER
	uint8_t I2C_Clock_Stretch;					// Possible values from @I2C_CLOCK_STRETCH
} i2c_config_t;

typedef struct
{
	i2c_type* p_I2Cx;						// Holds the base address of the I2C peripheral
	i2c_config_t i2c_config;					// Holds I2C peripheral configuration settings
	uint8_t* p_tx_buffer;						// Stores application Tx buffer address
	uint8_t* p_rx_buffer;						// Stores application Rx buffer address
	uint32_t tx_len;						// Tx length
	uint32_t rx_len;						// Rx length
} i2c_handle_t;

typedef enum {
	I2C_OK = 0,							// Success
	I2C_ERROR_INVALID_STATE,					// Function called in an invalid driver state
	I2C_ERROR_NULL_PTR,						// NULL pointer passed
	I2C_ERROR_INVALID_PORT,						// Invalid I2C peripheral base address
	I2C_ERROR_INVALID_IRQ,						// Invalid IRQ number; keep only if IRQ API exists
	I2C_ERROR_INVALID_MODE,						// Invalid controller/target role
	I2C_ERROR_INVALID_CLOCK_SRC,					// Invalid functional clock source
	I2C_ERROR_INVALID_CLKDIV,					// Invalid CLKDIV.RATIO configuration
	I2C_ERROR_INVALID_SPEED,					// Invalid SCL speed / MTPR.TPR value
	I2C_ERROR_INVALID_ADDR_MODE,					// Invalid 7-bit/10-bit address-mode selection
	I2C_ERROR_INVALID_ADDRESS,					// Address outside the selected range
	I2C_ERROR_INVALID_LEN,						// Zero or unsupported transfer length

	/* Peripheral / transfer state */
	I2C_ERROR_NOT_ENABLED,						// CCR.ACTIVE or TCTR.ACTIVE is clear
	I2C_ERROR_TIMEOUT,						// Polling timeout expired
	I2C_ERROR_BUS_BUSY,						// Bus already busy before transaction
	I2C_BUSY,							// Controller/target currently servicing a transfer

	/* I2C protocol errors */
	I2C_ERROR_NACK_ADDR,						// Address byte was NACKed
	I2C_ERROR_NACK_DATA,						// Data byte was NACKed
	I2C_ERROR_ARBITRATION_LOST,					// Controller lost arbitration
	I2C_ERROR_BUS_ERROR,						// Generic hardware/bus error

	/* FIFO errors */
	I2C_ERROR_RX_OVERFLOW,						// Controller or target RX FIFO overflow
	I2C_ERROR_TX_UNDERFLOW						// Target TX FIFO underrun / stale data
} i2c_status_t;

// USAGE: --- @I2C_DEVICE_MODE ---

#define I2C_DEVICE_MODE_CONTROLLER    0U
#define I2C_DEVICE_MODE_TARGET        1U

// USAGE: --- @I2C_CLOCK_SOURCE ---

#define I2C_CLOCK_SRC_BUSCLK          0U
#define I2C_CLOCK_SRC_MFCLK           1U

// USAGE: --- @I2C_CLOCK_PRESCALE ---

#define I2C_CLOCK_PRESCALE_DIV_1      0U
#define I2C_CLOCK_PRESCALE_DIV_2      1U
#define I2C_CLOCK_PRESCALE_DIV_3      2U
#define I2C_CLOCK_PRESCALE_DIV_4      3U
#define I2C_CLOCK_PRESCALE_DIV_5      4U
#define I2C_CLOCK_PRESCALE_DIV_6      5U
#define I2C_CLOCK_PRESCALE_DIV_7      6U
#define I2C_CLOCK_PRESCALE_DIV_8      7U

// USAGE: --- @I2C_TIMER_PERIOD ---

#define I2C_TIMER_PERIOD_MIN          0U
#define I2C_TIMER_PERIOD_MAX          127U

// USAGE: --- @I2C_ADDRESSING_MODE ---

#define I2C_ADDRESSING_MODE_7BIT      0U
#define I2C_ADDRESSING_MODE_10BIT     1U

// USAGE: --- @I2C_OWN_ADDRESS ---

#define I2C_OWN_ADDRESS_MIN           0x000U
#define I2C_OWN_ADDRESS_7BIT_MAX      0x07FU
#define I2C_OWN_ADDRESS_10BIT_MAX     0x3FFU

// USAGE: --- @I2C_ENABLE_GLITCH_FILTER ---

#define I2C_GLITCH_FILTER_DISABLE     0U
#define I2C_GLITCH_FILTER_ENABLE      1U

// USAGE: --- @I2C_CLOCK_STRETCH ---

#define I2C_CLOCK_STRETCH_DISABLE     0U
#define I2C_CLOCK_STRETCH_ENABLE      1U

// NOTE: --- I2C Validation macros ---

#define VALIDATE_I2C_PORT(port) do {                              \
    if ((port) == NULL) {                                         \
        return I2C_ERROR_NULL_PTR;                                \
    }                                                             \
    if (((port) != I2C0) && ((port) != I2C1)) {                   \
        return I2C_ERROR_INVALID_PORT;                            \
    }                                                             \
} while (0)
#define VALIDATE_I2C_DEVICE_MODE(mode)          VALIDATE_ENUM((mode), I2C_DEVICE_MODE_TARGET,I2C_ERROR_INVALID_MODE)
#define VALIDATE_I2C_ADDRESSING_MODE(mode)          VALIDATE_ENUM((mode), I2C_ADDRESSING_MODE_10BIT,I2C_ERROR_INVALID_ADDR_MODE)
#define VALIDATE_I2C_CLOCK_SOURCE(src)          VALIDATE_ENUM((src), I2C_CLOCK_SRC_MFCLK, I2C_ERROR_INVALID_CLOCK_SRC)
#define VALIDATE_I2C_CLOCK_PRESCALE(prescale)   VALIDATE_ENUM((prescale), I2C_CLOCK_PRESCALE_DIV_8,I2C_ERROR_INVALID_CLKDIV)
#define VALIDATE_I2C_TIMER_PERIOD(period) do {        \
    if ((period) > I2C_TIMER_PERIOD_MAX) {            \
        return I2C_ERROR_INVALID_SPEED;               \
    }                                                  \
} while (0)
#define VALIDATE_I2C_ADDRESS(addr, addr_mode) do {             \
    if (((addr_mode) == I2C_ADDRESSING_MODE_7BIT) &&           \
        ((addr) > I2C_OWN_ADDRESS_7BIT_MAX)) {                 \
        return I2C_ERROR_INVALID_ADDRESS;                      \
    }                                                          \
    if (((addr_mode) == I2C_ADDRESSING_MODE_10BIT) &&          \
        ((addr) > I2C_OWN_ADDRESS_10BIT_MAX)) {                \
        return I2C_ERROR_INVALID_ADDRESS;                      \
    }                                                          \
} while (0)
#define VALIDATE_I2C_LEN(len) do {                     \
    if ((len) == 0U) {                                 \
        return I2C_ERROR_INVALID_LEN;                  \
    }                                                   \
} while (0)

// NOTE: --- Bit position definitions I2C_CSA ---

#define I2C_CSA_DIR_STATE			0U
#define I2C_CSA_TADDR_STATE			1U
#define I2C_CSA_CMODE_STATE			15U

#define I2C_CSA_DIR				(1U << I2C_CSA_DIR_STATE)
#define I2C_CSA_CMODE				(1U << I2C_CSA_CMODE_STATE)

// NOTE: --- Bit position definitions I2C_CCTR ---

#define I2C_CCTR_BURSTRUN_STATE			0U
#define I2C_CCTR_START_STATE			1U
#define I2C_CCTR_STOP_STATE			2U
#define I2C_CCTR_ACK_STATE			3U
#define I2C_CCTR_CACKOEN_STATE			4U
#define I2C_CCTR_RD_ON_TXEMPTY_STATE		5U
#define I2C_CCTR_CBLEN_STATE			16U

#define I2C_CCTR_BURSTRUN			(1U << I2C_CCTR_BURSTRUN_STATE)
#define I2C_CCTR_START				(1U << I2C_CCTR_START_STATE)
#define I2C_CCTR_STOP				(1U << I2C_CCTR_STOP_STATE)
#define I2C_CCTR_ACK				(1U << I2C_CCTR_ACK_STATE)
#define I2C_CCTR_CACKOEN			(1U << I2C_CCTR_CACKOEN_STATE)
#define I2C_CCTR_RD_ON_TXEMPTY			(1U << I2C_CCTR_RD_ON_TXEMPTY_STATE)

// NOTE: --- Bit position definitions I2C_CSR ---

#define I2C_CSR_BUSY_STATE			0U
#define I2C_CSR_ERR_STATE			1U
#define I2C_CSR_ADRACK_STATE			2U
#define I2C_CSR_DATACK_STATE			3U
#define I2C_CSR_ARBLST_STATE			4U
#define I2C_CSR_IDLE_STATE			5U
#define I2C_CSR_BUSBSY_STATE			6U
#define I2C_CSR_CBCNT_STATE			16U

#define I2C_CSR_BUSY				(1U << I2C_CSR_BUSY_STATE)
#define I2C_CSR_ERR				(1U << I2C_CSR_ERR_STATE)
#define I2C_CSR_ADRACK				(1U << I2C_CSR_ADRACK_STATE)
#define I2C_CSR_DATACK				(1U << I2C_CSR_DATACK_STATE)
#define I2C_CSR_ARBLST				(1U << I2C_CSR_ARBLST_STATE)
#define I2C_CSR_IDLE				(1U << I2C_CSR_IDLE_STATE)
#define I2C_CSR_BUSBSY				(1U << I2C_CSR_BUSBSY_STATE)

// NOTE: --- Bit position definitions I2C_CCR ---

#define I2C_CCR_ACTIVE_STATE			0U
#define I2C_CCR_MCTL_STATE			1U
#define I2C_CCR_CLKSTRETCH_STATE		2U
#define I2C_CCR_LPBK_STATE			8U

#define I2C_CCR_ACTIVE				(1U << I2C_CCR_ACTIVE_STATE)
#define I2C_CCR_MCTL				(1U << I2C_CCR_MCTL_STATE)
#define I2C_CCR_CLKSTRETCH			(1U << I2C_CCR_CLKSTRETCH_STATE)
#define I2C_CCR_LPBK				(1U << I2C_CCR_LPBK_STATE)

// NOTE: --- Bit position definitions I2C_TOAR ---

#define I2C_TOAR_OAR_STATE			0U
#define I2C_TOAR_OAREN_STATE			14U
#define I2C_TOAR_TMODE_STATE			15U

#define I2C_TOAR_OAREN				(1U << I2C_TOAR_OAREN_STATE)
#define I2C_TOAR_TMODE				(1U << I2C_TOAR_TMODE_STATE)

// NOTE: --- Bit position definitions I2C_TCTR ---

#define I2C_TCTR_ACTIVE_STATE			0U
#define I2C_TCTR_GENCALL_STATE			1U
#define I2C_TCTR_TCLKSTRETCH_STATE		2U
#define I2C_TCTR_TXEMPTY_ON_TREQ_STATE		3U
#define I2C_TCTR_TXTRIG_TXMODE_STATE		4U
#define I2C_TCTR_TXWAIT_STALE_TXFIFO_STATE	5U
#define I2C_TCTR_RXFULL_ON_PREQ_STATE		6U
#define I2C_TCTR_EN_DEFHOSTADR_STATE		7U
#define I2C_TCTR_EN_ALRESPADR_STATE		8U
#define I2C_TCTR_EN_DEFDEVADR_STATE		9U
#define I2C_TCTR_TWUEN_STATE			10U

#define I2C_TCTR_ACTIVE				(1U << I2C_TCTR_ACTIVE_STATE)
#define I2C_TCTR_GENCALL			(1U << I2C_TCTR_GENCALL_STATE)
#define I2C_TCTR_TCLKSTRETCH			(1U << I2C_TCTR_TCLKSTRETCH_STATE)
#define I2C_TCTR_TXEMPTY_ON_TREQ		(1U << I2C_TCTR_TXEMPTY_ON_TREQ_STATE)
#define I2C_TCTR_TXTRIG_TXMODE			(1U << I2C_TCTR_TXTRIG_TXMODE_STATE)
#define I2C_TCTR_TXWAIT_STALE_TXFIFO		(1U << I2C_TCTR_TXWAIT_STALE_TXFIFO_STATE)
#define I2C_TCTR_RXFULL_ON_PREQ			(1U << I2C_TCTR_RXFULL_ON_PREQ_STATE)
#define I2C_TCTR_EN_DEFHOSTADR			(1U << I2C_TCTR_EN_DEFHOSTADR_STATE)
#define I2C_TCTR_EN_ALRESPADR			(1U << I2C_TCTR_EN_ALRESPADR_STATE)
#define I2C_TCTR_EN_DEFDEVADR			(1U << I2C_TCTR_EN_DEFDEVADR_STATE)
#define I2C_TCTR_TWUEN				(1U << I2C_TCTR_TWUEN_STATE)

// NOTE: --- Bit position definitions I2C_TSR ---

#define I2C_TSR_RREQ_STATE			0U
#define I2C_TSR_TREQ_STATE			1U
#define I2C_TSR_RXMODE_STATE			2U
#define I2C_TSR_OAR2SEL_STATE			3U
#define I2C_TSR_QCMDST_STATE			4U
#define I2C_TSR_QCMDRW_STATE			5U
#define I2C_TSR_BUSBSY_STATE			6U
#define I2C_TSR_TXMODE_STATE			7U
#define I2C_TSR_STALE_TXFIFO_STATE		8U
#define I2C_TSR_ADDRMATCH_STATE			9U

#define I2C_TSR_RREQ				(1U << I2C_TSR_RREQ_STATE)
#define I2C_TSR_TREQ				(1U << I2C_TSR_TREQ_STATE)
#define I2C_TSR_RXMODE				(1U << I2C_TSR_RXMODE_STATE)
#define I2C_TSR_OAR2SEL				(1U << I2C_TSR_OAR2SEL_STATE)
#define I2C_TSR_QCMDST				(1U << I2C_TSR_QCMDST_STATE)
#define I2C_TSR_QCMDRW				(1U << I2C_TSR_QCMDRW_STATE)
#define I2C_TSR_BUSBSY				(1U << I2C_TSR_BUSBSY_STATE)
#define I2C_TSR_TXMODE				(1U << I2C_TSR_TXMODE_STATE)
#define I2C_TSR_STALE_TXFIFO			(1U << I2C_TSR_STALE_TXFIFO_STATE)
#define I2C_TSR_ADDRMATCH			(1U << I2C_TSR_ADDRMATCH_STATE)

// Peripheral clock setup
i2c_status_t i2c_peri_clk_control(i2c_type* p_i2c_x, uint8_t EN_or_DI);

// Init / de-init
i2c_status_t i2c_init(i2c_handle_t* p_i2c_handle);
i2c_status_t i2c_de_init(i2c_type* p_i2c_x);

// Data send / receive ( polling )
// Controller
i2c_status_t i2c_controller_write_pl(
		i2c_handle_t* p_i2c_handle,
		uint16_t addr,
                const uint8_t* p_tx_buffer,
		uint32_t len,
                uint32_t timeout
		);
i2c_status_t i2c_controller_read_pl(
		i2c_handle_t* p_i2c_handle,
		uint16_t addr,
                uint8_t* p_rx_buffer,
		uint32_t len,
                uint32_t timeout
		);
i2c_status_t i2c_controller_write_read_pl(
		i2c_handle_t* h,
		uint16_t addr,
                const uint8_t* wbuf,
		uint32_t wlen,
                uint8_t* rbuf,
		uint32_t rlen,
                uint32_t timeout
		);

// Target
i2c_status_t i2c_target_write_pl(
		i2c_handle_t* p_i2c_handle,
		const uint8_t* p_tx_buffer,
                uint32_t len,
		uint32_t timeout
		);
i2c_status_t i2c_target_read_pl(
		i2c_handle_t* p_i2c_handle,
		uint8_t* p_tx_buffer,
		uint32_t len,
                uint32_t timeout
		);

// Peripheral control API
i2c_status_t i2c_peri_control(i2c_type* p_i2c_x, uint8_t EN_or_DI);

#endif
