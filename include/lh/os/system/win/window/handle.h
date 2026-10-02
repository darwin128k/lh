/**
 * @file handle.h
 * @brief Win32-specific handle header — forwards to the platform-agnostic
 *        window handle in `lh/os/system/window/handle.h`.
 *
 * Kept as a separate header for symmetry with the rest of lh (each
 * platform namespace has its own `*/handle.h`); the bit-pattern typedef
 * is the same on every backend, so the actual definition lives in the
 * shared header.
 */

#ifndef LH_OS_SYSTEM_WIN_WINDOW_HANDLE_H
#define LH_OS_SYSTEM_WIN_WINDOW_HANDLE_H

#include <lh/os/system/window/handle.h>

#endif /* LH_OS_SYSTEM_WIN_WINDOW_HANDLE_H */