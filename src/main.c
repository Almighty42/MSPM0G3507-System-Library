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
	spi.spi_config.SPI_Clock_Prescaler = 19U;
	spi.spi_config.SPI_Data_Width = SPI_DATA_WIDTH_8;
	spi.spi_config.SPI_CPOL = SPI_CPOL_HIGH;
	spi.spi_config.SPI_CPHA = SPI_CPHA_HIGH;
	spi.spi_config.SPI_CS_Select = SPI_CS_0;
	spi.spi_config.SPI_Frame_Format_Select = SPI_FRAMEFORMAT_4WIRE;
	spi.spi_config.SPI_MSB = SPI_MSB_MSB;

	static const iomux_config_t spi1_sck_cfg = {
	    .pincm_index = IOMUX_PIN_PB9,
	    .pf = IOMUX_PIN_PB9_PF_SPI1_SCK,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
	};

	static const iomux_config_t spi1_pico_cfg = {
	    .pincm_index = IOMUX_PIN_PB8,
	    .pf = IOMUX_PIN_PB8_PF_SPI1_PICO,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
	};

	static const iomux_config_t spi1_poci_cfg = {
	    .pincm_index = IOMUX_PIN_PB7,
	    .pf = IOMUX_PIN_PB7_PF_SPI1_POCI,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_NONE,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_ENABLE,
	};

	static const iomux_config_t spi1_cs0_cfg = {
	    .pincm_index = IOMUX_PIN_PB6,
	    .pf = IOMUX_PIN_PB6_PF_SPI1_CS0,
	    .pad_connect = IOMUX_PC_CONNECT,
	    .pull = IOMUX_PULL_UP,
	    .drive_strength = IOMUX_DRIVE_HIGH,
	    .input_enable = IOMUX_INPUT_DISABLE,
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

	static volatile uint32_t ok_count, err_count;

	uint8_t rx[8] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

	while (1) {
		if (spi_read_data_pl(&spi, rx, 8U, SPI_SOFTWARE_TIMEOUT) ==
		    SPI_OK) {
			ok_count++;
		}
		else {
			err_count++;
		}
	}
}
