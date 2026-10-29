/**
 * @file pointer.c
 * @brief Implementation of `lh/ui/view/pointer.h`.
 */
#include <lh/assert.h>

#include <lh/math.h>
#include <lh/ui/view.h>
#include <lh/ui/view/pointer.h>
#include <lh/util/return.h>

lh_void
lh_ui_view_pointer_reset(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_set_target(self, lh_null);
    lh_ui_view_pointer_set_drag(self, lh_null);
    lh_ui_view_pointer_set_grab(self, lh_null);
    lh_ui_view_pointer_set_grab_offset(self, lh_ui_scalar(0));
    lh_ui_view_pointer_set_pressed(self, lh_bool_false);
    lh_ui_view_pointer_set_dragged(self, lh_bool_false);
}

lh_void
lh_ui_view_pointer_init(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_reset(self);
    lh_ui_point_init(lh_ui_view_pointer_get_press_point_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_point_init(lh_ui_view_pointer_get_last_point_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_ui_point_init(lh_ui_view_pointer_get_velocity_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_void
lh_ui_view_pointer_start(lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_reset(self);
    lh_ui_view_pointer_set_pressed(self, lh_bool_true);
    lh_ui_view_pointer_set_press_point(self, point);
    lh_ui_view_pointer_set_last_point(self, point);
    lh_ui_point_init(lh_ui_view_pointer_get_velocity_mut(self), lh_ui_scalar(0), lh_ui_scalar(0));
}

lh_bool_t
lh_ui_view_pointer_is_active(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_type_cast(lh_bool_t, lh_ui_view_pointer_is_pressed(self) || lh_null_ne(lh_ui_view_pointer_get_grab(self)));
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
lh_ui_view_pointer_grab(lh_ui_view_pointer_t *self, lh_ui_scrollbar_t *grab,
                            lh_ui_scalar_t offset)
{
    lh_assert_runtime_ref(self);
    lh_ui_view_pointer_set_grab(self, grab);
    lh_ui_view_pointer_set_grab_offset(self, offset);
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
lh_ui_view_pointer_copy_press_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_view_pointer_get_press_point(self));
}

lh_void
lh_ui_view_pointer_copy_last_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_view_pointer_get_last_point(self));
}

lh_void
lh_ui_view_pointer_copy_velocity(const lh_ui_view_pointer_t *self, lh_ui_point_t *out)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_ui_view_pointer_get_velocity(self));
}

lh_void
lh_ui_view_pointer_track(lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_ui_scalar_t dx;
    lh_ui_scalar_t dy;

    lh_assert_runtime_ref(self);
    dx = lh_ui_point_get_x(lh_ui_view_pointer_get_last_point_mut(self)) - lh_ui_point_get_x(lh_addr_of(point));
    dy = lh_ui_point_get_y(lh_ui_view_pointer_get_last_point_mut(self)) - lh_ui_point_get_y(lh_addr_of(point));
    lh_ui_point_init(lh_ui_view_pointer_get_velocity_mut(self),
                     (lh_ui_point_get_x(lh_ui_view_pointer_get_velocity_mut(self)) + dx) / 2,
                     (lh_ui_point_get_y(lh_ui_view_pointer_get_velocity_mut(self)) + dy) / 2);
    lh_ui_view_pointer_set_last_point(self, point);
}

lh_bool_t
lh_ui_view_pointer_is_past_threshold(const lh_ui_view_pointer_t *self, lh_ui_point_t point)
{
    lh_ui_scalar_t dx;
    lh_ui_scalar_t dy;

    lh_assert_runtime_ref(self);
    dx = lh_ui_point_get_x(lh_addr_of(point)) - lh_ui_point_get_x(lh_ui_view_pointer_get_press_point(self));
    dy = lh_ui_point_get_y(lh_addr_of(point)) - lh_ui_point_get_y(lh_ui_view_pointer_get_press_point(self));
    return lh_type_cast(lh_bool_t, lh_math_abs(dx) >= LH_UI_VIEW_DRAG_THRESHOLD ||
                                       lh_math_abs(dy) >= LH_UI_VIEW_DRAG_THRESHOLD);
}

lh_void
lh_ui_view_pointer_set_grab_offset(lh_ui_view_pointer_t *self, lh_ui_scalar_t grab_offset)
{
    lh_assert_runtime_ref(self);
    self->grab_offset = grab_offset;
}

lh_ui_point_t *
lh_ui_view_pointer_get_press_point_mut(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->press_point);
}

lh_void
lh_ui_view_pointer_set_press_point(lh_ui_view_pointer_t *self, lh_ui_point_t press_point)
{
    lh_assert_runtime_ref(self);
    self->press_point = press_point;
}

lh_ui_point_t *
lh_ui_view_pointer_get_last_point_mut(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->last_point);
}

lh_void
lh_ui_view_pointer_set_last_point(lh_ui_view_pointer_t *self, lh_ui_point_t last_point)
{
    lh_assert_runtime_ref(self);
    self->last_point = last_point;
}

lh_ui_point_t *
lh_ui_view_pointer_get_velocity_mut(lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->velocity);
}

lh_void
lh_ui_view_pointer_set_velocity(lh_ui_view_pointer_t *self, lh_ui_point_t velocity)
{
    lh_assert_runtime_ref(self);
    self->velocity = velocity;
}

lh_void
lh_ui_view_pointer_set_pressed(lh_ui_view_pointer_t *self, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

lh_void
lh_ui_view_pointer_set_grab(lh_ui_view_pointer_t *self, lh_ui_scrollbar_t * grab)
{
    lh_assert_runtime_ref(self);
    self->grab = grab;
}

const lh_ui_point_t *
lh_ui_view_pointer_get_press_point(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->press_point);
}

const lh_ui_point_t *
lh_ui_view_pointer_get_last_point(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->last_point);
}

const lh_ui_point_t *
lh_ui_view_pointer_get_velocity(const lh_ui_view_pointer_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->velocity);
}
