/**
 * @file input.c
 * @brief Implementation of `lh/gx/window/input.h`.
 */

#include <lh/gx/window/input.h>

#include <lh/core/util/return.h>

lh_bool_t
lh_gx_window_input_try_get_key(lh_os_key_t key, lh_gx_key_t *out)
{
    switch (key)
    {
    case lh_os_key_left:
        lh_ptr_deref(out) = lh_gx_key_left;
        return lh_bool_true;
    case lh_os_key_right:
        lh_ptr_deref(out) = lh_gx_key_right;
        return lh_bool_true;
    case lh_os_key_up:
        lh_ptr_deref(out) = lh_gx_key_up;
        return lh_bool_true;
    case lh_os_key_down:
        lh_ptr_deref(out) = lh_gx_key_down;
        return lh_bool_true;
    case lh_os_key_home:
        lh_ptr_deref(out) = lh_gx_key_home;
        return lh_bool_true;
    case lh_os_key_end:
        lh_ptr_deref(out) = lh_gx_key_end;
        return lh_bool_true;
    case lh_os_key_backspace:
        lh_ptr_deref(out) = lh_gx_key_backspace;
        return lh_bool_true;
    case lh_os_key_delete:
        lh_ptr_deref(out) = lh_gx_key_delete;
        return lh_bool_true;
    case lh_os_key_enter:
        lh_ptr_deref(out) = lh_gx_key_enter;
        return lh_bool_true;
    case lh_os_key_escape:
        lh_ptr_deref(out) = lh_gx_key_escape;
        return lh_bool_true;
    case lh_os_key_tab:
        lh_ptr_deref(out) = lh_gx_key_tab;
        return lh_bool_true;
    default:
        return lh_bool_false;
    }
}

lh_bool_t
lh_gx_window_input_try_get_event(const lh_os_input_event_t *event, lh_gx_event_t *out)
{
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);
    lh_gx_point_t at;

    /* The system reports a point in Q24.8; the screen counts whole pixels. */
    lh_gx_point_init(lh_addr_of(at),
                     lh_gx_scalar_of(lh_os_input_coord_to_int(lh_os_input_event_get_x(event))),
                     lh_gx_scalar_of(lh_os_input_coord_to_int(lh_os_input_event_get_y(event))));
    switch (kind)
    {
    case lh_os_input_kind_pointer_down:
    case lh_os_input_kind_pointer_up:
        lh_return_if(lh_os_input_event_get_button(event) != lh_os_input_button_primary,
                     lh_bool_false);
        lh_gx_event_init(out,
                         kind == lh_os_input_kind_pointer_down ? lh_gx_event_kind_press
                                                               : lh_gx_event_kind_release,
                         at);
        return lh_bool_true;
    case lh_os_input_kind_pointer_move:
        /* Only a move with the button held is part of a gesture. */
        lh_return_if(lh_bit_and(lh_os_input_event_get_buttons(event),
                                lh_os_input_button_to_mask(lh_os_input_button_primary)) == 0U,
                     lh_bool_false);
        lh_gx_event_init(out, lh_gx_event_kind_move, at);
        return lh_bool_true;
    case lh_os_input_kind_pointer_cancel:
        lh_gx_event_init_kind(out, lh_gx_event_kind_cancel);
        return lh_bool_true;
    case lh_os_input_kind_key_down:
    {
        lh_gx_key_t key = lh_gx_key_none;

        lh_return_if(
            !lh_gx_window_input_try_get_key(lh_os_input_event_get_key(event), lh_addr_of(key)),
            lh_bool_false);
        lh_gx_event_init_key(out, key);
        return lh_bool_true;
    }
    case lh_os_input_kind_text:
        lh_return_if(!lh_gx_event_is_text_code(lh_os_input_event_get_code(event)), lh_bool_false);
        lh_gx_event_init_text(out, lh_os_input_event_get_code(event));
        return lh_bool_true;
    default:
        return lh_bool_false;
    }
}
