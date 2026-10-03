/**
 * @file window.c
 * @brief Application-level wrapper around `lh/os/system/window.h`.
 *
 * A `lh_os_window_t` is a one-field value handle around the platform
 * bit-pattern window handle. The methods delegate to the system layer
 * (Win32 in `src/lh/os/system/win/window.c`) without adding any state —
 * paint bookkeeping and input queues belong to the caller, layered above
 * this header.
 */

#include <lh/bool.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os/window.h>
#include <lh/math.h>

void
lh_os_window_init(lh_os_window_t *self)
{
    if (lh_null_eq(self))
    {
        return;
    }
    self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
}

lh_bool_t
lh_os_window_open(lh_os_window_t *self, const lh_ptr title, lh_int_t width, lh_int_t height)
{
    if (lh_null_eq(self))
    {
        return lh_bool_false;
    }
    self->handle = lh_os_system_window_open(title, width, height);
    return lh_os_system_window_is_valid(self->handle);
}

void
lh_os_window_close(lh_os_window_t *self)
{
    if (lh_null_eq(self))
    {
        return;
    }
    if (!lh_math_eq(self->handle, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID))
    {
        lh_os_system_window_close(self->handle);
        self->handle = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }
}

lh_os_system_window_handle_t
lh_os_window_get_handle(const lh_os_window_t *self)
{
    if (lh_null_eq(self))
    {
        return LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    }
    return self->handle;
}

lh_bool_t
lh_os_window_is_valid(const lh_os_window_t *self)
{
    if (lh_null_eq(self))
    {
        return lh_bool_false;
    }
    return lh_os_system_window_is_valid(self->handle);
}

void
lh_os_window_show(lh_os_window_t *self)
{
    lh_os_system_window_show(self->handle);
}

lh_bool_t
lh_os_window_pump_messages(void)
{
    return lh_os_system_window_pump_messages();
}

void
lh_os_window_wait_messages(void)
{
    lh_os_system_window_wait_messages();
}

void
lh_os_window_set_handler(lh_os_system_window_handler_cb handler, lh_self_ptr self)
{
    lh_os_system_window_set_handler(handler, self);
}

lh_bool_t
lh_os_window_present(lh_os_window_t *self, const lh_ptr pixels, lh_int_t stride, lh_int_t x,
                     lh_int_t y, lh_int_t width, lh_int_t height)
{
    return lh_os_system_window_present(lh_os_window_get_handle(self), pixels, stride, x, y, width,
                                       height);
}
