/**
 * @file fn.h
 * @brief Callable signature for one logger level slot.
 *
 * Every level field (::lh_logger_t `emergency_cb` … `debug_cb`) uses this
 * type. The same function pointer may be stored in several slots; @p level
 * still tells the sink which record this is.
 *
 * The logger does not format. @p fmt and @p args are the caller's mask and
 * substitutions — the callback writes them (UART, ::lh_io_writer_t, …).
 */

#ifndef LH_LOGGER_EMIT_FN_H
#define LH_LOGGER_EMIT_FN_H

#include <lh/logger/level.h>
#include <lh/ptr.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

#include <stdarg.h>

/**
 * @typedef lh_logger_emit_fn
 * @brief Function type for one log record (already accepted by the filter).
 *
 * @param context Identifies the sink (an ::lh_io_writer_t *, UART handle,
 *                `FILE *`, test buffer, …) — the same idea as
 *                ::lh_io_writer_write_fn's context.
 * @param level   Severity of this record (the slot that was invoked).
 * @param fmt     NUL-terminated format mask (::lh_str_ptr_format_text).
 * @param args    Substitutions for @p fmt; do not `va_end` this list.
 *
 * @return Characters actually emitted, or a negative value if the sink
 *         failed. 0 is a successful no-op from the callback's point of view.
 */
typedef lh_ssize_t(lh_logger_emit_fn)(lh_ptr context, lh_logger_level_t level, lh_str_cptr fmt,
                                      va_list args);

#endif /* LH_LOGGER_EMIT_FN_H */
