/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_window_t.
 */

#ifndef LH_GX_WINDOW_FIELDS_H
#define LH_GX_WINDOW_FIELDS_H

/**
 * @def lh_gx_window_fields(window_type, surface_type, bounds_type, pixmap_type, sw_type,
 * canvas_type, render_type, present_type, pointer_type, focus_type)
 * @brief A gx screen in a desktop window, and everything between the two.
 *
 * - `window` — the system's window (::lh_os_window_t);
 * - `surface` — the system's surface the pixels live in, the window's size;
 *   `bits`, `pixmap` — those pixels as gx sees them;
 * - `sw` — the software backend drawing into `pixmap`; `canvas` — its table;
 * - `render` — the renderer of the screen;
 * - `present` — the window's present table: a frame drawn asks the system to
 *   repaint the rectangles that changed;
 * - `pointer`, `focus` — where the window's input goes.
 *
 * The members point at each other, so the record must not move after
 * ::lh_gx_window_init.
 */
#define lh_gx_window_fields(window_type, surface_type, bounds_type, pixmap_type, sw_type,          \
                            canvas_type, render_type, present_type, pointer_type, focus_type)      \
    window_type window;                                                                            \
    surface_type surface;                                                                          \
    bounds_type bits;                                                                              \
    pixmap_type pixmap;                                                                            \
    sw_type sw;                                                                                    \
    canvas_type canvas;                                                                            \
    render_type render;                                                                            \
    present_type present;                                                                          \
    pointer_type pointer;                                                                          \
    focus_type focus

#endif /* LH_GX_WINDOW_FIELDS_H */
