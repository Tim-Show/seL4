/*
 * Copyright 2019, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#ifndef __DRIVERS_TIMER_IMX31_GPT_H
#define __DRIVERS_TIMER_IMX31_GPT_H

#include <config.h>

#ifndef CONFIG_KERNEL_MCS
#error "This driver should only be selected for MCS kernel"
#endif /* CONFIG_KERNEL_MCS */

/* gptir and gptsr bits */
#define OF1IE 0 /* output compare 1 */
#define ROV   5 /* roll over */

/* Memory map for GPT (General Purpose Timer). */
struct timer {
    uint32_t gptcr; /* control */
    uint32_t gptpr; /* prescaler */
    uint32_t gptsr; /* status register */
    uint32_t gptir; /* interrupt register */
    uint32_t gptcr1;
    uint32_t gptcr2;
    uint32_t gptcr3;
    uint32_t gpticr1;
    uint32_t gpticr2;
    uint32_t gptcnt;
};
typedef volatile struct timer timer_t;
extern timer_t *gpt;
extern ticks_t high_bits;

static inline ticks_t getCurrentTime(void)
{
    return ((high_bits + !!(gpt->gptsr & BIT(ROV))) << 32llu) + gpt->gptcnt;
}

static inline void setDeadline(ticks_t deadline)
{
    if (((uint32_t) deadline) > gpt->gptcnt) {
        /* turn on compare irq */
        gpt->gptir |= BIT(OF1IE);
        /* set the deadline */
        do {
            gpt->gptcr1 = (uint32_t) deadline;
        } while (gpt->gptcr1 != (uint32_t) deadline);
    }
}

static inline void ackDeadlineIRQ(void)
{
    if (gpt->gptsr & BIT(ROV)) {
        high_bits++;
    }

    /* turn off compare irq */
    gpt->gptir &= ~(BIT(OF1IE));
    /* ack either irq */
    gpt->gptsr |= (BIT(OF1IE) | BIT(ROV));
}

#endif /* !__DRIVERS_TIMER_IMX31_GPT_H */
