/**
 * @file daily.h
 * @brief ::lh_os_logger_daily_t — a log sink that writes each channel to one file a day.
 *
 * A channel's lines go to `<dir>/YYYY-MM-DD.log`, named by the local day of each
 * line, appended to. A new day closes the old file and opens the next. A channel
 * that keeps `keep_days` days deletes older `YYYY-MM-DD.log` files of its directory
 * when it opens — by the time its opening record carries — and when a flush finds
 * a new day on the sink's clock; other files there are left alone. Missing
 * directories are created.
 *
 * Plug it into a queue (`lh/os/logger/queue.h`) with ::lh_os_logger_daily_copy_sink;
 * it then runs on the queue's worker thread. Channels are opened and closed by
 * records in the queue (::lh_os_logger_record_init_open), so it needs no lock. A
 * line of a channel that was never opened is dropped.
 *
 * The first file failure is reported once on the runtime logger
 * (`lh/runtime/logger.h`), not through the queue being written.
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_LOGGER_DAILY_H
#define LH_OS_LOGGER_DAILY_H

#include <lh/os/logger/daily/clock/cb.h>
#include <lh/os/logger/daily/format/cb.h>
#include <lh/os/logger/sink.h>

/** @brief A sink of daily files; opaque. */
typedef struct lh_os_logger_daily lh_os_logger_daily_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create the sink, with no channel yet.
 *
 * @param format How a line reads, or ::lh_null for ::lh_os_logger_daily_format_standard.
 * @param clock  What time it is, or ::lh_null for ::lh_os_timestamp_try_get_now.
 * @param user   The value @p format and @p clock are given.
 * @return The sink, or ::lh_null when memory is short.
 */
lh_os_logger_daily_t *
lh_os_logger_daily_try_create(lh_os_logger_daily_format_cb format,
                              lh_os_logger_daily_clock_cb clock, lh_ptr user);

/**
 * @brief Copy into @p out the sink callbacks that write to @p self.
 *
 * @param self The sink.
 * @param out  Receives the callbacks, for ::lh_os_logger_queue_try_create.
 */
lh_void
lh_os_logger_daily_copy_sink(lh_os_logger_daily_t *self, lh_os_logger_sink_t *out);

/**
 * @brief Free the sink.
 *
 * Its queue must be destroyed first: that closes the files.
 *
 * @param self The sink, or ::lh_null.
 */
lh_void
lh_os_logger_daily_destroy(lh_os_logger_daily_t *self);

/**
 * @brief Append @p text to @p out with one newline at its end, not two.
 *
 * @param out  Where the line goes.
 * @param text The text; a newline it ends with is kept as the line's own.
 */
lh_void
lh_os_logger_daily_append_text(lh_str_t *out, const lh_char_t *text);

/**
 * @brief The default line: `YYYY-MM-DD HH:MM:SS [level] [tag] text`, local time, the tag
 *        left out when empty (an ::lh_os_logger_daily_format_fn).
 */
lh_void
lh_os_logger_daily_format_standard(lh_ptr user, const lh_os_logger_record_t *record,
                                   lh_timestamp_t local, lh_str_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_DAILY_H */
