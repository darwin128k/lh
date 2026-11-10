/**
 * @file debug.h
 * @brief Kernel: text to the debugger attached to the process.
 *
 * Windows: `OutputDebugStringW`, shown in Visual Studio's Output window (or
 * DebugView). POSIX has no such channel: the call does nothing there. The
 * backend is picked by CMake, not by `#if`.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_SYSTEM_DEBUG_H
#define LH_OS_SYSTEM_DEBUG_H

#include <lh/config.h>
#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/str/ptr.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/system/debug.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Hand @p text, UTF-8, to the debugger, as it is.
 *
 * @param text NUL-terminated text; not ::lh_null.
 * @return ::lh_bool_true when the platform has a debugger channel and took the
 *         text; ::lh_bool_false when it has none or the text could not be converted.
 */
lh_bool_t
lh_os_system_debug_print(const lh_str_ptr text);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_DEBUG_H */
