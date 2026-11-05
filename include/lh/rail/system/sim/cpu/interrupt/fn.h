/**
 * @file fn.h
 * @brief ::lh_rail_system_sim_cpu_interrupt_fn — the interrupt of the CPU model.
 */

#ifndef LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_FN_H
#define LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What runs as "the next interrupt" of the model, with interrupts
 *        unmasked, each time the model waits for one.
 *
 * A test makes it the board's timer interrupt: one call to lh_rail_bare_tick is
 * one millisecond.
 *
 * @param user The value it was installed with.
 */
typedef lh_void(lh_rail_system_sim_cpu_interrupt_fn)(lh_ptr user);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_FN_H */
