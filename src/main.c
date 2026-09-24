#include "../inc/mspm0g350x_iomux.h"
#include "../inc/mspm0g350x_spi.h"
#include "../inc/mspm0g350x_startup.h"
#include <stdint.h>

void SysTick_Handler(void)
{
}

int main(void)
{

	spi_handle_t spi = {0};

	spi.p_SPIx = SPI1;
	spi.spi_config.SPI_Device_Mode = SPI_DEVICE_MODE_CONTROLLER;
	spi.spi_config.SPI_Clock_Source = SPI_CLOCK_SRC_BUSCLK;
	spi.spi_config.SPI_Clock_Divide_Ratio = SPI_SCLK_SPEED_DIV_1;
	spi.spi_config.SPI_Clock_Prescaler = 1U;
	spi.spi_config.SPI_Data_Width = SPI_DATA_WIDTH_8;
	spi.spi_config.SPI_CPOL = SPI_CPOL_HIGH;
	spi.spi_config.SPI_CPHA = SPI_CPHA_HIGH;
	spi.spi_config.SPI_CS_Select = SPI_CS_0;
	spi.spi_config.SPI_MSB = SPI_MSB_MSB;

	static const iomux_pin_config_t spi1_sck_cfg = {
	    .pincm_index = IOMUX_PIN_PB23,
	    .pf = IOMUX_PIN_PB23_PF_SPI1_SCK,
	    .connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_STATE_DISABLE,
	};

	static const iomux_pin_config_t spi1_pico_cfg = {
	    .pincm_index = IOMUX_PIN_PB22,
	    .pf = IOMUX_PIN_PB22_PF_SPI1_PICO,
	    .connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_STATE_DISABLE,
	};

	static const iomux_pin_config_t spi1_poci_cfg = {
	    .pincm_index = IOMUX_PIN_PB21,
	    .pf = IOMUX_PIN_PB21_PF_SPI1_POCI,
	    .connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_STATE_ENABLE,
	};

	static const iomux_pin_config_t spi1_cs0_cfg = {
	    .pincm_index = IOMUX_PIN_PB20,
	    .pf = IOMUX_PIN_PB20_PF_SPI1_CS0,
	    .connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_STATE_DISABLE,
	};

	iomux_configure_pin(&spi1_sck_cfg);
	iomux_configure_pin(&spi1_pico_cfg);
	iomux_configure_pin(&spi1_poci_cfg);
	iomux_configure_pin(&spi1_cs0_cfg);

	spi_status_t status = spi_init(&spi);
	if (status != SPI_OK) {
		while (1) {
		}
	}

	while (1) {
	}
}
