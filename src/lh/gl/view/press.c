/**
 * @file press.c
 * @brief Implementation of `lh/gl/view.h`: the press of each pointer: down, move, up, cancel, and
 * the click.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_reset_pointer(lh_gl_view_t *self)
{
    for (lh_input_pointer_id_t id = 0U; id < LH_INPUT_POINTER_MAX; ++id)
    {
        lh_gl_view_pointer_reset(lh_addr_of(lh_gl_view_get_pointers_mut(self)[id]));
    }
}

const lh_gl_view_pointer_t *
lh_gl_view_get_pointer(const lh_gl_view_t *self, lh_input_pointer_id_t id)
{
    lh_return_if(id >= LH_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(lh_gl_view_get_pointers(self)[id]);
}

lh_gl_view_pointer_t *
lh_gl_view_get_slot(lh_gl_view_t *self, lh_input_pointer_id_t id)
{
    lh_return_if(id >= LH_INPUT_POINTER_MAX, lh_null);
    return lh_addr_of(lh_gl_view_get_pointers_mut(self)[id]);
}

lh_bool_t
lh_gl_view_is_driven_by_other(const lh_gl_view_t *self, const lh_gl_view_pointer_t *pointer,
                              const lh_gl_container_t *box)
{
    lh_return_if(lh_null_eq(box), lh_bool_false);
    for (lh_input_pointer_id_t id = 0U; id < LH_INPUT_POINTER_MAX; ++id)
    {
        const lh_gl_view_pointer_t *other = lh_addr_of(lh_gl_view_get_pointers(self)[id]);
        const lh_gl_scrollbar_t *grab = lh_gl_view_pointer_get_grab(other);

        if (other != pointer && (lh_gl_view_pointer_get_drag(other) == box ||
                                 (lh_null_ne(grab) && lh_gl_scrollbar_get_container(grab) == box)))
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_bool_t
lh_gl_view_is_held_by_other(const lh_gl_view_t *self, const lh_gl_view_pointer_t *pointer,
                            const lh_gl_entity_t *entity)
{
    lh_return_if(lh_null_eq(entity), lh_bool_false);
    for (lh_input_pointer_id_t id = 0U; id < LH_INPUT_POINTER_MAX; ++id)
    {
        const lh_gl_view_pointer_t *other = lh_addr_of(lh_gl_view_get_pointers(self)[id]);

        if (other != pointer && lh_gl_view_pointer_get_target(other) == entity)
        {
            return lh_bool_true;
        }
    }
    return lh_bool_false;
}

lh_gl_entity_t *
lh_gl_view_click_plain(lh_gl_view_t *self, lh_gl_point_t point)
{
    lh_gl_entity_t *clicked = lh_gl_entity_click(lh_gl_view_get_root(self), point);

    lh_gl_view_damage_shape(self, clicked);
    return clicked;
}

lh_void
lh_gl_view_set_pressed(lh_gl_view_t *self, lh_gl_entity_t *entity, lh_bool_t pressed)
{
    lh_return_if(lh_null_eq(entity) || lh_gl_entity_is_pressed(entity) == pressed);
    lh_gl_view_damage_shape(self, entity);
    lh_gl_entity_set_pressed(entity, pressed);
}

lh_void
lh_gl_view_finish_gesture(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer)
{
    lh_gl_container_t *box = lh_gl_view_pointer_get_drag(pointer);

    if (lh_null_ne(box) && lh_gl_view_pointer_is_dragged(pointer))
    {
        /* The glide is the view's, at the speed of the finger that let go. */
        lh_gl_view_pointer_copy_velocity(pointer, lh_gl_view_get_velocity_mut(self));
        lh_gl_view_start_throw(self, box);
        return;
    }
    lh_gl_view_end_scroll_of(self, pointer);
}

lh_gl_entity_t *
lh_gl_view_click_scrolling(lh_gl_view_t *self, lh_gl_container_t *box, lh_gl_point_t point)
{
    lh_return_if(lh_null_eq(box), lh_gl_view_click_plain(self, point));
    lh_gl_point_t before;
    lh_gl_container_get_clamped_scroll(box, lh_addr_of(before));
    lh_gl_entity_t *clicked = lh_gl_view_click_plain(self, point);
    LH_ATTRIBUTE_UNUSED(lh_gl_view_damage_scroll(self, box, before));
    lh_gl_view_end_scroll(self);
    return clicked;
}

lh_void
lh_gl_view_unpress_target(lh_gl_view_t *self, const lh_gl_view_pointer_t *pointer)
{
    lh_gl_entity_t *target = lh_gl_view_pointer_get_target(pointer);

    lh_return_if(lh_gl_view_is_held_by_other(self, pointer, target));
    lh_gl_view_set_pressed(self, target, lh_bool_false);
}

lh_bool_t
lh_gl_view_move_pointer(lh_gl_view_t *self, lh_input_pointer_id_t id, lh_gl_point_t point)
{
    lh_gl_view_pointer_t *pointer = lh_gl_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer), lh_bool_false);
    lh_gl_scrollbar_t *grab = lh_gl_view_pointer_get_grab(pointer);
    if (lh_null_ne(grab))
    {
        lh_gl_view_pointer_set_dragged(pointer, lh_bool_true);
        return lh_gl_view_drag_thumb(self, pointer, grab, point);
    }
    lh_return_if(!lh_gl_view_pointer_is_pressed(pointer), lh_bool_false);
    return lh_gl_view_drag_content(self, pointer, point);
}

lh_void
lh_gl_view_press_on(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_entity_t *hit,
                    lh_gl_point_t point)
{
    lh_assert_runtime_ref(pointer);
    lh_gl_entity_t *target = lh_gl_entity_click_target(hit);
    lh_gl_view_pointer_set_target(pointer, target);
    /* Pressed is the entity's own state and this is the one place that sets it:
       a press is a look (::lh_gl_entity_get_style_now), so it has to be paired
       with the rect it changed, or the picture keeps the old look until
       something else draws. The rect is ::lh_gl_view_take_damage's. */
    lh_gl_view_set_pressed(self, target, lh_bool_true);
    lh_gl_entity_send_pointer(target, lh_gl_entity_event_press, point);
    lh_gl_view_set_focus_v(self, lh_gl_entity_find_focusable(target));
    lh_gl_view_grab_thumb(self, pointer, lh_gl_entity_as_scrollbar(target), point);
    lh_gl_container_t *box =
        lh_null_eq(lh_gl_view_pointer_get_grab(pointer)) ? lh_gl_view_get_drag_box(hit) : lh_null;
    /* A container another pointer drags stays that pointer's: this one presses and
       may click inside it, but does not pull the content from under the other. */
    lh_gl_view_pointer_set_drag(pointer,
                                lh_gl_view_is_driven_by_other(self, pointer, box) ? lh_null : box);
}

lh_gl_entity_t *
lh_gl_view_click(lh_gl_view_t *self, lh_gl_point_t point)
{
    lh_return_if(lh_null_eq(lh_gl_view_get_root(self)), lh_null);
    return lh_gl_view_click_scrolling(
        self, lh_gl_scrollbar_get_driven(lh_gl_view_hit_test(self, point)), point);
}

lh_gl_entity_t *
lh_gl_view_release_target(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_point_t point)
{
    lh_gl_entity_t *target = lh_gl_view_pointer_get_target(pointer);
    lh_gl_entity_send_pointer(target, lh_gl_entity_event_release, point);
    lh_gl_view_unpress_target(self, pointer);
    lh_gl_view_pointer_set_target(pointer, lh_null);
    return target;
}

lh_bool_t
lh_gl_view_move(lh_gl_view_t *self, lh_gl_point_t point)
{
    return lh_gl_view_move_pointer(self, 0U, point);
}

lh_void
lh_gl_view_start_pointer(lh_gl_view_t *self, lh_gl_view_pointer_t *pointer, lh_gl_point_t point)
{
    lh_assert_runtime_ref(pointer);
    lh_gl_view_stop_throw(self);
    /* A press of this pointer still open when it goes down again (an app that
       went away without a release) is let go rather than left pressed under
       nothing — that is the one way a pressed look could stick. */
    LH_ATTRIBUTE_UNUSED(lh_gl_view_release_target(
        self, pointer, lh_ptr_deref(lh_gl_view_pointer_get_last_point(pointer))));
    lh_gl_view_pointer_start(pointer, point);
}

lh_gl_entity_t *
lh_gl_view_release_pointer(lh_gl_view_t *self, lh_input_pointer_id_t id, lh_gl_point_t point)
{
    lh_gl_view_pointer_t *pointer = lh_gl_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer), lh_null);
    lh_gl_entity_t *target = lh_gl_view_release_target(self, pointer, point);
    lh_gl_view_finish_gesture(self, pointer);
    /* A click is a press and a release on one thing: a finger that slid off the
       button it went down on and lifted over another one clicked neither. */
    lh_bool_t clicked = lh_type_cast(
        lh_bool_t, lh_gl_view_pointer_is_pressed(pointer) &&
                       !lh_gl_view_pointer_is_dragged(pointer) &&
                       lh_gl_entity_click_target(lh_gl_view_hit_test(self, point)) == target);
    lh_gl_view_pointer_reset(pointer);
    lh_return_if(!clicked, lh_null);
    return lh_gl_view_click(self, point);
}

lh_void
lh_gl_view_cancel_pointer(lh_gl_view_t *self, lh_input_pointer_id_t id)
{
    lh_gl_view_pointer_t *pointer = lh_gl_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer) || !lh_gl_view_pointer_is_active(pointer));
    lh_gl_point_t last;
    lh_gl_view_pointer_copy_last_point(pointer, lh_addr_of(last));
    LH_ATTRIBUTE_UNUSED(lh_gl_view_release_target(self, pointer, last));
    lh_gl_view_end_scroll_of(self, pointer);
    lh_gl_view_pointer_reset(pointer);
}

lh_gl_entity_t *
lh_gl_view_release(lh_gl_view_t *self, lh_gl_point_t point)
{
    return lh_gl_view_release_pointer(self, 0U, point);
}

lh_void
lh_gl_view_press_pointer(lh_gl_view_t *self, lh_input_pointer_id_t id, lh_gl_point_t point)
{
    lh_gl_view_pointer_t *pointer;

    pointer = lh_gl_view_get_slot(self, id);
    lh_return_if(lh_null_eq(pointer));
    lh_gl_view_start_pointer(self, pointer, point);
    /* Pressing is clicking: what the pointer went down on is the thing that takes
       the pointer, not whatever is painted under it. */
    lh_gl_view_press_on(self, pointer, lh_gl_view_hit_test(self, point), point);
}

lh_void
lh_gl_view_cancel(lh_gl_view_t *self)
{
    for (lh_input_pointer_id_t id = 0U; id < LH_INPUT_POINTER_MAX; ++id)
    {
        lh_gl_view_cancel_pointer(self, id);
    }
}

lh_void
lh_gl_view_press(lh_gl_view_t *self, lh_gl_point_t point)
{
    lh_gl_view_press_pointer(self, 0U, point);
}
