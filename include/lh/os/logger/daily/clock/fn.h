/**
 * @file fn.h
 * @brief ::lh_os_logger_daily_clock_fn — what time it is for daily files.
 */

#ifndef LH_OS_LOGGER_DAILY_CLOCK_FN_H
#define LH_OS_LOGGER_DAILY_CLOCK_FN_H

#include <lh/timestamp.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The current moment, in UTC.
 *
 * Called on the queue's worker thread when a flush checks whether a new day has
 * come. A program that stamps its lines with a clock of its own gives the same one
 * here, so files are cleaned by the same days the lines are written by.
 *
 * @param user The value the sink was given.
 * @param out  Receives the moment; untouched on failure.
 * @return ::lh_bool_true on success.
 */
typedef lh_bool_t(lh_os_logger_daily_clock_fn)(lh_ptr user, lh_timestamp_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_LOGGER_DAILY_CLOCK_FN_H */
