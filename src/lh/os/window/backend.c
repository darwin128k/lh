/**
 * @file backend.c
 * @brief Implementation of `lh/os/window/backend.h`.
 */

#include <lh/os/window/backend.h>

#include <lh/core/assert/runtime.h>

lh_os_window_backend_open_fn *
lh_os_window_backend_get_open(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->open;
}

lh_void
lh_os_window_backend_set_open(lh_os_window_backend_t *self, lh_os_window_backend_open_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->open = fn;
}

lh_os_window_backend_close_fn *
lh_os_window_backend_get_close(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close;
}

lh_void
lh_os_window_backend_set_close(lh_os_window_backend_t *self, lh_os_window_backend_close_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->close = fn;
}

lh_os_window_backend_invalidate_fn *
lh_os_window_backend_get_invalidate(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->invalidate;
}

lh_void
lh_os_window_backend_set_invalidate(lh_os_window_backend_t *self,
                                    lh_os_window_backend_invalidate_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->invalidate = fn;
}

lh_os_window_backend_invalidate_rect_fn *
lh_os_window_backend_get_invalidate_rect(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->invalidate_rect;
}

lh_void
lh_os_window_backend_set_invalidate_rect(lh_os_window_backend_t *self,
                                         lh_os_window_backend_invalidate_rect_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->invalidate_rect = fn;
}

lh_timer_clock_fn *
lh_os_window_backend_get_now(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->now;
}

lh_void
lh_os_window_backend_set_now(lh_os_window_backend_t *self, lh_timer_clock_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->now = fn;
}

lh_os_window_backend_set_timer_fn *
lh_os_window_backend_get_set_timer(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_timer;
}

lh_void
lh_os_window_backend_set_set_timer(lh_os_window_backend_t *self,
                                   lh_os_window_backend_set_timer_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->set_timer = fn;
}

lh_os_window_backend_set_enabled_fn *
lh_os_window_backend_get_set_enabled(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_enabled;
}

lh_void
lh_os_window_backend_set_set_enabled(lh_os_window_backend_t *self,
                                     lh_os_window_backend_set_enabled_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->set_enabled = fn;
}

lh_os_window_backend_minimize_fn *
lh_os_window_backend_get_minimize(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->minimize;
}

lh_void
lh_os_window_backend_set_minimize(lh_os_window_backend_t *self,
                                  lh_os_window_backend_minimize_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->minimize = fn;
}

lh_os_window_backend_set_maximized_fn *
lh_os_window_backend_get_set_maximized(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_maximized;
}

lh_void
lh_os_window_backend_set_set_maximized(lh_os_window_backend_t *self,
                                       lh_os_window_backend_set_maximized_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->set_maximized = fn;
}

lh_os_window_backend_set_corner_radius_fn *
lh_os_window_backend_get_set_corner_radius(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->set_corner_radius;
}

lh_void
lh_os_window_backend_set_set_corner_radius(lh_os_window_backend_t *self,
                                           lh_os_window_backend_set_corner_radius_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->set_corner_radius = fn;
}

lh_os_window_backend_get_client_size_fn *
lh_os_window_backend_get_get_client_size(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_client_size;
}

lh_void
lh_os_window_backend_set_get_client_size(lh_os_window_backend_t *self,
                                         lh_os_window_backend_get_client_size_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_client_size = fn;
}

lh_os_window_backend_get_position_fn *
lh_os_window_backend_get_get_position(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_position;
}

lh_void
lh_os_window_backend_set_get_position(lh_os_window_backend_t *self,
                                      lh_os_window_backend_get_position_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_position = fn;
}

lh_os_window_backend_get_paint_region_fn *
lh_os_window_backend_get_get_paint_region(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->get_paint_region;
}

lh_void
lh_os_window_backend_set_get_paint_region(lh_os_window_backend_t *self,
                                          lh_os_window_backend_get_paint_region_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->get_paint_region = fn;
}

lh_os_window_backend_now_us_fn *
lh_os_window_backend_get_now_us(const lh_os_window_backend_t *self)
{
    lh_assert_runtime_ref(self);
    return self->now_us;
}

lh_void
lh_os_window_backend_set_now_us(lh_os_window_backend_t *self, lh_os_window_backend_now_us_fn *fn)
{
    lh_assert_runtime_ref(self);
    self->now_us = fn;
}

lh_void
lh_os_window_backend_init(lh_os_window_backend_t *self)
{
    lh_os_window_backend_set_open(self, lh_null);
    lh_os_window_backend_set_close(self, lh_null);
    lh_os_window_backend_set_invalidate(self, lh_null);
    lh_os_window_backend_set_invalidate_rect(self, lh_null);
    lh_os_window_backend_set_now(self, lh_null);
    lh_os_window_backend_set_set_timer(self, lh_null);
    lh_os_window_backend_set_set_enabled(self, lh_null);
    lh_os_window_backend_set_minimize(self, lh_null);
    lh_os_window_backend_set_set_maximized(self, lh_null);
    lh_os_window_backend_set_set_corner_radius(self, lh_null);
    lh_os_window_backend_set_get_client_size(self, lh_null);
    lh_os_window_backend_set_get_position(self, lh_null);
    lh_os_window_backend_set_get_paint_region(self, lh_null);
    lh_os_window_backend_set_now_us(self, lh_null);
}

lh_bool_t
lh_os_window_backend_is_complete(const lh_os_window_backend_t *self)
{
    return lh_cast_static(
        lh_bool_t,
        lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_backend_get_open(self))) &&
            lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_backend_get_close(self))) &&
            lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_backend_get_invalidate(self))) &&
            lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_backend_get_invalidate_rect(self))) &&
            lh_null_ne(lh_ptr_rcast(lh_void, lh_os_window_backend_get_now(self))));
}
