/**
 * @file focus.c
 * @brief Implementation of `lh/gl/entity.h`: whether an entity takes the focus, and the next one
 * that does.
 */

#include <lh/gl/entity.h>

#include <lh/core/util/return.h>

lh_bool_t
lh_gl_entity_is_focusable(const lh_gl_entity_t *self)
{
    lh_bool_t focusable = lh_bool_false;

    lh_return_if(!lh_gl_entity_is_shown(self), lh_bool_false);
    lh_gl_entity_send(self, lh_gl_entity_event_focusable, lh_addr_of(focusable));
    return focusable;
}

lh_gl_entity_t *
lh_gl_entity_find_focusable(lh_gl_entity_t *self)
{
    for (; lh_null_ne(self); self = lh_gl_entity_get_parent(self))
    {
        lh_return_if(lh_gl_entity_is_focusable(self), self);
    }
    return lh_null;
}
