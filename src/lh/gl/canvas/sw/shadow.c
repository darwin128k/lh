/**
 * @file shadow.c
 * @brief Implementation of `lh/gl/canvas/sw.h`: the shadow: row by row, or a mask computed once and
 * stamped again.
 */

#include <lh/gl/canvas/sw.h>

#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/gl/canvas/sw/shadow/key.h>
#include <lh/core/util/return.h>

lh_void
lh_gl_canvas_sw_shadow_span(lh_gl_canvas_sw_t *self, const lh_gl_canvas_sw_shadow_pass_t *pass,
                            lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
{
    x0 = lh_gl_canvas_sw_cut_x0(self, x0);
    x1 = lh_gl_canvas_sw_cut_x1(self, x1);
    while (x0 < x1)
    {
        lh_s32_t end = lh_math_min(x1, x0 + LH_GL_PIXMAP_RUN);
        lh_byte_t alpha[LH_GL_PIXMAP_RUN];

        lh_pixel_shadow_get_span(lh_gl_canvas_sw_shadow_pass_get_shape(pass),
                                 lh_gl_canvas_sw_shadow_pass_get_ramp(pass), y, x0, end, alpha);
        /* The shadow is soft on its own account and then cut by the clip, in that
           order — the same order the canvas fallback rounds in. What the shape hands
           back already carries the peak, so it goes in as the alpha and the coverage
           is the full 255: scaling it by the colour alpha a second time is what makes
           a slot shadow lighter than the very same shadow drawn without a slot. */
        for (lh_s32_t i = 0; lh_gl_canvas_sw_shadow_pass_is_rounded(pass) && i < end - x0; ++i)
        {
            alpha[i] = lh_gl_canvas_sw_edge_alpha(self, alpha[i], 255, x0 + i, y);
        }
        lh_gl_pixmap_blend_alpha_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, end, y,
                                      lh_gl_canvas_sw_shadow_pass_get_rgb(pass), alpha);
        x0 = end;
    }
}

/* One repeated box. Larger than ::LH_LIBRARY_OPTION_GL_SHADOW_MASK on either
   side, the shadow is drawn row by row. */
lh_assert_static(LH_LIBRARY_OPTION_GL_SHADOW_MASK >= 1 && LH_LIBRARY_OPTION_GL_SHADOW_MASK <= 1024,
                 "LH_LIBRARY_OPTION_GL_SHADOW_MASK must be in 1..1024");

static lh_byte_t lh_gl_canvas_sw_shadow_mask[LH_LIBRARY_OPTION_GL_SHADOW_MASK *
                                             LH_LIBRARY_OPTION_GL_SHADOW_MASK];
static lh_gl_canvas_sw_shadow_key_t lh_gl_canvas_sw_shadow_kept;

lh_void
lh_gl_canvas_sw_shadow_mask_side(const lh_gl_canvas_sw_shadow_pass_t *pass, lh_byte_t *line,
                                 lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
{
    const lh_s32_t left = lh_gl_canvas_sw_shadow_pass_get_left(pass);

    for (lh_s32_t x = x0; x < x1;)
    {
        const lh_s32_t end = lh_math_min(x1, x + LH_GL_PIXMAP_RUN);

        lh_pixel_shadow_get_span(lh_gl_canvas_sw_shadow_pass_get_shape(pass),
                                 lh_gl_canvas_sw_shadow_pass_get_ramp(pass), y, x, end,
                                 line + (x - left));
        x = end;
    }
}

lh_void
lh_gl_canvas_sw_shadow_mask_stamp(lh_gl_canvas_sw_t *self,
                                  const lh_gl_canvas_sw_shadow_pass_t *pass, const lh_byte_t *mask,
                                  lh_s32_t width)
{
    const lh_s32_t left = lh_gl_canvas_sw_shadow_pass_get_left(pass);
    const lh_s32_t top = lh_gl_canvas_sw_shadow_pass_get_top(pass);
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, top);
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_sw_shadow_pass_get_bottom(pass));
    const lh_u32_t rgb = lh_gl_canvas_sw_shadow_pass_get_rgb(pass);

    for (lh_s32_t y = y0; y < y1; ++y)
    {
        const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, left);
        const lh_s32_t x1 =
            lh_gl_canvas_sw_cut_x1(self, lh_gl_canvas_sw_shadow_pass_get_right(pass));
        const lh_byte_t *row = mask + (y - top) * width + (x0 - left);

        for (lh_s32_t x = x0; x < x1;)
        {
            const lh_s32_t end = lh_math_min(x1, x + LH_GL_PIXMAP_RUN);

            lh_gl_pixmap_blend_alpha_span(lh_gl_canvas_sw_get_pixmap_mut(self), x, end, y, rgb,
                                          row + (x - x0));
            x = end;
        }
    }
}

lh_void
lh_gl_canvas_sw_shadow_make_key(const lh_gl_canvas_sw_shadow_pass_t *pass, const lh_gl_rect_t *rect,
                                const lh_gl_shadow_t *shadow, lh_gl_canvas_sw_shadow_key_t *out)
{
    const lh_gl_size_t *box = lh_gl_rect_get_size(rect);

    lh_gl_canvas_sw_shadow_key_init(
        out, lh_gl_canvas_sw_shadow_pass_get_width(pass),
        lh_gl_canvas_sw_shadow_pass_get_height(pass),
        lh_gl_scalar_floor_s32(lh_gl_canvas_sw_shadow_pass_get_corner(pass)),
        lh_gl_scalar_floor_s32(lh_gl_size_get_width(box)),
        lh_gl_scalar_floor_s32(lh_gl_size_get_height(box)),
        lh_gl_scalar_floor_s32(lh_gl_shadow_get_spread(shadow)),
        lh_gl_scalar_floor_s32(lh_gl_shadow_get_offset_x(shadow)),
        lh_gl_scalar_floor_s32(lh_gl_shadow_get_offset_y(shadow)),
        lh_gl_color_get_a(lh_gl_shadow_get_color(shadow)));
}

lh_void
lh_gl_canvas_sw_shadow_prepare(lh_gl_canvas_sw_shadow_pass_t *pass, const lh_gl_rect_t *rect,
                               const lh_gl_shadow_t *shadow)
{
    lh_gl_shadow_get_pixel_shadow(shadow, rect, lh_gl_canvas_sw_shadow_pass_get_corner(pass),
                                  lh_gl_canvas_sw_shadow_pass_get_shape_mut(pass));
    lh_pixel_shadow_ramp_init(lh_gl_canvas_sw_shadow_pass_get_ramp_mut(pass),
                              lh_gl_canvas_sw_shadow_pass_get_shape_mut(pass));
}

lh_void
lh_gl_canvas_sw_shadow_mask_fill(const lh_gl_canvas_sw_shadow_pass_t *pass, lh_byte_t *mask,
                                 lh_s32_t width, lh_s32_t height)
{
    const lh_s32_t left = lh_gl_canvas_sw_shadow_pass_get_left(pass);
    const lh_s32_t right = lh_gl_canvas_sw_shadow_pass_get_right(pass);
    const lh_s32_t top = lh_gl_canvas_sw_shadow_pass_get_top(pass);

    for (lh_s32_t row = 0; row < height; ++row)
    {
        lh_byte_t *line = mask + row * width;
        lh_s32_t full0;
        lh_s32_t full1;

        lh_memory_set(line, lh_cast_static(lh_usize_t, width), 0U);
        lh_gl_radius_full_span(lh_gl_canvas_sw_shadow_pass_get_rect(pass),
                               lh_gl_canvas_sw_shadow_pass_get_corner(pass), top + row,
                               lh_addr_of(full0), lh_addr_of(full1));
        lh_gl_canvas_sw_shadow_mask_side(pass, line, left, lh_math_min(right, full0), top + row);
        lh_gl_canvas_sw_shadow_mask_side(pass, line, lh_math_max(left, full1), right, top + row);
    }
}

lh_void
lh_gl_canvas_sw_shadow_row(lh_gl_canvas_sw_t *self, const lh_gl_canvas_sw_shadow_pass_t *pass,
                           lh_s32_t y)
{
    const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, lh_gl_canvas_sw_shadow_pass_get_left(pass));
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, lh_gl_canvas_sw_shadow_pass_get_right(pass));

    /* The span the fill covers wholly is a span the shadow does not paint
       (::lh_gl_shadow_alpha_at_pixel). Skipping it is the same pixels, and
       it is most of a card. */
    {
        lh_s32_t full0;
        lh_s32_t full1;

        lh_gl_radius_full_span(lh_gl_canvas_sw_shadow_pass_get_rect(pass),
                               lh_gl_canvas_sw_shadow_pass_get_corner(pass), y, lh_addr_of(full0),
                               lh_addr_of(full1));
        lh_gl_canvas_sw_shadow_span(self, pass, x0, lh_math_min(x1, full0), y);
        lh_gl_canvas_sw_shadow_span(self, pass, lh_math_max(x0, full1), x1, y);
    }
}

lh_void
lh_gl_canvas_sw_shadow_rows(lh_gl_canvas_sw_t *self, const lh_gl_canvas_sw_shadow_pass_t *pass)
{
    const lh_s32_t end = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_sw_shadow_pass_get_bottom(pass));

    for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(self, lh_gl_canvas_sw_shadow_pass_get_top(pass));
         y < end; ++y)
    {
        lh_gl_canvas_sw_shadow_row(self, pass, y);
    }
}

lh_bool_t
lh_gl_canvas_sw_shadow(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                       const lh_gl_shadow_t *shadow)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    lh_gl_canvas_sw_shadow_pass_t pass;

    lh_return_if(
        !lh_gl_canvas_sw_shadow_pass_init_bounds(self, rect, radius, shadow, lh_addr_of(pass)),
        lh_bool_false);
    /* A rounded pass is cut by its corners and a large one has no room in the mask:
       both are drawn row by row. Any other pass is a mask, made once and stamped
       again for the next pass with the same key. */
    const lh_bool_t masked =
        lh_type_cast(lh_bool_t, lh_gl_canvas_sw_shadow_pass_fits_mask(lh_addr_of(pass)) &&
                                    !lh_gl_canvas_sw_shadow_pass_is_rounded(lh_addr_of(pass)));
    const lh_s32_t width = lh_gl_canvas_sw_shadow_pass_get_width(lh_addr_of(pass));
    lh_gl_canvas_sw_shadow_key_t key;

    lh_gl_canvas_sw_shadow_make_key(lh_addr_of(pass), rect, shadow, lh_addr_of(key));
    if (masked && lh_gl_canvas_sw_shadow_key_is_equal(lh_addr_of(key),
                                                      lh_addr_of(lh_gl_canvas_sw_shadow_kept)))
    {
        lh_gl_canvas_sw_shadow_mask_stamp(self, lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask,
                                          width);
        return lh_bool_true;
    }
    lh_gl_canvas_sw_shadow_prepare(lh_addr_of(pass), rect, shadow);
    if (!masked)
    {
        lh_gl_canvas_sw_shadow_rows(self, lh_addr_of(pass));
        return lh_bool_true;
    }
    lh_gl_canvas_sw_shadow_mask_fill(lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask, width,
                                     lh_gl_canvas_sw_shadow_pass_get_height(lh_addr_of(pass)));
    lh_gl_canvas_sw_shadow_kept = key;
    lh_gl_canvas_sw_shadow_mask_stamp(self, lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask, width);
    return lh_bool_true;
}
