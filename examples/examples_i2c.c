#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_iomux.h"
#include "example_i2c.h"
#include "mspm0g350x_startup.h"
#include <stdint.h>

/********************************************************************************
 * @fn      - i2c1_config
 *
 * @brief   - Configures I2C1 as a 7-bit controller at about 100 kHz on
 *            PB2 (SCL) and PB3 (SDA).

 ** @pre
 * WARNING: SDA and SCL need external pull-ups to 3.3 V (typically
 * 2.2 k to 4.7 k). The IOMUX pads are set open-drain with the
 * internal pull-ups off.
 *
 * @param[*p_i2c] - Handle structure of the I2C1 peripheral
 *
 * @return  - Success / Failure status of the function
 *
* @Note    - SCL frequency assumes a 32 MHz I2C clock and TPR = 31.
 *            Recompute TPR if the clock configuration changes:
 *            f_scl = f_i2c / ((1 + TPR) * 10).
 *******************************************************************************/
i2c_status_t i2c1_config(i2c_handle_t* p_i2c)
{
	VALIDATE_PTR(p_i2c, I2C_ERROR_NULL_PTR);

	*p_i2c = (i2c_handle_t){0};
	p_i2c->p_I2Cx = I2C1;

	p_i2c->i2c_config.I2C_Device_Mode = I2C_DEVICE_MODE_CONTROLLER;
	p_i2c->i2c_config.I2C_Clock_Source = I2C_CLOCK_SRC_BUSCLK;
	p_i2c->i2c_config.I2C_Clock_Divider = I2C_CLOCK_PRESCALE_DIV_1;
	p_i2c->i2c_config.I2C_Timer_Period = 31U;
	p_i2c->i2c_config.I2C_Addressing_Mode = I2C_ADDRESSING_MODE_7BIT;
	p_i2c->i2c_config.I2C_Enable_Glitch_Filter = I2C_GLITCH_FILTER_ENABLE;
	p_i2c->i2c_config.I2C_Clock_Stretch = I2C_CLOCK_STRETCH_DISABLE;

	static const iomux_config_t i2c1_scl_cfg = {
	    .pincm_index = IOMUX_PIN_PB2,
	    .pf = IOMUX_PIN_PB2_PF_I2C1_SCL,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_LOW,
	    .input_enable = IOMUX_INPUT_ENABLE,
	    .invert = IOMUX_INVERT_DISABLE,
	    .hiz1 = IOMUX_HIZ1_ENABLE};

	static const iomux_config_t i2c1_sda_cfg = {
	    .pincm_index = IOMUX_PIN_PB3,
	    .pf = IOMUX_PIN_PB3_PF_I2C1_SDA,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_LOW,
	    .input_enable = IOMUX_INPUT_ENABLE,
	    .invert = IOMUX_INVERT_DISABLE,
	    .hiz1 = IOMUX_HIZ1_ENABLE};

	iomux_configure_pin(&i2c1_scl_cfg);
	iomux_configure_pin(&i2c1_sda_cfg);

	return i2c_init(p_i2c);
}
