/**
 * @file send.c
 * @brief Implementation of `lh/gl/entity.h`: the entity events it is sent, and the click.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_entity_send(const lh_gl_entity_t *self, lh_gl_entity_event_code_t code, lh_ptr context)
{
    lh_assert_runtime_ref(lh_gl_entity_get_class(self));
    lh_gl_entity_event_t event;
    lh_gl_entity_event_init(lh_addr_of(event), code, context);
    lh_gl_entity_class_get_event_fn(lh_gl_entity_get_class(self))(self, lh_addr_of(event));
}

lh_bool_t
lh_gl_entity_is_clickable(const lh_gl_entity_t *self)
{
    lh_bool_t clickable = lh_bool_false;

    lh_return_if(!lh_gl_entity_is_shown(self), lh_bool_false);
    lh_gl_entity_send(self, lh_gl_entity_event_clickable, lh_addr_of(clickable));
    return clickable;
}

lh_void
lh_gl_entity_send_pointer(const lh_gl_entity_t *self, lh_gl_entity_event_code_t code,
                          lh_gl_point_t point)
{
    lh_return_if(lh_null_eq(self));
    lh_gl_point_t local;
    lh_gl_entity_to_local(self, point, lh_addr_of(local));
    lh_gl_entity_send(self, code, lh_addr_of(local));
}

lh_gl_entity_t *
lh_gl_entity_click_target(lh_gl_entity_t *self)
{
    lh_gl_entity_t *node = self;

    /* Up the tree, the same walk the focus takes: the nearest thing that takes the
       pointer. What is under the pointer may be a caption, a picture or a row, and
       whether that is the whole of the click or only where it landed is not a
       question about geometry — it is a question about who is clickable. */
    for (; lh_null_ne(node); node = lh_gl_entity_get_parent(node))
    {
        lh_return_if(lh_gl_entity_is_clickable(node), node);
    }
    /* Nobody above claims it, so the click lands where it was pointed — on the
       background, or on something an app wants to hear about. */
    return self;
}

lh_gl_entity_t *
lh_gl_entity_click(lh_gl_entity_t *self, lh_gl_point_t point)
{
    lh_gl_entity_t *hit = lh_gl_entity_find_at(self, point);
    lh_return_if(lh_null_eq(hit), lh_null);
    /* What is under the pointer is where the click landed; who takes it is asked
       separately, so a click on a button's own caption or picture reaches the button
       (::lh_gl_entity_click_target). The point is moved into the space of the node
       that receives it — an ancestor has a children offset of its own. */
    lh_gl_entity_t *target = lh_gl_entity_click_target(hit);
    lh_return_if(lh_null_eq(target), lh_null);
    lh_gl_point_t local;
    lh_gl_entity_to_local(target, point, lh_addr_of(local));
    lh_gl_entity_send(target, lh_gl_entity_event_click, lh_addr_of(local));
    return target;
}
