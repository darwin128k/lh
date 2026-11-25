/**
 * @file row.c
 * @brief Implementation of `lh/gl/canvas/sw.h`: pixels and rows: cutting to the limit, blending,
 * covering, masking a run.
 */

#include <lh/gl/canvas/sw.h>

#include <lh/core/assert.h>
#include <lh/core/byte/limits.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/gl/canvas/round.h>
#include <lh/core/util/return.h>

lh_s32_t
lh_gl_canvas_sw_cut_x0(const lh_gl_canvas_sw_t *self, lh_s32_t x)
{
    return lh_math_max(x, lh_gl_canvas_round_left(lh_gl_canvas_sw_get_limit(self)));
}

lh_s32_t
lh_gl_canvas_sw_cut_x1(const lh_gl_canvas_sw_t *self, lh_s32_t x)
{
    return lh_math_min(x, lh_gl_canvas_round_right(lh_gl_canvas_sw_get_limit(self)));
}

lh_s32_t
lh_gl_canvas_sw_cut_y0(const lh_gl_canvas_sw_t *self, lh_s32_t y)
{
    return lh_math_max(y, lh_gl_canvas_round_top(lh_gl_canvas_sw_get_limit(self)));
}

lh_s32_t
lh_gl_canvas_sw_cut_y1(const lh_gl_canvas_sw_t *self, lh_s32_t y)
{
    return lh_math_min(y, lh_gl_canvas_round_bottom(lh_gl_canvas_sw_get_limit(self)));
}

lh_byte_t
lh_gl_canvas_sw_edge_alpha(const lh_gl_canvas_sw_t *self, lh_byte_t alpha, lh_byte_t coverage,
                           lh_s32_t x, lh_s32_t y)
{
    const lh_byte_t shaped = lh_gl_radius_scale(alpha, coverage);

    lh_return_if(shaped == 0U || !lh_gl_canvas_sw_is_rounded(self), shaped);
    return lh_gl_radius_scale(shaped,
                              lh_gl_canvas_clip_coverage(lh_gl_canvas_sw_get_clip(self), x, y));
}

lh_void
lh_gl_canvas_sw_blend_run(lh_gl_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                          const lh_gl_color_t *color, lh_byte_ptr coverage)
{
    const lh_byte_t alpha = lh_gl_color_get_a(color);
    lh_s32_t mid0 = x0;
    lh_s32_t mid1 = x1;

    /* The middle of the row lies wholly inside every rounded cut, so its clip
       coverage is 255 and only the shape's own alpha applies. Outside the middle
       the product is per pixel — lh_gl_canvas_clip_split_row is the one place
       that knows where that is. */
    if (lh_gl_canvas_sw_is_rounded(self))
    {
        lh_gl_canvas_clip_split_row(lh_gl_canvas_sw_get_clip_mut(self), x0, x1, y, lh_addr_of(mid0),
                                    lh_addr_of(mid1));
    }

    for (lh_s32_t i = 0; i < mid0 - x0; ++i)
    {
        coverage[i] = lh_gl_canvas_sw_edge_alpha(self, alpha, coverage[i], x0 + i, y);
    }
    /* An opaque colour scales nothing: lh_gl_radius_scale(255, c) is c. Text is
       drawn in one, and this loop was a multiply per pixel of every glyph row. */
    if (alpha != LH_BYTE_T_MAX)
    {
        for (lh_s32_t i = mid0 - x0; i < mid1 - x0; ++i)
        {
            coverage[i] = lh_gl_radius_scale(alpha, coverage[i]);
        }
    }
    for (lh_s32_t i = mid1 - x0; i < x1 - x0; ++i)
    {
        coverage[i] = lh_gl_canvas_sw_edge_alpha(self, alpha, coverage[i], x0 + i, y);
    }
    lh_gl_pixmap_blend_alpha_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, x1, y,
                                  lh_gl_color_get_argb(color) & 0x00FFFFFFU, coverage);
}

lh_void
lh_gl_canvas_sw_clip_pixels(lh_gl_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                            const lh_gl_color_t *color)
{
    while (x0 < x1)
    {
        lh_s32_t end = lh_math_min(x1, x0 + LH_GL_PIXMAP_RUN);
        lh_byte_t coverage[LH_GL_PIXMAP_RUN];

        lh_memory_set(coverage, sizeof(coverage), 255U);
        lh_gl_canvas_sw_blend_run(self, x0, end, y, color, coverage);
        x0 = end;
    }
}

lh_void
lh_gl_canvas_sw_cover_run_row(lh_gl_canvas_sw_t *self, const struct lh_gl_radius_run *run,
                              lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    lh_byte_t coverage[LH_GL_PIXMAP_RUN];

    lh_gl_radius_run_row(run, x0, x1, y, coverage);
    lh_gl_canvas_sw_blend_run(self, x0, x1, y, color, coverage);
}

lh_void
lh_gl_canvas_sw_mask_run(lh_gl_canvas_sw_t *self, const lh_gl_mask_t *mask, lh_s32_t mx,
                         lh_s32_t my, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                         const lh_gl_color_t *color)
{
    lh_byte_t coverage[LH_GL_PIXMAP_RUN];

    lh_gl_mask_coverage_run(mask, mx, mx + x1 - x0, my, coverage);
    lh_gl_canvas_sw_blend_run(self, x0, x1, y, color, coverage);
}

lh_void
lh_gl_canvas_sw_fill_span(lh_gl_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                          const lh_gl_color_t *color)
{
    x0 = lh_gl_canvas_sw_cut_x0(self, x0);
    x1 = lh_gl_canvas_sw_cut_x1(self, x1);
    {
        lh_s32_t mid0;
        lh_s32_t mid1;

        lh_gl_canvas_clip_split_row(lh_gl_canvas_sw_get_clip_mut(self), x0, lh_math_max(x0, x1), y,
                                    lh_addr_of(mid0), lh_addr_of(mid1));
        lh_gl_canvas_sw_clip_pixels(self, x0, mid0, y, color);
        lh_gl_pixmap_fill_span(lh_gl_canvas_sw_get_pixmap_mut(self), mid0, mid1, y, color);
        lh_gl_canvas_sw_clip_pixels(self, mid1, x1, y, color);
    }
}

lh_void
lh_gl_canvas_sw_cover_run(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                          lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    struct lh_gl_radius_run run;

    lh_gl_radius_run_init(lh_addr_of(run), rect, radius);
    lh_gl_canvas_sw_cover_run_row(self, lh_addr_of(run), x0, x1, y, color);
}

lh_void
lh_gl_canvas_sw_cover_span_run(lh_gl_canvas_sw_t *self, const struct lh_gl_radius_run *run,
                               lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    lh_s32_t end;

    x1 = lh_gl_canvas_sw_cut_x1(self, x1);
    for (x0 = lh_gl_canvas_sw_cut_x0(self, x0); x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_GL_PIXMAP_RUN);
        lh_gl_canvas_sw_cover_run_row(self, run, x0, end, y, color);
    }
}

lh_void
lh_gl_canvas_sw_mask_row(lh_gl_canvas_sw_t *self, const lh_gl_mask_t *mask, lh_s32_t x0,
                         lh_s32_t y0, lh_s32_t y, const lh_gl_color_t *color)
{
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, x0 + lh_gl_mask_get_width(mask));
    lh_s32_t end;

    for (lh_s32_t x = lh_gl_canvas_sw_cut_x0(self, x0); x < x1; x = end)
    {
        end = lh_math_min(x1, x + LH_GL_PIXMAP_RUN);
        lh_gl_canvas_sw_mask_run(self, mask, x - x0, y - y0, x, end, y, color);
    }
}

lh_void
lh_gl_canvas_sw_fill_rows(lh_gl_canvas_sw_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1,
                          lh_s32_t y1, const lh_gl_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_gl_canvas_sw_fill_span(self, x0, x1, y0, color);
    }
}

lh_void
lh_gl_canvas_sw_cover_span(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                           lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_gl_color_t *color)
{
    struct lh_gl_radius_run run;

    lh_gl_radius_run_init(lh_addr_of(run), rect, radius);
    lh_gl_canvas_sw_cover_span_run(self, lh_addr_of(run), x0, x1, y, color);
}

lh_void
lh_gl_canvas_sw_round_band(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                           lh_s32_t y0, lh_s32_t y1, const lh_gl_color_t *color)
{
    const lh_s32_t left = lh_gl_canvas_round_left(rect);
    const lh_s32_t right = lh_gl_canvas_round_right(rect);
    struct lh_gl_radius_run run;

    lh_gl_radius_run_init(lh_addr_of(run), rect, radius);
    for (lh_s32_t y = y0; y < y1; ++y)
    {
        lh_s32_t full0;
        lh_s32_t full1;

        lh_gl_canvas_round_full_span(rect, radius, y, lh_addr_of(full0), lh_addr_of(full1));
        /* An arc that reaches this row can leave either end empty, and the whole
           row cost is call overhead — so an end with no partial pixel never pays
           for its cover call. */
        if (left < full0)
        {
            lh_gl_canvas_sw_cover_span_run(self, lh_addr_of(run), left, full0, y, color);
        }
        lh_gl_canvas_sw_fill_span(self, full0, full1, y, color);
        if (full1 < right)
        {
            lh_gl_canvas_sw_cover_span_run(self, lh_addr_of(run), full1, right, y, color);
        }
    }
}

lh_void
lh_gl_canvas_sw_cover_row(lh_gl_canvas_sw_t *self, const lh_gl_mask_t *mask, lh_s32_t x0,
                          lh_s32_t y0, lh_s32_t y, const lh_gl_color_t *color)
{
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, x0 + lh_gl_mask_get_width(mask));
    const lh_byte_t alpha = lh_gl_color_get_a(color);
    const lh_u32_t src = lh_gl_color_get_argb(color) & 0x00FFFFFFU;

    lh_assert_runtime(!lh_gl_canvas_sw_is_rounded(self), lh_runtime_error_code_invalid_argument);
    /* Eight pixels a step, the coverages never leaving a register: a glyph row is
       one or two steps. The buffered run (::lh_gl_canvas_sw_mask_row) took a
       coverage pass, an alpha pass and a blend pass per row, each through its own
       calls, and on rows this short that was the cost of the text. */
    for (lh_s32_t x = lh_gl_canvas_sw_cut_x0(self, x0); x < x1; x += 8)
    {
        lh_u64_t cover = lh_pixel_mask_get_coverage_8(mask, x - x0, y - y0);

        /* Nothing covered is nothing to blend, and the edges of a glyph are. */
        if (cover == 0U)
        {
            continue;
        }
        if (alpha != LH_BYTE_T_MAX)
        {
            cover = lh_gl_radius_scale_8(alpha, cover);
        }
        lh_gl_pixmap_blend_cover_8(lh_gl_canvas_sw_get_pixmap_mut(self), x, y,
                                   lh_math_min(x1 - x, 8), cover, src);
    }
}

lh_void
lh_gl_canvas_sw_round_row(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                          lh_s32_t y, const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_round_band(self, rect, radius, y, y + 1, color);
}
