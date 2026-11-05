/**
 * @file cb.h
 * @brief ::lh_rail_system_sim_cpu_interrupt_cb — pointer to an
 *        ::lh_rail_system_sim_cpu_interrupt_fn.
 */

#ifndef LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_CB_H
#define LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_CB_H

#include <lh/rail/system/sim/cpu/interrupt/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_rail_system_sim_cpu_interrupt_fn. */
#define lh_rail_system_sim_cpu_interrupt_cb lh_ptr_of(lh_rail_system_sim_cpu_interrupt_fn)

#endif /* LH_RAIL_SYSTEM_SIM_CPU_INTERRUPT_CB_H */
