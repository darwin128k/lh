/**
 * @file handle.h
 * @brief Raw OS window handle type and its "no window" sentinel.
 *
 * A Win32 ::HWND is a `HANDLE` (pointer-width), Xlib `Window` is a
 * `XID` (`unsigned long`, fits in pointer-width on every platform lh
 * targets), and Cocoa `NSWindow *` is a Cocoa object pointer — three
 * different types. All three are just a bit pattern that fits in a
 * pointer-width integer; the only true "no window" sentinel they share
 * is the all-ones pattern (`-1` as a signed integer), which is the
 * value ::NULL has never held but `INVALID_HANDLE_VALUE` / a Cocoa `nil`
 * reshape to. Storing the handle as ::lh_ssize_t (signed, pointer-width)
 * makes one sentinel
 * (::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID, `-1`) correct on every
 * platform, without this header (or anything that includes it) ever
 * naming `<windows.h>` / `<X11/Xlib.h>` / `<AppKit/AppKit.h>`.
 *
 * On Windows, ::HWND-as-int is `0` for ::NULL, not `-1`. The
 * ::lh_os_system_window_is_valid helper below treats both as "no
 * window"; compare to `0` explicitly when the value must be ::NULL.
 */

#ifndef LH_OS_SYSTEM_WINDOW_HANDLE_H
#define LH_OS_SYSTEM_WINDOW_HANDLE_H

#include <lh/cast/static.h>
#include <lh/size.h>

/**
 * @typedef lh_os_system_window_handle_t
 * @brief Raw OS window handle, stored by bit pattern.
 */
typedef lh_ssize_t lh_os_system_window_handle_t;

/**
 * @def LH_OS_SYSTEM_WINDOW_HANDLE_INVALID
 * @brief Sentinel for "no window".
 */
#define LH_OS_SYSTEM_WINDOW_HANDLE_INVALID                                                       \
    (lh_cast_static(lh_os_system_window_handle_t, -1))

/**
 * @def LH_OS_SYSTEM_WINDOW_HANDLE_NULL
 * @brief `NULL` window handle, distinct from ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID.
 *
 * Matches `HWND` from `NULL` (== `0`). A live Win32 window handle is *never*
 * equal to `0`, so this is also a usable "no window" sentinel — but keep the
 * two concepts separate: `INVALID` is what `*_open` returns on failure;
 * `NULL` is what every other API uses for "no window" arguments.
 */
#define LH_OS_SYSTEM_WINDOW_HANDLE_NULL                                                           \
    (lh_cast_static(lh_os_system_window_handle_t, lh_cast_static(lh_ssize_t, 0)))

#endif /* LH_OS_SYSTEM_WINDOW_HANDLE_H */