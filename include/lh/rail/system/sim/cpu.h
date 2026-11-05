/**
 * @file cpu.h
 * @brief The CPU model behind lh/rail/system/cpu.h where there is no Cortex-M.
 *
 * One core and one interrupt line, in plain C: the mask is a variable, and
 * waiting for an interrupt runs the installed interrupt call at once, unmasked,
 * as a board would on its next timer tick. It lets the base without an OS
 * (lh/rail/bare.h) and the loops on it run in a PC test, in simulated time.
 * Single-threaded: nothing here is safe to call from two threads.
 */

#ifndef LH_RAIL_SYSTEM_SIM_CPU_H
#define LH_RAIL_SYSTEM_SIM_CPU_H

#include <lh/rail/system/cpu.h>
#include <lh/rail/system/sim/cpu/interrupt/cb.h>
#include <lh/core/bool.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Install what runs as the model's interrupt.
 * @param interrupt The call, or ::lh_null for none: then waiting for an
 *                  interrupt returns at once and no time passes.
 * @param user      The value it is given.
 */
lh_void
lh_rail_system_sim_cpu_set_interrupt(lh_rail_system_sim_cpu_interrupt_cb interrupt, lh_ptr user);

/**
 * @brief The model's interrupt call (level 1).
 * @return The call, or ::lh_null.
 */
lh_rail_system_sim_cpu_interrupt_cb
lh_rail_system_sim_cpu_get_interrupt(void);

/**
 * @brief The value the model's interrupt call is given (level 1).
 * @return The value.
 */
lh_ptr
lh_rail_system_sim_cpu_get_interrupt_user(void);

/**
 * @brief The model's interrupt mask (level 1).
 * @return Nonzero while interrupts are masked.
 */
lh_usize_t
lh_rail_system_sim_cpu_get_mask(void);

/**
 * @brief Store the model's interrupt mask (level 1).
 * @param mask Nonzero to mask interrupts.
 */
lh_void
lh_rail_system_sim_cpu_set_mask(lh_usize_t mask);

/**
 * @brief Whether the model's interrupts are masked now.
 * @return ::lh_bool_true inside a section.
 */
lh_bool_t
lh_rail_system_sim_cpu_is_masked(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_SYSTEM_SIM_CPU_H */
