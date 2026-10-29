/**
 * @file thread.c
 * @brief ::lh_os_thread_t over the native thread.
 */

#include <lh/os/thread.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/os/system/tick.h>
#include <lh/assert/runtime.h>

/**
 * @brief Report an invalid thread or lifecycle state in the LH error slot.
 */
static lh_bool_t
lh_os_thread_invalid(void)
{
    lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "invalid thread or state");
    return lh_bool_false;
}

lh_os_system_thread_handle_t
lh_os_thread_get_handle(const lh_os_thread_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_thread_set_handle(lh_os_thread_t *self, lh_os_system_thread_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

/**
 * @brief Initialize an empty thread object.
 */
lh_void
lh_os_thread_init(lh_os_thread_t *self)
{
    if (self)
    {
        lh_os_thread_set_handle(self, lh_null);
    }
}

/**
 * @brief Start a native worker in an initialized, empty object.
 */
lh_bool_t
lh_os_thread_start(lh_os_thread_t *self, lh_os_thread_fn run, lh_ptr user, lh_s32_t cpu,
                   lh_u32_t flags)
{
    if (!self || lh_os_thread_get_handle(self))
    {
        return lh_os_thread_invalid();
    }
    lh_os_thread_set_handle(self, lh_os_system_thread_start(run, user, cpu, flags));
    return lh_os_thread_get_handle(self) != lh_null;
}

/**
 * @brief Wait for the worker and release its native resources.
 */
lh_bool_t
lh_os_thread_join(lh_os_thread_t *self)
{
    if (!self)
    {
        return lh_os_thread_invalid();
    }
    if (!lh_os_thread_get_handle(self))
    {
        return lh_bool_true;
    }
    if (!lh_os_system_thread_join(lh_os_thread_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_os_thread_set_handle(self, lh_null);
    return lh_bool_true;
}

/**
 * @brief Suspend the calling thread for a finite interval.
 */
lh_bool_t
lh_os_thread_sleep(lh_u32_t milliseconds)
{
    return lh_os_system_sleep_ms(milliseconds);
}
