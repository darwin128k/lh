/**
 * @file initializer.h
 * @brief Brace initializer for ::lh_time_t.
 */

#ifndef LH_TIME_INITIALIZER_H
#define LH_TIME_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_time_initializer(hour, minute, second)
 * @brief Brace-enclosed initializer for ::lh_time_t.
 *
 * Does not wrap — pass values already in range for each field.
 *
 * @param hour   Hour (`0`–::LH_TIME_HOUR_MAX).
 * @param minute Minute (`0`–::LH_TIME_MINUTE_MAX).
 * @param second Second (`0`–::LH_TIME_SECOND_MAX).
 *
 * Example usage:
 * @code{.c}
 * static lh_time_t time = lh_time_initializer(9, 5, 0);
 * @endcode
 */
#define lh_time_initializer(hour, minute, second) lh_initializer(hour, minute, second)

#endif /* LH_TIME_INITIALIZER_H */
