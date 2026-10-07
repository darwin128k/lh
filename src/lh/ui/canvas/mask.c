/**
 * @file mask.c
 * @brief Implementation of `lh/ui/canvas/mask.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/null.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/mask.h>
#include <lh/ui/canvas/round.h>
#include <lh/util/addr.h>
#include <lh/util/return.h>

lh_void
lh_ui_canvas_fill_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_ui_point_t origin,
                       const lh_ui_color_t *color)
{
    const lh_ui_rect_t rect = lh_ui_mask_get_rect(mask, origin);
    const lh_ui_rect_t target = lh_ui_canvas_state_to_target(lh_addr_of(self->state), lh_addr_of(rect));

    lh_assert_runtime_ref(color);
    lh_return_if(!lh_ui_canvas_shows(self, lh_addr_of(target)));
    lh_ui_canvas_fill_target_mask(self, mask, lh_addr_of(target), color);
}

lh_bool_t
lh_ui_canvas_can_fill_mask(const struct lh_ui_canvas *self, const lh_ui_rect_t *target)
{
    lh_return_if(lh_null_eq(self->backend) || lh_null_eq(self->backend->fill_mask), lh_bool_false);
    return lh_ui_canvas_can_send_whole(self, target);
}

lh_bool_t
lh_ui_canvas_try_fill_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, const lh_ui_rect_t *target,
                           const lh_ui_color_t *color)
{
    lh_ui_point_t origin;

    lh_return_if(!lh_ui_canvas_can_fill_mask(self, target), lh_bool_false);
    lh_ui_point_init(lh_addr_of(origin), lh_ui_canvas_round_left(target), lh_ui_canvas_round_top(target));
    return self->backend->fill_mask(self->context, lh_addr_of(origin), mask, color);
}

lh_void
lh_ui_canvas_fill_target_mask(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, const lh_ui_rect_t *target,
                              const lh_ui_color_t *color)
{
    const lh_ui_rect_t cut = lh_ui_canvas_state_cut(lh_addr_of(self->state), target);

    lh_return_if(lh_ui_rect_is_empty(lh_addr_of(cut)));
    lh_ui_canvas_add_damage(self, lh_addr_of(cut));
    lh_return_if(lh_ui_canvas_try_fill_mask(self, mask, target, color));
    lh_ui_canvas_fill_mask_by_pixels(self, mask, lh_ui_canvas_round_left(target), lh_ui_canvas_round_top(target),
                                     color);
}

lh_void
lh_ui_canvas_fill_mask_by_pixels(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                                 const lh_ui_color_t *color)
{
    lh_s32_t y;

    for (y = 0; y < lh_ui_mask_get_height(mask); ++y)
    {
        lh_ui_canvas_fill_mask_row(self, mask, x0, y0, y, color);
    }
}

lh_void
lh_ui_canvas_fill_mask_row(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                           lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t x;

    for (x = 0; x < lh_ui_mask_get_width(mask); ++x)
    {
        lh_ui_canvas_fill_mask_pixel(self, mask, x0, y0, x, y, color);
    }
}

lh_void
lh_ui_canvas_fill_mask_pixel(struct lh_ui_canvas *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0,
                             lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color)
{
    const lh_byte_t cover = lh_ui_mask_get_coverage(mask, x, y);
    const lh_ui_color_t pixel = lh_ui_color_with_coverage(color, cover);

    lh_return_if(cover == 0U);
    lh_ui_canvas_fill_pixels(self, x0 + x, y0 + y, 1, 1, lh_addr_of(pixel));
}
