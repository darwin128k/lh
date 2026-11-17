/**
 * @file backend.c
 * @brief Implementation of `lh/os/app/backend.h`.
 */

#include <lh/os/app/backend.h>

#include <lh/core/assert/runtime.h>

lh_timer_clock_fn *
lh_os_app_backend_get_now(const lh_os_app_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->now;
}

lh_void
lh_os_app_backend_set_now(lh_os_app_backend_t *self, lh_timer_clock_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->now = fn;
}

lh_os_app_backend_wait_fn *
lh_os_app_backend_get_wait(const lh_os_app_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wait;
}

lh_void
lh_os_app_backend_set_wait(lh_os_app_backend_t *self, lh_os_app_backend_wait_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->wait = fn;
}

lh_os_app_backend_wake_fn *
lh_os_app_backend_get_wake(const lh_os_app_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wake;
}

lh_void
lh_os_app_backend_set_wake(lh_os_app_backend_t *self, lh_os_app_backend_wake_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->wake = fn;
}

lh_void
lh_os_app_backend_init(lh_os_app_backend_t *self)
{
    lh_os_app_backend_set_now(self, lh_null);
    lh_os_app_backend_set_wait(self, lh_null);
    lh_os_app_backend_set_wake(self, lh_null);
}

lh_bool_t
lh_os_app_backend_is_complete(const lh_os_app_backend_t *self)
{
    return lh_cast_static(lh_bool_t,
                          lh_null_ne(lh_ptr_rcast(lh_void, lh_os_app_backend_get_now(self))) &&
                              lh_null_ne(lh_ptr_rcast(lh_void, lh_os_app_backend_get_wait(self))) &&
                              lh_null_ne(lh_ptr_rcast(lh_void, lh_os_app_backend_get_wake(self))));
}
