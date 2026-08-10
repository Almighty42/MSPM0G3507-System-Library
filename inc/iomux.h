#include <stdint.h>

#define PIN_PA0                                     1
#define PIN_PA1                                     2
#define PIN_PA2                                     7
#define PIN_PA3                                     8
#define PIN_PA4                                     9
#define PIN_PA5                                     10
#define PIN_PA6                                     11
#define PIN_PA7                                     14
#define PIN_PA8                                     19
#define PIN_PA9                                     20
#define PIN_PA10                                    21
#define PIN_PA11                                    22
#define PIN_PA12                                    34
#define PIN_PA13                                    35
#define PIN_PA14                                    36
#define PIN_PA15                                    37
#define PIN_PA16                                    38
#define PIN_PA17                                    39
#define PIN_PA18                                    40
#define PIN_PA19                                    41
#define PIN_PA20                                    42
#define PIN_PA21                                    46
#define PIN_PA22                                    47
#define PIN_PA23                                    53
#define PIN_PA24                                    54
#define PIN_PA25                                    55
#define PIN_PA26                                    59
#define PIN_PA27                                    60
#define PIN_PA28                                    3
#define PIN_PA29                                    4
#define PIN_PA30                                    5
#define PIN_PA31                                    6
#define PIN_PB0                                     12
#define PIN_PB1                                     13
#define PIN_PB2                                     15
#define PIN_PB3                                     16
#define PIN_PB4                                     17
#define PIN_PB5                                     18
#define PIN_PB6                                     23
#define PIN_PB7                                     24
#define PIN_PB8                                     25
#define PIN_PB9                                     26
#define PIN_PB10                                    27
#define PIN_PB11                                    28
#define PIN_PB12                                    29
#define PIN_PB13                                    30
#define PIN_PB14                                    31
#define PIN_PB15                                    32
#define PIN_PB16                                    33
#define PIN_PB17                                    43
#define PIN_PB18                                    44
#define PIN_PB19                                    45
#define PIN_PB20                                    48
#define PIN_PB21                                    49
#define PIN_PB22                                    50
#define PIN_PB23                                    51
#define PIN_PB24                                    52
#define PIN_PB25                                    56
#define PIN_PB26                                    57
#define PIN_PB27                                    58

#define IOMUX_PULL_NONE             0
#define IOMUX_PULL_UP               1
#define IOMUX_PULL_DOWN             2

typedef enum {
    IOMUX_PF_GPIO = 0x1U,
    IOMUX_PF_CLK_OUT = 0x6U
} iomux_pf_t;

typedef struct {
    uint8_t pincm_index;            
    uint8_t pf;                     // Peripheral function select
    uint8_t pull;                   // IOMUX_PULL_NONE / UP / DOWN
    uint8_t drive_strength;         // IOMUX_DRIVE_LOW / HIGH (HS/HD pins only)
    uint8_t input_enable;           // 1 if this pin needs INENA (input or bidirectional)
    uint8_t invert;                 // Logic inversion
    uint8_t open_drain;             // HIZ1
} iomux_pin_config_t;

void iomux_configure_pin(const iomux_pin_config_t *cfg);