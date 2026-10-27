/** @file thread.h @brief OS thread primitive, independent of game hosts.
 * Initialize before use. Live objects must not be copied or reinitialized.
 * Calls report native failures through lh_os_system_last_error, contract errors
 * through lh_os_last_error. Only lock/wait/signal operations may run concurrently;
 * opening/closing/joining require external lifetime coordination.
 */
#ifndef LH_OS_THREAD_H
#define LH_OS_THREAD_H
#include <lh/os/system/concurrency.h>
#include <lh/os/thread/fields.h>

struct lh_os_thread
{
    lh_os_thread_fields(lh_os_system_thread_handle_t);
};
typedef struct lh_os_thread lh_os_thread_t;
LH_COMPILER_EXTERN_C_BEGIN
/** Initialize an empty object; no native resource is acquired. */
lh_void
lh_os_thread_init(lh_os_thread_t *self);
/** Start once; cpu=-1 inherits affinity, flags=0 inherits the signal mask.
 * On failure no user callback executes. A successful callback may run before
 * start returns. SIGPIPE blocking applies to POSIX only. Affinity is supported
 * on Linux/Windows; an unavailable affinity request fails before callback.
 * Join on another thread, then reuse or discard the initialized object.
 * No cancellation, detachment or module ownership is imposed here. */
lh_bool_t
lh_os_thread_start(lh_os_thread_t *self, lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu,
                   lh_u32_t flags);
/** Wait and release; idempotent on a closed initialized object. Failure retains ownership. */
lh_bool_t
lh_os_thread_join(lh_os_thread_t *self);
/** Sleep at least the requested interval unless the OS reports failure. Retries POSIX EINTR. */
lh_bool_t
lh_os_thread_sleep(lh_u32_t milliseconds);

LH_COMPILER_EXTERN_C_END
#endif
