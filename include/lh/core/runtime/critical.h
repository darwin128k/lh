/**
 * @file critical.h
 * @brief The critical section slot: the one ::lh_critical_t the core enters.
 *
 * The slot starts empty, like the other runtime slots: the core assumes
 * nothing comes between — one thread, no interrupt handler on shared data —
 * until the host says otherwise. A host whose atomics are built on it
 * (::LH_LIBRARY_OPTION_ATOMIC = ::LH_ATOMIC_CRITICAL) fills it before it starts
 * a second thread or enables an interrupt that touches shared data: a
 * Cortex-M host with a PRIMASK save-and-mask, a host with threads with one
 * process-wide lock.
 */

#ifndef LH_RUNTIME_CRITICAL_H
#define LH_RUNTIME_CRITICAL_H

#include <lh/core/critical.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The critical section slot, for filling.
 *
 * Fill it once at start, while nothing else runs; it is not guarded against a
 * change while a section is open.
 *
 * @return The slot; never ::lh_null.
 */
lh_critical_t *
lh_runtime_critical_get_mut(void);

/**
 * @brief The critical section slot.
 * @return The slot; never ::lh_null.
 */
const lh_critical_t *
lh_runtime_critical_get(void);

/**
 * @brief Begin a section with the slot (::lh_critical_enter).
 * @return The token for ::lh_runtime_critical_leave.
 */
lh_usize_t
lh_runtime_critical_enter(void);

/**
 * @brief End a section begun with ::lh_runtime_critical_enter.
 * @param state The token it returned.
 */
lh_void
lh_runtime_critical_leave(lh_usize_t state);

LH_COMPILER_EXTERN_C_END

#endif /* LH_RUNTIME_CRITICAL_H */
