/**
 * @file condition.c
 * @brief ::lh_os_condition_t over the native condition variable.
 */

#include <lh/os/condition.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/assert/runtime.h>

/**
 * @brief Report an invalid condition variable or lifecycle state in the LH error slot.
 */
static lh_bool_t
lh_os_condition_invalid(void)
{
    lh_os_set_last_error_lit(lh_os_error_code_invalid_argument,
                             "invalid condition variable or state");
    return lh_bool_false;
}

lh_os_system_condition_handle_t
lh_os_condition_get_handle(const lh_os_condition_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_condition_set_handle(lh_os_condition_t *self, lh_os_system_condition_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

/**
 * @brief Initialize an empty condition-variable object.
 */
lh_void
lh_os_condition_init(lh_os_condition_t *self)
{
    if (self)
    {
        lh_os_condition_set_handle(self, lh_null);
    }
}

/**
 * @brief Acquire a native condition variable.
 */
lh_bool_t
lh_os_condition_open(lh_os_condition_t *self)
{
    if (!self || lh_os_condition_get_handle(self))
    {
        return lh_os_condition_invalid();
    }
    lh_os_condition_set_handle(self, lh_os_system_condition_open());
    return lh_os_condition_get_handle(self) != lh_null;
}

/**
 * @brief Release an unused condition variable.
 */
lh_bool_t
lh_os_condition_close(lh_os_condition_t *self)
{
    if (!self)
    {
        return lh_os_condition_invalid();
    }
    if (!lh_os_condition_get_handle(self))
    {
        return lh_bool_true;
    }
    if (!lh_os_system_condition_close(lh_os_condition_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_os_condition_set_handle(self, lh_null);
    return lh_bool_true;
}

/**
 * @brief Atomically release the mutex, wait and reacquire it.
 */
lh_bool_t
lh_os_condition_wait(lh_os_condition_t *self, lh_os_mutex_t *mutex)
{
    return self && lh_os_condition_get_handle(self) && mutex && lh_os_mutex_get_handle(mutex)
               ? lh_os_system_condition_wait(lh_os_condition_get_handle(self),
                                             lh_os_mutex_get_handle(mutex))
               : lh_os_condition_invalid();
}

/**
 * @brief Release the mutex, wait at most @p timeout_ms and reacquire it.
 */
lh_bool_t
lh_os_condition_wait_for(lh_os_condition_t *self, lh_os_mutex_t *mutex, lh_u32_t timeout_ms)
{
    return self && lh_os_condition_get_handle(self) && mutex && lh_os_mutex_get_handle(mutex)
               ? lh_os_system_condition_wait_for(lh_os_condition_get_handle(self),
                                                 lh_os_mutex_get_handle(mutex), timeout_ms)
               : lh_os_condition_invalid();
}

/**
 * @brief Wake one current waiter, if any.
 */
lh_bool_t
lh_os_condition_signal(lh_os_condition_t *self)
{
    return self && lh_os_condition_get_handle(self)
               ? lh_os_system_condition_signal(lh_os_condition_get_handle(self))
               : lh_os_condition_invalid();
}

/**
 * @brief Wake all current waiters.
 */
lh_bool_t
lh_os_condition_broadcast(lh_os_condition_t *self)
{
    return self && lh_os_condition_get_handle(self)
               ? lh_os_system_condition_broadcast(lh_os_condition_get_handle(self))
               : lh_os_condition_invalid();
}
