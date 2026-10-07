/**
 * @file fields.h
 * @brief Member fields of ::lh_os_window_t.
 */

#ifndef LH_OS_WINDOW_FIELDS_H
#define LH_OS_WINDOW_FIELDS_H

#include <lh/bool.h>
#include <lh/list.h>
#include <lh/list/node.h>
#include <lh/os/window/on/click/cb.h>
#include <lh/os/window/on/close/cb.h>
#include <lh/os/window/on/move/cb.h>
#include <lh/os/window/on/paint/cb.h>
#include <lh/os/window/on/press/cb.h>
#include <lh/os/window/on/release/cb.h>
#include <lh/os/window/on/key/cb.h>
#include <lh/os/window/on/text/cb.h>
#include <lh/os/window/on/wheel/cb.h>
#include <lh/ptr.h>

/**
 * @def lh_os_window_fields(handle_type, app_type, window_type)
 * @brief Native handle, app or parent, children, input/paint/close notify, and link.
 *
 * Top-level windows are added from outside into the app list (`app` set).
 * Index 0 there is the main window. Modal children sit in a parent's
 * `children` list. Memory is not owned — only the links.
 *
 * `chrome_title` and `chrome_corner` are the window's own frame: with
 * `chrome_title` at zero the OS draws its title bar and frame, and above zero the
 * caller draws that many rows of chrome itself and the window is created without an
 * OS frame at all. See ::lh_os_window_set_chrome.
 *
 * `paint_dc` is set only for the duration of ::lh_os_window_on_paint_fn.
 * Click synthesis (press then release without a drag) lives above the OS:
 * the backend fires press / move / release / wheel / key / text; ::lh_os_window_on_click
 * stays for callers that still wire it.
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
    int chrome_title;                                                                               \
    int chrome_corner;                                                                              \
    lh_ptr paint_dc;                                                                                \
    int paint_left;                                                                                 \
    int paint_top;                                                                                  \
    int paint_right;                                                                                \
    int paint_bottom;                                                                               \
    lh_os_window_on_paint_cb on_paint;                                                              \
    lh_ptr on_paint_context;                                                                        \
    lh_os_window_on_press_cb on_press;                                                              \
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
    lh_ptr on_close_context

#endif /* LH_OS_WINDOW_FIELDS_H */
