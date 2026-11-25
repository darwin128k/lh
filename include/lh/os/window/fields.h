/**
 * @file fields.h
 * @brief Member fields of ::lh_os_window_t.
 */

#ifndef LH_OS_WINDOW_FIELDS_H
#define LH_OS_WINDOW_FIELDS_H

#include <lh/core/list.h>
#include <lh/core/input/sink/cb.h>
#include <lh/os/window/frame.h>
#include <lh/os/window/on/close/cb.h>
#include <lh/os/window/on/paint/cb.h>
#include <lh/os/window/on/resize/cb.h>
#include <lh/os/window/on/tick/cb.h>
#include <lh/os/window/on/zone/cb.h>
#include <lh/os/window/placement.h>

/**
 * @def lh_os_window_fields(app_type, window_type, controller_type, backend_type)
 * @brief The tree, the place, the reactions, the input consumer, and the backend.
 *
 * Everything here belongs to the **application**, not to the operating system:
 * the tree of windows, the place the window occupies, what the application does
 * when something happens to it, and the recognizer that reads its input. What
 * the backend keeps for itself is one opaque pointer, `native`.
 *
 * Top-level windows are added from outside into the app list (`app` set).
 * Index 0 there is the main window. Modal children sit in a parent's
 * `children` list. Memory is not owned — only the links.
 *
 * `x`, `y`, `width` and `height` are the **place** of the window: where it is
 * and how big it is. With an operating system the backend reports the size
 * (::lh_os_window_on_native_resize); without one the application says so itself
 * (::lh_os_window_set_size). See ::lh_os_window_get_rect and
 * ::lh_os_window_set_rect.
 *
 * The reactions are the application's: `on_resize` is told when the window
 * changed size, `on_paint` when it is to be drawn, `on_input` for every raw
 * event, `on_close` when the window dies, and `on_tick` on the window's timer.
 * Each has its context beside it, and `::lh_null` clears the slot.
 *
 * `tick_ms` is the period the application's timer runs at; `0` means no timer.
 * The backend runs it when it has a timer (::lh_os_window_set_on_tick), and the
 * application keeps it either way.
 *
 * `controller` is the standard consumer of the input: every event the sink saw
 * goes on to it (::lh_os_controller_feed), a focus loss also cancels it, and the
 * window keeps its recognizer's timers running (::lh_os_controller_update) on the
 * window timer; see ::lh_os_window_set_controller. `controller_armed` is true
 * while the window timer runs for the controller alone -- no `on_tick` asked for
 * one -- to bring the recognizer's next deadline (a long press) on time.
 *
 * `backend` and `backend_context` are the table the window calls down through
 * (::lh_os_window_backend_t) and its context, taken from the application when
 * the window is opened. `native` is the backend's own state for this window,
 * opaque here; ::lh_null means the window is not open.
 *
 * `paint_target` and the paint rectangle are what the backend handed the
 * current paint (::lh_os_window_on_native_paint): a device context on Win32,
 * the backend's paint context on X11 and AppKit, a pixmap on a board. They are
 * empty outside a paint.
 *
 * `frame`, `corner` and `placement` are the application's **asks**: whose frame
 * the window has, how its corners are cut and where it opens. The backend reads
 * them in `open`. `maximized` is the state the backend last reported, and
 * `on_zone` the application's answer to the hit test. `pressed_buttons` is what
 * the input left held, whoever fed the input.
 *
 * @param app_type        Type of the owning app pointer (::lh_os_app_t).
 * @param window_type     Type of the parent / child window pointer.
 * @param controller_type Type of the controller pointer (::lh_os_controller_t).
 * @param backend_type    Type of the backend table (::lh_os_window_backend_t).
 */
#define lh_os_window_fields(app_type, window_type, controller_type, backend_type)                  \
    app_type *app;                                                                                 \
    window_type *parent;                                                                           \
    lh_list_t children;                                                                            \
    lh_list_node_t link;                                                                           \
    lh_bool_t modal;                                                                               \
    lh_bool_t closing;                                                                             \
    int x;                                                                                         \
    int y;                                                                                         \
    int width;                                                                                     \
    int height;                                                                                    \
    lh_os_window_on_resize_cb on_resize;                                                           \
    lh_ptr on_resize_context;                                                                      \
    lh_os_window_on_paint_cb on_paint;                                                             \
    lh_ptr on_paint_context;                                                                       \
    lh_input_sink_cb on_input;                                                                     \
    lh_ptr on_input_context;                                                                       \
    lh_os_window_on_close_cb on_close;                                                             \
    lh_ptr on_close_context;                                                                       \
    lh_os_window_on_tick_cb on_tick;                                                               \
    lh_ptr on_tick_context;                                                                        \
    lh_u32_t tick_ms;                                                                              \
    controller_type *controller;                                                                   \
    lh_bool_t controller_armed;                                                                    \
    const backend_type *backend;                                                                   \
    lh_ptr backend_context;                                                                        \
    lh_ptr native;                                                                                 \
    lh_ptr paint_target;                                                                           \
    int paint_left;                                                                                \
    int paint_top;                                                                                 \
    int paint_right;                                                                               \
    int paint_bottom;                                                                              \
    lh_os_window_frame_t frame;                                                                    \
    int corner;                                                                                    \
    lh_os_window_placement_t placement;                                                            \
    lh_bool_t maximized;                                                                           \
    lh_os_window_on_zone_cb on_zone;                                                               \
    lh_ptr on_zone_context;                                                                        \
    lh_u32_t pressed_buttons

#endif /* LH_OS_WINDOW_FIELDS_H */
