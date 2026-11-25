/**
 * @file damage.c
 * @brief Implementation of `lh/gl/entity.h`: the damage an entity leaves on its view when it
 * changes.
 */

#include <lh/gl/entity.h>

#include <lh/core/assert.h>
#include <lh/core/util/return.h>
#include <lh/gl/view.h>

lh_void
lh_gl_entity_note_rect(const lh_gl_entity_t *self, const lh_gl_rect_t *rect)
{
    lh_return_if(lh_null_eq(self) || lh_null_eq(rect) || lh_gl_rect_is_empty(rect));
    lh_return_if(!lh_gl_entity_chain_shown(self));
    struct lh_gl_view *view = lh_gl_entity_own_view(self);
    lh_return_if(lh_null_eq(view));
    lh_gl_point_t offset;
    lh_gl_entity_get_root_offset(self, lh_addr_of(offset));
    lh_gl_rect_t moved;
    lh_gl_rect_offset(rect, lh_gl_point_get_x(lh_addr_of(offset)),
                      lh_gl_point_get_y(lh_addr_of(offset)), lh_addr_of(moved));
    lh_gl_view_add_damage(view, lh_addr_of(moved));
}

/* What an entity paints is not only its rect: a shadow reaches past the box of
   the thing casting it — the panel card in the demo drops one 18 rows below its own
   box — and the damage side has said so for a while (::lh_gl_view_damage_looks
   grows the rect by the outset of the shadow). The cull side did not, so a frame
   clipped to the fringe threw the whole card away and left the ground bare exactly
   where the full frame draws the shadow; it showed only in the corners of a
   rounded widget resting on the fringe, which are the one place nothing else
   covers it. The two answers have to be one thing, so this is what the draw walk
   tests and the damage records. */
lh_void
lh_gl_entity_get_painted_rect(const lh_gl_entity_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_gl_style_t *style = lh_gl_entity_get_style_now(self);
    const lh_gl_scalar_t outset =
        lh_null_eq(style)
            ? lh_gl_scalar(0)
            : lh_gl_shadow_get_outset(lh_gl_style_get_shadow(style), lh_gl_entity_get_rect(self));

    lh_gl_rect_inset(lh_gl_entity_get_rect(self), lh_math_neg(outset), lh_math_neg(outset), out);
}

lh_void
lh_gl_entity_add_damage(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(self) || lh_null_eq(canvas));
    lh_gl_rect_t rect;
    lh_gl_entity_get_root_rect(self, lh_addr_of(rect));
    lh_gl_canvas_add_damage(canvas, lh_addr_of(rect));
}

lh_void
lh_gl_entity_note(const lh_gl_entity_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_return_if(!lh_gl_entity_chain_shown(self));
    lh_gl_rect_t rect;
    lh_gl_entity_get_painted_rect(self, lh_addr_of(rect));
    /* A label draws past its box and on purpose (::lh_gl_label_on_children: the tail
       of a 'p', and any text wider than the box it was given), so the box alone is
       not what is on screen. Recording only the box left the overflow behind when the
       text changed: "Конфигуратор" in a 38-wide crumb became "DV205" over
       "...игуратор", and "УСТРОЙСТВА · 1" kept showing the 0 it used to end in.
       A leaf only: a parent's measure is its children, and a scrolled pane's children
       are the whole table. */
    if (lh_null_eq(lh_gl_entity_get_first_child(self)))
    {
        lh_gl_rect_t content;
        lh_gl_entity_get_measure_bounds(self, lh_addr_of(content));

        if (!lh_gl_rect_is_empty(lh_addr_of(content)))
        {
            lh_gl_rect_union(lh_addr_of(rect), lh_addr_of(content), lh_addr_of(rect));
        }
    }
    lh_gl_entity_note_rect(self, lh_addr_of(rect));
}
