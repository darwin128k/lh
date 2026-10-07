/**
 * @file gdi.c
 * @brief GDI canvas backend over ::lh_ui_surface_t and ::lh_os_system_*.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/render/backend/gdi.h>
#include <lh/os/render/backend/gdi/plus.h>
#include <lh/os/system/hdc.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/ui/color.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

lh_os_render_backend_gdi_context_t *
lh_os_render_backend_gdi_context_from(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_ptr_rcast(lh_os_render_backend_gdi_context_t, context);
    lh_assert_runtime_ref(gdi);
    return gdi;
}

lh_ptr
lh_os_render_backend_gdi_context_get_draw_hdc(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_surface_get_draw_target(lh_addr_of(self->surface));
}

lh_void
lh_os_render_backend_gdi_fill_area(lh_ptr hdc, int left, int top, int right, int bottom,
                                   const lh_ui_color_t *color)
{
    lh_os_system_hdc_fill_rect(hdc, left, top, right, bottom, lh_ui_color_get_r(color),
                               lh_ui_color_get_g(color), lh_ui_color_get_b(color));
}

lh_void
lh_os_render_backend_gdi_end_frame(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    lh_return_if(lh_null_eq(self->frame));
    lh_os_render_backend_gdi_plus_frame_end(self->frame);
    self->frame = lh_null;
}

lh_void
lh_os_render_backend_gdi_reset_counters(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    self->mask_calls = 0U;
    self->rect_calls = 0U;
    self->round_calls = 0U;
    self->clip_calls = 0U;
    self->frame_start_us = lh_os_tick_us();
}

lh_bool_t
lh_os_render_backend_gdi_begin_surface(lh_os_render_backend_gdi_context_t *self)
{
    int width;
    int height;
    lh_ui_size_t size;

    lh_return_if(!lh_os_system_window_get_client_size(self->hwnd, lh_addr_of(width), lh_addr_of(height)),
                 lh_bool_false);
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(width), lh_ui_scalar(height));
    (void)lh_ui_surface_set_size(lh_addr_of(self->surface), size);
    lh_os_render_backend_gdi_end_frame(self);
    lh_os_render_backend_gdi_reset_counters(self);
    return lh_bool_true;
}

lh_void
lh_os_render_backend_gdi_begin(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ptr draw_hdc;

    lh_return_if(!lh_os_render_backend_gdi_begin_surface(gdi));
    draw_hdc = lh_os_render_backend_gdi_context_get_draw_hdc(gdi);
    lh_return_if(lh_null_eq(draw_hdc) || !lh_os_render_backend_gdi_plus_is_ready());
    gdi->frame = lh_os_render_backend_gdi_plus_frame_begin(draw_hdc);
}

lh_void
lh_os_render_backend_gdi_end(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_os_render_backend_gdi_end_frame(gdi);
    lh_return_if(lh_null_eq(gdi->hdc));
    (void)lh_ui_surface_present(lh_addr_of(gdi->surface), gdi->hdc);
}

lh_void
lh_os_render_backend_gdi_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ptr hdc = lh_os_render_backend_gdi_context_get_draw_hdc(gdi);
    lh_ui_size_t size;

    lh_return_if(lh_null_eq(hdc));
    size = lh_ui_surface_get_size(lh_addr_of(gdi->surface));
    lh_os_render_backend_gdi_fill_area(hdc, 0, 0, lh_cast_static(int, lh_ui_size_get_width(lh_addr_of(size))),
                                       lh_cast_static(int, lh_ui_size_get_height(lh_addr_of(size))),
                                       color);
}

lh_void
lh_os_render_backend_gdi_fill_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                   const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ptr hdc = lh_os_render_backend_gdi_context_get_draw_hdc(gdi);
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_point_t corner = lh_ui_rect_far(rect);

    lh_return_if(lh_null_eq(hdc));
    ++gdi->rect_calls;
    lh_os_render_backend_gdi_fill_area(
        hdc, lh_cast_static(int, lh_ui_point_get_x(origin)),
        lh_cast_static(int, lh_ui_point_get_y(origin)),
        lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(corner))),
        lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(corner))), color);
}

lh_bool_t
lh_os_render_backend_gdi_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                         lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_return_if(lh_null_eq(gdi->frame), lh_bool_false);
    ++gdi->round_calls;
    return lh_os_render_backend_gdi_plus_fill_round_rect(gdi->frame, rect, radius, color);
}

lh_void
lh_os_render_backend_gdi_set_clip(lh_ptr context, const lh_ui_rect_t *clip)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ptr hdc = lh_os_render_backend_gdi_context_get_draw_hdc(gdi);
    const lh_ui_point_t *origin;
    lh_ui_point_t corner;

    lh_return_if(lh_null_eq(hdc));
    ++gdi->clip_calls;
    if (clip == lh_null)
    {
        lh_os_system_hdc_clear_clip(hdc);
        if (lh_null_ne(gdi->frame))
        {
            lh_os_render_backend_gdi_plus_clear_clip(gdi->frame);
        }
        return;
    }
    lh_assert_runtime_ref(gdi->clip_region);
    origin = lh_ui_rect_get_origin_as_const(clip);
    corner = lh_ui_rect_far(clip);
    lh_os_system_hdc_set_clip(hdc, gdi->clip_region,
                              lh_cast_static(int, lh_ui_point_get_x(origin)),
                              lh_cast_static(int, lh_ui_point_get_y(origin)),
                              lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(corner))),
                              lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(corner))));
    if (lh_null_ne(gdi->frame))
    {
        lh_os_render_backend_gdi_plus_set_clip(
            gdi->frame, lh_cast_static(int, lh_ui_point_get_x(origin)),
            lh_cast_static(int, lh_ui_point_get_y(origin)),
            lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(corner))),
            lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(corner))));
    }
}

lh_bool_t
lh_os_render_backend_gdi_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                   const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_return_if(lh_null_eq(gdi->frame), lh_bool_false);
    ++gdi->mask_calls;
    return lh_os_render_backend_gdi_plus_fill_mask(gdi->frame, origin, mask, color);
}

lh_u32_t
lh_os_render_backend_gdi_context_get_mask_calls(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->mask_calls;
}

lh_u32_t
lh_os_render_backend_gdi_context_get_rect_calls(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->rect_calls;
}

lh_u32_t
lh_os_render_backend_gdi_context_get_round_calls(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->round_calls;
}

lh_u32_t
lh_os_render_backend_gdi_context_get_clip_calls(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->clip_calls;
}

lh_u64_t
lh_os_render_backend_gdi_context_get_frame_us(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_os_tick_us() - self->frame_start_us;
}

const lh_ui_canvas_backend_t lh_os_render_backend_gdi = {
    lh_os_render_backend_gdi_begin,
    lh_os_render_backend_gdi_end,
    lh_os_render_backend_gdi_clear,
    lh_os_render_backend_gdi_fill_rect,
    lh_os_render_backend_gdi_fill_round_rect,
    lh_os_render_backend_gdi_set_clip,
    lh_os_render_backend_gdi_fill_mask,
};

/* Same slots; null round → canvas AA via fill_round_rect_by_rects. */
const lh_ui_canvas_backend_t lh_os_render_backend_gdi_soft = {
    lh_os_render_backend_gdi_begin,
    lh_os_render_backend_gdi_end,
    lh_os_render_backend_gdi_clear,
    lh_os_render_backend_gdi_fill_rect,
    lh_null,
    lh_os_render_backend_gdi_set_clip,
    lh_os_render_backend_gdi_fill_mask,
};

/* ── Software table ──────────────────────────────────────────────────────── */

lh_void
lh_os_render_backend_gdi_sw_begin(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ui_pixmap_t pixmap;

    lh_ui_pixmap_init_empty(lh_addr_of(pixmap));
    if (lh_os_render_backend_gdi_begin_surface(gdi))
    {
        (void)lh_ui_surface_get_pixmap(lh_addr_of(gdi->surface), lh_addr_of(pixmap));
    }
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(gdi->sw), lh_addr_of(pixmap));
}

lh_void
lh_os_render_backend_gdi_sw_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_clear(lh_addr_of(lh_os_render_backend_gdi_context_from(context)->sw), color);
}

lh_void
lh_os_render_backend_gdi_sw_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->rect_calls;
    lh_ui_canvas_sw_fill_rect(lh_addr_of(gdi->sw), rect, color);
}

lh_bool_t
lh_os_render_backend_gdi_sw_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                            const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->round_calls;
    return lh_ui_canvas_sw_fill_round_rect(lh_addr_of(gdi->sw), rect, radius, color);
}

lh_void
lh_os_render_backend_gdi_sw_set_clip(lh_ptr context, const lh_ui_rect_t *clip)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->clip_calls;
    lh_ui_canvas_sw_set_clip(lh_addr_of(gdi->sw), clip);
}

lh_bool_t
lh_os_render_backend_gdi_sw_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                      const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->mask_calls;
    return lh_ui_canvas_sw_fill_mask(lh_addr_of(gdi->sw), origin, mask, color);
}

const lh_ui_canvas_backend_t lh_os_render_backend_gdi_sw = {
    lh_os_render_backend_gdi_sw_begin,
    lh_os_render_backend_gdi_end,
    lh_os_render_backend_gdi_sw_clear,
    lh_os_render_backend_gdi_sw_fill_rect,
    lh_os_render_backend_gdi_sw_fill_round_rect,
    lh_os_render_backend_gdi_sw_set_clip,
    lh_os_render_backend_gdi_sw_fill_mask,
};

lh_void
lh_os_render_backend_gdi_context_init(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    self->hwnd = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    self->hdc = lh_null;
    lh_ui_surface_init(lh_addr_of(self->surface));
    self->frame = lh_null;
    self->clip_region = lh_os_system_hdc_region_create();
    /* Without a region set_clip cannot clip, while the canvas would trust it to. */
    lh_assert_runtime_ref(self->clip_region);
    lh_ui_canvas_sw_init(lh_addr_of(self->sw));
    self->mask_calls = 0U;
    self->rect_calls = 0U;
    self->round_calls = 0U;
    self->clip_calls = 0U;
    self->frame_start_us = 0;
    (void)lh_os_render_backend_gdi_plus_acquire();
}

lh_void
lh_os_render_backend_gdi_context_deinit(lh_os_render_backend_gdi_context_t *self)
{
    lh_os_render_backend_gdi_end_frame(self);
    lh_ui_canvas_sw_init(lh_addr_of(self->sw));
    lh_ui_surface_deinit(lh_addr_of(self->surface));
    lh_os_system_hdc_region_destroy(self->clip_region);
    self->hwnd = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    self->hdc = lh_null;
    self->clip_region = lh_null;
    self->mask_calls = 0U;
    self->rect_calls = 0U;
    self->round_calls = 0U;
    self->clip_calls = 0U;
    self->frame_start_us = 0;
    lh_os_render_backend_gdi_plus_release();
}

lh_void
lh_os_render_backend_gdi_context_set_hwnd(lh_os_render_backend_gdi_context_t *self,
                                          lh_os_system_window_handle_t hwnd)
{
    lh_assert_runtime_ref(self);
    self->hwnd = hwnd;
}

lh_os_system_window_handle_t
lh_os_render_backend_gdi_context_get_hwnd(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hwnd;
}

lh_void
lh_os_render_backend_gdi_context_set_hdc(lh_os_render_backend_gdi_context_t *self, lh_ptr hdc)
{
    lh_assert_runtime_ref(self);
    self->hdc = hdc;
}

lh_ptr
lh_os_render_backend_gdi_context_get_hdc(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->hdc;
}
