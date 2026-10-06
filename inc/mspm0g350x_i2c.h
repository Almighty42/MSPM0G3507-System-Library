#ifndef MSPM0G3507_I2C_DRIVER_H
#define MSPM0G3507_I2C_DRIVER_H

#include "../inc/mspm0g350x_startup.h"
#include <stdint.h>

// Polling / timing constants
#define I2C_DEFAULT_TIMEOUT_MS          100U
#define I2C_PWREN_STARTUP_ULPCLK_CYCLES 4U
#define I2C_FIFO_DEPTH			8U
#define I2C_TX_TRIGGER_LEVEL		2U 

#define I2C_CSA_DIR_VAL_TRANSMIT  0U   
#define I2C_CSA_DIR_VAL_RECEIVE   1U  

// NOTE: --- Structures for I2C ---

typedef struct
{
	uint8_t I2C_Device_Mode;					// Possible values from @I2C_DEVICE_MODE
	uint8_t I2C_Clock_Source;					// Possible values from @I2C_CLOCK_SOURCE
	uint8_t I2C_Clock_Divider;					// Possible values from @I2C_CLOCK_PRESCALE
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

// NOTE: --- I2C_PWREN bitfields ---

#define I2C_PWREN_ENABLE		0U
#define I2C_PWREN_ENABLE_WIDTH		1U

#define I2C_PWREN_KEY			24U
#define I2C_PWREN_KEY_WIDTH		8U
#define I2C_PWREN_KEY_VAL_UNLOCK	((uint32_t)0x00000026U)
#define I2C_PWREN_KEY_VAL_LOCK		((uint32_t)0x00000000U)

// NOTE: --- I2C_RSTCTL bitfields ---

#define I2C_RSTCTL_RESETASSERT		0U
#define I2C_RSTCTL_RESETASSERT_WIDTH	1U

#define I2C_RSTCTL_RESETSTKYCLR		1U
#define I2C_RSTCTL_RESETSTKYCLR_WIDTH	1U

#define I2C_RSTCTL_KEY			24U
#define I2C_RSTCTL_KEY_WIDTH		8U
#define I2C_RSTCTL_KEY_VAL_UNLOCK	((uint32_t)0x000000B1U)
#define I2C_RSTCTL_KEY_VAL_LOCK		((uint32_t)0x00000000U)

// NOTE: --- I2C_STAT0 bitfields ---

#define I2C_STAT0_RESETSTKY		16U
#define I2C_STAT0_RESETSTKY_WIDTH	1U

// NOTE: --- I2C_CLKSEL bitfields ---

#define I2C_CLKSEL_MFCLK_SEL		2U
#define I2C_CLKSEL_MFCLK_SEL_WIDTH	1U

#define I2C_CLKSEL_BUSCLK_SEL		3U
#define I2C_CLKSEL_BUSCLK_SEL_WIDTH	1U

// NOTE: --- I2C_CLKDIV bitfields ---

#define I2C_CLKDIV_RATIO		0U
#define I2C_CLKDIV_RATIO_WIDTH		3U
#define I2C_CLKDIV_RATIO_VAL_DIVNONE	((uint32_t)0x00000000U)
#define I2C_CLKDIV_RATIO_VAL_DIV2	((uint32_t)0x00000001U)
#define I2C_CLKDIV_RATIO_VAL_DIV3	((uint32_t)0x00000002U)
#define I2C_CLKDIV_RATIO_VAL_DIV4	((uint32_t)0x00000003U)
#define I2C_CLKDIV_RATIO_VAL_DIV5	((uint32_t)0x00000004U)
#define I2C_CLKDIV_RATIO_VAL_DIV6	((uint32_t)0x00000005U)
#define I2C_CLKDIV_RATIO_VAL_DIV7	((uint32_t)0x00000006U)
#define I2C_CLKDIV_RATIO_VAL_DIV8	((uint32_t)0x00000007U)

// NOTE: --- SPI_GFCTL bitfields ---

#define I2C_GFCTL_DGFSEL		0U
#define I2C_GFCTL_DGFSEL_WIDTH		3U
#define I2C_GFCTL_DGFSEL_VAL_BYPASS	((uint32_t)0x00000000U)
#define I2C_GFCTL_DGFSEL_VAL_1CLK	((uint32_t)0x00000001U)
#define I2C_GFCTL_DGFSEL_VAL_2CLK	((uint32_t)0x00000002U)
#define I2C_GFCTL_DGFSEL_VAL_3CLK	((uint32_t)0x00000003U)
#define I2C_GFCTL_DGFSEL_VAL_4CLK	((uint32_t)0x00000004U)
#define I2C_GFCTL_DGFSEL_VAL_8CLK	((uint32_t)0x00000005U)
#define I2C_GFCTL_DGFSEL_VAL_16CLK	((uint32_t)0x00000006U)
#define I2C_GFCTL_DGFSEL_VAL_31CLK	((uint32_t)0x00000007U)

#define I2C_GFCTL_AGFEN			8U
#define I2C_GFCTL_AGFEN_WIDTH		1U

#define I2C_GFCTL_AGFSEL		9U
#define I2C_GFCTL_AGFSEL_WIDTH		2U
#define I2C_GFCTL_AGFSEL_VAL_5NS_FILT	((uint32_t)0x00000000U)
#define I2C_GFCTL_AGFSEL_VAL_10NS_FILT	((uint32_t)0x00000001U)
#define I2C_GFCTL_AGFSEL_VAL_25NS_FILT	((uint32_t)0x00000002U)
#define I2C_GFCTL_AGFSEL_VAL_50NS_FILT	((uint32_t)0x00000003U)

#define I2C_GFCTL_CHAIN			11U
#define I2C_GFCTL_CHAIN_WIDTH		1U

// NOTE: --- SPI_CFIFOSR bitfields ---

#define I2C_CFIFOSR_RXFIFOCNT		0U
#define I2C_CFIFOSR_RXFIFOCNT_WIDTH	4U

#define I2C_CFIFOSR_RXFLUSH		7U
#define I2C_CFIFOSR_RXFLUSH_WIDTH	1U

#define I2C_CFIFOSR_TXFIFOCNT		8U
#define I2C_CFIFOSR_TXFIFOCNT_WIDTH	4U

#define I2C_CFIFOSR_TXFLUSH		15U
#define I2C_CFIFOSR_TXFLUSH_WIDTH	1U

// NOTE: --- SPI_CFIFOCTL bitfields ---

#define I2C_CFIFOCTL_TXTRIG		0U
#define I2C_CFIFOCTL_TXTRIG_WIDTH	3U
#define I2C_CFIFOCTL_TXTRIG_VAL_EMPTY	((uint32_t)0x00000000)
#define I2C_CFIFOCTL_TXTRIG_VAL_1B	((uint32_t)0x00000001)
#define I2C_CFIFOCTL_TXTRIG_VAL_2B	((uint32_t)0x00000002)
#define I2C_CFIFOCTL_TXTRIG_VAL_3B	((uint32_t)0x00000003)
#define I2C_CFIFOCTL_TXTRIG_VAL_4B	((uint32_t)0x00000004)
#define I2C_CFIFOCTL_TXTRIG_VAL_5B	((uint32_t)0x00000005)
#define I2C_CFIFOCTL_TXTRIG_VAL_6B	((uint32_t)0x00000006)
#define I2C_CFIFOCTL_TXTRIG_VAL_7B	((uint32_t)0x00000007)

#define I2C_CFIFOCTL_TXFLUSH		7U
#define I2C_CFIFOCTL_TXFLUSH_WIDTH	1U

#define I2C_CFIFOCTL_RXTRIG		8U
#define I2C_CFIFOCTL_RXTRIG_WIDTH	3U
#define I2C_CFIFOCTL_RXTRIG_VAL_EMPTY	((uint32_t)0x00000000)
#define I2C_CFIFOCTL_RXTRIG_VAL_1B	((uint32_t)0x00000001)
#define I2C_CFIFOCTL_RXTRIG_VAL_2B	((uint32_t)0x00000002)
#define I2C_CFIFOCTL_RXTRIG_VAL_3B	((uint32_t)0x00000003)
#define I2C_CFIFOCTL_RXTRIG_VAL_4B	((uint32_t)0x00000004)
#define I2C_CFIFOCTL_RXTRIG_VAL_5B	((uint32_t)0x00000005)
#define I2C_CFIFOCTL_RXTRIG_VAL_6B	((uint32_t)0x00000006)
#define I2C_CFIFOCTL_RXTRIG_VAL_7B	((uint32_t)0x00000007)

#define I2C_CFIFOCTL_RXFLUSH		15U
#define I2C_CFIFOCTL_RXFLUSH_WIDTH	1U

// NOTE: --- SPI_CTPR bitfields ---

#define I2C_CTPR_TPR			0U
#define I2C_CTPR_TPR_WIDTH		7U

// NOTE: --- I2C_CSA bitfields ---

#define I2C_CSA_DIR			0U
#define I2C_CSA_DIR_WIDTH		1U

#define I2C_CSA_TADDR			1U
#define I2C_CSA_TADDR_WIDTH		10U

#define I2C_CSA_CMODE			15U
#define I2C_CSA_CMODE_WIDTH		1U

// NOTE: --- I2C_CCTR bitfields ---
//
#define I2C_CCTR_BURSTRUN		0U
#define I2C_CCTR_BURSTRUN_WIDTH		1U

#define I2C_CCTR_START			1U
#define I2C_CCTR_START_WIDTH		1U

#define I2C_CCTR_STOP			2U
#define I2C_CCTR_STOP_WIDTH		1U

#define I2C_CCTR_ACK			3U
#define I2C_CCTR_ACK_WIDTH		1U

#define I2C_CCTR_CACKOEN		4U
#define I2C_CCTR_CACKOEN_WIDTH		1U

#define I2C_CCTR_RDONTXEMPTY		5U
#define I2C_CCTR_RDONTXEMPTY_WIDTH	1U

#define I2C_CCTR_CBLEN			16U
#define I2C_CCTR_CBLEN_WIDTH		12U
#define I2C_CCTR_CBLEN_VAL_MIN		((uint32_t)0x00000000U)
#define I2C_CCTR_CBLEN_VAL_MAX		((uint32_t)0x00000FFFU)

// NOTE: --- I2C_CSR bitfields ---

#define I2C_CSR_BUSY			0U
#define I2C_CSR_BUSY_WIDTH		1U

#define I2C_CSR_ERR			1U
#define I2C_CSR_ERR_WIDTH		1U

#define I2C_CSR_ADRACK			2U
#define I2C_CSR_ADRACK_WIDTH		1U

#define I2C_CSR_DATACK			3U
#define I2C_CSR_DATACK_WIDTH		1U

#define I2C_CSR_ARBLST			4U
#define I2C_CSR_ARBLST_WIDTH		1U

#define I2C_CSR_IDLE			5U
#define I2C_CSR_IDLE_WIDTH		1U

#define I2C_CSR_BUSBSY			6U
#define I2C_CSR_BUSBSY_WIDTH		1U

#define I2C_CSR_CBCNT			16U
#define I2C_CSR_CBCNT_WIDTH		12U

// NOTE: --- I2C_CCR bitfields ---

#define I2C_CCR_ACTIVE			0U
#define I2C_CCR_ACTIVE_WIDTH		1U

#define I2C_CCR_MCTL			1U
#define I2C_CCR_MCTL_WIDTH		1U

#define I2C_CCR_CLKSTRETCH		2U
#define I2C_CCR_CLKSTRETCH_WIDTH	1U

#define I2C_CCR_LPBK			8U
#define I2C_CCR_LPBK_WIDTH		1U

// NOTE: --- I2C_TOAR bitfields ---

#define I2C_TOAR_OAR			0U
#define I2C_TOAR_OAR_WIDTH		10U

#define I2C_TOAR_OAREN			14U
#define I2C_TOAR_OAREN_WIDTH		1U

#define I2C_TOAR_TMODE			15U
#define I2C_TOAR_TMODE_WIDTH		1U

// NOTE: --- I2C_TCTR bitfields ---

#define I2C_TCTR_ACTIVE			0U
#define I2C_TCTR_ACTIVE_WIDTH		1U

#define I2C_TCTR_GENCALL		1U
#define I2C_TCTR_GENCALL_WIDTH		1U

#define I2C_TCTR_TCLKSTRETCH		2U
#define I2C_TCTR_TCLKSTRETCH_WIDTH	1U

#define I2C_TCTR_TXEMPTY_ON_TREQ	3U
#define I2C_TCTR_TXEMPTY_ON_TREQ_WIDTH	1U

#define I2C_TCTR_TXTRIG_TXMODE		4U
#define I2C_TCTR_TXTRIG_TXMODE_WIDTH	1U

#define I2C_TCTR_TXWAIT_STALE_TXFIFO	5U
#define I2C_TCTR_TXWAIT_STALE_TXFIFO_WIDTH	1U

#define I2C_TCTR_RXFULL_ON_RREQ		6U
#define I2C_TCTR_RXFULL_ON_RREQ_WIDTH	1U

#define I2C_TCTR_EN_DEFHOSTADR		7U
#define I2C_TCTR_EN_DEFHOSTADR_WIDTH	1U

#define I2C_TCTR_EN_ALRESPADR		8U
#define I2C_TCTR_EN_ALRESPADR_WIDTH	1U

#define I2C_TCTR_EN_DEFDEVADR		9U
#define I2C_TCTR_EN_DEFDEVADR_WIDTH	1U

#define I2C_TCTR_TWUEN			10U
#define I2C_TCTR_TWUEN_WIDTH		1U

// NOTE: --- I2C_TSR bitfields ---

#define I2C_TSR_RREQ			0U
#define I2C_TSR_RREQ_WIDTH		1U

#define I2C_TSR_TREQ			1U
#define I2C_TSR_TREQ_WIDTH		1U

#define I2C_TSR_RXMODE			2U
#define I2C_TSR_RXMODE_WIDTH		1U

#define I2C_TSR_OAR2SEL			3U
#define I2C_TSR_OAR2SEL_WIDTH		1U

#define I2C_TSR_QCMDST			4U
#define I2C_TSR_QCMDST_WIDTH		1U

#define I2C_TSR_QCMDRW			5U
#define I2C_TSR_QCMDRW_WIDTH		1U

#define I2C_TSR_BUSBSY			6U
#define I2C_TSR_BUSBSY_WIDTH		1U

#define I2C_TSR_TXMODE			7U
#define I2C_TSR_TXMODE_WIDTH		1U

#define I2C_TSR_STALE_TXFIFO		8U
#define I2C_TSR_STALE_TXFIFO_WIDTH	1U

#define I2C_TSR_ADDRMATCH		9U
#define I2C_TSR_ADDRMATCH_WIDTH		10U

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
