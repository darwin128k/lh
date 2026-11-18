/**
 * @file tap.c
 * @brief Implementation of `lh/gx/tap.h`.
 */

#include <lh/gx/tap.h>

#include <lh/core/util/return.h>
#include <lh/gx/hit.h>

lh_bool_t
lh_gx_tap_is_pressed(const lh_gx_tap_t *self)
{
    lh_assert_runtime_ref(self);
    return self->pressed;
}

lh_void
lh_gx_tap_set_pressed(lh_gx_tap_t *self, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    self->pressed = pressed;
}

lh_void
lh_gx_tap_init(lh_gx_tap_t *self)
{
    lh_gx_tap_set_pressed(self, lh_bool_false);
}

lh_gx_tap_step_t
lh_gx_tap_end(lh_gx_tap_t *self, lh_bool_t on)
{
    lh_return_if(!lh_gx_tap_is_pressed(self), lh_gx_tap_step_none);
    lh_gx_tap_set_pressed(self, lh_bool_false);
    return on ? lh_gx_tap_step_tap : lh_gx_tap_step_up;
}

lh_gx_tap_step_t
lh_gx_tap_take(lh_gx_tap_t *self, const lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    switch (lh_gx_event_get_kind(event))
    {
    case lh_gx_event_kind_press:
        lh_gx_tap_set_pressed(self, lh_bool_true);
        return lh_gx_tap_step_down;
    case lh_gx_event_kind_release:
    {
        lh_gx_rect_t place;

        lh_gx_entity_get_screen_rect(entity, lh_addr_of(place));
        return lh_gx_tap_end(self,
                             lh_gx_hit_is_on(entity, lh_addr_of(place), lh_gx_event_get_at(event)));
    }
    case lh_gx_event_kind_cancel:
        return lh_gx_tap_end(self, lh_bool_false);
    case lh_gx_event_kind_move:
    default:
        return lh_gx_tap_step_none;
    }
}
