/**
 * @file hover.c
 * @brief Implementation of `lh/gl/view.h`: the hover: the entity drawn hot under the pointer.
 */

#include <lh/gl/view.h>

#include <lh/core/util/return.h>

lh_void
lh_gl_view_set_hot(lh_gl_view_t *self, lh_gl_button_t *button, lh_bool_t hot)
{
    lh_return_if(lh_null_eq(button));
    lh_return_if(lh_gl_button_get_hot(button) == hot);
    /* Both looks, in whichever order they are: the one being left and the one
       arriving are painted by the same rule as a press. */
    lh_gl_view_damage_looks(self, lh_gl_button_as_entity(button), lh_gl_button_get_style(button),
                            lh_gl_button_get_hot_style(button));
    lh_gl_button_set_hot_v(button, hot);
}
