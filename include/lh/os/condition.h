/** @file condition.h @brief OS condition primitive, independent of game hosts.
 * Initialize before use. Live objects must not be copied or reinitialized.
 * Calls report native failures through lh_os_system_last_error, contract errors
 * through lh_os_last_error. Only lock/wait/signal operations may run concurrently;
 * opening/closing/joining require external lifetime coordination.
 */
#ifndef LH_OS_CONDITION_H
#define LH_OS_CONDITION_H
#include <lh/os/system/concurrency.h>
#include <lh/os/condition/fields.h>
#include <lh/os/mutex.h>
struct lh_os_condition
{
    lh_os_condition_fields(lh_os_system_condition_handle_t);
};
typedef struct lh_os_condition lh_os_condition_t;
LH_COMPILER_EXTERN_C_BEGIN
/** Initialize an empty object; no native resource is acquired. */
lh_void
lh_os_condition_init(lh_os_condition_t *self);
lh_bool_t
lh_os_condition_open(lh_os_condition_t *self);
/** Close only after all waiters have returned and no thread can use it again. */
lh_bool_t
lh_os_condition_close(lh_os_condition_t *self);
/** Caller holds mutex exactly once. Atomically release/wait/reacquire.
 * Always recheck the predicate in a loop; spurious wakeups are permitted.
 * Concurrent waiters must use the same mutex. Windows requires Vista+ at runtime. */
lh_bool_t
lh_os_condition_wait(lh_os_condition_t *self, lh_os_mutex_t *mutex);
lh_bool_t
lh_os_condition_signal(lh_os_condition_t *self);
lh_bool_t
lh_os_condition_broadcast(lh_os_condition_t *self);

LH_COMPILER_EXTERN_C_END
#endif
