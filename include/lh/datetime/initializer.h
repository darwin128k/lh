/**
 * @file initializer.h
 * @brief Brace initializer for ::lh_datetime_t.
 */

#ifndef LH_DATETIME_INITIALIZER_H
#define LH_DATETIME_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_datetime_initializer(date, time)
 * @brief Brace-enclosed initializer for ::lh_datetime_t.
 *
 * Pass ::lh_date_initializer / ::lh_time_initializer (or equivalent braces).
 *
 * @param date Nested date initializer.
 * @param time Nested time initializer.
 *
 * Example usage:
 * @code{.c}
 * static lh_datetime_t dt = lh_datetime_initializer(
 *     lh_date_initializer(2026, 9, 16),
 *     lh_time_initializer(9, 5, 0));
 * @endcode
 */
#define lh_datetime_initializer(date, time) lh_initializer(date, time)

#endif /* LH_DATETIME_INITIALIZER_H */
