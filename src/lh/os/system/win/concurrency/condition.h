/**
 * @file condition.h
 * @brief Backend-private: one Win32 condition variable and the API it was opened with.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_CONDITION_H
#define LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_CONDITION_H

#include <windows.h>
#include <lh/compiler/extern/c.h>
#include <lh/void.h>

/* CONDITION_VARIABLE is a pointer-sized opaque state. Resolve its API at open,
 * so other LH OS services can still load on systems lacking condition variables.
 * All calls use the Win32 calling convention, including on 32-bit Windows. */
typedef VOID(WINAPI *lh_os_condition_change_fn)(PVOID state);
typedef BOOL(WINAPI *lh_os_condition_wait_fn)(PVOID state, PCRITICAL_SECTION mutex, DWORD timeout);

typedef struct lh_os_native_condition
{
    PVOID state;
    lh_os_condition_change_fn signal, broadcast;
    lh_os_condition_wait_fn wait;
} lh_os_native_condition_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The opaque CONDITION_VARIABLE state (level 1).
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 */
const PVOID *
lh_os_native_condition_get_state(const lh_os_native_condition_t *self);

/**
 * @brief The opaque CONDITION_VARIABLE state, to change in place (level 1).
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
PVOID *
lh_os_native_condition_get_state_mut(lh_os_native_condition_t *self);

/**
 * @brief WakeConditionVariable, resolved at open (level 1).
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 */
lh_os_condition_change_fn
lh_os_native_condition_get_signal_fn(const lh_os_native_condition_t *self);

/**
 * @brief Store WakeConditionVariable, resolved at open (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 * @param signal The new value.
 */
lh_void
lh_os_native_condition_set_signal_fn(lh_os_native_condition_t *self,
                                     lh_os_condition_change_fn signal);

/**
 * @brief WakeAllConditionVariable, resolved at open (level 1).
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 */
lh_os_condition_change_fn
lh_os_native_condition_get_broadcast_fn(const lh_os_native_condition_t *self);

/**
 * @brief Store WakeAllConditionVariable, resolved at open (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 * @param broadcast The new value.
 */
lh_void
lh_os_native_condition_set_broadcast_fn(lh_os_native_condition_t *self,
                                        lh_os_condition_change_fn broadcast);

/**
 * @brief SleepConditionVariableCS, resolved at open (level 1).
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 */
lh_os_condition_wait_fn
lh_os_native_condition_get_wait_fn(const lh_os_native_condition_t *self);

/**
 * @brief Store SleepConditionVariableCS, resolved at open (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The condition; checked by ::lh_assert_runtime_ref.
 * @param wait The new value.
 */
lh_void
lh_os_native_condition_set_wait_fn(lh_os_native_condition_t *self, lh_os_condition_wait_fn wait);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_CONCURRENCY_CONDITION_H */
