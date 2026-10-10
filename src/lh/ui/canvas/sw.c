/**
 * @file sw.c
 * @brief Implementation of `lh/ui/canvas/sw.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/ui/blur.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/radius.h>
#include <lh/ui/shadow.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Lifetime ────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_sw_init(lh_ui_canvas_sw_t *self)
{
    lh_ui_pixmap_t empty;

    lh_ui_pixmap_init_empty(lh_addr_of(empty));
    lh_ui_canvas_sw_set_pixmap(self, lh_addr_of(empty));
}

lh_void
lh_ui_canvas_sw_set_pixmap(lh_ui_canvas_sw_t *self, const lh_ui_pixmap_t *pixmap)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(pixmap);
    self->pixmap = *pixmap;
    self->limit = lh_ui_pixmap_get_bounds(pixmap);
    lh_ui_canvas_clip_init_empty(lh_addr_of(self->clip));
}

const lh_ui_pixmap_t *
lh_ui_canvas_sw_get_pixmap(const lh_ui_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->pixmap);
}

lh_ui_rect_t
lh_ui_canvas_sw_get_limit(const lh_ui_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return self->limit;
}

lh_bool_t
lh_ui_canvas_sw_is_rounded(const lh_ui_canvas_sw_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_canvas_clip_get_round_count(lh_addr_of(self->clip)) > 0U ? lh_bool_true : lh_bool_false;
}

lh_ui_canvas_sw_t *
lh_ui_canvas_sw_from(lh_ptr context)
{
    lh_ui_canvas_sw_t *self = lh_ptr_rcast(lh_ui_canvas_sw_t, context);

    lh_assert_runtime_ref(self);
    return self;
}

/* ── Cutting to the limit ────────────────────────────────────────────────── */

lh_s32_t
lh_ui_canvas_sw_cut_x0(const lh_ui_canvas_sw_t *self, lh_s32_t x)
{
    return lh_math_max(x, lh_ui_canvas_round_left(lh_addr_of(self->limit)));
}

lh_s32_t
lh_ui_canvas_sw_cut_x1(const lh_ui_canvas_sw_t *self, lh_s32_t x)
{
    return lh_math_min(x, lh_ui_canvas_round_right(lh_addr_of(self->limit)));
}

lh_s32_t
lh_ui_canvas_sw_cut_y0(const lh_ui_canvas_sw_t *self, lh_s32_t y)
{
    return lh_math_max(y, lh_ui_canvas_round_top(lh_addr_of(self->limit)));
}

lh_s32_t
lh_ui_canvas_sw_cut_y1(const lh_ui_canvas_sw_t *self, lh_s32_t y)
{
    return lh_math_min(y, lh_ui_canvas_round_bottom(lh_addr_of(self->limit)));
}

/* ── Pixels and rows ─────────────────────────────────────────────────────── */

lh_byte_t
lh_ui_canvas_sw_edge_alpha(const lh_ui_canvas_sw_t *self, lh_byte_t alpha, lh_byte_t coverage, lh_s32_t x, lh_s32_t y)
{
    const lh_byte_t shaped = lh_ui_radius_scale(alpha, coverage);

    lh_return_if(shaped == 0U || !lh_ui_canvas_sw_is_rounded(self), shaped);
    return lh_ui_radius_scale(shaped, lh_ui_canvas_clip_coverage(lh_addr_of(self->clip), x, y));
}

lh_void
lh_ui_canvas_sw_blend_run(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color,
                          lh_byte_t *coverage)
{
    const lh_byte_t alpha = lh_ui_color_get_a(color);
    lh_s32_t mid0 = x0;
    lh_s32_t mid1 = x1;
    lh_s32_t i;

    /* The middle of the row lies wholly inside every rounded cut, so its clip
       coverage is 255 and only the shape's own alpha applies. Outside the middle
       the product is per pixel — lh_ui_canvas_clip_split_row is the one place
       that knows where that is. */
    if (lh_ui_canvas_sw_is_rounded(self))
    {
        lh_ui_canvas_clip_split_row(lh_addr_of(self->clip), x0, x1, y, lh_addr_of(mid0), lh_addr_of(mid1));
    }

    for (i = 0; i < mid0 - x0; ++i)
    {
        coverage[i] = lh_ui_canvas_sw_edge_alpha(self, alpha, coverage[i], x0 + i, y);
    }
    for (i = mid0 - x0; i < mid1 - x0; ++i)
    {
        coverage[i] = lh_ui_radius_scale(alpha, coverage[i]);
    }
    for (i = mid1 - x0; i < x1 - x0; ++i)
    {
        coverage[i] = lh_ui_canvas_sw_edge_alpha(self, alpha, coverage[i], x0 + i, y);
    }
    lh_ui_pixmap_blend_alpha_span(lh_addr_of(self->pixmap), x0, x1, y, lh_ui_color_get_argb(color) & 0x00FFFFFFU,
                                  coverage);
}

lh_void
lh_ui_canvas_sw_clip_pixels(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y,
                            const lh_ui_color_t *color)
{
    lh_byte_t coverage[LH_UI_PIXMAP_RUN];
    lh_s32_t end;

    for (; x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_PIXMAP_RUN);
        lh_memory_set(coverage, sizeof(coverage), 255U);
        lh_ui_canvas_sw_blend_run(self, x0, end, y, color, coverage);
    }
}

lh_void
lh_ui_canvas_sw_fill_span(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t mid0;
    lh_s32_t mid1;

    x0 = lh_ui_canvas_sw_cut_x0(self, x0);
    x1 = lh_ui_canvas_sw_cut_x1(self, x1);
    lh_ui_canvas_clip_split_row(lh_addr_of(self->clip), x0, lh_math_max(x0, x1), y, lh_addr_of(mid0),
                                lh_addr_of(mid1));
    lh_ui_canvas_sw_clip_pixels(self, x0, mid0, y, color);
    lh_ui_pixmap_fill_span(lh_addr_of(self->pixmap), mid0, mid1, y, color);
    lh_ui_canvas_sw_clip_pixels(self, mid1, x1, y, color);
}

lh_void
lh_ui_canvas_sw_fill_rows(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t y0, lh_s32_t x1, lh_s32_t y1,
                          const lh_ui_color_t *color)
{
    for (; y0 < y1; ++y0)
    {
        lh_ui_canvas_sw_fill_span(self, x0, x1, y0, color);
    }
}

lh_void
lh_ui_canvas_sw_cover_run_row(lh_ui_canvas_sw_t *self, const struct lh_ui_radius_run *run, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, const lh_ui_color_t *color)
{
    lh_byte_t coverage[LH_UI_PIXMAP_RUN];

    lh_ui_radius_run_row(run, x0, x1, y, coverage);
    lh_ui_canvas_sw_blend_run(self, x0, x1, y, color, coverage);
}

lh_void
lh_ui_canvas_sw_cover_run(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                          lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    struct lh_ui_radius_run run;

    lh_ui_radius_run_init(lh_addr_of(run), rect, radius);
    lh_ui_canvas_sw_cover_run_row(self, lh_addr_of(run), x0, x1, y, color);
}

lh_void
lh_ui_canvas_sw_cover_span(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    struct lh_ui_radius_run run;

    lh_ui_radius_run_init(lh_addr_of(run), rect, radius);
    lh_ui_canvas_sw_cover_span_run(self, lh_addr_of(run), x0, x1, y, color);
}

lh_void
lh_ui_canvas_sw_cover_span_run(lh_ui_canvas_sw_t *self, const struct lh_ui_radius_run *run, lh_s32_t x0, lh_s32_t x1,
                               lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t end;

    x1 = lh_ui_canvas_sw_cut_x1(self, x1);
    for (x0 = lh_ui_canvas_sw_cut_x0(self, x0); x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_PIXMAP_RUN);
        lh_ui_canvas_sw_cover_run_row(self, run, x0, end, y, color);
    }
}

lh_void
lh_ui_canvas_sw_round_band(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y0,
                           lh_s32_t y1, const lh_ui_color_t *color)
{
    const lh_s32_t left = lh_ui_canvas_round_left(rect);
    const lh_s32_t right = lh_ui_canvas_round_right(rect);
    struct lh_ui_radius_run run;
    lh_s32_t y;

    lh_ui_radius_run_init(lh_addr_of(run), rect, radius);
    for (y = y0; y < y1; ++y)
    {
        lh_s32_t full0;
        lh_s32_t full1;

        lh_ui_canvas_round_full_span(rect, radius, y, lh_addr_of(full0), lh_addr_of(full1));
        /* An arc that reaches this row can leave either end empty, and the whole
           row cost is call overhead — so an end with no partial pixel never pays
           for its cover call. */
        if (left < full0)
        {
            lh_ui_canvas_sw_cover_span_run(self, lh_addr_of(run), left, full0, y, color);
        }
        lh_ui_canvas_sw_fill_span(self, full0, full1, y, color);
        if (full1 < right)
        {
            lh_ui_canvas_sw_cover_span_run(self, lh_addr_of(run), full1, right, y, color);
        }
    }
}

lh_void
lh_ui_canvas_sw_round_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                          const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_round_band(self, rect, radius, y, y + 1, color);
}

lh_void
lh_ui_canvas_sw_mask_run(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t mx, lh_s32_t my, lh_s32_t x0,
                         lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_byte_t coverage[LH_UI_PIXMAP_RUN];

    lh_ui_mask_coverage_run(mask, mx, mx + x1 - x0, my, coverage);
    lh_ui_canvas_sw_blend_run(self, x0, x1, y, color, coverage);
}

lh_void
lh_ui_canvas_sw_mask_row(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0, lh_s32_t y,
                         const lh_ui_color_t *color)
{
    const lh_s32_t x1 = lh_ui_canvas_sw_cut_x1(self, x0 + lh_ui_mask_get_width(mask));
    lh_s32_t x;
    lh_s32_t end;

    for (x = lh_ui_canvas_sw_cut_x0(self, x0); x < x1; x = end)
    {
        end = lh_math_min(x1, x + LH_UI_PIXMAP_RUN);
        lh_ui_canvas_sw_mask_run(self, mask, x - x0, y - y0, x, end, y, color);
    }
}

/* ── Backend slots ───────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_sw_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_pixmap_clear(lh_addr_of(lh_ui_canvas_sw_from(context)->pixmap), color);
}

lh_void
lh_ui_canvas_sw_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_s32_t x0 = lh_ui_canvas_sw_cut_x0(self, lh_ui_canvas_round_left(rect));
    const lh_s32_t y0 = lh_ui_canvas_sw_cut_y0(self, lh_ui_canvas_round_top(rect));
    const lh_s32_t x1 = lh_ui_canvas_sw_cut_x1(self, lh_ui_canvas_round_right(rect));
    const lh_s32_t y1 = lh_ui_canvas_sw_cut_y1(self, lh_ui_canvas_round_bottom(rect));

    if (lh_ui_canvas_sw_is_rounded(self))
    {
        lh_ui_canvas_sw_fill_rows(self, x0, y0, x1, y1, color);
        return;
    }
    lh_ui_pixmap_fill_box(lh_addr_of(self->pixmap), x0, y0, x1, y1, color);
}

lh_bool_t
lh_ui_canvas_sw_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_s32_t y0 = lh_ui_canvas_sw_cut_y0(self, lh_ui_canvas_round_top(rect));
    const lh_s32_t y1 = lh_ui_canvas_sw_cut_y1(self, lh_ui_canvas_round_bottom(rect));
    /* Only the rows the arc actually reaches need a row at a time. Everywhere
       between them lh_ui_canvas_round_full_span answers the whole width, so that
       band is a plain rectangle and is drawn as one — the same single
       lh_ui_pixmap_fill_box a square rect gets, instead of one round_row per
       row. Measured on this machine that per-row path costs about 120 ns before
       it stores a single pixel, so a 240x160 r8 rectangle spent 20 us of its 24
       on 144 rows whose coverage is a flat 255. */
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);
    const lh_s32_t near = lh_ui_canvas_round_near_end(y0, y1, zone);
    const lh_s32_t far = lh_ui_canvas_round_far_start(y0, y1, zone);

    if (near < far)
    {
        lh_ui_rect_t middle;

        lh_ui_rect_init(lh_addr_of(middle), lh_ui_canvas_round_left(rect), near,
                        lh_ui_canvas_round_right(rect) - lh_ui_canvas_round_left(rect), far - near);
        lh_ui_canvas_sw_fill_rect(context, lh_addr_of(middle), color);
    }
    lh_ui_canvas_sw_round_band(self, rect, radius, y0, near, color);
    lh_ui_canvas_sw_round_band(self, rect, radius, lh_ui_canvas_sw_cut_y0(self, far), y1, color);
    return lh_bool_true;
}

static lh_void
sw_shadow_span(lh_ui_canvas_sw_t *self, const lh_ui_shadow_t *shadow, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
               lh_s32_t x0, lh_s32_t x1, lh_s32_t y, lh_u32_t rgb, lh_bool_t rounded)
{
    lh_byte_t alpha[LH_UI_PIXMAP_RUN];
    lh_s32_t end;

    x0 = lh_ui_canvas_sw_cut_x0(self, x0);
    x1 = lh_ui_canvas_sw_cut_x1(self, x1);
    for (; x0 < x1; x0 = end)
    {
        lh_s32_t i;

        end = lh_math_min(x1, x0 + LH_UI_PIXMAP_RUN);
        for (i = 0; i < end - x0; ++i)
        {
            const lh_byte_t shape = lh_ui_shadow_alpha_at_pixel(shadow, x0 + i, y, rect, radius);

            /* The shadow is soft on its own account and then cut by the clip, in
               that order — the same order the canvas fallback rounds in. What
               `alpha_at` hands back already carries the peak, so it goes in as the
               alpha and the coverage is the full 255: scaling it by the colour
               alpha a second time is what makes a slot shadow lighter than the
               very same shadow drawn without a slot. */
            alpha[i] = rounded ? lh_ui_canvas_sw_edge_alpha(self, shape, 255, x0 + i, y) : shape;
        }
        lh_ui_pixmap_blend_alpha_span(lh_addr_of(self->pixmap), x0, end, y, rgb, alpha);
    }
}

lh_bool_t
lh_ui_canvas_sw_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, const lh_ui_shadow_t *shadow)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_ui_size_t *size = lh_ui_rect_get_size_as_const(rect);
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_color_t color = lh_ui_shadow_get_color(shadow);
    const lh_u32_t rgb = lh_ui_color_get_argb(&color) & 0x00FFFFFFU;
    const lh_ui_scalar_t corner = lh_ui_radius_clamp(rect, radius);
    const lh_bool_t rounded = lh_ui_canvas_sw_is_rounded(self);
    const lh_s32_t outset = lh_ui_shadow_get_outset(shadow, rect);
    const lh_s32_t left = lh_ui_point_get_x(origin) - outset;
    const lh_s32_t top = lh_ui_point_get_y(origin) - outset;
    const lh_s32_t right = left + lh_ui_size_get_width(size) + outset * 2;
    const lh_s32_t bottom = top + lh_ui_size_get_height(size) + outset * 2;
    lh_s32_t y;

    lh_return_if(outset <= 0, lh_bool_false);
    for (y = lh_ui_canvas_sw_cut_y0(self, top); y < lh_ui_canvas_sw_cut_y1(self, bottom); ++y)
    {
        const lh_s32_t x0 = lh_ui_canvas_sw_cut_x0(self, left);
        const lh_s32_t x1 = lh_ui_canvas_sw_cut_x1(self, right);
        lh_s32_t full0;
        lh_s32_t full1;

        /* The span the fill covers wholly is a span the shadow does not paint
           (::lh_ui_shadow_alpha_at_pixel). Skipping it is the same pixels, and
           it is most of a card. */
        lh_ui_radius_full_span(rect, corner, y, &full0, &full1);
        sw_shadow_span(self, shadow, rect, corner, x0, lh_math_min(x1, full0), y, rgb, rounded);
        sw_shadow_span(self, shadow, rect, corner, lh_math_max(x0, full1), x1, y, rgb, rounded);
    }
    return lh_bool_true;
}

static lh_bool_t
sw_soften(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius, lh_u8_t *scratch,
          lh_usize_t bytes, const lh_ui_rect_t *shape, lh_ui_scalar_t corner)
{
    const lh_ui_rect_t cut = lh_ui_rect_intersection(rect, &self->limit);
    const lh_s32_t radius = blur_radius > 0 ? lh_ui_scalar_floor_s32(blur_radius) : 0;

    lh_return_if(lh_ui_rect_is_empty(&cut), lh_bool_false);
    lh_return_if(lh_null_eq(scratch) || lh_ui_blur_scratch_size(&cut, radius) > bytes, lh_bool_false);
    lh_return_if(radius <= 0, lh_bool_true);
    lh_ui_blur_rows(&self->pixmap, &cut, radius, scratch);
    lh_ui_blur_columns(&self->pixmap, &cut, radius, scratch, shape, corner);
    return lh_bool_true;
}

lh_bool_t
lh_ui_canvas_sw_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius, lh_u8_t *scratch,
                     lh_usize_t bytes)
{
    /* The buffer is the caller's, so a blur too big for it is one we cannot
       draw — and saying so beats writing past what we were given. */
    return sw_soften(lh_ui_canvas_sw_from(context), rect, blur_radius, scratch, bytes, lh_null, lh_ui_scalar(0));
}

lh_bool_t
lh_ui_canvas_sw_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner, lh_ui_scalar_t blur_radius,
                     const lh_ui_color_t *tint, lh_u8_t *scratch, lh_usize_t bytes)
{
    /* The blur is written only where the rounded panel covers. A square corner
       the arc misses stays the picture that was there; blurring it first and
       tinting the arc afterwards leaves that corner smeared. */
    lh_return_if(!sw_soften(lh_ui_canvas_sw_from(context), rect, blur_radius, scratch, bytes,
                            corner > lh_ui_scalar(0) ? rect : lh_null, corner),
                 lh_bool_false);
    lh_ui_canvas_sw_fill_round_rect(context, rect, corner, tint);
    return lh_bool_true;
}

lh_void
lh_ui_canvas_sw_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_ui_rect_t bounds = lh_ui_pixmap_get_bounds(lh_addr_of(self->pixmap));

    lh_ui_canvas_clip_init_empty(lh_addr_of(self->clip));
    self->limit = bounds;
    lh_return_if(lh_null_eq(clip));
    self->clip = *clip;
    self->limit = lh_ui_rect_intersection(lh_addr_of(bounds), lh_ui_canvas_clip_get_rect(clip));
}

lh_bool_t
lh_ui_canvas_sw_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                          const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_s32_t x0 = lh_ui_scalar_floor_s32(lh_ui_point_get_x(origin));
    const lh_s32_t y0 = lh_ui_scalar_floor_s32(lh_ui_point_get_y(origin));
    const lh_s32_t y1 = lh_ui_canvas_sw_cut_y1(self, y0 + lh_ui_mask_get_height(mask));
    lh_s32_t y;

    for (y = lh_ui_canvas_sw_cut_y0(self, y0); y < y1; ++y)
    {
        lh_ui_canvas_sw_mask_row(self, mask, x0, y0, y, color);
    }
    return lh_bool_true;
}

/* No begin_area: the software backend has no buffer of its own to size, so it
   draws whatever pixmap it was given and a partial frame falls back to a whole
   target one. A caller that owns a strip buffer gives it one per frame. */
const lh_ui_canvas_backend_t lh_ui_canvas_backend_sw = {
    lh_null,
    lh_null,
    lh_null,
    lh_ui_canvas_sw_clear,
    lh_ui_canvas_sw_fill_rect,
    lh_ui_canvas_sw_fill_round_rect,
    lh_ui_canvas_sw_set_clip,
    lh_ui_canvas_sw_fill_mask,
    lh_ui_canvas_sw_shadow,
    lh_ui_canvas_sw_blur,
    lh_ui_canvas_sw_glass,
};
