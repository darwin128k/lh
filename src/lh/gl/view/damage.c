/**
 * @file damage.c
 * @brief Implementation of `lh/gl/view.h`: the damage: the region the window has yet to paint, and
 * what grows it.
 */

#include <lh/gl/view.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_view_add_damage(lh_gl_view_t *self, const lh_gl_rect_t *rect)
{
    lh_gl_rects_add(lh_gl_view_get_pending_mut(self), rect);
}

lh_bool_t
lh_gl_view_has_damage(const lh_gl_view_t *self)
{
    return !lh_gl_rects_is_empty(lh_gl_view_get_pending(self));
}

lh_bool_t
lh_gl_view_take_damage(lh_gl_view_t *self, lh_gl_rects_t *out)
{
    lh_assert_runtime_ref(out);
    lh_return_if(!lh_gl_view_has_damage(self), lh_bool_false);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_view_get_pending(self));
    lh_gl_rects_init(lh_gl_view_get_pending_mut(self));
    return lh_bool_true;
}

lh_void
lh_gl_view_damage_looks(lh_gl_view_t *self, const lh_gl_entity_t *entity,
                        const lh_gl_style_t *first, const lh_gl_style_t *second)
{
    lh_return_if(lh_null_eq(entity));
    lh_gl_rect_t rect;
    lh_gl_entity_copy_rect(entity, lh_addr_of(rect));
    lh_gl_scalar_t outset =
        lh_null_eq(first)
            ? lh_gl_scalar(0)
            : lh_gl_shadow_get_outset(lh_gl_style_get_shadow(first), lh_addr_of(rect));
    if (lh_null_ne(second))
    {
        outset = lh_math_max(
            outset, lh_gl_shadow_get_outset(lh_gl_style_get_shadow(second), lh_addr_of(rect)));
    }
    lh_gl_rect_t bounds;
    lh_gl_rect_inset(lh_addr_of(rect), lh_math_neg(outset), lh_math_neg(outset),
                     lh_addr_of(bounds));
    /* The canvas record stays in the rect's own space: that is what a frame
       reads back. The window paints root space, because a scrolled row's rect
       is where it was laid out and the pixels are where the scroll put them. */
    if (lh_null_ne(lh_gl_view_get_canvas(self)))
    {
        lh_gl_canvas_add_damage(lh_gl_view_get_canvas(self), lh_addr_of(bounds));
    }
    lh_gl_point_t offset;
    lh_gl_entity_get_root_offset(entity, lh_addr_of(offset));
    lh_gl_rect_offset(lh_addr_of(bounds), lh_gl_point_get_x(lh_addr_of(offset)),
                      lh_gl_point_get_y(lh_addr_of(offset)), lh_addr_of(bounds));
    lh_gl_view_add_damage(self, lh_addr_of(bounds));
}

lh_void
lh_gl_view_damage_shape(lh_gl_view_t *self, const lh_gl_entity_t *entity)
{
    const lh_gl_style_t *style;

    lh_return_if(lh_null_eq(entity));
    style = lh_gl_entity_get_style(entity);
    lh_gl_view_damage_looks(self, entity, style,
                            lh_null_eq(style) ? lh_null : lh_gl_style_get_pressed(style));
}
