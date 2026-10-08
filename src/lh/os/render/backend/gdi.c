/**
 * @file gdi.c
 * @brief GDI as a screen: ::lh_ui_canvas_backend_sw into a surface, `BitBlt`.
 */

#include <lh/assert/runtime.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/render/backend/gdi.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/size.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/util/return.h>

/* ── Context ─────────────────────────────────────────────────────────────── */

lh_void
lh_os_render_backend_gdi_context_init(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    self->hwnd = LH_OS_SYSTEM_WINDOW_HANDLE_INVALID;
    self->hdc = lh_null;
    lh_ui_surface_init(lh_addr_of(self->surface));
    /* ARGB8888, which is what ::lh_ui_surface_init starts with, and staying there
       is a measured decision rather than a default nobody looked at: RGB565 draws
       a desktop frame measurably faster and presents it far slower. The numbers
       are on ::lh_os_render_backend_gdi_context_get_format. */
    lh_ui_canvas_sw_init(lh_addr_of(self->sw));
    lh_ui_point_init(lh_addr_of(self->present_at), lh_ui_scalar(0), lh_ui_scalar(0));
    lh_os_render_backend_gdi_reset_counters(self);
    self->frame_start_us = 0;
}

lh_void
lh_os_render_backend_gdi_context_deinit(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_surface_deinit(lh_addr_of(self->surface));
    lh_os_render_backend_gdi_context_init(self);
}

lh_os_render_backend_gdi_context_t *
lh_os_render_backend_gdi_context_from(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_ptr_rcast(lh_os_render_backend_gdi_context_t, context);

    lh_assert_runtime_ref(gdi);
    return gdi;
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

lh_ui_pixmap_format_t
lh_os_render_backend_gdi_context_get_format(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_surface_get_format(lh_addr_of(self->surface));
}

lh_void
lh_os_render_backend_gdi_context_set_format(lh_os_render_backend_gdi_context_t *self,
                                            lh_ui_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    lh_ui_surface_set_format(lh_addr_of(self->surface), format);
}

/* ── Counters ────────────────────────────────────────────────────────────── */

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

/* ── Backend slots ───────────────────────────────────────────────────────── */

lh_void
lh_os_render_backend_gdi_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ui_pixmap_t pixmap;

    /* The surface is the area and nothing more: a whole frame of 800x600 is
       960 KB of RGB565 DIB, a 32-row strip is 48 KB, and the pixels that did not
       change are never cleared, drawn or blitted at all. */
    gdi->present_at = *lh_ui_rect_get_origin_as_const(area);
    (void)lh_ui_surface_set_size(lh_addr_of(gdi->surface), *lh_ui_rect_get_size_as_const(area));
    lh_ui_pixmap_init_empty(lh_addr_of(pixmap));
    (void)lh_ui_surface_get_pixmap(lh_addr_of(gdi->surface), lh_addr_of(pixmap));
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(gdi->sw), lh_addr_of(pixmap));
    lh_os_render_backend_gdi_reset_counters(gdi);
}

lh_void
lh_os_render_backend_gdi_begin(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ui_rect_t area;
    lh_ui_size_t size;
    int width;
    int height;

    lh_return_if(!lh_os_system_window_get_client_size(gdi->hwnd, lh_addr_of(width), lh_addr_of(height)));
    lh_ui_size_init(lh_addr_of(size), lh_cast_static(lh_ui_scalar_t, width), lh_cast_static(lh_ui_scalar_t, height));
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_size_get_width(lh_addr_of(size)),
                    lh_ui_size_get_height(lh_addr_of(size)));
    lh_os_render_backend_gdi_begin_area(context, lh_addr_of(area));
}

lh_void
lh_os_render_backend_gdi_end(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    if (lh_null_ne(gdi->hdc))
    {
        (void)lh_ui_surface_present_at(lh_addr_of(gdi->surface), gdi->hdc, gdi->present_at);
    }
}

lh_void
lh_os_render_backend_gdi_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_clear(lh_addr_of(lh_os_render_backend_gdi_context_from(context)->sw), color);
}

lh_void
lh_os_render_backend_gdi_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->rect_calls;
    lh_ui_canvas_sw_fill_rect(lh_addr_of(gdi->sw), rect, color);
}

lh_bool_t
lh_os_render_backend_gdi_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                         const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->round_calls;
    return lh_ui_canvas_sw_fill_round_rect(lh_addr_of(gdi->sw), rect, radius, color);
}

lh_void
lh_os_render_backend_gdi_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->clip_calls;
    lh_ui_canvas_sw_set_clip(lh_addr_of(gdi->sw), clip);
}

lh_bool_t
lh_os_render_backend_gdi_fill_mask(lh_ptr context, const lh_ui_point_t *origin, const lh_ui_mask_t *mask,
                                   const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    ++gdi->mask_calls;
    return lh_ui_canvas_sw_fill_mask(lh_addr_of(gdi->sw), origin, mask, color);
}

/* The three effects are CPU work on the DIB, and GDI only creates the DIB and
   blits it — so they are the software ones and nothing else. Each one needs its
   own wrapper like every other slot above: a backend slot is handed the *GDI*
   context, and the software functions expect the software context, so plugging
   them in raw does not draw a little wrong, it reads a different struct. */
lh_bool_t
lh_os_render_backend_gdi_shadow(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                               const lh_ui_shadow_t *shadow)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    return lh_ui_canvas_sw_shadow(lh_addr_of(gdi->sw), rect, radius, shadow);
}

lh_bool_t
lh_os_render_backend_gdi_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                             lh_u8_t *scratch, lh_usize_t bytes)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    return lh_ui_canvas_sw_blur(lh_addr_of(gdi->sw), rect, blur_radius, scratch, bytes);
}

lh_bool_t
lh_os_render_backend_gdi_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                              lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint, lh_u8_t *scratch,
                              lh_usize_t bytes)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    return lh_ui_canvas_sw_glass(lh_addr_of(gdi->sw), rect, corner, blur_radius, tint, scratch, bytes);
}

const lh_ui_canvas_backend_t lh_os_render_backend_gdi = {
    lh_os_render_backend_gdi_begin,
    lh_os_render_backend_gdi_begin_area,
    lh_os_render_backend_gdi_end,
    lh_os_render_backend_gdi_clear,
    lh_os_render_backend_gdi_fill_rect,
    lh_os_render_backend_gdi_fill_round_rect,
    lh_os_render_backend_gdi_set_clip,
    lh_os_render_backend_gdi_fill_mask,
    lh_os_render_backend_gdi_shadow,
    lh_os_render_backend_gdi_blur,
    lh_os_render_backend_gdi_glass,
};
