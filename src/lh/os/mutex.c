/**
 * @file mutex.c
 * @brief ::lh_os_mutex_t over the native mutex.
 */

#include <lh/os/mutex.h>
#include <lh/os.h>
#include <lh/os/error/code.h>
#include <lh/assert/runtime.h>

/**
 * @brief Report an invalid mutex or lifecycle state in the LH error slot.
 */
static lh_bool_t
lh_os_mutex_invalid(void)
{
    lh_os_set_last_error_lit(lh_os_error_code_invalid_argument, "invalid mutex or state");
    return lh_bool_false;
}

lh_os_system_mutex_handle_t
lh_os_mutex_get_handle(const lh_os_mutex_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handle;
}

lh_void
lh_os_mutex_set_handle(lh_os_mutex_t *self, lh_os_system_mutex_handle_t handle)
{
    lh_assert_runtime_ref(self);
    self->handle = handle;
}

/**
 * @brief Initialize an empty mutex object.
 */
lh_void
lh_os_mutex_init(lh_os_mutex_t *self)
{
    if (self)
    {
        lh_os_mutex_set_handle(self, lh_null);
    }
}

/**
 * @brief Acquire a native mutex for an initialized, empty object.
 */
lh_bool_t
lh_os_mutex_open(lh_os_mutex_t *self)
{
    if (!self || lh_os_mutex_get_handle(self))
    {
        return lh_os_mutex_invalid();
    }
    lh_os_mutex_set_handle(self, lh_os_system_mutex_open());
    return lh_os_mutex_get_handle(self) != lh_null;
}

/**
 * @brief Release an unused native mutex.
 */
lh_bool_t
lh_os_mutex_close(lh_os_mutex_t *self)
{
    if (!self)
    {
        return lh_os_mutex_invalid();
    }
    if (!lh_os_mutex_get_handle(self))
    {
        return lh_bool_true;
    }
    if (!lh_os_system_mutex_close(lh_os_mutex_get_handle(self)))
    {
        return lh_bool_false;
    }
    lh_os_mutex_set_handle(self, lh_null);
    return lh_bool_true;
}

/**
 * @brief Acquire exclusive ownership of an open mutex.
 */
lh_bool_t
lh_os_mutex_lock(lh_os_mutex_t *self)
{
    return self && lh_os_mutex_get_handle(self)
               ? lh_os_system_mutex_lock(lh_os_mutex_get_handle(self))
               : lh_os_mutex_invalid();
}

/**
 * @brief Release a mutex held by the calling thread.
 */
lh_bool_t
lh_os_mutex_unlock(lh_os_mutex_t *self)
{
    return self && lh_os_mutex_get_handle(self)
               ? lh_os_system_mutex_unlock(lh_os_mutex_get_handle(self))
               : lh_os_mutex_invalid();
}
