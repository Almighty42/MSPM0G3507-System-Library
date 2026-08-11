#include <stdint.h>
#include "../inc/mspm0g350x_startup.h"
#include "../inc/mspm0g350x_gpio.h"
#include "../inc/mspm0g350x_iomux.h"

void SysTick_Handler(void)
{

}

void led_pin_init(void);

void clk_out_init(void);

int main(void)
{

	// led_pin_init();

    clk_out_init();

    // gpio_write(GPIO0, DIO0, 1);

	while (1) {
	}
	return 0;
}

// void led_pin_init(void)
// {
//     gpio_enable_power(GPIO0);   /* must come before any DOE/DOUT/PINCM writes */

//     iomux_pin_config_t iomux_cfg = {
//         .pincm_index = PIN_PA0,
//         .pf = IOMUX_PF_GPIO,
//         .pull = IOMUX_PULL_NONE,
//         .input_enable = 0,
//     };
//     iomux_configure_pin(&iomux_cfg);

//     gpio_pin_config_t gpio_cfg = {
//         .port = GPIO0,
//         .dio_bit = DIO0,
//         .direction = GPIO_DIR_OUTPUT,
//     };
//     gpio_configure_pin(&gpio_cfg);

//     gpio_write(GPIO0, DIO0, 0);
// }

void clk_out_init(void)
{
    // 1. iomux_configure_pin() with .pincm_index = PIN_PA22, .pf = IOMUX_PF_CLK_OUT.
    iomux_pin_config_t iomux_cfg = {
        .pincm_index = PIN_PA22,
        .pf = IOMUX_PF_CLK_OUT,
        .pull = IOMUX_PULL_NONE,
        .input_enable = 0,
    };
    iomux_configure_pin(&iomux_cfg);


    // 2. Set SYSCTL->GENCLKCFG.EXCLKSRC = SYSOSC's encoding (confirm which value maps to SYSOSC in G-series — do not assume it's 0 like the L-series).
    // How to do this
    SYSCTL->GENCLKCFG = (SYSCTL->GENCLKCFG & ~SYSCTL_GENCLKCFG_EXCLKSRC_MASK)
                   | (SYSCTL_GENCLKCFG_EXCLKSRC_SYSOSC << SYSCTL_GENCLKCFG_EXCLKSRC);

    // 3. Set CLK_OUT source as SYSPLL
    WRITE_FIELD(SYSCTL->GENCLKCFG, SYSCTL_GENCLKCFG_EXCLKSRC, 3, 0x5U);

    // 4. Leave EXCLKDIVEN = 0 (passthrough) for now — you want raw 32 MHz, not divided.
    SET_BIT(SYSCTL->GENCLKCFG, SYSCTL_GENCLKCFG_EXCLKDIVEN);
    WRITE_FIELD(SYSCTL->GENCLKCFG,SYSCTL_GENCLKCFG_EXCLKDIVVAL, 3, 0x7U );

    // 5. Set SYSCTL->GENCLKEN.EXCLKEN = 1.
    SET_BIT(SYSCTL->GENCLKEN, SYSCTL_GENCLKEN_EXCLKEN);
}
