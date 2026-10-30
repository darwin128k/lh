/**
 * @file fn.h
 * @brief ::lh_os_loop_wait_fn — half of a loop's waiter.
 */

#ifndef LH_OS_LOOP_WAIT_FN_H
#define LH_OS_LOOP_WAIT_FN_H

#include <lh/ptr.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Sleep until woken, something to do is ready, or @p timeout_ms passed.
 *
 * Runs on the loop's thread. It may return early; the loop then runs its step.
 *
 * @param waiter     The value the waiter was given.
 * @param timeout_ms Longest sleep, in milliseconds.
 */
typedef lh_void(lh_os_loop_wait_fn)(lh_ptr waiter, lh_u32_t timeout_ms);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOOP_WAIT_FN_H */
