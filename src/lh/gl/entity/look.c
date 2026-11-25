/**
 * @file look.c
 * @brief Implementation of `lh/gl/entity.h`: the look in effect: the style now, its fill, radii and
 * padding.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>
#include <lh/gl/view.h>

const lh_gl_style_t *
lh_gl_entity_get_style_now(const lh_gl_entity_t *self)
{
    const lh_gl_style_t *style;

    style = lh_gl_entity_get_style(self);
    /* Painting is the only thing the pressed style is asked for, so it cannot
       change where anything lands. What a press *hits* keeps to the own radius
       (::lh_gl_entity_contains_point): a hit test runs before the press that
       asked for it, so a shape read from the pressed flag would answer one
       frame too late. */
    if (!lh_gl_entity_is_pressed(self) || lh_null_eq(style))
    {
        return style;
    }
    return lh_null_eq(lh_gl_style_get_pressed(style)) ? style : lh_gl_style_get_pressed(style);
}

lh_gl_scalar_t
lh_gl_entity_get_radius(const lh_gl_entity_t *self)
{
    return lh_null_eq(lh_gl_entity_get_style(self))
               ? lh_gl_scalar(0)
               : lh_gl_style_get_radius(lh_gl_entity_get_style(self));
}

lh_gl_scalar_t
lh_gl_entity_get_hit_radius(const lh_gl_entity_t *self)
{
    return lh_null_eq(lh_gl_entity_get_style(self))
               ? lh_gl_scalar(0)
               : lh_gl_style_get_hit_radius(lh_gl_entity_get_style(self));
}

lh_void
lh_gl_entity_get_padding(const lh_gl_entity_t *self, lh_gl_insets_t *out)
{
    lh_assert_runtime_ref(out);
    lh_gl_insets_t none;

    lh_gl_insets_init_all(lh_addr_of(none), lh_gl_scalar(0));
    lh_ptr_deref(out) = lh_null_eq(lh_gl_entity_get_style(self))
                            ? none
                            : lh_ptr_deref(lh_gl_style_get_padding(lh_gl_entity_get_style(self)));
}

lh_void
lh_gl_entity_set_style_at_once(lh_gl_entity_t *self, const lh_gl_style_t *style)
{
    /* A shared style switched to itself is the steady state of a card that was
       already selected. Both looks are recorded: a shadow that only the old
       one casts would otherwise stay on the surface. */
    lh_return_if(lh_gl_entity_get_style(self) == style);
    lh_gl_entity_note(self);
    lh_gl_entity_set_style(self, style);
    lh_gl_entity_note(self);
}

const lh_gl_color_t *
lh_gl_entity_get_fill_color(const lh_gl_entity_t *self)
{
    const lh_gl_style_t *style;

    style = lh_gl_entity_get_style_now(self);
    if (lh_null_eq(style))
    {
        return lh_null;
    }
    return lh_gl_style_get_fill_color(style);
}

lh_gl_scalar_t
lh_gl_entity_get_radius_now(const lh_gl_entity_t *self)
{
    const lh_gl_style_t *style;

    style = lh_gl_entity_get_style_now(self);
    return lh_null_eq(style) ? lh_gl_scalar(0) : lh_gl_style_get_radius(style);
}

lh_void
lh_gl_entity_set_style_v(lh_gl_entity_t *self, const lh_gl_style_t *style)
{
    struct lh_gl_view *view;

    view = lh_gl_entity_own_view(self);
    if (lh_null_ne(view))
    {
        /* Already on its way there: a hover that is asked for on every move of the
           pointer must not start the fade again on every move. */
        lh_return_if(lh_null_ne(style) && lh_gl_view_get_transition_target(view, self) == style);
        if (lh_null_ne(style) && lh_null_ne(lh_gl_entity_get_style(self)) &&
            lh_gl_entity_get_style(self) != style && lh_gl_style_get_transition(style) > 0 &&
            lh_gl_view_start_transition(view, self, style))
        {
            return;
        }
        /* At once: whatever was in flight is over, from the look it had. */
        lh_gl_view_drop_transition(view, self);
    }
    lh_gl_entity_set_style_at_once(self, style);
}
