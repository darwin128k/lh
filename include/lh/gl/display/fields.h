/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_display_t.
 */

#ifndef LH_GL_DISPLAY_FIELDS_H
#define LH_GL_DISPLAY_FIELDS_H

/**
 * @def lh_gl_display_fields(size_type, scalar_type, format_type, pixmap_type, sw_type,
 *                           canvas_type, sink_type, u32_type)
 * @brief Panel size, optional strip height, host pixmap view, software canvas,
 *        and a fixed sink table with an enable mask.
 *
 * Pixels are **not** owned: @c bits come from the application (SRAM strip on
 * F4VE, DIB on Windows). @c strip_height `0` means the pixmap is the full
 * panel; otherwise it is one band and the host moves it with
 * ::lh_gl_display_set_strip_y_v before each band.
 *
 * @param size_type    ::lh_gl_size_t (panel).
 * @param scalar_type  ::lh_gl_scalar_t (strip_height, strip_y).
 * @param format_type  ::lh_gl_pixmap_format_t.
 * @param pixmap_type  ::lh_gl_pixmap_t.
 * @param sw_type      ::lh_gl_canvas_sw_t.
 * @param canvas_type  ::lh_gl_canvas_t.
 * @param sink_type    ::lh_gl_present_sink_t[::LH_GL_PRESENT_SINKS_MAX].
 * @param u32_type     ::lh_u32_t (sink_count, sink_enable).
 */
#define lh_gl_display_fields(size_type, scalar_type, format_type, pixmap_type, sw_type,            \
                             canvas_type, sink_type, u32_type)                                     \
    size_type panel;                                                                               \
    scalar_type strip_height;                                                                      \
    scalar_type strip_y;                                                                           \
    format_type format;                                                                            \
    pixmap_type pixmap;                                                                            \
    sw_type sw;                                                                                    \
    canvas_type canvas;                                                                            \
    sink_type sinks[LH_GL_PRESENT_SINKS_MAX];                                                      \
    u32_type sink_count;                                                                           \
    u32_type sink_enable

#endif /* LH_GL_DISPLAY_FIELDS_H */
