/**
 * @file pointer.c
 * @brief Implementation of `lh/ui/view/pointer.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/ui/view.h>
#include <lh/ui/view/pointer.h>
#include <lh/util/return.h>

lh_void
lh_ui_view_pointer_reset(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    self->target = lh_null;
    self->drag = lh_null;
    self->grab = lh_null;
    self->grab_offset = lh_ui_scalar(0);
    self->pressed = lh_bool_false;
    self->dragged = lh_bool_false;
}

lh_void
lh_ui_view_pointer_init(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_reset(self);
    lh_ui_point_init(lh_addr_of(self->press_point), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_point_init(lh_addr_of(self->last_point), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_void
lh_ui_view_pointer_start(lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_reset(self);
    self->pressed = lh_bool_true;
    self->press_point = point;
    self->last_point = point;
    lh_ui_point_init(lh_addr_of(self->velocity), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_bool_t
lh_ui_view_pointer_is_active(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_type_cast(lh_bool_t, self->pressed || lh_null_ne(self->grab));
}

lh_ui_entity_t *
lh_ui_view_pointer_get_target(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->target;
}

lh_void
lh_ui_view_pointer_set_target(lh_ui_view_pointer_t *self, lh_ui_entity_t *target)
{
    lh_assert_runtime_ref(self);
    self->target = target;
}

lh_ui_container_t *
lh_ui_view_pointer_get_drag(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->drag;
}

lh_void
lh_ui_view_pointer_set_drag(lh_ui_view_pointer_t *self, lh_ui_container_t *drag)
{
    lh_assert_runtime_ref(self);
    self->drag = drag;
}

lh_ui_scrollbar_t *
lh_ui_view_pointer_get_grab(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->grab;
}

lh_ui_scalar_t
lh_ui_view_pointer_get_grab_offset(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->grab_offset;
}

lh_void
lh_ui_view_pointer_set_grab(lh_ui_view_pointer_t *self, lh_ui_scrollbar_t *grab,
                            lh_ui_scalar_t offset)
{
    lh_assert_runtime_ref(self);
    self->grab = grab;
    self->grab_offset = offset;
}

lh_bool_t
lh_ui_view_pointer_is_pressed(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_bool_t
lh_ui_view_pointer_is_dragged(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dragged;
}

lh_void
lh_ui_view_pointer_set_dragged(lh_ui_view_pointer_t *self, lh_bool_t dragged)
{
    lh_assert_runtime_ref(self);
    self->dragged = dragged;
}

lh_void
lh_ui_view_pointer_get_press_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = self->press_point;
}

lh_void
lh_ui_view_pointer_get_last_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = self->last_point;
}

lh_void
lh_ui_view_pointer_get_velocity(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = self->velocity;
}

lh_void
lh_ui_view_pointer_track(lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_ui_scalar_t dx;
    lh_ui_scalar_t dy;

    lh_assert_runtime_ref(self);
    dx = lh_ui_point_get_x(lh_addr_of(self->last_point)) - lh_ui_point_get_x(lh_addr_of(point));
    dy = lh_ui_point_get_y(lh_addr_of(self->last_point)) - lh_ui_point_get_y(lh_addr_of(point));
    lh_ui_point_init(lh_addr_of(self->velocity),
                     (lh_ui_point_get_x(lh_addr_of(self->velocity)) + dx) / 2,
                     (lh_ui_point_get_y(lh_addr_of(self->velocity)) + dy) / 2);
    self->last_point = point;
}

lh_bool_t
lh_ui_view_pointer_is_past_threshold(const lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_ui_scalar_t dx;
    lh_ui_scalar_t dy;

    lh_assert_runtime_ref(self);
    dx = lh_ui_point_get_x(lh_addr_of(point)) - lh_ui_point_get_x(lh_addr_of(self->press_point));
    dy = lh_ui_point_get_y(lh_addr_of(point)) - lh_ui_point_get_y(lh_addr_of(self->press_point));
    return lh_type_cast(lh_bool_t, lh_math_abs(dx) >= LH_UI_VIEW_DRAG_THRESHOLD ||
                                       lh_math_abs(dy) >= LH_UI_VIEW_DRAG_THRESHOLD);
}
