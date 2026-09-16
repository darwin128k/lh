/**
 * @file initializer.h
 * @brief Brace initializer for a lone ::lh_time_minute_t.
 *
 * Does not wrap — pass a value already in `[0, ::LH_TIME_MINUTE_MAX]`.
 * Use ::lh_time_minute_set when the value may overflow into hours.
 * Runtime init of an existing object is ::lh_time_minute_init.
 */

#ifndef LH_TIME_MINUTE_INITIALIZER_H
#define LH_TIME_MINUTE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_time_minute_initializer(value)
 * @brief Brace-enclosed initializer for ::lh_time_minute_t.
 *
 * Example usage:
 * @code{.c}
 * static lh_time_minute_t minute = lh_time_minute_initializer(10);
 * @endcode
 */
#define lh_time_minute_initializer(value) lh_initializer(value)

#endif /* LH_TIME_MINUTE_INITIALIZER_H */
