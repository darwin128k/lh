/**
 * @file focus.c
 * @brief Implementation of `lh/gl/view.h`: the focus and the keys: who gets the keys, Tab, typed
 * text.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_set_focus_v(lh_gl_view_t *self, lh_gl_entity_t *entity)
{
    lh_return_if(lh_gl_view_get_focus(self) == entity);
    if (lh_null_ne(lh_gl_view_get_focus(self)))
    {
        lh_gl_entity_send(lh_gl_view_get_focus(self), lh_gl_entity_event_defocus, lh_null);
    }
    lh_gl_view_set_focus(self, entity);
    lh_return_if(lh_null_eq(entity));
    lh_gl_entity_send(entity, lh_gl_entity_event_focus, lh_null);
}

lh_bool_t
lh_gl_view_visit_focus(const lh_gl_entity_t *entity, lh_ptr context)
{
    lh_gl_view_focus_walk_t *walk = lh_ptr_rcast(lh_gl_view_focus_walk_t, context);

    lh_return_if(!lh_gl_entity_is_focusable(entity), lh_bool_true);
    if (lh_null_eq(lh_gl_view_focus_walk_get_first(walk)))
    {
        lh_gl_view_focus_walk_set_first(walk, entity);
    }
    lh_return_if(lh_gl_view_focus_walk_is_passed(walk) &&
                     lh_null_eq(lh_gl_view_focus_walk_get_next(walk)),
                 (walk->next = entity, lh_bool_false));
    lh_gl_view_focus_walk_set_passed(walk, entity == lh_gl_view_focus_walk_get_current(walk)
                                               ? lh_bool_true
                                               : lh_gl_view_focus_walk_is_passed(walk));
    return lh_bool_true;
}

lh_bool_t
lh_gl_view_send_input(lh_gl_view_t *self, const lh_gl_key_input_t *input)
{
    lh_gl_container_t *box = lh_gl_entity_as_container(lh_gl_view_get_focus(self));

    lh_return_if(lh_null_eq(lh_gl_view_get_focus(self)), lh_bool_false);
    lh_gl_point_t before;
    lh_gl_point_init(lh_addr_of(before), lh_gl_scalar(0), lh_gl_scalar(0));
    if (lh_null_ne(box))
    {
        lh_gl_container_get_clamped_scroll(box, lh_addr_of(before));
    }
    lh_gl_entity_send(lh_gl_view_get_focus(self), lh_gl_entity_event_key,
                      lh_ptr_rcast(lh_void, input));
    lh_return_if(lh_null_eq(box), lh_bool_false);
    lh_bool_t moved = lh_gl_view_damage_scroll(self, box, before);
    lh_gl_view_end_scroll(self);
    return moved;
}

lh_gl_entity_t *
lh_gl_view_get_next_focus(const lh_gl_view_t *self)
{
    lh_gl_view_focus_walk_t walk = {lh_null, lh_bool_false, lh_null, lh_null};

    lh_return_if(lh_null_eq(lh_gl_view_get_root(self)), lh_null);
    lh_gl_view_focus_walk_set_current(lh_addr_of(walk), lh_gl_view_get_focus(self));
    lh_gl_view_focus_walk_set_passed(
        lh_addr_of(walk), lh_null_eq(lh_gl_view_get_focus(self)) ? lh_bool_true : lh_bool_false);
    LH_ATTRIBUTE_UNUSED(
        lh_gl_entity_walk(lh_gl_view_get_root(self), lh_gl_view_visit_focus, lh_addr_of(walk)));
    return lh_ptr_rcast(lh_gl_entity_t, lh_null_ne(lh_gl_view_focus_walk_get_next(lh_addr_of(walk)))
                                            ? lh_gl_view_focus_walk_get_next(lh_addr_of(walk))
                                            : lh_gl_view_focus_walk_get_first(lh_addr_of(walk)));
}

lh_bool_t
lh_gl_view_text(lh_gl_view_t *self, lh_u32_t code)
{
    lh_gl_key_input_t input;

    lh_gl_key_input_init_text(lh_addr_of(input), code);
    return lh_gl_view_send_input(self, lh_addr_of(input));
}

lh_bool_t
lh_gl_view_key(lh_gl_view_t *self, lh_key_t key, lh_bool_t pressed)
{
    lh_assert_runtime_ref(self);
    if (key == lh_key_tab && pressed)
    {
        lh_gl_view_set_focus_v(self, lh_gl_view_get_next_focus(self));
        return lh_bool_false;
    }
    lh_gl_key_input_t input;
    lh_gl_key_input_init_key(lh_addr_of(input), key, pressed);
    return lh_gl_view_send_input(self, lh_addr_of(input));
}
