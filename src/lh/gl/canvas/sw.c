/**
 * @file sw.c
 * @brief Implementation of `lh/gl/canvas/sw.h`.
 */
#include <lh/core/assert.h>

#include <lh/core/byte/limits.h>
#include <lh/core/math.h>
#include <lh/core/memory.h>
#include <lh/gl/blur.h>
#include <lh/gl/canvas/round.h>
#include <lh/gl/canvas/sw.h>
#include <lh/core/util/return.h>

/* -- Lifetime -------------------------------------------------------------- */

lh_void
lh_gl_canvas_sw_use_pixmap(lh_gl_canvas_sw_t *self, const lh_gl_pixmap_t *pixmap)
{
    lh_assert_runtime_ref(pixmap);
    lh_gl_canvas_sw_set_pixmap(self, lh_ptr_deref(pixmap));
    lh_gl_pixmap_get_bounds(pixmap, lh_gl_canvas_sw_get_limit_mut(self));
    lh_gl_canvas_clip_init_empty(lh_gl_canvas_sw_get_clip_mut(self));
}

lh_void
lh_gl_canvas_sw_init(lh_gl_canvas_sw_t *self)
{
    lh_gl_pixmap_t empty;

    lh_gl_pixmap_init_empty(lh_addr_of(empty));
    lh_gl_canvas_sw_use_pixmap(self, lh_addr_of(empty));
}

const lh_gl_pixmap_t *
lh_gl_canvas_sw_get_pixmap(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_void
lh_gl_canvas_sw_copy_limit(const lh_gl_canvas_sw_t *self, lh_gl_rect_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr_deref(out) = lh_ptr_deref(lh_gl_canvas_sw_get_limit(self));
}

lh_bool_t
lh_gl_canvas_sw_is_rounded(const lh_gl_canvas_sw_t *self)
{
    return lh_gl_canvas_clip_get_round_count(lh_gl_canvas_sw_get_clip(self)) > 0U ? lh_bool_true
                                                                                  : lh_bool_false;
}

lh_gl_canvas_sw_t *
lh_gl_canvas_sw_from(lh_ptr context)
{
    lh_gl_canvas_sw_t *self = lh_ptr_rcast(lh_gl_canvas_sw_t, context);

    lh_assert_runtime_ref(self);
    return self;
}

/* -- Cutting to the limit -------------------------------------------------- */

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

/* -- Pixels and rows ------------------------------------------------------- */

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
lh_gl_canvas_sw_fill_rows(lh_gl_canvas_sw_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1,
                          lh_s32_t y1, const lh_gl_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_gl_canvas_sw_fill_span(self, x0, x1, y0, color);
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
lh_gl_canvas_sw_round_row(lh_gl_canvas_sw_t *self, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                          lh_s32_t y, const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_round_band(self, rect, radius, y, y + 1, color);
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
        lh_u64_t cover = lh_gl_mask_get_coverage_8(mask, x - x0, y - y0);

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

/* -- Backend slots --------------------------------------------------------- */

lh_void
lh_gl_canvas_sw_clear(lh_ptr context, const lh_gl_color_t *color)
{
    lh_gl_pixmap_clear(lh_addr_of(lh_gl_canvas_sw_from(context)->pixmap), color);
}

lh_void
lh_gl_canvas_sw_fill_rect(lh_ptr context, const lh_gl_rect_t *rect, const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, lh_gl_canvas_round_left(rect));
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, lh_gl_canvas_round_top(rect));
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, lh_gl_canvas_round_right(rect));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_round_bottom(rect));

    if (lh_gl_canvas_sw_is_rounded(self))
    {
        lh_gl_canvas_sw_fill_rows(self, x0, y0, x1, y1, color);
        return;
    }
    lh_gl_pixmap_fill_box(lh_gl_canvas_sw_get_pixmap_mut(self), x0, y0, x1, y1, color);
}

lh_bool_t
lh_gl_canvas_sw_fill_round_rect(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                                const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, lh_gl_canvas_round_top(rect));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_round_bottom(rect));
    /* Only the rows the arc actually reaches need a row at a time. Everywhere
       between them lh_gl_canvas_round_full_span answers the whole width, so that
       band is a plain rectangle and is drawn as one — the same single
       lh_gl_pixmap_fill_box a square rect gets, instead of one round_row per
       row. Measured on this machine that per-row path costs about 120 ns before
       it stores a single pixel, so a 240x160 r8 rectangle spent 20 us of its 24
       on 144 rows whose coverage is a flat 255. */
    const lh_s32_t zone = lh_gl_scalar_ceil_s32(radius);
    const lh_s32_t near = lh_gl_canvas_round_near_end(y0, y1, zone);
    const lh_s32_t far = lh_gl_canvas_round_far_start(y0, y1, zone);

    if (near < far)
    {
        lh_gl_rect_t middle;

        lh_gl_rect_init(lh_addr_of(middle), lh_gl_canvas_round_left(rect), near,
                        lh_gl_canvas_round_right(rect) - lh_gl_canvas_round_left(rect), far - near);
        lh_gl_canvas_sw_fill_rect(context, lh_addr_of(middle), color);
    }
    lh_gl_canvas_sw_round_band(self, rect, radius, y0, near, color);
    lh_gl_canvas_sw_round_band(self, rect, radius, lh_gl_canvas_sw_cut_y0(self, far), y1, color);
    return lh_bool_true;
}

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

        lh_gl_shadow_shape_span(lh_gl_canvas_sw_shadow_pass_get_shape(pass),
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

/* One repeated box. Larger than this, the shadow is drawn row by row. */
#define LH_GL_CANVAS_SW_SHADOW_MASK 160

static lh_byte_t
    lh_gl_canvas_sw_shadow_mask[LH_GL_CANVAS_SW_SHADOW_MASK * LH_GL_CANVAS_SW_SHADOW_MASK];
static lh_s32_t lh_gl_canvas_sw_shadow_kept_width;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_height;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_corner;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_box_w;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_box_h;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_spread;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_offset_x;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_offset_y;
static lh_s32_t lh_gl_canvas_sw_shadow_kept_peak;

lh_void
lh_gl_canvas_sw_shadow_mask_side(const lh_gl_canvas_sw_shadow_pass_t *pass, lh_byte_t *line,
                                 lh_s32_t x0, lh_s32_t x1, lh_s32_t y)
{
    const lh_s32_t left = lh_gl_canvas_sw_shadow_pass_get_left(pass);

    for (lh_s32_t x = x0; x < x1;)
    {
        const lh_s32_t end = lh_math_min(x1, x + LH_GL_PIXMAP_RUN);

        lh_gl_shadow_shape_span(lh_gl_canvas_sw_shadow_pass_get_shape(pass),
                                lh_gl_canvas_sw_shadow_pass_get_ramp(pass), y, x, end,
                                line + (x - left));
        x = end;
    }
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
lh_gl_canvas_sw_shadow_mask_stamp(lh_gl_canvas_sw_t *self, const lh_gl_canvas_sw_shadow_pass_t *pass,
                                  const lh_byte_t *mask, lh_s32_t width)
{
    const lh_s32_t left = lh_gl_canvas_sw_shadow_pass_get_left(pass);
    const lh_s32_t top = lh_gl_canvas_sw_shadow_pass_get_top(pass);
    const lh_s32_t y0 = lh_gl_canvas_sw_cut_y0(self, top);
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, lh_gl_canvas_sw_shadow_pass_get_bottom(pass));
    const lh_u32_t rgb = lh_gl_canvas_sw_shadow_pass_get_rgb(pass);

    for (lh_s32_t y = y0; y < y1; ++y)
    {
        const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, left);
        const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, lh_gl_canvas_sw_shadow_pass_get_right(pass));
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

lh_bool_t
lh_gl_canvas_sw_shadow(lh_ptr context, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                       const lh_gl_shadow_t *shadow)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    lh_gl_canvas_sw_shadow_pass_t pass;

    lh_return_if(!lh_gl_canvas_sw_shadow_pass_init_bounds(self, rect, radius, shadow,
                                                         lh_addr_of(pass)),
                 lh_bool_false);
    {
        const lh_gl_size_t *box = lh_gl_rect_get_size(rect);
        const lh_s32_t width = lh_gl_canvas_sw_shadow_pass_get_right(lh_addr_of(pass)) -
                               lh_gl_canvas_sw_shadow_pass_get_left(lh_addr_of(pass));
        const lh_s32_t height = lh_gl_canvas_sw_shadow_pass_get_bottom(lh_addr_of(pass)) -
                                lh_gl_canvas_sw_shadow_pass_get_top(lh_addr_of(pass));
        const lh_s32_t corner =
            lh_gl_scalar_floor_s32(lh_gl_canvas_sw_shadow_pass_get_corner(lh_addr_of(pass)));
        const lh_s32_t box_w = lh_gl_scalar_floor_s32(lh_gl_size_get_width(box));
        const lh_s32_t box_h = lh_gl_scalar_floor_s32(lh_gl_size_get_height(box));
        const lh_s32_t spread = lh_gl_scalar_floor_s32(lh_gl_shadow_get_spread(shadow));
        const lh_s32_t offset_x = lh_gl_scalar_floor_s32(lh_gl_shadow_get_offset_x(shadow));
        const lh_s32_t offset_y = lh_gl_scalar_floor_s32(lh_gl_shadow_get_offset_y(shadow));
        const lh_s32_t peak = lh_gl_color_get_a(lh_gl_shadow_get_color(shadow));
        const lh_bool_t fits =
            width > 0 && height > 0 && width <= LH_GL_CANVAS_SW_SHADOW_MASK &&
                    height <= LH_GL_CANVAS_SW_SHADOW_MASK
                ? lh_bool_true
                : lh_bool_false;
        const lh_bool_t same =
            fits && !lh_gl_canvas_sw_shadow_pass_is_rounded(lh_addr_of(pass)) &&
                    lh_gl_canvas_sw_shadow_kept_width == width &&
                    lh_gl_canvas_sw_shadow_kept_height == height &&
                    lh_gl_canvas_sw_shadow_kept_corner == corner &&
                    lh_gl_canvas_sw_shadow_kept_box_w == box_w &&
                    lh_gl_canvas_sw_shadow_kept_box_h == box_h &&
                    lh_gl_canvas_sw_shadow_kept_spread == spread &&
                    lh_gl_canvas_sw_shadow_kept_offset_x == offset_x &&
                    lh_gl_canvas_sw_shadow_kept_offset_y == offset_y &&
                    lh_gl_canvas_sw_shadow_kept_peak == peak
                ? lh_bool_true
                : lh_bool_false;

        if (same)
        {
            lh_gl_canvas_sw_shadow_mask_stamp(self, lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask,
                                              width);
            return lh_bool_true;
        }
        if (!fits || lh_gl_canvas_sw_shadow_pass_is_rounded(lh_addr_of(pass)))
        {
            lh_gl_shadow_shape_init(lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_addr_of(pass)),
                                    shadow, rect,
                                    lh_gl_canvas_sw_shadow_pass_get_corner(lh_addr_of(pass)));
            lh_gl_shadow_ramp_init(lh_gl_canvas_sw_shadow_pass_get_ramp_mut(lh_addr_of(pass)),
                                   lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_addr_of(pass)));
            for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(
                     self, lh_gl_canvas_sw_shadow_pass_get_top(lh_addr_of(pass)));
                 y < lh_gl_canvas_sw_cut_y1(
                         self, lh_gl_canvas_sw_shadow_pass_get_bottom(lh_addr_of(pass)));
                 ++y)
            {
                lh_gl_canvas_sw_shadow_row(self, lh_addr_of(pass), y);
            }
            return lh_bool_true;
        }
        lh_gl_shadow_shape_init(lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_addr_of(pass)), shadow,
                                rect, lh_gl_canvas_sw_shadow_pass_get_corner(lh_addr_of(pass)));
        lh_gl_shadow_ramp_init(lh_gl_canvas_sw_shadow_pass_get_ramp_mut(lh_addr_of(pass)),
                               lh_gl_canvas_sw_shadow_pass_get_shape_mut(lh_addr_of(pass)));
        lh_gl_canvas_sw_shadow_mask_fill(lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask, width,
                                         height);
        lh_gl_canvas_sw_shadow_kept_width = width;
        lh_gl_canvas_sw_shadow_kept_height = height;
        lh_gl_canvas_sw_shadow_kept_corner = corner;
        lh_gl_canvas_sw_shadow_kept_box_w = box_w;
        lh_gl_canvas_sw_shadow_kept_box_h = box_h;
        lh_gl_canvas_sw_shadow_kept_spread = spread;
        lh_gl_canvas_sw_shadow_kept_offset_x = offset_x;
        lh_gl_canvas_sw_shadow_kept_offset_y = offset_y;
        lh_gl_canvas_sw_shadow_kept_peak = peak;
        lh_gl_canvas_sw_shadow_mask_stamp(self, lh_addr_of(pass), lh_gl_canvas_sw_shadow_mask,
                                          width);
    }
    return lh_bool_true;
}

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

lh_void
lh_gl_canvas_sw_use_clip(lh_ptr context, const lh_gl_canvas_clip_t *clip)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    lh_gl_rect_t bounds;
    lh_gl_pixmap_get_bounds(lh_gl_canvas_sw_get_pixmap_mut(self), lh_addr_of(bounds));

    lh_gl_canvas_clip_init_empty(lh_gl_canvas_sw_get_clip_mut(self));
    lh_gl_canvas_sw_set_limit(self, bounds);
    lh_return_if(lh_null_eq(clip));
    lh_gl_canvas_sw_set_clip(self, lh_ptr_deref(clip));
    lh_gl_rect_intersection(lh_addr_of(bounds), lh_gl_canvas_clip_get_rect(clip),
                            lh_gl_canvas_sw_get_limit_mut(self));
}

lh_bool_t
lh_gl_canvas_sw_fill_mask(lh_ptr context, const lh_gl_point_t *origin, const lh_gl_mask_t *mask,
                          const lh_gl_color_t *color)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_scalar_floor_s32(lh_gl_point_get_x(origin));
    const lh_s32_t y0 = lh_gl_scalar_floor_s32(lh_gl_point_get_y(origin));
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, y0 + lh_gl_mask_get_height(mask));

    /* A rounded cut is a coverage a pixel of its own, which the run path
       multiplies in; without one, the mask's coverages go to the pixels straight
       from the register. */
    if (lh_gl_canvas_sw_is_rounded(self))
    {
        for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(self, y0); y < y1; ++y)
        {
            lh_gl_canvas_sw_mask_row(self, mask, x0, y0, y, color);
        }
        return lh_bool_true;
    }
    for (lh_s32_t y = lh_gl_canvas_sw_cut_y0(self, y0); y < y1; ++y)
    {
        lh_gl_canvas_sw_cover_row(self, mask, x0, y0, y, color);
    }
    return lh_bool_true;
}

lh_bool_t
lh_gl_canvas_sw_fill_colors(lh_ptr context, lh_s32_t x, lh_s32_t y, lh_s32_t w, lh_s32_t h,
                            const lh_gl_color_t *colors, lh_gl_axis_t axis)
{
    lh_gl_canvas_sw_t *self = lh_gl_canvas_sw_from(context);
    const lh_s32_t x0 = lh_gl_canvas_sw_cut_x0(self, x);
    const lh_s32_t x1 = lh_gl_canvas_sw_cut_x1(self, x + w);
    const lh_s32_t y1 = lh_gl_canvas_sw_cut_y1(self, y + h);

    /* A rounded cut is a coverage a pixel on top of the colours' own alpha; the
       canvas sends the box through fill_rect, which knows it. */
    lh_return_if(lh_gl_canvas_sw_is_rounded(self), lh_bool_false);
    lh_return_if(x1 <= x0, lh_bool_true);
    for (lh_s32_t row = lh_gl_canvas_sw_cut_y0(self, y); row < y1; ++row)
    {
        if (axis == lh_gl_axis_horizontal)
        {
            lh_gl_pixmap_fill_colors_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, x1, row,
                                          colors + (x0 - x));
        }
        else
        {
            lh_gl_pixmap_fill_span(lh_gl_canvas_sw_get_pixmap_mut(self), x0, x1, row,
                                   colors + (row - y));
        }
    }
    return lh_bool_true;
}

/* No begin_area: the software backend has no buffer of its own to size, so it
   draws whatever pixmap it was given and a partial frame falls back to a whole
   target one. A caller that owns a strip buffer gives it one per frame. */
const lh_gl_canvas_backend_t lh_gl_canvas_backend_sw = {
    lh_null,
    lh_null,
    lh_null,
    lh_gl_canvas_sw_clear,
    lh_gl_canvas_sw_fill_rect,
    lh_gl_canvas_sw_fill_round_rect,
    lh_gl_canvas_sw_use_clip,
    lh_gl_canvas_sw_fill_mask,
    lh_gl_canvas_sw_shadow,
    lh_gl_canvas_sw_blur,
    lh_gl_canvas_sw_glass,
    lh_gl_canvas_sw_fill_colors,
};

lh_gl_pixmap_t *
lh_gl_canvas_sw_get_pixmap_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_gl_rect_t *
lh_gl_canvas_sw_get_limit_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->limit);
}

lh_void
lh_gl_canvas_sw_set_limit(lh_gl_canvas_sw_t *self, lh_gl_rect_t limit)
{
    lh_assert_runtime_ref(self);
    self->limit = limit;
}

const lh_gl_canvas_clip_t *
lh_gl_canvas_sw_get_clip(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

lh_gl_canvas_clip_t *
lh_gl_canvas_sw_get_clip_mut(lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->clip);
}

const lh_gl_rect_t *
lh_gl_canvas_sw_get_limit(const lh_gl_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->limit);
}

lh_void
lh_gl_canvas_sw_set_pixmap(lh_gl_canvas_sw_t *self, lh_gl_pixmap_t pixmap)
{
    lh_assert_runtime_ref(self);
    self->pixmap = pixmap;
}

lh_void
lh_gl_canvas_sw_set_clip(lh_gl_canvas_sw_t *self, lh_gl_canvas_clip_t clip)
{
    lh_assert_runtime_ref(self);
    self->clip = clip;
}
