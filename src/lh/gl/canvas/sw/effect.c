/**
 * @file effect.c
 * @brief Implementation of `lh/gl/canvas/sw.h`: the effects: a soft edge, a blur, glass.
 */

#include <lh/gl/canvas/sw.h>

#include <lh/gl/blur.h>
#include <lh/core/util/return.h>

lh_bool_t
lh_gl_canvas_sw_soften(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect,
                       lh_gl_scalar_t blur_radius, lh_u8_t *scratch, lh_usize_t bytes,
                       const lh_gl_rect_t *shape, lh_gl_scalar_t corner)
{
    lh_gl_rect_t cut;
    lh_gl_rect_intersection(rect, lh_gl_canvas_sw_get_limit_mut(self), lh_addr_of(cut));
    const lh_s32_t radius = blur_radius > 0 ? lh_gl_scalar_floor_s32(blur_radius) : 0;

    lh_return_if(lh_gl_rect_is_empty(lh_addr_of(cut)), lh_bool_false);
    lh_return_if(lh_null_eq(scratch) || lh_gl_blur_scratch_size(lh_addr_of(cut), radius) > bytes,
                 lh_bool_false);
    lh_return_if(radius <= 0, lh_bool_true);
    lh_gl_blur_rows(lh_gl_canvas_sw_get_pixmap_mut(self), lh_addr_of(cut), radius, scratch);
    lh_gl_blur_columns(lh_gl_canvas_sw_get_pixmap_mut(self), lh_addr_of(cut), radius, scratch,
                       shape, corner);
    return lh_bool_true;
}

lh_bool_t
lh_gl_canvas_sw_blur(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t blur_radius,
                     lh_u8_t *scratch, lh_usize_t bytes)
{
    /* The buffer is the caller's, so a blur too big for it is one we cannot
       draw — and saying so beats writing past what we were given. */
    return lh_gl_canvas_sw_soften(lh_gl_canvas_sw_from(context), rect, blur_radius, scratch, bytes,
                                  lh_null, lh_gl_scalar(0));
}

lh_bool_t
lh_gl_canvas_sw_glass(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t corner,
                      lh_gl_scalar_t blur_radius, const lh_gl_color_t *tint, lh_u8_t *scratch,
                      lh_usize_t bytes)
{
    /* The blur is written only where the rounded panel covers. A square corner
       the arc misses stays the picture that was there; blurring it first and
       tinting the arc afterwards leaves that corner smeared. */
    lh_return_if(!lh_gl_canvas_sw_soften(lh_gl_canvas_sw_from(context), rect, blur_radius, scratch,
                                         bytes, corner > lh_gl_scalar(0) ? rect : lh_null, corner),
                 lh_bool_false);
    lh_gl_canvas_sw_fill_round_rect(context, rect, corner, tint);
    return lh_bool_true;
}
