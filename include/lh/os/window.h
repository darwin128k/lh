/**
 * @file window.h
 * @brief A native OS window — the application's window handle.
 *
 * One struct holding the bit-pattern window handle
 * (::lh_os_system_window_handle_t). The handle is opaque to the caller;
 * the actual Win32 / Xlib / Cocoa dispatch lives under
 * `lh/os/system/window.h` and is picked by CMake at configure time.
 *
 * Deliberately narrow: lifetime, show, pump, and the frame itself
 * (corner radius, dark caption, caption colors). Pixel drawing, input
 * queues, and dirty regions are layered above this header; they don't
 * belong here.
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW (itself requires ::LH_LIBRARY_OPTION_OS).
 */

#ifndef LH_OS_WINDOW_H
#define LH_OS_WINDOW_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/types.h>
#include <lh/ui/color.h>

#include <lh/os/system/window.h>
#include <lh/os/system/window/handle.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/window.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/window.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

/**
 * @struct lh_os_window
 * @typedef lh_os_window_t
 * @brief A single native window.
 */
struct lh_os_window
{
    lh_os_system_window_handle_t handle;
};
typedef struct lh_os_window lh_os_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── construct / lifetime ────────────────────────────────────────────────── */

/**
 * @brief Set @p self to the empty (not-yet-open) state.
 *
 * Pure value initialization — does not touch the OS. Call this (or
 * ::lh_os_window_open directly) before any other operation.
 *
 * @param self Window object to initialize.
 */
void
lh_os_window_init(lh_os_window_t *self);

/**
 * @brief Open @p self as a new top-level native window.
 *
 * @param self   Window object to open; must be in the empty state
 *               (::lh_os_window_init or freshly ::lh_os_window_close'd).
 * @param title  View into the title bytes (UTF-16LE on Windows, UTF-8 on
 *               POSIX-like systems). Must outlive this call.
 * @param width  Initial client-area width, in pixels. Must be > 0.
 * @param height Initial client-area height, in pixels. Must be > 0.
 * @return ::lh_bool_true on success, ::lh_bool_false if the OS call failed.
 */
lh_bool_t
lh_os_window_open(lh_os_window_t *self, const lh_ptr title, lh_int_t width, lh_int_t height);

/**
 * @brief Close @p self's handle (if open) and return it to the empty state.
 *
 * Safe to call on an already-empty window.
 *
 * @param self Window object to close.
 */
void
lh_os_window_close(lh_os_window_t *self);

/* ── accessors ───────────────────────────────────────────────────────────── */

/**
 * @brief Return the raw window handle stored in @p self.
 *
 * @param self Window to read from.
 * @return Current ::lh_os_system_window_handle_t
 *         (::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID if not open).
 */
lh_os_system_window_handle_t
lh_os_window_get_handle(const lh_os_window_t *self);

/**
 * @brief Test whether @p self currently holds an open window.
 *
 * @param self Window to test.
 * @return ::lh_bool_true if open, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_os_window_is_valid(const lh_os_window_t *self);

/* ── operations ──────────────────────────────────────────────────────────── */

/**
 * @brief Make @p self visible.
 *
 * @param self Open window.
 */
void
lh_os_window_show(lh_os_window_t *self);

/**
 * @brief Pull and dispatch one pending OS message, or return immediately
 *        if none are ready.
 *
 * @return ::lh_bool_true if a quit was requested by the OS, ::lh_bool_false
 *         otherwise (kept running, or no event ready).
 */
lh_bool_t
lh_os_window_pump_messages(void);

/**
 * @brief Sleep until a message arrives; see ::lh_os_system_window_wait_messages.
 */
void
lh_os_window_wait_messages(void);

/**
 * @brief Report every window's events to @p handler with @p self; see
 *        ::lh_os_system_window_set_handler.
 */
void
lh_os_window_set_handler(lh_os_system_window_handler_cb handler, lh_self_ptr self);

/**
 * @brief Show part of an RGBA image in @p self; see
 *        ::lh_os_system_window_present.
 */
lh_bool_t
lh_os_window_present(lh_os_window_t *self, const lh_ptr pixels, lh_int_t stride, lh_int_t x,
                     lh_int_t y, lh_int_t width, lh_int_t height);

/* See ::LH_OS_SYSTEM_WINDOW_FRAME_SYSTEM and ::LH_OS_SYSTEM_WINDOW_FRAME_CLIENT. */
#define LH_OS_WINDOW_FRAME_SYSTEM LH_OS_SYSTEM_WINDOW_FRAME_SYSTEM
#define LH_OS_WINDOW_FRAME_CLIENT LH_OS_SYSTEM_WINDOW_FRAME_CLIENT

/**
 * @brief Choose the caption of @p self; see ::lh_os_system_window_set_frame.
 *
 * The default is the OS caption. The client frame removes it so the caller
 * can paint that area. lh does not draw a caption.
 */
void
lh_os_window_set_frame(lh_os_window_t *self, lh_int_t frame);

/**
 * @brief ::LH_OS_WINDOW_FRAME_SYSTEM or ::LH_OS_WINDOW_FRAME_CLIENT.
 */
lh_int_t
lh_os_window_get_frame(const lh_os_window_t *self);

/**
 * @brief Remember a corner radius for @p self; see
 *        ::lh_os_system_window_set_corner_radius.
 */
void
lh_os_window_set_corner_radius(lh_os_window_t *self, lh_int_t radius);

/**
 * @brief Corner radius last set on @p self, or zero when none was set.
 */
lh_int_t
lh_os_window_get_corner_radius(const lh_os_window_t *self);

/**
 * @brief Draw a soft shadow around @p self; see
 *        ::lh_os_system_window_set_shadow.
 */
void
lh_os_window_set_shadow(lh_os_window_t *self, lh_int_t spread);

/**
 * @brief Shadow spread last set on @p self, in pixels.
 */
lh_int_t
lh_os_window_get_shadow(const lh_os_window_t *self);

/**
 * @brief Ask for a dark caption on @p self; see
 *        ::lh_os_system_window_set_dark.
 */
void
lh_os_window_set_dark(lh_os_window_t *self, lh_bool_t dark);

/**
 * @brief True when ::lh_os_window_set_dark last asked for a dark caption.
 */
lh_bool_t
lh_os_window_get_dark(const lh_os_window_t *self);

/**
 * @brief Color the caption, its text, and the border of @p self.
 *
 * Channels are the red, green, and blue of ::lh_ui_color_t; alpha is
 * ignored. Where the system cannot color the frame, the call changes
 * nothing.
 */
void
lh_os_window_set_chrome(lh_os_window_t *self, lh_ui_color_t caption, lh_ui_color_t text,
                        lh_ui_color_t border);

/**
 * @brief Ask the OS to close @p self; see ::lh_os_system_window_close_frame.
 */
void
lh_os_window_request_close(lh_os_window_t *self);

/**
 * @brief Minimize @p self; see ::lh_os_system_window_minimize.
 */
void
lh_os_window_minimize(lh_os_window_t *self);

/**
 * @brief Maximize or restore @p self; see ::lh_os_system_window_zoom.
 */
void
lh_os_window_zoom(lh_os_window_t *self);

/**
 * @brief Drag @p self; see ::lh_os_system_window_begin_move.
 */
void
lh_os_window_begin_move(lh_os_window_t *self);

/**
 * @brief Outer width of @p self; see ::lh_os_system_window_get_width.
 */
lh_int_t
lh_os_window_get_width(const lh_os_window_t *self);

/**
 * @brief Outer height of @p self; see ::lh_os_system_window_get_height.
 */
lh_int_t
lh_os_window_get_height(const lh_os_window_t *self);

/**
 * @brief Canvas width of @p self; see ::lh_os_system_window_get_client_width.
 */
lh_int_t
lh_os_window_get_client_width(const lh_os_window_t *self);

/**
 * @brief Canvas height of @p self; see ::lh_os_system_window_get_client_height.
 */
lh_int_t
lh_os_window_get_client_height(const lh_os_window_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_H */