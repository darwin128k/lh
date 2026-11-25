/**
 * @file draw.c
 * @brief Implementation of `lh/gl/entity.h`: drawing: the entity, its border and its children.
 */

#include <lh/gl/entity.h>

#include <lh/core/util/return.h>
#include <lh/gl/canvas/stroke.h>

lh_bool_t
lh_gl_entity_shows_on(const lh_gl_entity_t *self, const lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(canvas), lh_bool_true);
    lh_gl_rect_t painted;
    lh_gl_entity_get_painted_rect(self, lh_addr_of(painted));
    lh_return_if(lh_gl_canvas_shows_rect(canvas, lh_addr_of(painted)), lh_bool_true);
    lh_gl_rect_t content;
    /* The rect misses the clip, and measuring is not free: ask what the class
       paints of itself only now, when the cheap answer said no. A label's box is
       the cap line to the baseline (::lh_gl_text_get_size) while its ink hangs
       below it, so a clip that catches the tail of a 'p' catches no part of the
       box and would have dropped the whole label — the same defect as the shadow
       above, one layer down. Children are not asked about here: they are
       ::lh_gl_entity_shows_children_on's business, and a plain parent whose child
       lies outside it must still be culled out of its own draw. */
    lh_gl_entity_get_measure_bounds(self, lh_addr_of(content));
    return lh_gl_canvas_shows_rect(canvas, lh_addr_of(content));
}

lh_bool_t
lh_gl_entity_shows_children_on(const lh_gl_entity_t *self, const lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(lh_gl_entity_get_first_child(self)), lh_bool_false);
    /* The rect test is cheap; the clip question may measure content. */
    lh_return_if(lh_gl_entity_shows_on(self, canvas), lh_bool_true);
    return !lh_gl_entity_is_clipping(self) ? lh_bool_true : lh_bool_false;
}

lh_void
lh_gl_entity_draw_self(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(!lh_gl_entity_shows_on(self, canvas));
    lh_gl_entity_send(self, lh_gl_entity_event_draw, canvas);
}

lh_void
lh_gl_entity_fill_edge(lh_gl_canvas_t *canvas, lh_gl_scalar_t x, lh_gl_scalar_t y,
                       lh_gl_scalar_t width, lh_gl_scalar_t height, const lh_gl_color_t *color)
{
    lh_return_if(width <= lh_gl_scalar(0) || height <= lh_gl_scalar(0));
    lh_gl_rect_t strip;
    lh_gl_rect_init(lh_addr_of(strip), x, y, width, height);
    lh_gl_canvas_fill_rect(canvas, lh_addr_of(strip), color);
}

lh_void
lh_gl_entity_draw_border(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(lh_null_eq(canvas));
    const lh_gl_style_t *style = lh_gl_entity_get_style_now(self);
    lh_return_if(lh_null_eq(style));
    const lh_gl_insets_t *border = lh_gl_style_get_border(style);
    lh_gl_scalar_t left = lh_gl_insets_get_left(border);
    lh_gl_scalar_t top = lh_gl_insets_get_top(border);
    lh_gl_scalar_t right = lh_gl_insets_get_right(border);
    lh_gl_scalar_t bottom = lh_gl_insets_get_bottom(border);
    lh_return_if(left <= lh_gl_scalar(0) && top <= lh_gl_scalar(0) && right <= lh_gl_scalar(0) &&
                 bottom <= lh_gl_scalar(0));
    const lh_gl_color_t *color = lh_gl_paint_get_color_v(lh_gl_style_get_border_fill(style));
    lh_return_if(lh_null_eq(color));
    lh_gl_rect_t rect;
    lh_gl_entity_copy_rect(self, lh_addr_of(rect));
    if (left == top && top == right && right == bottom && left > lh_gl_scalar(0) &&
        (lh_gl_radii_get_min(lh_gl_style_get_radii(style)) > lh_gl_scalar(0) ||
         !lh_gl_radii_is_uniform(lh_gl_style_get_radii(style))))
    {
        lh_gl_canvas_stroke_box(canvas, lh_addr_of(rect), lh_gl_style_get_radii(style), left,
                                color);
        return;
    }
    const lh_gl_point_t *origin = lh_gl_rect_get_origin(lh_addr_of(rect));
    const lh_gl_size_t *size = lh_gl_rect_get_size(lh_addr_of(rect));
    lh_gl_scalar_t x = lh_gl_point_get_x(origin);
    lh_gl_scalar_t y = lh_gl_point_get_y(origin);
    lh_gl_scalar_t width = lh_gl_size_get_width(size);
    lh_gl_scalar_t height = lh_gl_size_get_height(size);
    if (top > height)
    {
        top = height;
    }
    if (bottom > height - top)
    {
        bottom = height - top;
    }
    if (left > width)
    {
        left = width;
    }
    if (right > width - left)
    {
        right = width - left;
    }
    lh_gl_entity_fill_edge(canvas, x, y, width, top, color);
    lh_gl_entity_fill_edge(canvas, x, y + height - bottom, width, bottom, color);
    lh_gl_entity_fill_edge(canvas, x, y + top, left, height - top - bottom, color);
    lh_gl_entity_fill_edge(canvas, x + width - right, y + top, right, height - top - bottom, color);
}

lh_void
lh_gl_entity_draw_each_child(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_gl_entity_t *child;

    for (child = lh_gl_entity_get_first_child(self); lh_null_ne(child);
         child = lh_gl_entity_get_next_child(self, child))
    {
        lh_gl_entity_draw(child, canvas);
    }
}

lh_void
lh_gl_entity_draw_children(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(!lh_gl_entity_shows_children_on(self, canvas));
    lh_bool_t pushed = lh_gl_entity_push_children(self, canvas);
    lh_gl_entity_draw_each_child(self, canvas);
    lh_return_if(!pushed);
    lh_gl_canvas_pop(canvas);
}

lh_void
lh_gl_entity_draw(const lh_gl_entity_t *self, lh_gl_canvas_t *canvas)
{
    lh_return_if(!lh_gl_entity_is_shown(self));
    lh_gl_entity_draw_self(self, canvas);
    lh_gl_entity_draw_children(self, canvas);
    /* After the children, and after their clip has been popped. A row's bottom
       edge is the line under its letters, and the letters are children: drawn
       with the fill it would sit underneath them. */
    lh_return_if(!lh_gl_entity_shows_on(self, canvas));
    lh_gl_entity_draw_border(self, canvas);
}
