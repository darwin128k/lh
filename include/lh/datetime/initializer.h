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
 */
#define lh_datetime_initializer(date, time) lh_initializer(date, time)

#endif /* LH_DATETIME_INITIALIZER_H */
