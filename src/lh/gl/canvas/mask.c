/**
 * @file mask.c
 * @brief Implementation of `lh/gl/canvas/mask.h`.
 */

#include <lh/gl/canvas.h>
#include <lh/gl/canvas/mask.h>
#include <lh/gl/canvas/round.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_canvas_can_fill_mask(const struct lh_gl_canvas *self, const lh_gl_rect_t *target)
{
    lh_return_if(lh_null_eq(lh_gl_canvas_get_backend(self)) || lh_null_eq(lh_gl_canvas_backend_get_fill_mask_fn(lh_gl_canvas_get_backend(self))), lh_bool_false);
    /* A mask is per pixel, like a rounded box: drawn whole, cut by whoever
       clips (::lh_gl_canvas_can_fill_round says the same). */
    lh_return_if(!lh_gl_canvas_is_cutting(self), lh_bool_true);
    return lh_gl_canvas_can_send_whole(self, target);
}

lh_bool_t
lh_gl_canvas_try_fill_mask(struct lh_gl_canvas *self, const lh_gl_mask_t *mask,
                           const lh_gl_rect_t *target, const lh_gl_color_t *color)
{
    lh_gl_point_t origin;

    lh_return_if(!lh_gl_canvas_can_fill_mask(self, target), lh_bool_false);
    lh_gl_point_init(lh_addr_of(origin), lh_gl_canvas_round_left(target),
                     lh_gl_canvas_round_top(target));
    return lh_gl_canvas_backend_get_fill_mask_fn(lh_gl_canvas_get_backend(self))(lh_gl_canvas_get_context(self), lh_addr_of(origin), mask, color);
}

lh_void
lh_gl_canvas_fill_mask_pixel(struct lh_gl_canvas *self, const lh_gl_mask_t *mask, lh_s32_t x0,
                             lh_s32_t y0, lh_s32_t x, lh_s32_t y, const lh_gl_color_t *color)
{
    const lh_byte_t cover = lh_gl_mask_get_coverage(mask, x, y);
    lh_gl_color_t pixel;
    lh_gl_color_with_coverage(color, cover, lh_addr_of(pixel));

    lh_return_if(cover == 0U);
    lh_gl_canvas_fill_pixels(self, x0 + x, y0 + y, 1, 1, lh_addr_of(pixel));
}

lh_void
lh_gl_canvas_fill_mask_row(struct lh_gl_canvas *self, const lh_gl_mask_t *mask, lh_s32_t x0,
                           lh_s32_t y0, lh_s32_t y, const lh_gl_color_t *color)
{
    for (lh_s32_t x = 0; x < lh_gl_mask_get_width(mask); ++x)
    {
        lh_gl_canvas_fill_mask_pixel(self, mask, x0, y0, x, y, color);
    }
}

lh_void
lh_gl_canvas_fill_mask_by_pixels(struct lh_gl_canvas *self, const lh_gl_mask_t *mask, lh_s32_t x0,
                                 lh_s32_t y0, const lh_gl_color_t *color)
{
    for (lh_s32_t y = 0; y < lh_gl_mask_get_height(mask); ++y)
    {
        lh_gl_canvas_fill_mask_row(self, mask, x0, y0, y, color);
    }
}

lh_void
lh_gl_canvas_fill_target_mask(struct lh_gl_canvas *self, const lh_gl_mask_t *mask,
                              const lh_gl_rect_t *target, const lh_gl_color_t *color)
{
    lh_gl_rect_t cut;
    lh_gl_canvas_state_cut(lh_gl_canvas_get_state_mut(self), target, lh_addr_of(cut));

    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)));
    lh_gl_canvas_add_damage(self, lh_addr_of(cut));
    lh_return_if(lh_gl_canvas_try_fill_mask(self, mask, target, color));
    lh_gl_canvas_fill_mask_by_pixels(self, mask, lh_gl_canvas_round_left(target),
                                     lh_gl_canvas_round_top(target), color);
}

lh_void
lh_gl_canvas_fill_mask(struct lh_gl_canvas *self, const lh_gl_mask_t *mask, lh_gl_point_t origin,
                       const lh_gl_color_t *color)
{
    lh_gl_rect_t rect;
    lh_gl_mask_get_rect(mask, origin, lh_addr_of(rect));
    lh_gl_rect_t target;
    lh_gl_canvas_state_to_target(lh_gl_canvas_get_state_mut(self), lh_addr_of(rect), lh_addr_of(target));

    lh_assert_runtime_ref(color);
    lh_return_if(!lh_gl_canvas_shows(self, lh_addr_of(target)));
    lh_gl_canvas_fill_target_mask(self, mask, lh_addr_of(target), color);
}
