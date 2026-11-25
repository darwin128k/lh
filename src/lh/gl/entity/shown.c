/**
 * @file shown.c
 * @brief Implementation of `lh/gl/entity.h`: whether an entity is shown, and hiding it.
 */

#include <lh/gl/entity.h>

#include <lh/core/util/return.h>

lh_bool_t
lh_gl_entity_chain_shown(const lh_gl_entity_t *self)
{
    const lh_gl_entity_t *walk;

    for (walk = self; lh_null_ne(walk); walk = lh_gl_entity_get_parent(walk))
    {
        if (lh_gl_entity_is_hidden(walk))
        {
            return lh_bool_false;
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_entity_is_shown(const lh_gl_entity_t *self)
{
    lh_bool_t visible = lh_bool_true;

    lh_return_if(lh_gl_entity_is_hidden(self), lh_bool_false);
    lh_gl_entity_send(self, lh_gl_entity_event_visible, lh_addr_of(visible));
    return visible;
}

lh_void
lh_gl_entity_set_hidden_v(lh_gl_entity_t *self, lh_bool_t hidden)
{
    lh_return_if(lh_gl_entity_is_hidden(self) == hidden);
    /* Hide records the rect that is still on screen. Show records the rect
       that is about to be: noting a hidden node is a no
       (::lh_gl_entity_note), so the flag has to change first. */
    if (hidden)
    {
        lh_gl_entity_note(self);
        lh_gl_entity_set_hidden(self, hidden);
        return;
    }
    lh_gl_entity_set_hidden(self, hidden);
    lh_gl_entity_note(self);
}
