#include "mspm0g350x_i2c.h"
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
 * @DATA_SEND_RECEIVE_POLLING_CONTROLLER
 * @DATA_SEND_RECEIVE_POLLING_TARGET
 * @PERIPHERAL_CONTROL_API
 *
 *******************************************************************************/

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
}
