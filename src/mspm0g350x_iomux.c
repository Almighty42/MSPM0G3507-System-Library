#include "../inc/mspm0g350x_iomux.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdbool.h>
#include <stdint.h>

/********************************************************************************
 *
 *******************************************************************************/

/********************************************************************************
 *
 * INFO: Driver map
 * @CONFIGURATION
 *
 *******************************************************************************/

/********************************************************************************
 * - PINCM bit positions — TRM SLAU846E, Section 8.3.1, Table 8-5
 *******************************************************************************/

#define IOMUX_PINCM_PF_POS 0
#define IOMUX_PINCM_PC_POS 7
#define IOMUX_PINCM_PF_WIDTH 6
#define IOMUX_PINCM_PIPD_POS 16
#define IOMUX_PINCM_PIPU_POS 17
#define IOMUX_PINCM_INENA_POS 18
#define IOMUX_PINCM_HYSTEN_POS 19
#define IOMUX_PINCM_DRV_POS 20
#define IOMUX_PINCM_HIZ1_POS 25
#define IOMUX_PINCM_INV_POS 26

/********************************************************************************
 * - iomux_configure_pin ( STATIC ) HELPER FUNCTIONS PROTOTYPES AND MACROS
 *******************************************************************************/

static bool iomux_pin_is_valid(iomux_pincm_index_t pincm_index);
static bool iomux_pf_is_valid_for_pin(iomux_pincm_index_t pincm_index,
                                      iomux_pf_t pf);

#define VALIDATE_IOMUX_PIN(pin)                                                \
	do {                                                                   \
		if (!iomux_pin_is_valid((pin))) {                              \
			return IOMUX_ERROR_INVALID_PIN;                        \
		}                                                              \
	} while (0)
#define VALIDATE_IOMUX_PF_FOR_PIN(pin, pf)                                     \
	do {                                                                   \
		if (!iomux_pf_is_valid_for_pin((pin), (pf))) {                 \
			return IOMUX_ERROR_INVALID_PF;                         \
		}                                                              \
	} while (0)

// NOTE: @PERIPHERAL_CLOCK_SETUP

/********************************************************************************
 * @fn				- iomux_configure_pin
 *
 * @brief			- Configure a pin's IOMUX
 *
 * @param[iomux_config_t]	- IOMUX configuration struct
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- Below
 * @pre  Any peripheral currently using this pin MUST already be disabled.
 * @pre  The pin has not been configured since reset, or was released with
 *       iomux_disconnect_pin().
 * @post The new function is selected and the pad is connected. The caller
 *       enables the new peripheral AFTER this call returns.
 *******************************************************************************/

iomux_status_t iomux_configure_pin(const iomux_config_t* iomux_cfg)
{
	uint32_t idx;
	uint32_t reg = 0;

	VALIDATE_PTR(iomux_cfg, IOMUX_ERROR_NULL_PTR);
	VALIDATE_IOMUX_PIN(iomux_cfg->pincm_index);
	VALIDATE_IOMUX_PF_FOR_PIN(iomux_cfg->pincm_index, iomux_cfg->pf);
	VALIDATE_IOMUX_PC(iomux_cfg->pad_connect);
	VALIDATE_IOMUX_PULL(iomux_cfg->pull);
	VALIDATE_IOMUX_DRIVE(iomux_cfg->drive_strength);
	VALIDATE_IOMUX_INPUT(iomux_cfg->input_enable);
	VALIDATE_IOMUX_INVERT(iomux_cfg->invert);
	VALIDATE_IOMUX_HIZ1(iomux_cfg->hiz1);

	idx = IOMUX_PINCM_ARRAY_INDEX(iomux_cfg->pincm_index);

	//  Runtime reconfiguration: clear previous selection first.
	IOMUX->SECCFG.PINCM[idx] = 0;

	WRITE_FIELD(reg, IOMUX_PINCM_PF_POS, 6, iomux_cfg->pf);

	IOMUX_SET_BIT_IF_ENABLED(reg,
	                         iomux_cfg->pad_connect == IOMUX_PC_CONNECT,
	                         IOMUX_PINCM_PC_POS);
	IOMUX_SET_BIT_IF_ENABLED(reg,
	                         iomux_cfg->input_enable == IOMUX_INPUT_ENABLE,
	                         IOMUX_PINCM_INENA_POS);
	IOMUX_SET_BIT_IF_ENABLED(reg,
	                         iomux_cfg->drive_strength == IOMUX_DRIVE_HIGH,
	                         IOMUX_PINCM_DRV_POS);
	IOMUX_SET_BIT_IF_ENABLED(reg, iomux_cfg->invert == IOMUX_INVERT_ENABLE,
	                         IOMUX_PINCM_INV_POS);
	IOMUX_SET_BIT_IF_ENABLED(reg, iomux_cfg->hiz1 == IOMUX_HIZ1_ENABLE,
	                         IOMUX_PINCM_HIZ1_POS);

	switch (iomux_cfg->pull) {
		case IOMUX_PULL_UP:
			reg |= (1UL << IOMUX_PINCM_PIPU_POS);
			break;
		case IOMUX_PULL_DOWN:
			reg |= (1UL << IOMUX_PINCM_PIPD_POS);
			break;
		case IOMUX_PULL_NONE:
			break;
		default:
			return IOMUX_ERROR_INVALID_PUPD;
	}

	IOMUX->SECCFG.PINCM[idx] = reg;

	return IOMUX_OK;
}

/********************************************************************************
 * @fn				- iomux_disconnect_pin
 *
 * @brief			- Disconnect a pin from its current digital
 * function
 *
 * @param[iomux_pincm_index_t]	- IOMUX pin number
 *
 * @return			- Success / Failure status of the function
 *
 * @Note			- Below
 * @pre The peripheral using the pin MUST already be disabled.
 *******************************************************************************/

iomux_status_t iomux_disconnect_pin(iomux_pincm_index_t pin)
{
	if (!iomux_pin_is_valid(pin)) {
		return IOMUX_ERROR_INVALID_PIN;
	}

	volatile uint32_t* pincm =
	    &IOMUX->SECCFG.PINCM[IOMUX_PINCM_ARRAY_INDEX(pin)];

	uint32_t v = *pincm;

	// Stop the digital input path and disconnect the pad
	CLEAR_BIT(v, IOMUX_PINCM_INENA_POS);
	CLEAR_BIT(v, IOMUX_PINCM_PC_POS);
	*pincm = v;

	// Only now deselect the peripheral function (PF = 0)
	CLEAR_FIELD_WITH_MASK(v, IOMUX_PINCM_PF_POS, IOMUX_PINCM_PF_WIDTH);
	*pincm = v;

	return IOMUX_OK;
}

static bool iomux_pin_is_valid(iomux_pincm_index_t pincm_index)
{
	switch (pincm_index) {
		case IOMUX_PIN_PA0:
		case IOMUX_PIN_PA1:
		case IOMUX_PIN_PA2:
		case IOMUX_PIN_PA3:
		case IOMUX_PIN_PA4:
		case IOMUX_PIN_PA5:
		case IOMUX_PIN_PA6:
		case IOMUX_PIN_PA7:
		case IOMUX_PIN_PA8:
		case IOMUX_PIN_PA9:
		case IOMUX_PIN_PA10:
		case IOMUX_PIN_PA11:
		case IOMUX_PIN_PA12:
		case IOMUX_PIN_PA13:
		case IOMUX_PIN_PA14:
		case IOMUX_PIN_PA15:
		case IOMUX_PIN_PA16:
		case IOMUX_PIN_PA17:
		case IOMUX_PIN_PA18:
		case IOMUX_PIN_PA19:
		case IOMUX_PIN_PA20:
		case IOMUX_PIN_PA21:
		case IOMUX_PIN_PA22:
		case IOMUX_PIN_PA23:
		case IOMUX_PIN_PA24:
		case IOMUX_PIN_PA25:
		case IOMUX_PIN_PA26:
		case IOMUX_PIN_PA27:
		case IOMUX_PIN_PA28:
		case IOMUX_PIN_PA29:
		case IOMUX_PIN_PA30:
		case IOMUX_PIN_PA31:
		case IOMUX_PIN_PB0:
		case IOMUX_PIN_PB1:
		case IOMUX_PIN_PB2:
		case IOMUX_PIN_PB3:
		case IOMUX_PIN_PB4:
		case IOMUX_PIN_PB5:
		case IOMUX_PIN_PB6:
		case IOMUX_PIN_PB7:
		case IOMUX_PIN_PB8:
		case IOMUX_PIN_PB9:
		case IOMUX_PIN_PB10:
		case IOMUX_PIN_PB11:
		case IOMUX_PIN_PB12:
		case IOMUX_PIN_PB13:
		case IOMUX_PIN_PB14:
		case IOMUX_PIN_PB15:
		case IOMUX_PIN_PB16:
		case IOMUX_PIN_PB17:
		case IOMUX_PIN_PB18:
		case IOMUX_PIN_PB19:
		case IOMUX_PIN_PB20:
		case IOMUX_PIN_PB21:
		case IOMUX_PIN_PB22:
		case IOMUX_PIN_PB23:
		case IOMUX_PIN_PB24:
		case IOMUX_PIN_PB25:
		case IOMUX_PIN_PB26:
		case IOMUX_PIN_PB27:
			return true;
		default:
			return false;
	}
}

static bool iomux_pf_is_valid_for_pin(iomux_pincm_index_t pincm_index,
                                      iomux_pf_t pf)
{
	switch (pincm_index) {
		case IOMUX_PIN_PA0:
			switch (pf) {
				case IOMUX_PIN_PA0_PF_GPIO:
				case IOMUX_PIN_PA0_PF_UART0_TX:
				case IOMUX_PIN_PA0_PF_I2C0_SDA:
				case IOMUX_PIN_PA0_PF_TIMA0_C0:
				case IOMUX_PIN_PA0_PF_TIMA_FAL1:
				case IOMUX_PIN_PA0_PF_TIMG8_C1:
				case IOMUX_PIN_PA0_PF_FCC_IN:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA1:
			switch (pf) {
				case IOMUX_PIN_PA1_PF_GPIO:
				case IOMUX_PIN_PA1_PF_UART0_RX:
				case IOMUX_PIN_PA1_PF_I2C0_SCL:
				case IOMUX_PIN_PA1_PF_TIMA0_C1:
				case IOMUX_PIN_PA1_PF_TIMA_FAL2:
				case IOMUX_PIN_PA1_PF_TIMG8_IDX:
				case IOMUX_PIN_PA1_PF_TIMG8_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA2:
			switch (pf) {
				case IOMUX_PIN_PA2_PF_GPIO:
				case IOMUX_PIN_PA2_PF_TIMG8_C1:
				case IOMUX_PIN_PA2_PF_SPI0_CS0:
				case IOMUX_PIN_PA2_PF_TIMG7_C1:
				case IOMUX_PIN_PA2_PF_SPI1_CS0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA3:
			switch (pf) {
				case IOMUX_PIN_PA3_PF_GPIO:
				case IOMUX_PIN_PA3_PF_TIMG8_C0:
				case IOMUX_PIN_PA3_PF_SPI0_CS1:
				case IOMUX_PIN_PA3_PF_UART2_CTS:
				case IOMUX_PIN_PA3_PF_TIMA0_C2:
				case IOMUX_PIN_PA3_PF_COMP1_OUT:
				case IOMUX_PIN_PA3_PF_TIMG7_C0:
				case IOMUX_PIN_PA3_PF_TIMA0_C1:
				case IOMUX_PIN_PA3_PF_I2C1_SDA:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA4:
			switch (pf) {
				case IOMUX_PIN_PA4_PF_GPIO:
				case IOMUX_PIN_PA4_PF_TIMG8_C1:
				case IOMUX_PIN_PA4_PF_SPI0_POCI:
				case IOMUX_PIN_PA4_PF_UART2_RTS:
				case IOMUX_PIN_PA4_PF_TIMA0_C3:
				case IOMUX_PIN_PA4_PF_LFCLK_IN:
				case IOMUX_PIN_PA4_PF_TIMG7_C1:
				case IOMUX_PIN_PA4_PF_TIMA0_C1N:
				case IOMUX_PIN_PA4_PF_I2C1_SCL:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA5:
			switch (pf) {
				case IOMUX_PIN_PA5_PF_GPIO:
				case IOMUX_PIN_PA5_PF_TIMG8_C0:
				case IOMUX_PIN_PA5_PF_SPI0_PICO:
				case IOMUX_PIN_PA5_PF_TIMA_FAL1:
				case IOMUX_PIN_PA5_PF_TIMG0_C0:
				case IOMUX_PIN_PA5_PF_TIMG6_C0:
				case IOMUX_PIN_PA5_PF_FCC_IN:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA6:
			switch (pf) {
				case IOMUX_PIN_PA6_PF_GPIO:
				case IOMUX_PIN_PA6_PF_TIMG8_C1:
				case IOMUX_PIN_PA6_PF_SPI0_SCK:
				case IOMUX_PIN_PA6_PF_TIMA_FAL0:
				case IOMUX_PIN_PA6_PF_TIMG0_C1:
				case IOMUX_PIN_PA6_PF_HFCLK_IN:
				case IOMUX_PIN_PA6_PF_TIMG6_C1:
				case IOMUX_PIN_PA6_PF_TIMA0_C2N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA7:
			switch (pf) {
				case IOMUX_PIN_PA7_PF_GPIO:
				case IOMUX_PIN_PA7_PF_COMP0_OUT:
				case IOMUX_PIN_PA7_PF_CLK_OUT:
				case IOMUX_PIN_PA7_PF_TIMG8_C0:
				case IOMUX_PIN_PA7_PF_TIMA0_C2:
				case IOMUX_PIN_PA7_PF_TIMG8_IDX:
				case IOMUX_PIN_PA7_PF_TIMG7_C1:
				case IOMUX_PIN_PA7_PF_TIMA0_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA8:
			switch (pf) {
				case IOMUX_PIN_PA8_PF_GPIO:
				case IOMUX_PIN_PA8_PF_UART1_TX:
				case IOMUX_PIN_PA8_PF_SPI0_CS0:
				case IOMUX_PIN_PA8_PF_UART0_RTS:
				case IOMUX_PIN_PA8_PF_TIMA0_C0:
				case IOMUX_PIN_PA8_PF_TIMA1_C0N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA9:
			switch (pf) {
				case IOMUX_PIN_PA9_PF_GPIO:
				case IOMUX_PIN_PA9_PF_UART1_RX:
				case IOMUX_PIN_PA9_PF_SPI0_PICO:
				case IOMUX_PIN_PA9_PF_UART0_CTS:
				case IOMUX_PIN_PA9_PF_TIMA0_C1:
				case IOMUX_PIN_PA9_PF_RTC_OUT:
				case IOMUX_PIN_PA9_PF_TIMA0_C0N:
				case IOMUX_PIN_PA9_PF_TIMA1_C1N:
				case IOMUX_PIN_PA9_PF_CLK_OUT:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA10:
			switch (pf) {
				case IOMUX_PIN_PA10_PF_GPIO:
				case IOMUX_PIN_PA10_PF_UART0_TX:
				case IOMUX_PIN_PA10_PF_SPI0_POCI:
				case IOMUX_PIN_PA10_PF_I2C0_SDA:
				case IOMUX_PIN_PA10_PF_TIMA1_C0:
				case IOMUX_PIN_PA10_PF_TIMG12_C0:
				case IOMUX_PIN_PA10_PF_TIMA0_C2:
				case IOMUX_PIN_PA10_PF_I2C1_SDA:
				case IOMUX_PIN_PA10_PF_CLK_OUT:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA11:
			switch (pf) {
				case IOMUX_PIN_PA11_PF_GPIO:
				case IOMUX_PIN_PA11_PF_UART0_RX:
				case IOMUX_PIN_PA11_PF_SPI0_SCK:
				case IOMUX_PIN_PA11_PF_I2C0_SCL:
				case IOMUX_PIN_PA11_PF_TIMA1_C1:
				case IOMUX_PIN_PA11_PF_COMP0_OUT:
				case IOMUX_PIN_PA11_PF_TIMA0_C2N:
				case IOMUX_PIN_PA11_PF_I2C1_SCL:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA12:
			switch (pf) {
				case IOMUX_PIN_PA12_PF_GPIO:
				case IOMUX_PIN_PA12_PF_UART3_CTS:
				case IOMUX_PIN_PA12_PF_SPI0_SCK:
				case IOMUX_PIN_PA12_PF_TIMG0_C0:
				case IOMUX_PIN_PA12_PF_CAN_TX:
				case IOMUX_PIN_PA12_PF_TIMA0_C3:
				case IOMUX_PIN_PA12_PF_FCC_IN:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA13:
			switch (pf) {
				case IOMUX_PIN_PA13_PF_GPIO:
				case IOMUX_PIN_PA13_PF_UART3_RTS:
				case IOMUX_PIN_PA13_PF_SPI0_POCI:
				case IOMUX_PIN_PA13_PF_UART3_RX:
				case IOMUX_PIN_PA13_PF_TIMG0_C1:
				case IOMUX_PIN_PA13_PF_CAN_RX:
				case IOMUX_PIN_PA13_PF_TIMA0_C3N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA14:
			switch (pf) {
				case IOMUX_PIN_PA14_PF_GPIO:
				case IOMUX_PIN_PA14_PF_UART0_CTS:
				case IOMUX_PIN_PA14_PF_SPI0_PICO:
				case IOMUX_PIN_PA14_PF_UART3_TX:
				case IOMUX_PIN_PA14_PF_TIMG12_C0:
				case IOMUX_PIN_PA14_PF_CLK_OUT:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA15:
			switch (pf) {
				case IOMUX_PIN_PA15_PF_GPIO:
				case IOMUX_PIN_PA15_PF_UART0_RTS:
				case IOMUX_PIN_PA15_PF_SPI1_CS2:
				case IOMUX_PIN_PA15_PF_I2C1_SCL:
				case IOMUX_PIN_PA15_PF_TIMA1_C0:
				case IOMUX_PIN_PA15_PF_TIMG8_IDX:
				case IOMUX_PIN_PA15_PF_TIMA1_C0N:
				case IOMUX_PIN_PA15_PF_TIMA0_C2:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA16:
			switch (pf) {
				case IOMUX_PIN_PA16_PF_GPIO:
				case IOMUX_PIN_PA16_PF_COMP2_OUT:
				case IOMUX_PIN_PA16_PF_SPI1_POCI:
				case IOMUX_PIN_PA16_PF_I2C1_SDA:
				case IOMUX_PIN_PA16_PF_TIMA1_C1:
				case IOMUX_PIN_PA16_PF_TIMA1_C1N:
				case IOMUX_PIN_PA16_PF_TIMA0_C2N:
				case IOMUX_PIN_PA16_PF_FCC_IN:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA17:
			switch (pf) {
				case IOMUX_PIN_PA17_PF_GPIO:
				case IOMUX_PIN_PA17_PF_UART1_TX:
				case IOMUX_PIN_PA17_PF_SPI1_SCK:
				case IOMUX_PIN_PA17_PF_I2C1_SCL:
				case IOMUX_PIN_PA17_PF_TIMA0_C3:
				case IOMUX_PIN_PA17_PF_TIMG7_C0:
				case IOMUX_PIN_PA17_PF_TIMA1_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA18:
			switch (pf) {
				case IOMUX_PIN_PA18_PF_GPIO:
				case IOMUX_PIN_PA18_PF_UART1_RX:
				case IOMUX_PIN_PA18_PF_SPI1_PICO:
				case IOMUX_PIN_PA18_PF_I2C1_SDA:
				case IOMUX_PIN_PA18_PF_TIMA0_C3N:
				case IOMUX_PIN_PA18_PF_TIMG7_C1:
				case IOMUX_PIN_PA18_PF_TIMA1_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA19:
			switch (pf) {
				case IOMUX_PIN_PA19_PF_GPIO:
				case IOMUX_PIN_PA19_PF_SWDIO:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA20:
			switch (pf) {
				case IOMUX_PIN_PA20_PF_GPIO:
				case IOMUX_PIN_PA20_PF_SWCLK:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA21:
			switch (pf) {
				case IOMUX_PIN_PA21_PF_GPIO:
				case IOMUX_PIN_PA21_PF_UART2_TX:
				case IOMUX_PIN_PA21_PF_TIMG8_C0:
				case IOMUX_PIN_PA21_PF_UART1_CTS:
				case IOMUX_PIN_PA21_PF_TIMA0_C0:
				case IOMUX_PIN_PA21_PF_TIMG6_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA22:
			switch (pf) {
				case IOMUX_PIN_PA22_PF_GPIO:
				case IOMUX_PIN_PA22_PF_UART2_RX:
				case IOMUX_PIN_PA22_PF_TIMG8_C1:
				case IOMUX_PIN_PA22_PF_UART1_RTS:
				case IOMUX_PIN_PA22_PF_TIMA0_C1:
				case IOMUX_PIN_PA22_PF_CLK_OUT:
				case IOMUX_PIN_PA22_PF_TIMA0_C0N:
				case IOMUX_PIN_PA22_PF_TIMG6_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA23:
			switch (pf) {
				case IOMUX_PIN_PA23_PF_GPIO:
				case IOMUX_PIN_PA23_PF_UART2_TX:
				case IOMUX_PIN_PA23_PF_SPI0_CS3:
				case IOMUX_PIN_PA23_PF_TIMA0_C3:
				case IOMUX_PIN_PA23_PF_TIMG0_C0:
				case IOMUX_PIN_PA23_PF_UART3_CTS:
				case IOMUX_PIN_PA23_PF_TIMG7_C0:
				case IOMUX_PIN_PA23_PF_TIMG8_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA24:
			switch (pf) {
				case IOMUX_PIN_PA24_PF_GPIO:
				case IOMUX_PIN_PA24_PF_UART2_RX:
				case IOMUX_PIN_PA24_PF_SPI0_CS2:
				case IOMUX_PIN_PA24_PF_TIMA0_C3N:
				case IOMUX_PIN_PA24_PF_TIMG0_C1:
				case IOMUX_PIN_PA24_PF_UART3_RTS:
				case IOMUX_PIN_PA24_PF_TIMG7_C1:
				case IOMUX_PIN_PA24_PF_TIMA1_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA25:
			switch (pf) {
				case IOMUX_PIN_PA25_PF_GPIO:
				case IOMUX_PIN_PA25_PF_UART3_RX:
				case IOMUX_PIN_PA25_PF_SPI1_CS3:
				case IOMUX_PIN_PA25_PF_TIMG12_C1:
				case IOMUX_PIN_PA25_PF_TIMA0_C3:
				case IOMUX_PIN_PA25_PF_TIMA0_C1N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA26:
			switch (pf) {
				case IOMUX_PIN_PA26_PF_GPIO:
				case IOMUX_PIN_PA26_PF_UART3_TX:
				case IOMUX_PIN_PA26_PF_SPI1_CS0:
				case IOMUX_PIN_PA26_PF_TIMG8_C0:
				case IOMUX_PIN_PA26_PF_TIMA_FAL0:
				case IOMUX_PIN_PA26_PF_CAN_TX:
				case IOMUX_PIN_PA26_PF_TIMG7_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA27:
			switch (pf) {
				case IOMUX_PIN_PA27_PF_GPIO:
				case IOMUX_PIN_PA27_PF_RTC_OUT:
				case IOMUX_PIN_PA27_PF_SPI1_CS1:
				case IOMUX_PIN_PA27_PF_TIMG8_C1:
				case IOMUX_PIN_PA27_PF_TIMA_FAL2:
				case IOMUX_PIN_PA27_PF_CAN_RX:
				case IOMUX_PIN_PA27_PF_TIMG7_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA28:
			switch (pf) {
				case IOMUX_PIN_PA28_PF_GPIO:
				case IOMUX_PIN_PA28_PF_UART0_TX:
				case IOMUX_PIN_PA28_PF_I2C0_SDA:
				case IOMUX_PIN_PA28_PF_TIMA0_C3:
				case IOMUX_PIN_PA28_PF_TIMA_FAL0:
				case IOMUX_PIN_PA28_PF_TIMG7_C0:
				case IOMUX_PIN_PA28_PF_TIMA1_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA29:
			switch (pf) {
				case IOMUX_PIN_PA29_PF_GPIO:
				case IOMUX_PIN_PA29_PF_I2C1_SCL:
				case IOMUX_PIN_PA29_PF_UART2_RTS:
				case IOMUX_PIN_PA29_PF_TIMG8_C0:
				case IOMUX_PIN_PA29_PF_TIMG6_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA30:
			switch (pf) {
				case IOMUX_PIN_PA30_PF_GPIO:
				case IOMUX_PIN_PA30_PF_I2C1_SDA:
				case IOMUX_PIN_PA30_PF_UART2_CTS:
				case IOMUX_PIN_PA30_PF_TIMG8_C1:
				case IOMUX_PIN_PA30_PF_TIMG6_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PA31:
			switch (pf) {
				case IOMUX_PIN_PA31_PF_GPIO:
				case IOMUX_PIN_PA31_PF_UART0_RX:
				case IOMUX_PIN_PA31_PF_I2C0_SCL:
				case IOMUX_PIN_PA31_PF_TIMA0_C3N:
				case IOMUX_PIN_PA31_PF_TIMG12_C1:
				case IOMUX_PIN_PA31_PF_CLK_OUT:
				case IOMUX_PIN_PA31_PF_TIMG7_C1:
				case IOMUX_PIN_PA31_PF_TIMA1_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB0:
			switch (pf) {
				case IOMUX_PIN_PB0_PF_GPIO:
				case IOMUX_PIN_PB0_PF_UART0_TX:
				case IOMUX_PIN_PB0_PF_SPI1_CS2:
				case IOMUX_PIN_PB0_PF_TIMA1_C0:
				case IOMUX_PIN_PB0_PF_TIMA0_C2:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB1:
			switch (pf) {
				case IOMUX_PIN_PB1_PF_GPIO:
				case IOMUX_PIN_PB1_PF_UART0_RX:
				case IOMUX_PIN_PB1_PF_SPI1_CS3:
				case IOMUX_PIN_PB1_PF_TIMA1_C1:
				case IOMUX_PIN_PB1_PF_TIMA0_C2N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB2:
			switch (pf) {
				case IOMUX_PIN_PB2_PF_GPIO:
				case IOMUX_PIN_PB2_PF_UART3_TX:
				case IOMUX_PIN_PB2_PF_UART2_CTS:
				case IOMUX_PIN_PB2_PF_I2C1_SCL:
				case IOMUX_PIN_PB2_PF_TIMA0_C3:
				case IOMUX_PIN_PB2_PF_UART1_CTS:
				case IOMUX_PIN_PB2_PF_TIMG6_C0:
				case IOMUX_PIN_PB2_PF_TIMA1_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB3:
			switch (pf) {
				case IOMUX_PIN_PB3_PF_GPIO:
				case IOMUX_PIN_PB3_PF_UART3_RX:
				case IOMUX_PIN_PB3_PF_UART2_RTS:
				case IOMUX_PIN_PB3_PF_I2C1_SDA:
				case IOMUX_PIN_PB3_PF_TIMA0_C3N:
				case IOMUX_PIN_PB3_PF_UART1_RTS:
				case IOMUX_PIN_PB3_PF_TIMG6_C1:
				case IOMUX_PIN_PB3_PF_TIMA1_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB4:
			switch (pf) {
				case IOMUX_PIN_PB4_PF_GPIO:
				case IOMUX_PIN_PB4_PF_UART1_TX:
				case IOMUX_PIN_PB4_PF_UART3_CTS:
				case IOMUX_PIN_PB4_PF_TIMA1_C0:
				case IOMUX_PIN_PB4_PF_TIMA0_C2:
				case IOMUX_PIN_PB4_PF_TIMA1_C0N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB5:
			switch (pf) {
				case IOMUX_PIN_PB5_PF_GPIO:
				case IOMUX_PIN_PB5_PF_UART1_RX:
				case IOMUX_PIN_PB5_PF_UART3_RTS:
				case IOMUX_PIN_PB5_PF_TIMA1_C1:
				case IOMUX_PIN_PB5_PF_TIMA0_C2N:
				case IOMUX_PIN_PB5_PF_TIMA1_C1N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB6:
			switch (pf) {
				case IOMUX_PIN_PB6_PF_GPIO:
				case IOMUX_PIN_PB6_PF_UART1_TX:
				case IOMUX_PIN_PB6_PF_SPI1_CS0:
				case IOMUX_PIN_PB6_PF_SPI0_CS1:
				case IOMUX_PIN_PB6_PF_TIMG8_C0:
				case IOMUX_PIN_PB6_PF_UART2_CTS:
				case IOMUX_PIN_PB6_PF_TIMG6_C0:
				case IOMUX_PIN_PB6_PF_TIMA1_C0N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB7:
			switch (pf) {
				case IOMUX_PIN_PB7_PF_GPIO:
				case IOMUX_PIN_PB7_PF_UART1_RX:
				case IOMUX_PIN_PB7_PF_SPI1_POCI:
				case IOMUX_PIN_PB7_PF_SPI0_CS2:
				case IOMUX_PIN_PB7_PF_TIMG8_C1:
				case IOMUX_PIN_PB7_PF_UART2_RTS:
				case IOMUX_PIN_PB7_PF_TIMG6_C1:
				case IOMUX_PIN_PB7_PF_TIMA1_C1N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB8:
			switch (pf) {
				case IOMUX_PIN_PB8_PF_GPIO:
				case IOMUX_PIN_PB8_PF_UART1_CTS:
				case IOMUX_PIN_PB8_PF_SPI1_PICO:
				case IOMUX_PIN_PB8_PF_TIMA0_C0:
				case IOMUX_PIN_PB8_PF_COMP1_OUT:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB9:
			switch (pf) {
				case IOMUX_PIN_PB9_PF_GPIO:
				case IOMUX_PIN_PB9_PF_UART1_RTS:
				case IOMUX_PIN_PB9_PF_SPI1_SCK:
				case IOMUX_PIN_PB9_PF_TIMA0_C1:
				case IOMUX_PIN_PB9_PF_TIMA0_C0N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB10:
			switch (pf) {
				case IOMUX_PIN_PB10_PF_GPIO:
				case IOMUX_PIN_PB10_PF_TIMG0_C0:
				case IOMUX_PIN_PB10_PF_TIMG8_C0:
				case IOMUX_PIN_PB10_PF_COMP1_OUT:
				case IOMUX_PIN_PB10_PF_TIMG6_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB11:
			switch (pf) {
				case IOMUX_PIN_PB11_PF_GPIO:
				case IOMUX_PIN_PB11_PF_TIMG0_C1:
				case IOMUX_PIN_PB11_PF_TIMG8_C1:
				case IOMUX_PIN_PB11_PF_CLK_OUT:
				case IOMUX_PIN_PB11_PF_TIMG6_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB12:
			switch (pf) {
				case IOMUX_PIN_PB12_PF_GPIO:
				case IOMUX_PIN_PB12_PF_UART3_TX:
				case IOMUX_PIN_PB12_PF_TIMA0_C2:
				case IOMUX_PIN_PB12_PF_TIMA_FAL1:
				case IOMUX_PIN_PB12_PF_TIMA0_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB13:
			switch (pf) {
				case IOMUX_PIN_PB13_PF_GPIO:
				case IOMUX_PIN_PB13_PF_UART3_RX:
				case IOMUX_PIN_PB13_PF_TIMA0_C3:
				case IOMUX_PIN_PB13_PF_TIMG12_C0:
				case IOMUX_PIN_PB13_PF_TIMA0_C1N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB14:
			switch (pf) {
				case IOMUX_PIN_PB14_PF_GPIO:
				case IOMUX_PIN_PB14_PF_SPI1_CS3:
				case IOMUX_PIN_PB14_PF_SPI1_POCI:
				case IOMUX_PIN_PB14_PF_SPI0_CS3:
				case IOMUX_PIN_PB14_PF_TIMG12_C1:
				case IOMUX_PIN_PB14_PF_TIMG8_IDX:
				case IOMUX_PIN_PB14_PF_TIMA0_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB15:
			switch (pf) {
				case IOMUX_PIN_PB15_PF_GPIO:
				case IOMUX_PIN_PB15_PF_UART2_TX:
				case IOMUX_PIN_PB15_PF_SPI1_PICO:
				case IOMUX_PIN_PB15_PF_UART3_CTS:
				case IOMUX_PIN_PB15_PF_TIMG8_C0:
				case IOMUX_PIN_PB15_PF_TIMG7_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB16:
			switch (pf) {
				case IOMUX_PIN_PB16_PF_GPIO:
				case IOMUX_PIN_PB16_PF_UART2_RX:
				case IOMUX_PIN_PB16_PF_SPI1_SCK:
				case IOMUX_PIN_PB16_PF_UART3_RTS:
				case IOMUX_PIN_PB16_PF_TIMG8_C1:
				case IOMUX_PIN_PB16_PF_TIMG7_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB17:
			switch (pf) {
				case IOMUX_PIN_PB17_PF_GPIO:
				case IOMUX_PIN_PB17_PF_UART2_TX:
				case IOMUX_PIN_PB17_PF_SPI0_PICO:
				case IOMUX_PIN_PB17_PF_SPI1_CS1:
				case IOMUX_PIN_PB17_PF_TIMA1_C0:
				case IOMUX_PIN_PB17_PF_TIMA0_C2:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB18:
			switch (pf) {
				case IOMUX_PIN_PB18_PF_GPIO:
				case IOMUX_PIN_PB18_PF_UART2_RX:
				case IOMUX_PIN_PB18_PF_SPI0_SCK:
				case IOMUX_PIN_PB18_PF_SPI1_CS2:
				case IOMUX_PIN_PB18_PF_TIMA1_C1:
				case IOMUX_PIN_PB18_PF_TIMA0_C2N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB19:
			switch (pf) {
				case IOMUX_PIN_PB19_PF_GPIO:
				case IOMUX_PIN_PB19_PF_COMP2_OUT:
				case IOMUX_PIN_PB19_PF_SPI0_POCI:
				case IOMUX_PIN_PB19_PF_TIMG8_C1:
				case IOMUX_PIN_PB19_PF_UART0_CTS:
				case IOMUX_PIN_PB19_PF_TIMG7_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB20:
			switch (pf) {
				case IOMUX_PIN_PB20_PF_GPIO:
				case IOMUX_PIN_PB20_PF_SPI0_CS2:
				case IOMUX_PIN_PB20_PF_SPI1_CS0:
				case IOMUX_PIN_PB20_PF_TIMA0_C2:
				case IOMUX_PIN_PB20_PF_TIMG12_C0:
				case IOMUX_PIN_PB20_PF_TIMA_FAL1:
				case IOMUX_PIN_PB20_PF_TIMA0_C1:
				case IOMUX_PIN_PB20_PF_TIMA1_C1N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB21:
			switch (pf) {
				case IOMUX_PIN_PB21_PF_GPIO:
				case IOMUX_PIN_PB21_PF_SPI1_POCI:
				case IOMUX_PIN_PB21_PF_TIMG8_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB22:
			switch (pf) {
				case IOMUX_PIN_PB22_PF_GPIO:
				case IOMUX_PIN_PB22_PF_SPI1_PICO:
				case IOMUX_PIN_PB22_PF_TIMG8_C1:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB23:
			switch (pf) {
				case IOMUX_PIN_PB23_PF_GPIO:
				case IOMUX_PIN_PB23_PF_SPI1_SCK:
				case IOMUX_PIN_PB23_PF_COMP0_OUT:
				case IOMUX_PIN_PB23_PF_TIMA_FAL0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB24:
			switch (pf) {
				case IOMUX_PIN_PB24_PF_GPIO:
				case IOMUX_PIN_PB24_PF_SPI0_CS3:
				case IOMUX_PIN_PB24_PF_SPI0_CS1:
				case IOMUX_PIN_PB24_PF_TIMA0_C3:
				case IOMUX_PIN_PB24_PF_TIMG12_C1:
				case IOMUX_PIN_PB24_PF_TIMA0_C1N:
				case IOMUX_PIN_PB24_PF_TIMA1_C0N:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB25:
			switch (pf) {
				case IOMUX_PIN_PB25_PF_GPIO:
				case IOMUX_PIN_PB25_PF_UART0_CTS:
				case IOMUX_PIN_PB25_PF_SPI0_CS0:
				case IOMUX_PIN_PB25_PF_TIMA_FAL2:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB26:
			switch (pf) {
				case IOMUX_PIN_PB26_PF_GPIO:
				case IOMUX_PIN_PB26_PF_UART0_RTS:
				case IOMUX_PIN_PB26_PF_SPI0_CS1:
				case IOMUX_PIN_PB26_PF_TIMA0_C3:
				case IOMUX_PIN_PB26_PF_TIMG6_C0:
				case IOMUX_PIN_PB26_PF_TIMA1_C0:
					return true;
				default:
					return false;
			}

		case IOMUX_PIN_PB27:
			switch (pf) {
				case IOMUX_PIN_PB27_PF_GPIO:
				case IOMUX_PIN_PB27_PF_COMP2_OUT:
				case IOMUX_PIN_PB27_PF_SPI1_CS1:
				case IOMUX_PIN_PB27_PF_TIMA0_C3N:
				case IOMUX_PIN_PB27_PF_TIMG6_C1:
				case IOMUX_PIN_PB27_PF_TIMA1_C1:
					return true;
				default:
					return false;
			}

		default:
			return false;
	}
}
