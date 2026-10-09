/**
 * @file fields.h
 * @brief Member fields of ::lh_os_render_backend_gdi_context_t.
 */

#ifndef LH_OS_RENDER_BACKEND_GDI_FIELDS_H
#define LH_OS_RENDER_BACKEND_GDI_FIELDS_H

#include <lh/ui/canvas/sw.h>
#include <lh/ui/rects.h>
#include <lh/ui/surface.h>

/**
 * @def lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, sw_type, count_type, tick_type,
 *                                      point_type, rects_type)
 * @brief Window, paint-time destination DC, off-screen surface, software
 *        context, per-primitive call counts, and frame start tick.
 *
 * `sw` draws into the surface pixels (its pixmap is set in each `begin` and
 * `begin_area`); ::lh_os_render_backend_gdi_end blits the surface into `hdc`,
 * with its top-left pixel at `present_at` — `(0, 0)` for a whole-target frame,
 * the area corner for one partial strip. The surface is owned by the context
 * and is the area, never more: that is the memory a partial frame saves.
 *
 * `drawn` is what the frame actually wrote, in surface pixels, and the present
 * is exactly it: a buffer holds no promise about the pixels nobody wrote, and
 * `begin_area` frees the old one whenever the area changes size, so those bytes
 * are whatever GDI hands back. Empty after a frame that was never clipped means
 * the whole surface was drawn.
 *
 * It is a ::lh_ui_rects_t and not one rect on purpose. Every clip of a frame is
 * an intersection with the frame's own, so the union of two cuts standing apart
 * on a strip is their hull — and the hull covers the gap between them, which
 * belongs to neither. Two widgets 6 px apart, which is one row of buttons, put
 * a strip's leftover bytes into that gap.
 *
 * @param hwnd_type    ::lh_os_system_window_handle_t.
 * @param hdc_type     Paint DC as ::lh_ptr.
 * @param surface_type ::lh_ui_surface_t.
 * @param sw_type      ::lh_ui_canvas_sw_t.
 * @param count_type   Unsigned call counter.
 * @param tick_type    Frame start time (::lh_u64_t microseconds).
 * @param point_type   Type of the present corner.
 * @param rects_type   ::lh_ui_rects_t.
 */
#define lh_os_render_backend_gdi_fields(hwnd_type, hdc_type, surface_type, sw_type, count_type,       \
                                        tick_type, point_type, rects_type)                            \
    hwnd_type hwnd;                                                                                \
    hdc_type hdc;                                                                                  \
    surface_type surface;                                                                          \
    sw_type sw;                                                                                    \
    point_type present_at;                                                                         \
    rects_type drawn;                                                                              \
    count_type mask_calls;                                                                         \
    count_type rect_calls;                                                                         \
    count_type round_calls;                                                                        \
    count_type clip_calls;                                                                         \
    tick_type frame_start_us

#endif /* LH_OS_RENDER_BACKEND_GDI_FIELDS_H */
