/**
 * @file fn.h
 * @brief ::lh_os_logger_daily_format_fn — how a line of daily files reads.
 */

#ifndef LH_OS_LOGGER_DAILY_FORMAT_FN_H
#define LH_OS_LOGGER_DAILY_FORMAT_FN_H

#include <lh/os/logger/record.h>
#include <lh/str.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Append the line of @p record to @p out, its newline included.
 *
 * Called on the queue's worker thread for each line of a channel that is not raw.
 * ::lh_os_logger_daily_format_standard is the default; ::lh_os_logger_daily_append_text
 * ends a line without doubling a newline the text already has.
 *
 * @param user   The value the format was given.
 * @param record The line; borrowed for the call.
 * @param local  Its moment on the local wall clock, read as if it were UTC
 *               (`lh/os/timestamp.h`): ::lh_timestamp_get_hour and the others give the
 *               local time of day.
 * @param out    Where the line goes; empty on entry.
 */
typedef lh_void(lh_os_logger_daily_format_fn)(lh_ptr user, const lh_os_logger_record_t *record,
                                              lh_timestamp_t local, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_DAILY_FORMAT_FN_H */
