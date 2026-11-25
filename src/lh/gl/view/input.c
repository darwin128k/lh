/**
 * @file input.c
 * @brief Implementation of `lh/gl/view.h`: input events: one ::lh_input_event_t to the part of the
 * view it is for.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_gl_point_t
lh_gl_view_point_of(lh_input_event_cptr event)
{
    lh_gl_point_t point;

    lh_gl_point_init(
        lh_addr_of(point),
        lh_type_cast(lh_gl_scalar_t, lh_input_coord_to_int(lh_input_event_get_x(event))),
        lh_type_cast(lh_gl_scalar_t, lh_input_coord_to_int(lh_input_event_get_y(event))));
    return point;
}

lh_bool_t
lh_gl_view_is_primary_press(lh_input_event_cptr event)
{
    return lh_type_cast(lh_bool_t, lh_input_event_get_button(event) == lh_input_button_primary);
}

lh_bool_t
lh_gl_view_input_pointer(lh_gl_view_t *self, lh_input_event_cptr event)
{
    const lh_input_pointer_id_t id = lh_input_event_get_pointer(event);

    lh_return_if(id >= LH_INPUT_POINTER_MAX, lh_bool_false);
    switch (lh_input_event_get_kind(event))
    {
    case lh_input_kind_pointer_down:
        lh_return_if(!lh_gl_view_is_primary_press(event), lh_bool_false);
        lh_gl_view_press_pointer(self, id, lh_gl_view_point_of(event));
        return lh_bool_true;
    case lh_input_kind_pointer_move:
        return lh_gl_view_move_pointer(self, id, lh_gl_view_point_of(event));
    case lh_input_kind_pointer_up:
        lh_return_if(!lh_gl_view_is_primary_press(event), lh_bool_false);
        LH_ATTRIBUTE_UNUSED(lh_gl_view_release_pointer(self, id, lh_gl_view_point_of(event)));
        return lh_bool_true;
    case lh_input_kind_pointer_cancel:
        lh_gl_view_cancel_pointer(self, id);
        return lh_bool_true;
    case lh_input_kind_wheel:
        return lh_gl_view_input_wheel(self, event);
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_gl_view_input(lh_gl_view_t *self, lh_input_event_cptr event)
{
    lh_assert_runtime_ref(event);
    lh_return_if(lh_input_event_is_synthesized(event), lh_bool_false);
    switch (lh_input_event_get_kind(event))
    {
    case lh_input_kind_key_down:
    case lh_input_kind_key_up:
        LH_ATTRIBUTE_UNUSED(lh_gl_view_key(
            self, lh_input_event_get_key(event),
            lh_type_cast(lh_bool_t, lh_input_event_get_kind(event) == lh_input_kind_key_down)));
        return lh_bool_true;
    case lh_input_kind_text:
        LH_ATTRIBUTE_UNUSED(lh_gl_view_text(self, lh_input_event_get_code(event)));
        return lh_bool_true;
    case lh_input_kind_focus_out:
        lh_gl_view_cancel(self);
        return lh_bool_true;
    default:
        return lh_gl_view_input_pointer(self, event);
    }
}
