#ifndef MSPM0G3507_IOMUX_DRIVER_H
#define MSPM0G3507_IOMUX_DRIVER_H

#include <stdint.h>

typedef struct {
    uint8_t pincm_index;            // @IOMUX_PIN
    uint8_t pf;                     // @IOMUX_PF
    uint8_t connect;                // @IOMUX_PC
    uint8_t pull;                   // @IOMUX_PUPD_CONTROL
    uint8_t drive_strength;         // @IOMUX_DRIVE
    uint8_t input_enable;           // @IOMUX_INPUT_STATE
    uint8_t invert;                 // @IOMUX_INVERT Logic inversion
    uint8_t open_drain;             // @IOMUX_DRAIN HIZ1
} iomux_pin_config_t;

// Function return status

typedef enum
{
    IOMUX_OK = 0,              
    IOMUX_ERROR_NULL_PTR,      
    IOMUX_ERROR_INVALID_PINCM_INDEX, 
    IOMUX_ERROR_INVALID_PULL, 
    IOMUX_ERROR_INVALID_PIN,   
    IOMUX_ERROR_INVALID_DRIVE,  
    IOMUX_ERROR_INVALID_STATE,  
} iomux_status_t;

// USAGE: --- @IOMUX_PIN ---

#define IOMUX_PIN_PA0                                     1
#define IOMUX_PIN_PA1                                     2
#define IOMUX_PIN_PA2                                     7
#define IOMUX_PIN_PA3                                     8
#define IOMUX_PIN_PA4                                     9
#define IOMUX_PIN_PA5                                     10
#define IOMUX_PIN_PA6                                     11
#define IOMUX_PIN_PA7                                     14
#define IOMUX_PIN_PA8                                     19
#define IOMUX_PIN_PA9                                     20
#define IOMUX_PIN_PA10                                    21
#define IOMUX_PIN_PA11                                    22
#define IOMUX_PIN_PA12                                    34
#define IOMUX_PIN_PA13                                    35
#define IOMUX_PIN_PA14                                    36
#define IOMUX_PIN_PA15                                    37
#define IOMUX_PIN_PA16                                    38
#define IOMUX_PIN_PA17                                    39
#define IOMUX_PIN_PA18                                    40
#define IOMUX_PIN_PA19                                    41
#define IOMUX_PIN_PA20                                    42
#define IOMUX_PIN_PA21                                    46
#define IOMUX_PIN_PA22                                    47
#define IOMUX_PIN_PA23                                    53
#define IOMUX_PIN_PA24                                    54
#define IOMUX_PIN_PA25                                    55
#define IOMUX_PIN_PA26                                    59
#define IOMUX_PIN_PA27                                    60
#define IOMUX_PIN_PA28                                    3
#define IOMUX_PIN_PA29                                    4
#define IOMUX_PIN_PA30                                    5
#define IOMUX_PIN_PA31                                    6
#define IOMUX_PIN_PB0                                     12
#define IOMUX_PIN_PB1                                     13
#define IOMUX_PIN_PB2                                     15
#define IOMUX_PIN_PB3                                     16
#define IOMUX_PIN_PB4                                     17
#define IOMUX_PIN_PB5                                     18
#define IOMUX_PIN_PB6                                     23
#define IOMUX_PIN_PB7                                     24
#define IOMUX_PIN_PB8                                     25
#define IOMUX_PIN_PB9                                     26
#define IOMUX_PIN_PB10                                    27
#define IOMUX_PIN_PB11                                    28
#define IOMUX_PIN_PB12                                    29
#define IOMUX_PIN_PB13                                    30
#define IOMUX_PIN_PB14                                    31
#define IOMUX_PIN_PB15                                    32
#define IOMUX_PIN_PB16                                    33
#define IOMUX_PIN_PB17                                    43
#define IOMUX_PIN_PB18                                    44
#define IOMUX_PIN_PB19                                    45
#define IOMUX_PIN_PB20                                    48
#define IOMUX_PIN_PB21                                    49
#define IOMUX_PIN_PB22                                    50
#define IOMUX_PIN_PB23                                    51
#define IOMUX_PIN_PB24                                    52
#define IOMUX_PIN_PB25                                    56
#define IOMUX_PIN_PB26                                    57
#define IOMUX_PIN_PB27                                    58

// USAGE -- @IOMUX_PF ---

#define IOMUX_PIN_PA0_PF_GPIO                              0x01
#define IOMUX_PIN_PA0_PF_UART0_TX                          0x02
#define IOMUX_PIN_PA0_PF_I2C0_SDA                          0x03
#define IOMUX_PIN_PA0_PF_TIMA0_C0                          0x04
#define IOMUX_PIN_PA0_PF_TIMA_FAL1                         0x05
#define IOMUX_PIN_PA0_PF_TIMG8_C1                          0x06
#define IOMUX_PIN_PA0_PF_FCC_IN                            0x07

#define IOMUX_PIN_PA1_PF_GPIO                              0x01
#define IOMUX_PIN_PA1_PF_UART0_RX                          0x02
#define IOMUX_PIN_PA1_PF_I2C0_SCL                          0x03
#define IOMUX_PIN_PA1_PF_TIMA0_C1                          0x04
#define IOMUX_PIN_PA1_PF_TIMA_FAL2                         0x05
#define IOMUX_PIN_PA1_PF_TIMG8_IDX                         0x06
#define IOMUX_PIN_PA1_PF_TIMG8_C0                          0x07

#define IOMUX_PIN_PA2_PF_GPIO                              0x01
#define IOMUX_PIN_PA2_PF_TIMG8_C1                          0x02
#define IOMUX_PIN_PA2_PF_SPI0_CS0                          0x03
#define IOMUX_PIN_PA2_PF_TIMG7_C1                          0x04
#define IOMUX_PIN_PA2_PF_SPI1_CS0                          0x05

#define IOMUX_PIN_PA3_PF_GPIO                              0x01
#define IOMUX_PIN_PA3_PF_TIMG8_C0                          0x02
#define IOMUX_PIN_PA3_PF_SPI0_CS1                          0x03
#define IOMUX_PIN_PA3_PF_UART2_CTS                         0x04
#define IOMUX_PIN_PA3_PF_TIMA0_C2                          0x05
#define IOMUX_PIN_PA3_PF_COMP1_OUT                         0x06
#define IOMUX_PIN_PA3_PF_TIMG7_C0                          0x07
#define IOMUX_PIN_PA3_PF_TIMA0_C1                          0x08
#define IOMUX_PIN_PA3_PF_I2C1_SDA                          0x09

#define IOMUX_PIN_PA4_PF_GPIO                              0x01
#define IOMUX_PIN_PA4_PF_TIMG8_C1                          0x02
#define IOMUX_PIN_PA4_PF_SPI0_POCI                         0x03
#define IOMUX_PIN_PA4_PF_UART2_RTS                         0x04
#define IOMUX_PIN_PA4_PF_TIMA0_C3                          0x05
#define IOMUX_PIN_PA4_PF_LFCLK_IN                          0x06
#define IOMUX_PIN_PA4_PF_TIMG7_C1                          0x07
#define IOMUX_PIN_PA4_PF_TIMA0_C1N                         0x08
#define IOMUX_PIN_PA4_PF_I2C1_SCL                          0x09

#define IOMUX_PIN_PA5_PF_GPIO                              0x01
#define IOMUX_PIN_PA5_PF_TIMG8_C0                          0x02
#define IOMUX_PIN_PA5_PF_SPI0_PICO                         0x03
#define IOMUX_PIN_PA5_PF_TIMA_FAL1                         0x04
#define IOMUX_PIN_PA5_PF_TIMG0_C0                          0x05
#define IOMUX_PIN_PA5_PF_TIMG6_C0                          0x06
#define IOMUX_PIN_PA5_PF_FCC_IN                            0x07

#define IOMUX_PIN_PA6_PF_GPIO                              0x01
#define IOMUX_PIN_PA6_PF_TIMG8_C1                          0x02
#define IOMUX_PIN_PA6_PF_SPI0_SCK                          0x03
#define IOMUX_PIN_PA6_PF_TIMA_FAL0                         0x04
#define IOMUX_PIN_PA6_PF_TIMG0_C1                          0x05
#define IOMUX_PIN_PA6_PF_HFCLK_IN                          0x06
#define IOMUX_PIN_PA6_PF_TIMG6_C1                          0x07
#define IOMUX_PIN_PA6_PF_TIMA0_C2N                         0x08

#define IOMUX_PIN_PA7_PF_GPIO                              0x01
#define IOMUX_PIN_PA7_PF_COMP0_OUT                         0x02
#define IOMUX_PIN_PA7_PF_CLK_OUT                           0x03
#define IOMUX_PIN_PA7_PF_TIMG8_C0                          0x04
#define IOMUX_PIN_PA7_PF_TIMA0_C2                          0x05
#define IOMUX_PIN_PA7_PF_TIMG8_IDX                         0x06
#define IOMUX_PIN_PA7_PF_TIMG7_C1                          0x07
#define IOMUX_PIN_PA7_PF_TIMA0_C1                          0x08

#define IOMUX_PIN_PA8_PF_GPIO                              0x01
#define IOMUX_PIN_PA8_PF_UART1_TX                          0x02
#define IOMUX_PIN_PA8_PF_SPI0_CS0                          0x03
#define IOMUX_PIN_PA8_PF_UART0_RTS                         0x04
#define IOMUX_PIN_PA8_PF_TIMA0_C0                          0x05
#define IOMUX_PIN_PA8_PF_TIMA1_C0N                         0x06

#define IOMUX_PIN_PA9_PF_GPIO                              0x01
#define IOMUX_PIN_PA9_PF_UART1_RX                          0x02
#define IOMUX_PIN_PA9_PF_SPI0_PICO                         0x03
#define IOMUX_PIN_PA9_PF_UART0_CTS                         0x04
#define IOMUX_PIN_PA9_PF_TIMA0_C1                          0x05
#define IOMUX_PIN_PA9_PF_RTC_OUT                           0x06
#define IOMUX_PIN_PA9_PF_TIMA0_C0N                         0x07
#define IOMUX_PIN_PA9_PF_TIMA1_C1N                         0x08
#define IOMUX_PIN_PA9_PF_CLK_OUT                           0x09

#define IOMUX_PIN_PA10_PF_GPIO                             0x01
#define IOMUX_PIN_PA10_PF_UART0_TX                         0x02
#define IOMUX_PIN_PA10_PF_SPI_POCI                         0x03
#define IOMUX_PIN_PA10_PF_I2C0_SDA                         0x04
#define IOMUX_PIN_PA10_PF_TIMA1_C0                         0x05
#define IOMUX_PIN_PA10_PF_TIMG12_C0                        0x06
#define IOMUX_PIN_PA10_PF_TIMA0_C2                         0x07
#define IOMUX_PIN_PA10_PF_I2C1_SDA                         0x08
#define IOMUX_PIN_PA10_PF_CLK_OUT                          0x09

#define IOMUX_PIN_PA11_PF_GPIO                             0x01
#define IOMUX_PIN_PA11_PF_UART0_RX                         0x02
#define IOMUX_PIN_PA11_PF_SPI0_SCK                         0x03
#define IOMUX_PIN_PA11_PF_I2C0_SCL                         0x04
#define IOMUX_PIN_PA11_PF_TIMA1_C1                         0x05
#define IOMUX_PIN_PA11_PF_COMP0_OUT                        0x06
#define IOMUX_PIN_PA11_PF_TIMA0_C2N                        0x07
#define IOMUX_PIN_PA11_PF_I2C1_SCL                         0x08

#define IOMUX_PIN_PA12_PF_GPIO                             0x01
#define IOMUX_PIN_PA12_PF_UART3_CTS                        0x02
#define IOMUX_PIN_PA12_PF_SPI0_SCK                         0x03
#define IOMUX_PIN_PA12_PF_TIMG0_C0                         0x04
#define IOMUX_PIN_PA12_PF_CAN_TX                           0x05
#define IOMUX_PIN_PA12_PF_TIMA0_C3                         0x06
#define IOMUX_PIN_PA12_PF_FCC_IN                           0x07

#define IOMUX_PIN_PA13_PF_GPIO                             0x01
#define IOMUX_PIN_PA13_PF_UART3_RTS                        0x02
#define IOMUX_PIN_PA13_PF_SPI0_POCI                        0x03
#define IOMUX_PIN_PA13_PF_UART3_RX                         0x04
#define IOMUX_PIN_PA13_PF_TIMG0_C1                         0x05
#define IOMUX_PIN_PA13_PF_CAN_RX                           0x06
#define IOMUX_PIN_PA13_PF_TIMA0_C3N                        0x07

#define IOMUX_PIN_PA14_PF_GPIO                             0x01
#define IOMUX_PIN_PA14_PF_UART0_CTS                        0x02
#define IOMUX_PIN_PA14_PF_SPI0_PICO                        0x03
#define IOMUX_PIN_PA14_PF_UART3_TX                         0x04
#define IOMUX_PIN_PA14_PF_TIMG12_C0                        0x05
#define IOMUX_PIN_PA14_PF_CLK_OUT                          0x06

#define IOMUX_PIN_PA15_PF_GPIO                             0x01
#define IOMUX_PIN_PA15_PF_UART0_RTS                        0x02
#define IOMUX_PIN_PA15_PF_SPI1_CS2                         0x03
#define IOMUX_PIN_PA15_PF_I2C1_SCL                         0x04
#define IOMUX_PIN_PA15_PF_TIMA1_C0                         0x05
#define IOMUX_PIN_PA15_PF_TIMG8_IDX                        0x06
#define IOMUX_PIN_PA15_PF_TIMA1_C0N                        0x07
#define IOMUX_PIN_PA15_PF_TIMA0_C2                         0x08

#define IOMUX_PIN_PA16_PF_GPIO                             0x01
#define IOMUX_PIN_PA16_PF_COMP2_OUT                        0x02
#define IOMUX_PIN_PA16_PF_SPI1_POCI                        0x03
#define IOMUX_PIN_PA16_PF_I2C1_SDA                         0x04
#define IOMUX_PIN_PA16_PF_TIMA1_C1                         0x05
#define IOMUX_PIN_PA16_PF_TIMA1_C1N                        0x06
#define IOMUX_PIN_PA16_PF_TIMA0_C2N                        0x07
#define IOMUX_PIN_PA16_PF_FCC_IN                           0x08

#define IOMUX_PIN_PA17_GPIO                                0x01
#define IOMUX_PIN_PA17_UART1_TX                            0x02
#define IOMUX_PIN_PA17_SPI1_SCK                            0x03
#define IOMUX_PIN_PA17_I2C1_SCL                            0x04
#define IOMUX_PIN_PA17_TIMA0_C3                            0x05
#define IOMUX_PIN_PA17_TIMG7_C0                            0x06
#define IOMUX_PIN_PA17_TIMA1_C0                            0x07

#define IOMUX_PIN_PA18_GPIO                                0x01
#define IOMUX_PIN_PA18_UART1_RX                            0x02
#define IOMUX_PIN_PA18_SPI1_PICO                           0x03
#define IOMUX_PIN_PA18_I2C1_SDA                            0x04
#define IOMUX_PIN_PA18_TIMA0_C3N                           0x05
#define IOMUX_PIN_PA18_TIMG7_C1                            0x06
#define IOMUX_PIN_PA18_TIMA1_C1                            0x07

#define IOMUX_PIN_PA19_PF_GPIO                             0x01
#define IOMUX_PIN_PA19_PF_SWDIO                            0x02

#define IOMUX_PIN_PA20_PF_GPIO                             0x01
#define IOMUX_PIN_PA20_PF_SWCLK                            0x02

#define IOMUX_PIN_PA21_PF_GPIO                             0x01
#define IOMUX_PIN_PA21_PF_UART2_TX                         0x02
#define IOMUX_PIN_PA21_PF_TIMG8_C0                         0x03
#define IOMUX_PIN_PA21_PF_UART1_CTS                        0x04
#define IOMUX_PIN_PA21_PF_TIMA0_C0                         0x05
#define IOMUX_PIN_PA21_PF_TIMG6_C0                         0x06

#define IOMUX_PIN_PA22_PF_GPIO                             0x01
#define IOMUX_PIN_PA22_PF_UART2_RX                         0x02
#define IOMUX_PIN_PA22_PF_TIMG8_C1                         0x03
#define IOMUX_PIN_PA22_PF_UART1_RTS                        0x04
#define IOMUX_PIN_PA22_PF_TIMA0_C1                         0x05
#define IOMUX_PIN_PA22_PF_CLK_OUT                          0x06
#define IOMUX_PIN_PA22_PF_TIMA0_C0N                        0x07
#define IOMUX_PIN_PA22_PF_TIMG6_C1                         0x08

#define IOMUX_PIN_PA23_PF_GPIO                             0x01
#define IOMUX_PIN_PA23_PF_UART2_TX                         0x02
#define IOMUX_PIN_PA23_PF_SPI0_CS3                         0x03
#define IOMUX_PIN_PA23_PF_TIMA0_C3                         0x04
#define IOMUX_PIN_PA23_PF_TIMG0_C0                         0x05
#define IOMUX_PIN_PA23_PF_UART3_CTS                        0x06
#define IOMUX_PIN_PA23_PF_TIMG7_C0                         0x07
#define IOMUX_PIN_PA23_PF_TIMG8_C0                         0x08

#define IOMUX_PIN_PA24_PF_GPIO                             0x01
#define IOMUX_PIN_PA24_PF_UART2_RX                         0x02
#define IOMUX_PIN_PA24_PF_SPI0_CS2                         0x03
#define IOMUX_PIN_PA24_PF_TIMA0_C3N                        0x04
#define IOMUX_PIN_PA24_PF_TIMG0_C1                         0x05
#define IOMUX_PIN_PA24_PF_UART3_RTS                        0x06
#define IOMUX_PIN_PA24_PF_TIMG7_C1                         0x07
#define IOMUX_PIN_PA24_PF_TIMA1_C1                         0x08

#define IOMUX_PIN_PA25_PF_GPIO                             0x01
#define IOMUX_PIN_PA25_PF_UART3_RX                         0x02
#define IOMUX_PIN_PA25_PF_SPI1_CS3                         0x03
#define IOMUX_PIN_PA25_PF_TIMG12_C1                        0x04
#define IOMUX_PIN_PA25_PF_TIMA0_C3                         0x05
#define IOMUX_PIN_PA25_PF_TIMA0_C1N                        0x06

#define IOMUX_PIN_PA26_PF_GPIO                             0x01
#define IOMUX_PIN_PA26_PF_UART3_TX                         0x02
#define IOMUX_PIN_PA26_PF_SPI1_CS0                         0x03
#define IOMUX_PIN_PA26_PF_TIMG8_C0                         0x04
#define IOMUX_PIN_PA26_PF_TIMA_FAL0                        0x05
#define IOMUX_PIN_PA26_PF_CAN_TX                           0x06
#define IOMUX_PIN_PA26_PF_TIMG7_C0                         0x07

#define IOMUX_PIN_PA27_PF_GPIO                              0x01
#define IOMUX_PIN_PA27_PF_RTC_OUT                           0x02
#define IOMUX_PIN_PA27_PF_SPI1_CS1                          0x03
#define IOMUX_PIN_PA27_PF_TIMG8_C1                          0x04
#define IOMUX_PIN_PA27_PF_TIMA_FAL2                         0x05
#define IOMUX_PIN_PA27_PF_CAN_RX                            0x06
#define IOMUX_PIN_PA27_PF_TIMG7_C1                          0x07

#define IOMUX_PIN_PA28_PF_GPIO                              0x01
#define IOMUX_PIN_PA28_PF_UART0_TX                          0x02
#define IOMUX_PIN_PA28_PF_I2C0_SDA                          0x03
#define IOMUX_PIN_PA28_PF_TIMA0_C3                          0x04
#define IOMUX_PIN_PA28_PF_TIMA_FAL0                         0x05
#define IOMUX_PIN_PA28_PF_TIMG7_C0                          0x06
#define IOMUX_PIN_PA28_PF_TIMA1_C0                          0x07

#define IOMUX_PIN_PA29_PF_GPIO                              0x01
#define IOMUX_PIN_PA29_PF_I2C1_SCL                          0x02
#define IOMUX_PIN_PA29_PF_UART2_RTS                         0x03
#define IOMUX_PIN_PA29_PF_TIMG8_C0                          0x04
#define IOMUX_PIN_PA29_PF_TIMG6_C0                          0x05

#define IOMUX_PIN_PA30_PF_GPIO                              0x01
#define IOMUX_PIN_PA30_PF_I2C1_SDA                          0x02
#define IOMUX_PIN_PA30_PF_UART2_CTS                         0x03
#define IOMUX_PIN_PA30_PF_TIMG8_C1                          0x04
#define IOMUX_PIN_PA30_PF_TIMG6_C1                          0x05

#define IOMUX_PIN_PA31_PF_GPIO                              0x01
#define IOMUX_PIN_PA31_PF_UART0_RX                          0x02
#define IOMUX_PIN_PA31_PF_I2C0_SCL                          0x03
#define IOMUX_PIN_PA31_PF_TIMA0_C3N                         0x04
#define IOMUX_PIN_PA31_PF_TIMG12_C1                         0x05
#define IOMUX_PIN_PA31_PF_CLK_OUT                           0x06
#define IOMUX_PIN_PA31_PF_TIMG7_C1                          0x07
#define IOMUX_PIN_PA31_PF_TIMA1_C1                          0x08

#define IOMUX_PIN_PB0_PF_GPIO                               0x01
#define IOMUX_PIN_PB0_PF_UART0_TX                           0x02
#define IOMUX_PIN_PB0_PF_SPI1_CS2                           0x03
#define IOMUX_PIN_PB0_PF_TIMA1_C0                           0x04
#define IOMUX_PIN_PB0_PF_TIMA0_C2                           0x05

#define IOMUX_PIN_PB1_PF_GPIO                               0x01
#define IOMUX_PIN_PB1_PF_UART0_RX                           0x02
#define IOMUX_PIN_PB1_PF_SPI1_CS3                           0x03
#define IOMUX_PIN_PB1_PF_TIMA1_C1                           0x04
#define IOMUX_PIN_PB1_PF_TIMA0_C2N                          0x05

#define IOMUX_PIN_PB2_PF_GPIO                               0x01
#define IOMUX_PIN_PB2_PF_UART3_TX                           0x02
#define IOMUX_PIN_PB2_PF_UART2_CTS                          0x03
#define IOMUX_PIN_PB2_PF_I2C1_SCL                           0x04
#define IOMUX_PIN_PB2_PF_TIMA0_C3                           0x05
#define IOMUX_PIN_PB2_PF_UART1_CTS                          0x06
#define IOMUX_PIN_PB2_PF_TIMG6_C0                           0x07
#define IOMUX_PIN_PB2_PF_TIMA1_C0                           0x08

#define IOMUX_PIN_PB3_PF_GPIO                               0x01
#define IOMUX_PIN_PB3_PF_UART3_RX                           0x02
#define IOMUX_PIN_PB3_PF_UART2_RTS                          0x03
#define IOMUX_PIN_PB3_PF_I2C1_SDA                           0x04
#define IOMUX_PIN_PB3_PF_TIMA0_C3N                          0x05
#define IOMUX_PIN_PB3_PF_UART1_RTS                          0x06
#define IOMUX_PIN_PB3_PF_TIMG6_C1                           0x07
#define IOMUX_PIN_PB3_PF_TIMA1_C1                           0x08

#define IOMUX_PIN_PB4_PF_GPIO                               0x01
#define IOMUX_PIN_PB4_PF_UART1_TX                           0x02
#define IOMUX_PIN_PB4_PF_UART3_CTS                          0x03
#define IOMUX_PIN_PB4_PF_TIMA1_C0                           0x04
#define IOMUX_PIN_PB4_PF_TIMA0_C2                           0x05
#define IOMUX_PIN_PB4_PF_TIMA1_C0N                          0x06

#define IOMUX_PIN_PB5_PF_GPIO                               0x01
#define IOMUX_PIN_PB5_PF_UART1_RX                           0x02
#define IOMUX_PIN_PB5_PF_UART3_RTS                          0x03
#define IOMUX_PIN_PB5_PF_TIMA1_C1                           0x04
#define IOMUX_PIN_PB5_PF_TIMA0_C2N                          0x05
#define IOMUX_PIN_PB5_PF_TIMA1_C1N                          0x06

#define IOMUX_PIN_PB6_PF_GPIO                               0x01
#define IOMUX_PIN_PB6_PF_UART1_TX                           0x02
#define IOMUX_PIN_PB6_PF_SPI1_CS0                           0x03
#define IOMUX_PIN_PB6_PF_SPI0_CS1                           0x04
#define IOMUX_PIN_PB6_PF_TIMG8_C0                           0x05
#define IOMUX_PIN_PB6_PF_UART2_CTS                          0x06
#define IOMUX_PIN_PB6_PF_TIMG6_C0                           0x07
#define IOMUX_PIN_PB6_PF_TIMA1_C0N                          0x08

#define IOMUX_PIN_PB7_PF_GPIO                               0x01
#define IOMUX_PIN_PB7_PF_UART1_RX                           0x02
#define IOMUX_PIN_PB7_PF_SPI1_POCI                          0x03
#define IOMUX_PIN_PB7_PF_SPI0_CS2                           0x04
#define IOMUX_PIN_PB7_PF_TIMG8_C1                           0x05
#define IOMUX_PIN_PB7_PF_UART2_RTS                          0x06
#define IOMUX_PIN_PB7_PF_TIMG6_C1                           0x07
#define IOMUX_PIN_PB7_PF_TIMA1_C1N                          0x08

#define IOMUX_PIN_PB8_PF_GPIO                               0x01
#define IOMUX_PIN_PB8_PF_UART1_CTS                          0x02
#define IOMUX_PIN_PB8_PF_SPI1_PICO                          0x03
#define IOMUX_PIN_PB8_PF_TIMA0_C0                           0x04
#define IOMUX_PIN_PB8_PF_COMP1_OUT                          0x05

#define IOMUX_PIN_PB9_PF_GPIO                               0x01
#define IOMUX_PIN_PB9_PF_UART1_RTS                          0x02
#define IOMUX_PIN_PB9_PF_SPI1_SCK                           0x03
#define IOMUX_PIN_PB9_PF_TIMA0_C1                           0x04
#define IOMUX_PIN_PB9_PF_TIMA0_C0N                          0x05

#define IOMUX_PIN_PB10_PF_GPIO                              0x01
#define IOMUX_PIN_PB10_PF_TIMG0_C0                          0x02
#define IOMUX_PIN_PB10_PF_TIMG8_C0                          0x03
#define IOMUX_PIN_PB10_PF_COMP1_OUT                         0x04
#define IOMUX_PIN_PB10_PF_TIMG6_C0                          0x05

#define IOMUX_PIN_PB11_PF_GPIO                              0x01
#define IOMUX_PIN_PB11_PF_TIMG0_C1                          0x02
#define IOMUX_PIN_PB11_PF_TIMG8_C1                          0x03
#define IOMUX_PIN_PB11_PF_CLK_OUT                           0x04
#define IOMUX_PIN_PB11_PF_TIMG6_C1                          0x05

#define IOMUX_PIN_PB12_PF_GPIO                              0x01
#define IOMUX_PIN_PB12_PF_UART3_TX                          0x02
#define IOMUX_PIN_PB12_PF_TIMA0_C2                          0x03
#define IOMUX_PIN_PB12_PF_TIMA_FAL1                         0x04
#define IOMUX_PIN_PB12_PF_TIMA0_C1                          0x05

#define IOMUX_PIN_PB13_PF_GPIO                              0x01
#define IOMUX_PIN_PB13_PF_UART3_RX                          0x02
#define IOMUX_PIN_PB13_PF_TIMA0_C3                          0x03
#define IOMUX_PIN_PB13_PF_TIMG12_C0                         0x04
#define IOMUX_PIN_PB13_PF_TIMA0_C1N                         0x05

#define IOMUX_PIN_PB14_PF_GPIO                              0x01
#define IOMUX_PIN_PB14_PF_SPI1_CS3                          0x02
#define IOMUX_PIN_PB14_PF_SPI1_POCI                         0x03
#define IOMUX_PIN_PB14_PF_SPI0_CS3                          0x04
#define IOMUX_PIN_PB14_PF_TIMG12_C1                         0x05
#define IOMUX_PIN_PB14_PF_TIMG8_IDX                         0x06
#define IOMUX_PIN_PB14_PF_TIMA0_C0                          0x07

#define IOMUX_PIN_PB15_PF_GPIO                              0x01
#define IOMUX_PIN_PB15_PF_UART2_TX                          0x02
#define IOMUX_PIN_PB15_PF_SPI1_PICO                         0x03
#define IOMUX_PIN_PB15_PF_UART3_CTS                         0x04
#define IOMUX_PIN_PB15_PF_TIMG8_C0                          0x05
#define IOMUX_PIN_PB15_PF_TIMG7_C0                          0x06

#define IOMUX_PIN_PB16_PF_GPIO                              0x01
#define IOMUX_PIN_PB16_PF_UART2_RX                          0x02
#define IOMUX_PIN_PB16_PF_SPI1_SCK                          0x03
#define IOMUX_PIN_PB16_PF_UART3_RTS                         0x04
#define IOMUX_PIN_PB16_PF_TIMG8_C1                          0x05
#define IOMUX_PIN_PB16_PF_TIMG7_C1                          0x06

#define IOMUX_PIN_PB17_PF_GPIO                              0x01
#define IOMUX_PIN_PB17_PF_UART2_TX                          0x02
#define IOMUX_PIN_PB17_PF_SPI0_PICO                         0x03
#define IOMUX_PIN_PB17_PF_SPI1_CS1                          0x04
#define IOMUX_PIN_PB17_PF_TIMA1_C0                          0x05
#define IOMUX_PIN_PB17_PF_TIMA0_C2                          0x06

#define IOMUX_PIN_PB18_PF_GPIO                              0x01
#define IOMUX_PIN_PB18_PF_UART2_RX                          0x02
#define IOMUX_PIN_PB18_PF_SPI0_SCK                          0x03
#define IOMUX_PIN_PB18_PF_SPI1_CS2                          0x04
#define IOMUX_PIN_PB18_PF_TIMA1_C1                          0x05
#define IOMUX_PIN_PB18_PF_TIMA0_C2N                         0x06

#define IOMUX_PIN_PB19_PF_GPIO                              0x01
#define IOMUX_PIN_PB19_PF_COMP2_OUT                         0x02
#define IOMUX_PIN_PB19_PF_SPI0_POCI                         0x03
#define IOMUX_PIN_PB19_PF_TIMG8_C1                          0x04
#define IOMUX_PIN_PB19_PF_UART0_CTS                         0x05
#define IOMUX_PIN_PB19_PF_TIMG7_C1                          0x06

#define IOMUX_PIN_PB20_PF_GPIO                              0x01
#define IOMUX_PIN_PB20_PF_SPI0_CS2                          0x02
#define IOMUX_PIN_PB20_PF_SPI1_CS0                          0x03
#define IOMUX_PIN_PB20_PF_TIMA0_C2                          0x04
#define IOMUX_PIN_PB20_PF_TIMG12_C0                         0x05
#define IOMUX_PIN_PB20_PF_TIMA_FAL1                         0x06
#define IOMUX_PIN_PB20_PF_TIMA0_C1                          0x07
#define IOMUX_PIN_PB20_PF_TIMA1_C1N                         0x08

#define IOMUX_PIN_PB21_PF_GPIO                              0x01
#define IOMUX_PIN_PB21_PF_SPI1_POCI                         0x02
#define IOMUX_PIN_PB21_PF_TIMG8_C0                          0x03

#define IOMUX_PIN_PB22_PF_GPIO                              0x01
#define IOMUX_PIN_PB22_PF_SPI1_PICO                         0x02
#define IOMUX_PIN_PB22_PF_TIMG8_C1                          0x03

#define IOMUX_PIN_PB23_PF_GPIO                              0x01
#define IOMUX_PIN_PB23_PF_SPI1_SCK                          0x02
#define IOMUX_PIN_PB23_PF_COMP0_OUT                         0x03
#define IOMUX_PIN_PB23_PF_TIMA_FAL0                         0x04

#define IOMUX_PIN_PB24_PF_GPIO                              0x01
#define IOMUX_PIN_PB24_PF_SPI0_CS3                          0x02
#define IOMUX_PIN_PB24_PF_SPI0_CS1                          0x03
#define IOMUX_PIN_PB24_PF_TIMA0_C3                          0x04
#define IOMUX_PIN_PB24_PF_TIMG12_C1                         0x05
#define IOMUX_PIN_PB24_PF_TIMA0_C1N                         0x06
#define IOMUX_PIN_PB24_PF_TIMA1_C0N                         0x07

#define IOMUX_PIN_PB25_PF_GPIO                              0x01
#define IOMUX_PIN_PB25_PF_UART0_CTS                         0x02
#define IOMUX_PIN_PB25_PF_SPI0_CS0                          0x03
#define IOMUX_PIN_PB25_PF_TIMA_FAL2                         0x04

#define IOMUX_PIN_PB26_PF_GPIO                              0x01
#define IOMUX_PIN_PB26_PF_UART0_RTS                         0x02
#define IOMUX_PIN_PB26_PF_SPI0_CS1                          0x03
#define IOMUX_PIN_PB26_PF_TIMA0_C3                          0x04
#define IOMUX_PIN_PB26_PF_TIMG6_C0                          0x05
#define IOMUX_PIN_PB26_PF_TIMA1_C0                          0x06

#define IOMUX_PIN_PB27_PF_GPIO                              0x01
#define IOMUX_PIN_PB27_PF_COMP2_OU                          0x02
#define IOMUX_PIN_PB27_PF_SPI1_CS2                          0x03
#define IOMUX_PIN_PB27_PF_TIMA0_C3N                         0x04
#define IOMUX_PIN_PB27_PF_TIMG6_C1                          0x05
#define IOMUX_PIN_PB27_PF_TIMA1_C1                          0x06

// USAGE: --- @IOMUX_PC ---

#define IOMUX_PC_DISCONNECT         0
#define IOMUX_PC_CONNECT            1

// USAGE: --- @IOMUX_PUPD_CONTROL ---

#define IOMUX_PULL_NONE             0
#define IOMUX_PULL_UP               1
#define IOMUX_PULL_DOWN             2

// USAGE --- @IOMUX_DRIVE ---

#define IOMUX_DRIVE_LOW             0
#define IOMUX_DRIVE_HIGH            0

// USAGE --- @IOMUX_INPUT_STATE

#define IOMUX_STATE_DISABLE         0
#define IOMUX_STATE_ENABLE          1

iomux_status_t iomux_configure_pin(const iomux_pin_config_t *cfg);

#endif