/**
 * @file initializer.h
 * @brief Brace initializer for a lone ::lh_time_hour_t.
 *
 * Does not wrap — pass a value already in `[0, ::LH_TIME_HOUR_MAX]`.
 * Use ::lh_time_hour_set when the value may overflow into days.
 * Runtime init of an existing object is ::lh_time_hour_init.
 */

#ifndef LH_TIME_HOUR_INITIALIZER_H
#define LH_TIME_HOUR_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_time_hour_initializer(value)
 * @brief Brace-enclosed initializer for ::lh_time_hour_t.
 *
 * Example usage:
 * @code{.c}
 * static lh_time_hour_t hour = lh_time_hour_initializer(9);
 * @endcode
 */
#define lh_time_hour_initializer(value) lh_initializer(value)

#endif /* LH_TIME_HOUR_INITIALIZER_H */
