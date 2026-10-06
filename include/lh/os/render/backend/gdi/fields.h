/**
 * @file fields.h
 * @brief Member fields of ::lh_os_render_backend_gdi_context_t.
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_FIELDS_H
#define LH_OS_RENDER_BACKEND_GDI_FIELDS_H

#include <lh/os/system/gdiplus.h>
#include <lh/ui/surface.h>

/**
 * @def lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, frame_type, region_type, count_type, tick_type)
 * @brief Window, paint-time destination DC, off-screen surface, GDI+ frame,
 *        reusable clip region, per-primitive call counts, and frame start tick.
 *
 * Drawing goes to the surface; ::lh_os_render_backend_gdi_end presents it into
 * `hdc`. The surface is owned by the context. `frame` lives from begin to end
 * (Graphics + reusable mask bitmap / path / solid). `clip_region` lives for
 * the context lifetime. Counters and `frame_start_us` reset in `begin`.
 *
 * @param hwnd_type    ::lh_os_system_window_handle_t.
 * @param hdc_type     Paint DC as ::lh_ptr.
 * @param surface_type ::lh_ui_surface_t.
 * @param frame_type   ::lh_os_system_gdiplus_frame_t.
 * @param region_type  Reusable clip region as ::lh_ptr.
 * @param count_type   Unsigned call counter.
 * @param tick_type    Frame start time (::lh_u64_t microseconds).
 */
#define lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, frame_type, region_type, \
                                        count_type, tick_type)                                     \
    hwnd_type hwnd;                                                                                \
    hdc_type hdc;                                                                                  \
    surface_type surface;                                                                          \
    frame_type frame;                                                                              \
    region_type clip_region;                                                                       \
    count_type mask_calls;                                                                         \
    count_type rect_calls;                                                                         \
    count_type round_calls;                                                                        \
    count_type clip_calls;                                                                         \
    tick_type frame_start_us

#endif /* LH_OS_RENDER_BACKEND_GDI_FIELDS_H */
