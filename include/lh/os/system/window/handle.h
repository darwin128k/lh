/**
 * @file handle.h
 * @brief Raw OS window handle and its "no window" sentinel.
 *
 * Windows `HWND`, Xlib `Window` (fits in a pointer), and Cocoa `NSWindow *`
 * are all pointer-width. The empty value is null.
 */

#ifndef LH_OS_SYSTEM_WINDOW_HANDLE_H
#define LH_OS_SYSTEM_WINDOW_HANDLE_H

#include <lh/null.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_system_window_handle_t
 * @brief Raw OS window handle (`HWND` / `Window` / `NSWindow *`).
 */
typedef lh_ptr lh_os_system_window_handle_t;

/**
 * @def LH_OS_SYSTEM_WINDOW_HANDLE_INVALID
 * @brief Sentinel for "no window".
 */
#define LH_OS_SYSTEM_WINDOW_HANDLE_INVALID lh_null

#endif /* LH_OS_SYSTEM_WINDOW_HANDLE_H */
