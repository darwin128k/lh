/**
 * @file fn.h
 * @brief ::lh_critical_enter_fn — begin a critical section.
 */

#ifndef LH_CRITICAL_ENTER_FN_H
#define LH_CRITICAL_ENTER_FN_H

#include <lh/core/size.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Shut out every other thread and interrupt handler that touches what the
 *        section guards, until the matching leave.
 *
 * How is the platform's own: a single core masks its interrupts (a Cortex-M
 * sets PRIMASK), a host with threads takes one lock. Entering is a full memory
 * barrier: everything written before the last leave is seen inside.
 *
 * Sections may nest. Each enter returns a token that its own leave takes back,
 * in reverse order: a nested leave must not unmask what the outer enter masked.
 *
 * @return The state the matching leave restores (a Cortex-M's PRIMASK before
 *         the enter); 0 when the platform needs none.
 */
typedef lh_usize_t(lh_critical_enter_fn)(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CRITICAL_ENTER_FN_H */
