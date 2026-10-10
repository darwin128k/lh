/**
 * @file fields.h
 * @brief Member fields of ::lh_os_window_t.
 */

#ifndef LH_OS_WINDOW_FIELDS_H
#define LH_OS_WINDOW_FIELDS_H

#include <lh/bool.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/numeric/types.h>
#include <lh/os/window/frame.h>
#include <lh/os/window/on/click/cb.h>
#include <lh/os/window/on/close/cb.h>
#include <lh/os/window/on/move/cb.h>
#include <lh/os/window/on/paint/cb.h>
#include <lh/os/window/on/press/cb.h>
#include <lh/os/window/on/release/cb.h>
#include <lh/os/window/on/resize/cb.h>
#include <lh/os/window/on/zone/cb.h>
#include <lh/os/window/on/key/cb.h>
#include <lh/os/window/on/text/cb.h>
#include <lh/os/window/on/tick/cb.h>
#include <lh/os/window/on/wheel/cb.h>
#include <lh/os/window/placement.h>
#include <lh/ptr.h>

/* Declared here and defined in `lh/ui/frame/stats.h`: a window holds a pointer to it
   and needs nothing else from the UI to do so. */
struct lh_ui_frame_stats;

/**
 * @def lh_os_window_fields(handle_type, app_type, window_type)
 * @brief Native handle, app or parent, children, input/paint/close notify, and link.
 *
 * Top-level windows are added from outside into the app list (`app` set).
 * Index 0 there is the main window. Modal children sit in a parent's
 * `children` list. Memory is not owned — only the links.
 *
 * `frame`, `corner`, `on_zone` and `on_zone_context` are how a caller styles the
 * window instead of taking the OS frame: `frame` picks whose frame it is, `corner`
 * cuts that window to a rounded region, and `on_zone` answers what the window system
 * should do with each point — move, resize, or leave it to the app. See
 * ::lh_os_window_set_frame, ::lh_os_window_set_corner_radius and
 * ::lh_os_window_set_on_zone.
 *
 * `placement` is where the window opens for the first time; it is read at creation
 * like `frame` and `corner`. See ::lh_os_window_set_placement.
 *
 * `paint_dc` is set only for the duration of ::lh_os_window_on_paint_fn.
 * `frame_stats` is the caller's, and every paint is recorded into it when it is set
 * (::lh_os_window_set_frame_stats).
 * Click synthesis (press then release without a drag) lives above the OS:
 * the backend fires press / move / release / wheel / key / text; ::lh_os_window_on_click
 * stays for callers that still wire it.
 *
 * `on_tick` and `tick_ms` are the window's timer: `tick_ms` is the period the window
 * system was asked for and `0` means no timer is running. It exists because an app
 * that polls something has to be told to come back, and the only thing that comes back
 * on its own is a paint -- and work done inside a paint can only ask for the **next**
 * one, which is a spin, not a schedule. See ::lh_os_window_set_on_tick.
 *
 * @param handle_type Type of `handle` (::lh_os_system_window_handle_t).
 * @param app_type    Type of the owning app pointer (::lh_os_app_t).
 * @param window_type Type of the parent / child window pointer.
 */
#define lh_os_window_fields(handle_type, app_type, window_type)                                     \
    handle_type handle;                                                                             \
    app_type *app;                                                                                  \
    window_type *parent;                                                                            \
    lh_list_t children;                                                                             \
    lh_list_node_t link;                                                                            \
    lh_bool_t modal;                                                                                \
    lh_bool_t closing;                                                                              \
    lh_os_window_frame_t frame;                                                                    \
    int corner;                                                                                    \
    lh_os_window_placement_t placement;                                                            \
    lh_bool_t maximized;                                                                           \
    lh_os_window_on_zone_cb on_zone;                                                              \
    lh_ptr on_zone_context;                                                                        \
    lh_os_window_on_resize_cb on_resize;                                                          \
    lh_ptr on_resize_context;                                                                      \
    lh_ptr paint_dc;                                                                                \
    int paint_left;                                                                                 \
    int paint_top;                                                                                  \
    int paint_right;                                                                                \
    int paint_bottom;                                                                               \
    lh_os_window_on_paint_cb on_paint;                                                              \
    lh_ptr on_paint_context;                                                                        \
    struct lh_ui_frame_stats *frame_stats;                                                          \
    lh_os_window_on_press_cb on_press;                                                            \
    lh_ptr on_press_context;                                                                        \
    lh_os_window_on_move_cb on_move;                                                                \
    lh_ptr on_move_context;                                                                         \
    lh_os_window_on_release_cb on_release;                                                          \
    lh_ptr on_release_context;                                                                      \
    lh_os_window_on_wheel_cb on_wheel;                                                              \
    lh_ptr on_wheel_context;                                                                        \
    lh_os_window_on_key_cb on_key;                                                                  \
    lh_ptr on_key_context;                                                                          \
    lh_os_window_on_text_cb on_text;                                                                \
    lh_ptr on_text_context;                                                                         \
    lh_os_window_on_click_cb on_click;                                                              \
    lh_ptr on_click_context;                                                                        \
    lh_os_window_on_close_cb on_close;                                                              \
    lh_ptr on_close_context;                                                                        \
    lh_os_window_on_tick_cb on_tick;                                                                \
    lh_ptr on_tick_context;                                                                         \
    lh_u32_t tick_ms

#endif /* LH_OS_WINDOW_FIELDS_H */
