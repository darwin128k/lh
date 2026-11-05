/**
 * @file cpu.h
 * @brief The CPU calls the rail base without an OS stands on: interrupt masking
 *        and waiting for an interrupt.
 *
 * Only the processor's own operations, as they are; one implementation per
 * backend under src/lh/rail/system/<backend>/: `cortexm` (PRIMASK, WFI) on an
 * ARM Cortex-M, `sim` (a model) everywhere else, so the base above it
 * (lh/rail/bare.h) runs in tests on a PC. CMake picks the backend from
 * CMAKE_SYSTEM_PROCESSOR.
 */

#ifndef LH_RAIL_SYSTEM_CPU_H
#define LH_RAIL_SYSTEM_CPU_H

#include <lh/core/size.h>
#include <lh/core/void.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Mask every maskable interrupt and return the mask as it was.
 *
 * A Cortex-M reads PRIMASK, then sets it. A compiler barrier: memory accesses
 * do not move across it.
 *
 * @return The mask before: 0 when interrupts were enabled.
 */
lh_usize_t
lh_rail_system_cpu_mask_interrupts(void);

/**
 * @brief Put the interrupt mask back as ::lh_rail_system_cpu_mask_interrupts
 *        found it.
 *
 * A compiler barrier, like the mask.
 *
 * @param state What the matching mask returned.
 */
lh_void
lh_rail_system_cpu_restore_interrupts(lh_usize_t state);

/**
 * @brief Sleep until an interrupt is pending (a Cortex-M's WFI).
 *
 * Returns on a pending interrupt even while interrupts are masked: the handler
 * runs once the mask is restored. That is what makes a check-then-sleep under
 * the mask free of lost wakes.
 */
lh_void
lh_rail_system_cpu_wait_for_interrupt(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RAIL_SYSTEM_CPU_H */
