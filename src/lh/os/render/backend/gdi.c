/**
 * @file gdi.c
 * @brief GDI as a screen: ::lh_ui_canvas_backend_sw into a surface, `BitBlt`.
 */
#include <lh/assert.h>

#include <lh/os/render/backend/gdi.h>
#include <lh/os/system/surface.h>
#include <lh/os/system/window.h>
#include <lh/os/tick.h>
#include <lh/util/return.h>

/* -- Context --------------------------------------------------------------- */

lh_os_render_backend_gdi_context_t *
lh_os_render_backend_gdi_context_from(lh_ptr context)
{
    lh_os_render_backend_gdi_context_t *gdi =
        lh_ptr_rcast(lh_os_render_backend_gdi_context_t, context);

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

lh_void
lh_os_render_backend_gdi_context_set_logical_size(lh_os_render_backend_gdi_context_t *self,
                                                  unsigned width, unsigned height)
{
    lh_assert_runtime_ref(self);
    lh_os_render_backend_gdi_context_set_logical_w(self, width);
    lh_os_render_backend_gdi_context_set_logical_h(self, height);
}

lh_ui_pixmap_format_t
lh_os_render_backend_gdi_context_get_format(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ui_surface_get_format(lh_os_render_backend_gdi_context_get_surface(self));
}

lh_void
lh_os_render_backend_gdi_context_set_format(lh_os_render_backend_gdi_context_t *self,
                                            lh_ui_pixmap_format_t format)
{
    lh_assert_runtime_ref(self);
    lh_ui_surface_set_format(lh_os_render_backend_gdi_context_get_surface_mut(self), format);
}

lh_void
lh_os_render_backend_gdi_reset_counters(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_render_backend_gdi_context_set_mask_calls(self, 0U);
    lh_os_render_backend_gdi_context_set_rect_calls(self, 0U);
    lh_os_render_backend_gdi_context_set_round_calls(self, 0U);
    lh_os_render_backend_gdi_context_set_clip_calls(self, 0U);
    lh_os_render_backend_gdi_context_set_frame_start_us(self, lh_os_tick_us());
}

lh_void
lh_os_render_backend_gdi_context_init(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    lh_os_render_backend_gdi_context_set_hwnd(self, LH_OS_SYSTEM_WINDOW_HANDLE_INVALID);
    lh_os_render_backend_gdi_context_set_hdc(self, lh_null);
    lh_ui_surface_init(lh_os_render_backend_gdi_context_get_surface_mut(self));
    /* ARGB8888, which is what ::lh_ui_surface_init starts with, and staying there
       is a measured decision rather than a default nobody looked at: RGB565 draws
       a desktop frame measurably faster and presents it far slower. The numbers
       are on ::lh_os_render_backend_gdi_context_get_format. */
    lh_ui_canvas_sw_init(lh_os_render_backend_gdi_context_get_sw_mut(self));
    lh_ui_point_init(lh_os_render_backend_gdi_context_get_present_at_mut(self), lh_ui_scalar(0),
                     lh_ui_scalar(0));
    lh_os_render_backend_gdi_context_set_logical_w(self, 0);
    lh_os_render_backend_gdi_context_set_logical_h(self, 0);
    lh_os_render_backend_gdi_reset_counters(self);
    lh_os_render_backend_gdi_context_set_frame_start_us(self, 0);
}

/* -- Counters -------------------------------------------------------------- */

lh_void
lh_os_render_backend_gdi_context_deinit(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    lh_ui_surface_deinit(lh_os_render_backend_gdi_context_get_surface_mut(self));
    lh_os_render_backend_gdi_context_init(self);
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
    return lh_os_tick_us() - lh_os_render_backend_gdi_context_get_frame_start_us(self);
}

/* -- Backend slots --------------------------------------------------------- */

lh_void
lh_os_render_backend_gdi_begin_area(lh_ptr context, const lh_ui_rect_t *area)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);
    lh_ui_pixmap_t pixmap;

    /* The surface is the area and nothing more: a whole frame of 800x600 is
       960 KB of RGB565 DIB, a 32-row strip is 48 KB, and the pixels that did not
       change are never cleared, drawn or blitted at all. */
    lh_os_render_backend_gdi_context_set_present_at(gdi, lh_ptr_deref(lh_ui_rect_get_origin(area)));
    (void)lh_ui_surface_set_size(lh_os_render_backend_gdi_context_get_surface_mut(gdi),
                                 lh_ptr_deref(lh_ui_rect_get_size(area)));
    lh_ui_pixmap_init_empty(lh_addr_of(pixmap));
    (void)lh_ui_surface_get_pixmap(lh_os_render_backend_gdi_context_get_surface_mut(gdi),
                                   lh_addr_of(pixmap));
    lh_ui_canvas_sw_use_pixmap(lh_os_render_backend_gdi_context_get_sw_mut(gdi),
                               lh_addr_of(pixmap));
    /* Nothing of this frame is drawn yet, and the area is not necessarily all of
       it: a strip is handed the damage it carries, and only that is cleared and
       drawn. What the rest of the buffer holds is not this frame's, and the
       `end` slot says which parts of it are. */
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

    if (lh_os_render_backend_gdi_context_get_logical_w(gdi) > 0u &&
        lh_os_render_backend_gdi_context_get_logical_h(gdi) > 0u)
    {
        width = lh_cast_static(int, lh_os_render_backend_gdi_context_get_logical_w(gdi));
        height = lh_cast_static(int, lh_os_render_backend_gdi_context_get_logical_h(gdi));
    }
    else
    {
        lh_return_if(!lh_os_system_window_get_client_size(
            lh_os_render_backend_gdi_context_get_hwnd(gdi), lh_addr_of(width), lh_addr_of(height)));
    }
    lh_ui_size_init(lh_addr_of(size), lh_cast_static(lh_ui_scalar_t, width),
                    lh_cast_static(lh_ui_scalar_t, height));
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0),
                    lh_ui_size_get_width(lh_addr_of(size)),
                    lh_ui_size_get_height(lh_addr_of(size)));
    lh_os_render_backend_gdi_begin_area(context, lh_addr_of(area));
}

lh_void
lh_os_render_backend_gdi_end(lh_ptr context, const lh_ui_rects_t *drawn)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_return_if(lh_null_eq(lh_os_render_backend_gdi_context_get_hdc(gdi)));
    if (lh_os_render_backend_gdi_context_get_logical_w(gdi) > 0u &&
        lh_os_render_backend_gdi_context_get_logical_h(gdi) > 0u)
    {
        int client_w = 0;
        int client_h = 0;

        if (lh_os_system_window_get_client_size(lh_os_render_backend_gdi_context_get_hwnd(gdi),
                                                lh_addr_of(client_w), lh_addr_of(client_h)) &&
            (client_w != lh_cast_static(int, lh_os_render_backend_gdi_context_get_logical_w(gdi)) ||
             client_h != lh_cast_static(int, lh_os_render_backend_gdi_context_get_logical_h(gdi))))
        {
            /* 0x00BBGGRR of `--canvas` #EDF0F5, for the letterbox. */
            (void)lh_os_system_surface_present_fit(
                lh_ui_surface_get_handle(lh_os_render_backend_gdi_context_get_surface(gdi)),
                lh_os_render_backend_gdi_context_get_hdc(gdi), client_w, client_h, 0x00F5F0EDu);
            return;
        }
    }
    /* @p drawn is the canvas's answer to what this frame drew, and it is a list
       because the clips a frame is cut to need not touch: their hull would carry
       the gap between two cuts, and the gap belongs to neither.

       The whole surface only when the list is empty, which means the frame was
       never cut and drew all of it. The rest of the buffer was never written this
       frame: `begin_area` drops the DIB whenever the area changes size, which
       between strips it always does, and a new DIB is whatever GDI hands back.
       Blitting that put a strip's leftover picture under the widget that moved, as
       vertical streaks next to it. */
    if (lh_null_eq(drawn) || lh_ui_rects_is_empty(drawn))
    {
        (void)lh_ui_surface_present_at(
            lh_os_render_backend_gdi_context_get_surface_mut(gdi),
            lh_os_render_backend_gdi_context_get_hdc(gdi),
            lh_ptr_deref(lh_os_render_backend_gdi_context_get_present_at(gdi)));
        return;
    }
    (void)lh_ui_surface_present_rects(
        lh_os_render_backend_gdi_context_get_surface_mut(gdi),
        lh_os_render_backend_gdi_context_get_hdc(gdi), drawn,
        lh_ptr_deref(lh_os_render_backend_gdi_context_get_present_at(gdi)));
}

lh_void
lh_os_render_backend_gdi_clear(lh_ptr context, const lh_ui_color_t *color)
{
    lh_ui_canvas_sw_clear(lh_addr_of(lh_os_render_backend_gdi_context_from(context)->sw), color);
}

lh_void
lh_os_render_backend_gdi_fill_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                   const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_os_render_backend_gdi_context_set_rect_calls(
        gdi, lh_os_render_backend_gdi_context_get_rect_calls(gdi) + 1U);
    lh_ui_canvas_sw_fill_rect(lh_os_render_backend_gdi_context_get_sw_mut(gdi), rect, color);
}

lh_bool_t
lh_os_render_backend_gdi_fill_round_rect(lh_ptr context, const lh_ui_rect_t *rect,
                                         lh_ui_scalar_t radius, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_os_render_backend_gdi_context_set_round_calls(
        gdi, lh_os_render_backend_gdi_context_get_round_calls(gdi) + 1U);
    return lh_ui_canvas_sw_fill_round_rect(lh_os_render_backend_gdi_context_get_sw_mut(gdi), rect,
                                           radius, color);
}

lh_void
lh_os_render_backend_gdi_set_clip(lh_ptr context, const lh_ui_canvas_clip_t *clip)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_os_render_backend_gdi_context_set_clip_calls(
        gdi, lh_os_render_backend_gdi_context_get_clip_calls(gdi) + 1U);
    /* Nothing is counted here any more. What the frame drew is the canvas's
       answer and it arrives with the `end` slot, because working it out here would
       be answering it a second time and differently from every other backend.
       Rounded cuts are inside the rect they come with, and un-clipping (a `pop`
       back to no clip) says nothing new about what was drawn. */
    lh_ui_canvas_sw_use_clip(lh_os_render_backend_gdi_context_get_sw_mut(gdi), clip);
}

lh_bool_t
lh_os_render_backend_gdi_fill_mask(lh_ptr context, const lh_ui_point_t *origin,
                                   const lh_ui_mask_t *mask, const lh_ui_color_t *color)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    lh_os_render_backend_gdi_context_set_mask_calls(
        gdi, lh_os_render_backend_gdi_context_get_mask_calls(gdi) + 1U);
    return lh_ui_canvas_sw_fill_mask(lh_os_render_backend_gdi_context_get_sw_mut(gdi), origin, mask,
                                     color);
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

    return lh_ui_canvas_sw_shadow(lh_os_render_backend_gdi_context_get_sw_mut(gdi), rect, radius,
                                  shadow);
}

lh_bool_t
lh_os_render_backend_gdi_blur(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t blur_radius,
                              lh_u8_t *scratch, lh_usize_t bytes)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    return lh_ui_canvas_sw_blur(lh_os_render_backend_gdi_context_get_sw_mut(gdi), rect, blur_radius,
                                scratch, bytes);
}

lh_bool_t
lh_os_render_backend_gdi_glass(lh_ptr context, const lh_ui_rect_t *rect, lh_ui_scalar_t corner,
                               lh_ui_scalar_t blur_radius, const lh_ui_color_t *tint,
                               lh_u8_t *scratch, lh_usize_t bytes)
{
    lh_os_render_backend_gdi_context_t *gdi = lh_os_render_backend_gdi_context_from(context);

    return lh_ui_canvas_sw_glass(lh_os_render_backend_gdi_context_get_sw_mut(gdi), rect, corner,
                                 blur_radius, tint, scratch, bytes);
}

const lh_ui_canvas_backend_t lh_os_render_backend_gdi = {
    lh_os_render_backend_gdi_begin,     lh_os_render_backend_gdi_begin_area,
    lh_os_render_backend_gdi_end,       lh_os_render_backend_gdi_clear,
    lh_os_render_backend_gdi_fill_rect, lh_os_render_backend_gdi_fill_round_rect,
    lh_os_render_backend_gdi_set_clip,  lh_os_render_backend_gdi_fill_mask,
    lh_os_render_backend_gdi_shadow,    lh_os_render_backend_gdi_blur,
    lh_os_render_backend_gdi_glass,     lh_null,
};

const lh_ui_surface_t *
lh_os_render_backend_gdi_context_get_surface(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->surface);
}

lh_ui_surface_t *
lh_os_render_backend_gdi_context_get_surface_mut(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->surface);
}

const lh_ui_canvas_sw_t *
lh_os_render_backend_gdi_context_get_sw(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sw);
}

lh_ui_canvas_sw_t *
lh_os_render_backend_gdi_context_get_sw_mut(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->sw);
}

const lh_ui_point_t *
lh_os_render_backend_gdi_context_get_present_at(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->present_at);
}

lh_ui_point_t *
lh_os_render_backend_gdi_context_get_present_at_mut(lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->present_at);
}

lh_u32_t
lh_os_render_backend_gdi_context_get_logical_w(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->logical_w;
}

lh_void
lh_os_render_backend_gdi_context_set_logical_w(lh_os_render_backend_gdi_context_t *self,
                                               lh_u32_t logical_w)
{
    lh_assert_runtime_ref(self);
    self->logical_w = logical_w;
}

lh_u32_t
lh_os_render_backend_gdi_context_get_logical_h(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->logical_h;
}

lh_void
lh_os_render_backend_gdi_context_set_logical_h(lh_os_render_backend_gdi_context_t *self,
                                               lh_u32_t logical_h)
{
    lh_assert_runtime_ref(self);
    self->logical_h = logical_h;
}

lh_void
lh_os_render_backend_gdi_context_set_mask_calls(lh_os_render_backend_gdi_context_t *self,
                                                lh_u32_t mask_calls)
{
    lh_assert_runtime_ref(self);
    self->mask_calls = mask_calls;
}

lh_void
lh_os_render_backend_gdi_context_set_rect_calls(lh_os_render_backend_gdi_context_t *self,
                                                lh_u32_t rect_calls)
{
    lh_assert_runtime_ref(self);
    self->rect_calls = rect_calls;
}

lh_void
lh_os_render_backend_gdi_context_set_round_calls(lh_os_render_backend_gdi_context_t *self,
                                                 lh_u32_t round_calls)
{
    lh_assert_runtime_ref(self);
    self->round_calls = round_calls;
}

lh_void
lh_os_render_backend_gdi_context_set_clip_calls(lh_os_render_backend_gdi_context_t *self,
                                                lh_u32_t clip_calls)
{
    lh_assert_runtime_ref(self);
    self->clip_calls = clip_calls;
}

lh_u64_t
lh_os_render_backend_gdi_context_get_frame_start_us(const lh_os_render_backend_gdi_context_t *self)
{
    lh_assert_runtime_ref(self);
    return self->frame_start_us;
}

lh_void
lh_os_render_backend_gdi_context_set_frame_start_us(lh_os_render_backend_gdi_context_t *self,
                                                    lh_u64_t frame_start_us)
{
    lh_assert_runtime_ref(self);
    self->frame_start_us = frame_start_us;
}

lh_void
lh_os_render_backend_gdi_context_set_present_at(lh_os_render_backend_gdi_context_t *self,
                                                lh_ui_point_t present_at)
{
    lh_assert_runtime_ref(self);
    self->present_at = present_at;
}
