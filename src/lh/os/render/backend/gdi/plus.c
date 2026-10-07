/**
 * @file plus.c
 * @brief Render-layer wrap of ::lh_os_system_gdiplus_* for UI types.
 */

#include <lh/cast/static.h>
#include <lh/os/render/backend/gdi/plus.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

lh_bool_t
lh_os_render_backend_gdi_plus_acquire(lh_void)
{
    return lh_os_system_gdiplus_acquire();
}

lh_void
lh_os_render_backend_gdi_plus_release(lh_void)
{
    lh_os_system_gdiplus_release();
}

lh_bool_t
lh_os_render_backend_gdi_plus_is_ready(lh_void)
{
    return lh_os_system_gdiplus_is_ready();
}

lh_os_system_gdiplus_frame_t
lh_os_render_backend_gdi_plus_frame_begin(lh_ptr hdc)
{
    return lh_os_system_gdiplus_frame_begin(hdc);
}

lh_void
lh_os_render_backend_gdi_plus_frame_end(lh_os_system_gdiplus_frame_t frame)
{
    lh_os_system_gdiplus_frame_end(frame);
}

lh_bool_t
lh_os_render_backend_gdi_plus_fill_mask(lh_os_system_gdiplus_frame_t frame, const lh_ui_point_t *origin,
                                        const lh_ui_mask_t *mask, const lh_ui_color_t *color)
{
    return lh_os_system_gdiplus_frame_fill_mask(
        frame, lh_cast_static(int, lh_ui_point_get_x(origin)),
        lh_cast_static(int, lh_ui_point_get_y(origin)), lh_ui_mask_get_width(mask),
        lh_ui_mask_get_height(mask), lh_ui_mask_get_bpp(mask), mask->row_bytes, mask->bits,
        lh_ui_color_get_r(color), lh_ui_color_get_g(color), lh_ui_color_get_b(color),
        lh_ui_color_get_a(color));
}

lh_bool_t
lh_os_render_backend_gdi_plus_fill_round_rect(lh_os_system_gdiplus_frame_t frame,
                                              const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                                              const lh_ui_color_t *color)
{
    const lh_ui_point_t *origin = lh_ui_rect_get_origin_as_const(rect);
    const lh_ui_point_t corner = lh_ui_rect_far(rect);

    return lh_os_system_gdiplus_frame_fill_round_rect(
        frame, lh_cast_static(int, lh_ui_point_get_x(origin)),
        lh_cast_static(int, lh_ui_point_get_y(origin)),
        lh_cast_static(int, lh_ui_point_get_x(lh_addr_of(corner))),
        lh_cast_static(int, lh_ui_point_get_y(lh_addr_of(corner))), lh_cast_static(int, radius),
        lh_ui_color_get_r(color), lh_ui_color_get_g(color), lh_ui_color_get_b(color),
        lh_ui_color_get_a(color));
}

lh_void
lh_os_render_backend_gdi_plus_set_clip(lh_os_system_gdiplus_frame_t frame, int left, int top,
                                       int right, int bottom)
{
    lh_os_system_gdiplus_frame_set_clip(frame, left, top, right, bottom);
}

lh_void
lh_os_render_backend_gdi_plus_clear_clip(lh_os_system_gdiplus_frame_t frame)
{
    lh_os_system_gdiplus_frame_clear_clip(frame);
}
