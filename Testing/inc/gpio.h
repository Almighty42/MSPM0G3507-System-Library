/**
 * @file    gpio_driver.h
 * @brief   Bare-metal GPIO output driver for TI MSPM0G3507 (Cortex-M0+).
 *
 * @warning REGISTER VERIFICATION STATUS
 *          - GPIO peripheral register OFFSETS (PWREN=0x800, RSTCTL=0x804,
 *            DOUT31_0=0x1280, DOUTSET31_0=0x1290, DOUTCLR31_0=0x12A0,
 *            DOUTTGL31_0=0x12B0, DOE31_0=0x12C0, DOESET31_0=0x12D0,
 *            DOECLR31_0=0x12E0) are CONFIRMED against SLAU846D Ch.9
 *            (GPIO), Table 9-2.
 *          - PWREN key (0x26) and RSTCTL key (0xB1) are CONFIRMED against
 *            SLAU846D Ch.9, Table 9-13 / Table 9-15.
 *          - IOMUX PINCM bit layout (PC=bit6, INENA=bit18, PF=bits[3:0])
 *            is CONFIRMED against SLAU846D Ch.8, Table 8-6.
 *          - GPIOA/GPIOB PERIPHERAL BASE ADDRESSES and the PINCMx index
 *            for each physical pin are DEVICE-MEMORY-MAP specific and are
 *            NOT contained in the IOMUX/GPIO chapters. These are marked
 *            TODO below and MUST be verified against the MSPM0G3507
 *            device-specific datasheet / memory map chapter (and cross
 *            checked with the CMSIS mspm0g3507.h header) before this is
 *            trusted on hardware.
 *          - The peripheral function ID (PF value) to select "GPIO" on a
 *            given pin is also datasheet/pinmux-table specific, not an
 *            IOMUX-chapter constant. Marked TODO.
 *
 * Do not flash this until the TODO items are resolved against primary
 * sources. Treat all TODO-marked values as placeholders, not verified
 * register content.
 */

#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------
 * TODO: VERIFY against device memory map / CMSIS header before use.
 * These base addresses are placeholders and are NOT sourced from the
 * IOMUX (Ch.8) or GPIO (Ch.9) chapters, which only give register OFFSETS
 * relative to a peripheral base, not the base addresses themselves.
 * ------------------------------------------------------------------------ */
#define GPIOA_BASE          (0x400A0000UL)   /* TODO: verify vs datasheet memory map */
#define GPIOB_BASE          (0x400A2000UL)   /* TODO: verify vs datasheet memory map */
#define IOMUX_BASE          (0x40428000UL)   /* TODO: verify vs datasheet memory map */

/* ------------------------------------------------------------------------
 * GPIO register offsets — CONFIRMED, SLAU846D Table 9-2.
 * ------------------------------------------------------------------------ */
#define GPIO_O_PWREN         (0x0800UL)
#define GPIO_O_RSTCTL        (0x0804UL)
#define GPIO_O_STAT          (0x0814UL)
#define GPIO_O_DOUT31_0      (0x1280UL)
#define GPIO_O_DOUTSET31_0   (0x1290UL)
#define GPIO_O_DOUTCLR31_0   (0x12A0UL)
#define GPIO_O_DOUTTGL31_0   (0x12B0UL)
#define GPIO_O_DOE31_0       (0x12C0UL)
#define GPIO_O_DOESET31_0    (0x12D0UL)
#define GPIO_O_DOECLR31_0    (0x12E0UL)
#define GPIO_O_DIN31_0       (0x1380UL)

/* PWREN/RSTCTL keys — CONFIRMED, SLAU846D Table 9-13 / Table 9-15. */
#define GPIO_PWREN_KEY       (0x26UL << 24)
#define GPIO_PWREN_ENABLE    (1UL << 0)
#define GPIO_RSTCTL_KEY      (0xB1UL << 24)
#define GPIO_RSTCTL_ASSERT   (1UL << 0)

/* ------------------------------------------------------------------------
 * IOMUX PINCM bit fields — CONFIRMED, SLAU846D Table 8-6.
 * ------------------------------------------------------------------------ */
#define PINCM_WCOMP_BIT      (28)
#define PINCM_WUEN_BIT       (27)
#define PINCM_INV_BIT        (26)
#define PINCM_HIZ1_BIT       (25)
#define PINCM_DRV_BIT        (20)
#define PINCM_HYSTEN_BIT     (19)
#define PINCM_INENA_BIT      (18)
#define PINCM_PIPU_BIT       (17)
#define PINCM_PIPD_BIT       (16)
#define PINCM_WAKESTAT_BIT   (13)
#define PINCM_PC_BIT         (7)
#define PINCM_PF_MASK        (0x3FUL)

/* IOMUX register block: offset 0x4 + (4 * PINCM index) per pin.
 * TODO: verify the PINCMx index for each physical pin (PA0, PA1, ...)
 * against the device-specific datasheet pinmux table. Do not assume
 * index == pin number. */
#define IOMUX_PINCM(idx)     (*(volatile uint32_t *)(IOMUX_BASE + 0x4UL + (4UL * (idx))))

/* TODO: verify actual PF value for "GPIO" function per pin from the
 * device datasheet pinmux table — this is NOT a fixed IOMUX-chapter
 * constant, it varies per pin. Placeholder only. */
#define PINCM_PF_GPIO_PLACEHOLDER   (0x1UL)

/**
 * @brief Simple GPIO port handle, one per GPIOA/GPIOB instance.
 */
typedef struct {
    volatile uint32_t *base;   /**< Peripheral base address (GPIOA_BASE / GPIOB_BASE) */
} gpio_port_t;

extern const gpio_port_t GPIOA;
extern const gpio_port_t GPIOB;

/**
 * @brief Power up and reset the GPIO peripheral (mandatory keyed sequence).
 *
 * Performs the RSTCTL assert (key 0xB1) followed by PWREN enable
 * (key 0x26), per SLAU846D Table 9-13 / Table 9-15.
 *
 * @param port  Pointer to gpio_port_t (GPIOA or GPIOB).
 */
void gpio_port_init(const gpio_port_t *port);

/**
 * @brief Configure a single pin as a digital output via IOMUX + GPIO DOE.
 *
 * @param port      Pointer to gpio_port_t (GPIOA or GPIOB).
 * @param pincm_idx TODO-verify: IOMUX PINCM index for the target pin.
 * @param pin_bit   Bit position within DOUT/DOE (0-31) for the target pin.
 */
void gpio_output_config(const gpio_port_t *port, uint32_t pincm_idx, uint32_t pin_bit);

/** @brief Set (drive high) a single output pin, no read-modify-write. */
void gpio_set_pin(const gpio_port_t *port, uint32_t pin_bit);

/** @brief Clear (drive low) a single output pin, no read-modify-write. */
void gpio_clear_pin(const gpio_port_t *port, uint32_t pin_bit);

/** @brief Toggle a single output pin, no read-modify-write. */
void gpio_toggle_pin(const gpio_port_t *port, uint32_t pin_bit);

/** @brief Write a pin high/low using the SET/CLR registers. */
void gpio_write_pin(const gpio_port_t *port, uint32_t pin_bit, bool level);

#ifdef __cplusplus
}
#endif

#endif /* GPIO_DRIVER_H */
