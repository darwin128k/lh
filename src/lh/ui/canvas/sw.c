/**
 * @file sw.c
 * @brief Implementation of `lh/ui/canvas/sw.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
#include <lh/memory.h>
#include <lh/null.h>
#include <lh/ui/canvas/round.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/radius.h>
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
lh_ui_canvas_sw_cover_run(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                          lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_byte_t coverage[LH_UI_PIXMAP_RUN];

    lh_ui_radius_coverage_run(rect, radius, x0, x1, y, coverage);
    lh_ui_canvas_sw_blend_run(self, x0, x1, y, color, coverage);
}

lh_void
lh_ui_canvas_sw_cover_span(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t end;

    x1 = lh_ui_canvas_sw_cut_x1(self, x1);
    for (x0 = lh_ui_canvas_sw_cut_x0(self, x0); x0 < x1; x0 = end)
    {
        end = lh_math_min(x1, x0 + LH_UI_PIXMAP_RUN);
        lh_ui_canvas_sw_cover_run(self, rect, radius, x0, end, y, color);
    }
}

lh_void
lh_ui_canvas_sw_round_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                          const lh_ui_color_t *color)
{
    lh_s32_t full0;
    lh_s32_t full1;

    lh_ui_canvas_round_full_span(rect, radius, y, lh_addr_of(full0), lh_addr_of(full1));
    lh_ui_canvas_sw_cover_span(self, rect, radius, lh_ui_canvas_round_left(rect), full0, y, color);
    lh_ui_canvas_sw_fill_span(self, full0, full1, y, color);
    lh_ui_canvas_sw_cover_span(self, rect, radius, full1, lh_ui_canvas_round_right(rect), y, color);
}

lh_void
lh_ui_canvas_sw_mask_run(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t mx, lh_s32_t my, lh_s32_t x0,
                         lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_byte_t coverage[LH_UI_PIXMAP_RUN];
    lh_s32_t i;

    for (i = 0; i < x1 - x0; ++i)
    {
        coverage[i] = lh_ui_mask_get_coverage(mask, mx + i, my);
    }
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
    const lh_s32_t y1 = lh_ui_canvas_sw_cut_y1(self, lh_ui_canvas_round_bottom(rect));
    lh_s32_t y;

    for (y = lh_ui_canvas_sw_cut_y0(self, lh_ui_canvas_round_top(rect)); y < y1; ++y)
    {
        lh_ui_canvas_sw_round_row(self, rect, radius, y, color);
    }
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

const lh_ui_canvas_backend_t lh_ui_canvas_backend_sw = {
    lh_null,
    lh_null,
    lh_ui_canvas_sw_clear,
    lh_ui_canvas_sw_fill_rect,
    lh_ui_canvas_sw_fill_round_rect,
    lh_ui_canvas_sw_set_clip,
    lh_ui_canvas_sw_fill_mask,
};
