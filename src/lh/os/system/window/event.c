#include <lh/os/system/window/event.h>
#include <lh/assert.h>

lh_uint_t
lh_os_system_window_event_get_type(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->type;
}

lh_int_t
lh_os_system_window_event_get_x(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->x;
}

lh_int_t
lh_os_system_window_event_get_y(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->y;
}

lh_int_t
lh_os_system_window_event_get_width(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->width;
}

lh_int_t
lh_os_system_window_event_get_height(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->height;
}

lh_int_t
lh_os_system_window_event_get_button(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->button;
}

lh_int_t
lh_os_system_window_event_get_delta(const lh_os_system_window_event_t *self)
{
    lh_assert_runtime_ref(self);
    return self->delta;
}
