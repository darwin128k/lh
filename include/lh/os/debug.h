/**
 * @file debug.h
 * @brief Lines to the debugger attached to the process, and a logger sink over it.
 *
 * On Windows a line shows in Visual Studio's Output window while the program runs
 * under the debugger (and in DebugView without one). Plug ::lh_os_debug_log into
 * a logger — the runtime logger, say — to see what lh reports there:
 *
 * @code
 * lh_logger_init(lh_runtime_logger_get_mut(), lh_logger_level_flags_all, lh_os_debug_log,
 *                lh_null);
 * @endcode
 *
 * Where the platform has no debugger channel (POSIX) the lines go nowhere.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_DEBUG_H
#define LH_OS_DEBUG_H

#include <lh/core/logger/level.h>
#include <lh/core/self.h>
#include <lh/core/size.h>
#include <lh/os/system/debug.h>

#include <stdarg.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Hand one line to the debugger: @p text and a line end.
 *
 * @param text NUL-terminated text, or ::lh_null for nothing.
 * @return ::lh_bool_true when the debugger channel took it.
 */
lh_bool_t
lh_os_debug_print_line(const lh_str_ptr text);

/**
 * @brief A logger sink (an ::lh_logger_emit_fn) that prints each record to the
 *        debugger as `[level] text`.
 *
 * A record too long for its line is printed as its format mask, so it still says
 * something.
 *
 * @param self  Unused.
 * @param level The record's level.
 * @param fmt   printf-style mask (::lh_str_ptr_format_text conversions).
 * @param args  Its arguments.
 * @return Characters handed over, or -1 when the debugger channel is missing.
 */
lh_ssize_t
lh_os_debug_log(lh_self_ptr self, lh_logger_level_t level, const lh_str_ptr fmt, va_list args);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_DEBUG_H */
