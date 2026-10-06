/**
 * @file fields.h
 * @brief Member fields of ::lh_os_render_backend_gdi_context_t.
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_FIELDS_H
#define LH_OS_RENDER_BACKEND_GDI_FIELDS_H

#include <lh/os/system/gdiplus.h>
#include <lh/ui/surface.h>

/**
 * @def lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, frame_type, count_type)
 * @brief Window, paint-time destination DC, off-screen surface, GDI+ frame,
 *        and fill_mask call count for the current begin/end.
 *
 * Drawing goes to the surface; ::lh_os_render_backend_gdi_end presents it into
 * `hdc`. The surface is owned by the context. `frame` lives from begin to end
 * (Graphics + reusable mask bitmap). `mask_calls` counts backend `fill_mask`
 * hits in the current frame.
 *
 * @param hwnd_type    ::lh_os_system_window_handle_t.
 * @param hdc_type     Paint DC as ::lh_ptr.
 * @param surface_type ::lh_ui_surface_t.
 * @param frame_type   ::lh_os_system_gdiplus_frame_t.
 * @param count_type   Unsigned call counter.
 */
#define lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, frame_type, count_type)  \
    hwnd_type hwnd;                                                                                 \
    hdc_type hdc;                                                                                   \
    surface_type surface;                                                                           \
    frame_type frame;                                                                               \
    count_type mask_calls

#endif /* LH_OS_RENDER_BACKEND_GDI_FIELDS_H */
