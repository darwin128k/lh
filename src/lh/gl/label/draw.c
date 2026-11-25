/**
 * @file draw.c
 * @brief Implementation of `lh/gl/label.h`: drawing the text.
 */

#include <lh/gl/label.h>

#include <lh/gl/text.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_label_draw_text(const lh_gl_label_t *self, lh_gl_canvas_t *canvas)
{
    const lh_gl_entity_t *entity = lh_gl_container_get_entity(lh_gl_label_get_container(self));
    const lh_gl_style_t *style = lh_gl_entity_get_style(entity);
    const lh_char_t *text = lh_gl_label_get_text(self);

    lh_return_if(!lh_gl_label_can_draw_text(self, canvas));
    /* text may point into shortened, so shortened lives as long as text. */
    lh_char_t shortened[256];
    /* A text that wraps has no line that runs past the box to shorten. */
    if (lh_null_ne(style) && lh_gl_style_get_overflow(style) == lh_gl_text_overflow_ellipsis &&
        lh_gl_style_get_wrap(style) == lh_gl_text_wrap_none)
    {
        lh_gl_insets_t padding;
        lh_gl_entity_get_padding(entity, lh_addr_of(padding));
        lh_gl_rect_t rect;
        lh_gl_entity_copy_rect(entity, lh_addr_of(rect));
        const lh_s32_t room =
            lh_gl_scalar_floor_s32(lh_gl_size_get_width(lh_gl_rect_get_size(lh_addr_of(rect))) -
                                   lh_gl_insets_get_left(lh_addr_of(padding)) -
                                   lh_gl_insets_get_right(lh_addr_of(padding)));
        /* A line that does not fit is drawn from where it starts anyway -- a line wider
           than its box starts at the left whatever the alignment -- so the shortened
           one is drawn from the same place. */
        if (lh_gl_label_shorten(lh_gl_label_get_font(self), lh_gl_label_get_text(self), room,
                                shortened, lh_cast_static(lh_u32_t, sizeof(shortened))))
        {
            text = shortened;
        }
    }
    lh_bool_t pushed = lh_gl_entity_push_children(entity, canvas);
    if (text == lh_gl_label_get_text(self))
    {
        /* Wrapped or not: with no width to wrap at, these are the plain lines. */
        lh_gl_point_t point;
        lh_gl_label_get_text_origin(self, lh_addr_of(point));
        lh_gl_text_draw_wrapped_from(
            canvas, lh_gl_label_get_font(self), text, point, lh_gl_label_get_text_color(self),
            lh_gl_label_get_ink_top(self), lh_gl_label_get_wrap_width(self),
            lh_gl_label_get_line_align(self));
    }
    else
    {
        lh_gl_point_t point1;
        lh_gl_label_get_text_origin(self, lh_addr_of(point1));
        lh_gl_text_draw(canvas, lh_gl_label_get_font(self), text, point1,
                        lh_gl_label_get_text_color(self));
    }
    lh_return_if(!pushed);
    lh_gl_canvas_pop(canvas);
}
