#include <stdint.h>
#include "../inc/iomux.h"
#include "../inc/mspm0g350x_startup.h"

/* PINCM bit positions — TRM SLAU846E, Section 8.3.1, Table 8-5 */
#define IOMUX_PINCM_PF_POS      0   /* bits 5:0 */
#define IOMUX_PINCM_PC_POS      7
#define IOMUX_PINCM_PIPD_POS    16
#define IOMUX_PINCM_PIPU_POS    17
#define IOMUX_PINCM_INENA_POS   18
#define IOMUX_PINCM_HYSTEN_POS  19
#define IOMUX_PINCM_DRV_POS     20
#define IOMUX_PINCM_HIZ1_POS    25
#define IOMUX_PINCM_INV_POS     26

void iomux_configure_pin(const iomux_pin_config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    /* Datasheet PINCMx values are 1-based; PINCM[] is 0-based. */
    uint32_t idx = (uint32_t)cfg->pincm_index - 1U;
    uint32_t reg = 0U;

    /* Step 1: flush previous function per TRM 8.2.1 runtime-reconfig sequence
     * (harmless on a pin still at its post-reset default of 0). */
    IOMUX->SECCFG.PINCM[idx] = 0x0U;

    /* Step 2: build the new PINCM word */
    reg |= ((uint32_t)cfg->pf & 0x3FU) << IOMUX_PINCM_PF_POS;
    reg |= (1UL << IOMUX_PINCM_PC_POS);   /* connect selected peripheral */

    if (cfg->input_enable) {
        reg |= (1UL << IOMUX_PINCM_INENA_POS);
    }

    switch (cfg->pull) {
        case IOMUX_PULL_UP:
            reg |= (1UL << IOMUX_PINCM_PIPU_POS);
            break;
        case IOMUX_PULL_DOWN:
            reg |= (1UL << IOMUX_PINCM_PIPD_POS);
            break;
        case IOMUX_PULL_NONE:
        default:
            break;
    }

    if (cfg->drive_strength) {
        reg |= (1UL << IOMUX_PINCM_DRV_POS);
    }
    if (cfg->invert) {
        reg |= (1UL << IOMUX_PINCM_INV_POS);
    }
    if (cfg->open_drain) {
        reg |= (1UL << IOMUX_PINCM_HIZ1_POS);
    }

    /* Step 3: single commit write */
    IOMUX->SECCFG.PINCM[idx] = reg;
}
