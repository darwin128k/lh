/**
 * @file fn.h
 * @brief ::lh_exec_loop_wake_fn — half of a loop's waiter.
 */

#ifndef LH_EXEC_LOOP_WAKE_FN_H
#define LH_EXEC_LOOP_WAKE_FN_H

#include <lh/core/ptr.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Make a running or a coming ::lh_exec_loop_wait_fn return.
 *
 * Any thread calls it, without waiting. The wake must stay pending until the
 * waiter consumes it, so one that comes before the wait is not lost (a
 * `lh/os/wake.h` signal does that).
 *
 * @param waiter The value the waiter was given.
 */
typedef lh_void(lh_exec_loop_wake_fn)(lh_ptr waiter);

LH_COMPILER_EXTERN_C_END

#endif /* LH_EXEC_LOOP_WAKE_FN_H */
