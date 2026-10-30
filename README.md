# MSPM0G3507-System-Library

[![Target](https://img.shields.io/badge/target-MSPM0G3507-blue)](https://www.ti.com/product/MSPM0G3507)

Bare-metal C drivers for the TI MSPM0G3507 (Arm Cortex-M0+), written at register level. Includes a custom linker script, startup code and an 80 MHz clock setup, verified on a logic analyzer.

## Drivers

| Driver | Purpose | Example | Status |
|---|---|---|---|
| Startup + linker script | Vector table, reset handler, memory layout | n/a | Done |
| Clock | 80 MHz HFCLK, output on CLK_OUT (PA22) | `main.c` | Done, verified on logic analyzer |
| GPIO + IOMUX | Pin configuration and I/O | `main.c` | Done |
| SysTick | Core tick timer | `main.c` | Done |
| SPI | Master transfers, polling | [`examples_spi.c`](examples/examples_spi.c) | Done |
| I2C | Master transfers, polling | [`examples_i2c.c`](examples/examples_i2c.c) | Done |
| UART | Serial communication | TBD | TBD |
| PWM, ADC | Timer output, analog input | TBD | TBD |

## Quick start

```sh
git clone https://github.com/Almighty42/MSPM0G3507-System-Library.git
cd MSPM0G3507-System-Library
cmake -B build
cmake --build build
```

Output: `build/mspm0g3507_app.elf`, plus `.hex` and `.bin` files.

Requirements: CMake 3.20 or newer, GNU Arm Embedded toolchain (`arm-none-eabi-gcc`) and OpenOCD.

Flash and debug with the included `openocd.cfg`:

```sh
openocd -f openocd.cfg -c "program build/mspm0g3507_app.elf verify reset exit"
```

## Examples

Select which example runs in `src/main.c`. The SPI and I2C examples live in `examples/`.

## Project structure

```
├── inc/ # Public headers, one per driver
├── src/ # Driver sources, startup, system init, main
├── examples/ # SPI and I2C example programs
├── mspm0g3507.ld # Linker script
├── toolchain-arm-none-eabi.cmake
├── openocd.cfg
└── CMakeLists.txt
```

## Limitations

Polling only, no interrupt-driven transfers yet. Tested on the MSPM0G3507 LaunchPad. Not tested on other MSPM0 parts. No unit tests or CI yet.
