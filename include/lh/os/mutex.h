/** @file mutex.h @brief OS mutex primitive, independent of game hosts.
 * Initialize before use. Live objects must not be copied or reinitialized.
 * Calls report native failures through lh_os_system_last_error, contract errors
 * through lh_os_last_error. Only lock/wait/signal operations may run concurrently;
 * opening/closing/joining require external lifetime coordination.
 */
#ifndef LH_OS_MUTEX_H
#define LH_OS_MUTEX_H
#include <lh/os/system/concurrency.h>
#include <lh/os/mutex/fields.h>

struct lh_os_mutex
{
    lh_os_mutex_fields(lh_os_system_mutex_handle_t);
};
typedef struct lh_os_mutex lh_os_mutex_t;
LH_COMPILER_EXTERN_C_BEGIN
/** Initialize an empty object; no native resource is acquired. */
lh_void
lh_os_mutex_init(lh_os_mutex_t *self);
lh_bool_t
lh_os_mutex_open(lh_os_mutex_t *self);
/** Close only when unlocked and no thread can use it again. */
lh_bool_t
lh_os_mutex_close(lh_os_mutex_t *self);
/** Do not lock recursively; unlock on the same thread. */
lh_bool_t
lh_os_mutex_lock(lh_os_mutex_t *self);
lh_bool_t
lh_os_mutex_unlock(lh_os_mutex_t *self);

LH_COMPILER_EXTERN_C_END
#endif
