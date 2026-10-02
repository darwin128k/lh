/**
 * @file window.h
 * @brief OS window primitives on a raw window handle.
 *
 * The only place that talks to the platform's native window API
 * (Win32 `CreateWindowExW` on Windows, Xlib `XCreateWindow` on Linux,
 * Cocoa `NSWindow` on macOS). Every function has one contract on every
 * platform; the backend is picked by CMake (`src/lh/os/system/win`),
 * not by `#if` at the call site.
 *
 * A window is an OS-level surface that can be sent paint, size, close,
 * key, and mouse messages. The application-level wrapper
 * (::lh_os_window_t in `lh/os/window.h`) is a value handle around the
 * platform handle here, plus state owned by `lh` (paint bookkeeping,
 * input queue).
 *
 * XP-compatibility notes (Windows backend):
 *   * Uses `CreateWindowExW` and `RegisterClassExW` (NT4 / 95, available on XP).
 *   * Avoids `WM_INPUT`, `WM_TOUCH`, `SetProcessDPIAware`, and any other
 *     post-XP API.
 *   * Mouse input via `WM_LBUTTONDOWN` / `WM_MOUSEMOVE` / `WM_MOUSEWHEEL`
 *     (legacy lparam-packed coordinates).
 *
 * On failure the native reason is in ::lh_os_system_last_error.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW (itself requires ::LH_LIBRARY_OPTION_OS).
 */

#ifndef LH_OS_SYSTEM_WINDOW_H
#define LH_OS_SYSTEM_WINDOW_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/size.h>

#include <lh/os/system/window/handle.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/window.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/system/window.h requires LH_LIBRARY_OPTION_OS_WINDOW (CMake: -DLH_LIBRARY_OPTION_OS_WINDOW=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create a top-level native window with the given title and size.
 *
 * The window is invisible until ::lh_os_system_window_show has been called
 * (Windows: `ShowWindow(SW_SHOW)`). On failure the native reason is in
 * ::lh_os_system_last_error.
 *
 * @param title  View into the title bytes (UTF-16LE on Windows, UTF-8 on
 *               POSIX-like systems). Must outlive this call.
 * @param width  Initial client-area width, in pixels. Must be > 0.
 * @param height Initial client-area height, in pixels. Must be > 0.
 * @return Opaque window handle, or ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID
 *         on failure.
 */
lh_os_system_window_handle_t
lh_os_system_window_open(const lh_ptr title, lh_int_t width, lh_int_t height);

/**
 * @brief Destroy a window opened with ::lh_os_system_window_open.
 *
 * Safe to call on an already-closed window. No-op on
 * ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID.
 *
 * @param self Window handle to destroy.
 */
void
lh_os_system_window_close(lh_os_system_window_handle_t self);

/**
 * @brief Test whether @p self is a live, non-null window handle.
 *
 * @param self Window handle to test.
 * @return ::lh_bool_true if live, ::lh_bool_false otherwise.
 */
lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t self);

/**
 * @brief Make @p self visible (no-op if already visible).
 *
 * @param self Window handle to show.
 */
void
lh_os_system_window_show(lh_os_system_window_handle_t self);

/**
 * @brief Pull and dispatch one pending OS message, or return immediately
 *        if none are ready.
 *
 * Wraps Win32 `PeekMessageW` / Xlib `XCheckMaskEvent` / Cocoa
 * `[NSApp nextEventMatchingMask]`. Dispatches the message to the window
 * proc registered with the class.
 *
 * @return ::lh_bool_true if a quit was requested by the OS
 *         (`WM_QUIT` / `WM_DELETE_WINDOW` / `applicationShouldTerminate`),
 *         ::lh_bool_false otherwise (kept running, or no event ready).
 */
lh_bool_t
lh_os_system_window_pump_messages(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_WINDOW_H */