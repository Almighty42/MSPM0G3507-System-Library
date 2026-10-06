#include "../inc/mspm0g350x_i2c.h"
#include "../inc/mspm0g350x_iomux.h"
#include "example_i2c.h"
#include "mspm0g350x_startup.h"
#include <stdint.h>

#define I2C_EXAMPLE_BYTES 4U
#define I2C_EXAMPLE_NACK_ADDR 0x7AU /* ADAPT: an address nothing answers */
#define I2C_SOFTWARE_TIMEOUT 100U   /* ms, matches the driver's ms deadline */

#define I2C_EXAMPLE_TARGET_ADDR 0x50U
#define I2C_EXAMPLE_ADDR_BYTES 1U /* VERIFY in the EEPROM datasheet: 1 or 2 */

#define I2C_EXAMPLE_BYTES 4U

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
	p_i2c->i2c_config.I2C_Timer_Period = 39U;
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

/********************************************************************************
 * @fn          - i2c1_example_write
 *
 * @brief	- Writes a fixed pattern to a target on I2C1
 *
 * @param[*p_i2c] - Handle structure of an I2C peripheral
 *
 * @return      - Success / Failure status of the function
 *
 * @Note	- Test setup: logic analyzer on SCL (PB2) and SDA (PB3), I2C
 *		decoder. External 4.7 kOhm pull-ups on both lines. PASS: decoder
 *shows START, address 0x48 + W, ACK, bytes DE AD BE EF each ACKed, STOP, and
 *the function returns I2C_OK. The function must return at or after the STOP.
 *******************************************************************************/

#define I2C_EXAMPLE_BYTES 4U

i2c_status_t i2c1_example_write(i2c_handle_t* p_i2c)
{
	/* byte 0 = memory address 0x00, bytes 1..3 = data DE AD BE */
	static const uint8_t pattern[I2C_EXAMPLE_BYTES] = {0x00U, 0xDEU, 0xADU,
	                                                   0xBEU};

	return i2c_controller_write_pl(p_i2c, I2C_EXAMPLE_TARGET_ADDR, pattern,
	                               I2C_EXAMPLE_BYTES, I2C_SOFTWARE_TIMEOUT);
}

/********************************************************************************
 * @fn		- i2c1_example_nack
 *
 * @brief	- Writes one byte to an address nobody answers
 *
 * @param[*p_i2c]- Handle structure of an I2C peripheral
 *
 * @return	- Success / Failure status of the function
 *
 * @Note	- Negative test. Same setup as i2c1_example_write.
 *		PASS: decoder shows START, address + W, NACK, STOP, and the
 *		function returns the address-NACK error (not I2C_OK, not a
 *		timeout). Call it twice in a row: the second call must give
 *		the same result, proving the error path leaves the peripheral
 *		clean.
 *******************************************************************************/

i2c_status_t i2c1_example_nack(i2c_handle_t* p_i2c)
{
	static const uint8_t byte = 0x00U;
	return i2c_controller_write_pl(p_i2c, I2C_EXAMPLE_NACK_ADDR, &byte, 1U,
	                               I2C_SOFTWARE_TIMEOUT);
}

/********************************************************************************
 * @fn          - i2c1_example_write_read
 *
 * @brief       - Random read of 3 bytes from memory address 0x00
 *
 * @param[*p_i2c] - Handle structure of an I2C peripheral
 * @param[*p_rx]  - Receive buffer, at least I2C_EXAMPLE_READ_BYTES long
 *
 * @return      - Success / Failure status of the function
 *
 * @Note        - Test setup: logic analyzer on SCL (PB2) and SDA (PB3), I2C
 *                decoder, pull-ups on the EEPROM board. Run after
 *                i2c1_example_write. PASS: decoder shows S, 0x50 + W, ACK,
 *                00, ACK, repeated S (no P between), 0x50 + R, ACK, then
 *                DE AD BE with ACK, ACK, NACK, then P. The function returns
 *                I2C_OK and p_rx holds DE AD BE.
 *******************************************************************************/

#define I2C_EXAMPLE_READ_BYTES 3U

i2c_status_t i2c1_example_write_read(i2c_handle_t* p_i2c, uint8_t* p_rx)
{
	static const uint8_t mem_addr[1] = {0x00U};

	return i2c_controller_write_read_pl(
	    p_i2c, I2C_EXAMPLE_TARGET_ADDR, mem_addr, 1U, p_rx,
	    I2C_EXAMPLE_READ_BYTES, I2C_SOFTWARE_TIMEOUT);
}

/********************************************************************************
 * @fn          - i2c1_example_read
 *
 * @brief       - Current-address read of 3 bytes
 *
 * @param[*p_i2c] - Handle structure of an I2C peripheral
 * @param[*p_rx]  - Receive buffer, at least I2C_EXAMPLE_READ_BYTES long
 *
 * @return      - Success / Failure status of the function
 *
 * @Note        - Test setup: as above. The EEPROM address pointer must
 *                already be at 0x00, for example after
 *                i2c1_example_write_read. PASS: decoder shows S, 0x50 + R,
 *                ACK, DE AD BE, last byte NACKed, P. The function returns
 *                I2C_OK.
 *******************************************************************************/

i2c_status_t i2c1_example_read(i2c_handle_t* p_i2c, uint8_t* p_rx)
{
	return i2c_controller_read_pl(p_i2c, I2C_EXAMPLE_TARGET_ADDR, p_rx,
	                              I2C_EXAMPLE_READ_BYTES,
	                              I2C_SOFTWARE_TIMEOUT);
}
