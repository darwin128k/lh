/**
 * @file sw.c
 * @brief Implementation of `lh/ui/canvas/sw.h`.
 */

#include <lh/assert/runtime.h>
#include <lh/math.h>
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

/* ── Rows ────────────────────────────────────────────────────────────────── */

lh_void
lh_ui_canvas_sw_fill_span(lh_ui_canvas_sw_t *self, lh_s32_t x0, lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_ui_pixmap_fill_span(lh_addr_of(self->pixmap), lh_ui_canvas_sw_cut_x0(self, x0),
                           lh_ui_canvas_sw_cut_x1(self, x1), y, color);
}

lh_void
lh_ui_canvas_sw_cover_span(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t x0,
                           lh_s32_t x1, lh_s32_t y, const lh_ui_color_t *color)
{
    lh_s32_t x;

    x1 = lh_ui_canvas_sw_cut_x1(self, x1);
    for (x = lh_ui_canvas_sw_cut_x0(self, x0); x < x1; ++x)
    {
        lh_ui_pixmap_cover_pixel(lh_addr_of(self->pixmap), x, y, color, lh_ui_radius_coverage(rect, radius, x, y));
    }
}

lh_bool_t
lh_ui_canvas_sw_is_corner_row(const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y)
{
    const lh_s32_t top = lh_ui_canvas_round_top(rect);
    const lh_s32_t bottom = lh_ui_canvas_round_bottom(rect);
    const lh_s32_t zone = lh_ui_scalar_ceil_s32(radius);

    return y < lh_ui_canvas_round_near_end(top, bottom, zone) ||
                   y >= lh_ui_canvas_round_far_start(top, bottom, zone)
               ? lh_bool_true
               : lh_bool_false;
}

lh_void
lh_ui_canvas_sw_corner_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                           const lh_ui_color_t *color)
{
    const lh_s32_t left = lh_ui_canvas_round_left(rect);
    const lh_s32_t right = lh_ui_canvas_round_right(rect);
    const lh_s32_t near_end = lh_ui_canvas_round_near_end(left, right, lh_ui_scalar_ceil_s32(radius));
    const lh_s32_t far_start = lh_ui_canvas_round_far_start(left, right, lh_ui_scalar_ceil_s32(radius));

    lh_ui_canvas_sw_cover_span(self, rect, radius, left, near_end, y, color);
    lh_ui_canvas_sw_fill_span(self, near_end, far_start, y, color);
    lh_ui_canvas_sw_cover_span(self, rect, radius, far_start, right, y, color);
}

lh_void
lh_ui_canvas_sw_round_row(lh_ui_canvas_sw_t *self, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_s32_t y,
                          const lh_ui_color_t *color)
{
    if (lh_ui_canvas_sw_is_corner_row(rect, radius, y))
    {
        lh_ui_canvas_sw_corner_row(self, rect, radius, y, color);
        return;
    }
    lh_ui_canvas_sw_fill_span(self, lh_ui_canvas_round_left(rect), lh_ui_canvas_round_right(rect), y, color);
}

lh_void
lh_ui_canvas_sw_mask_row(lh_ui_canvas_sw_t *self, const lh_ui_mask_t *mask, lh_s32_t x0, lh_s32_t y0, lh_s32_t y,
                         const lh_ui_color_t *color)
{
    const lh_s32_t x1 = lh_ui_canvas_sw_cut_x1(self, x0 + lh_ui_mask_get_width(mask));
    lh_s32_t x;

    for (x = lh_ui_canvas_sw_cut_x0(self, x0); x < x1; ++x)
    {
        lh_ui_pixmap_cover_pixel(lh_addr_of(self->pixmap), x, y, color,
                                 lh_ui_mask_get_coverage(mask, x - x0, y - y0));
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

    lh_ui_pixmap_fill_box(lh_addr_of(self->pixmap), lh_ui_canvas_sw_cut_x0(self, lh_ui_canvas_round_left(rect)),
                          lh_ui_canvas_sw_cut_y0(self, lh_ui_canvas_round_top(rect)),
                          lh_ui_canvas_sw_cut_x1(self, lh_ui_canvas_round_right(rect)),
                          lh_ui_canvas_sw_cut_y1(self, lh_ui_canvas_round_bottom(rect)), color);
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
lh_ui_canvas_sw_set_clip(lh_ptr context, const lh_ui_rect_t *clip)
{
    lh_ui_canvas_sw_t *self = lh_ui_canvas_sw_from(context);
    const lh_ui_rect_t bounds = lh_ui_pixmap_get_bounds(lh_addr_of(self->pixmap));

    self->limit = lh_null_eq(clip) ? bounds : lh_ui_rect_intersection(lh_addr_of(bounds), clip);
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
