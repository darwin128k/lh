/**
 * @file initializer.h
 * @brief Brace initializer for ::lh_date_t.
 */

#ifndef LH_DATE_INITIALIZER_H
#define LH_DATE_INITIALIZER_H

#include <lh/initializer.h>

/**
 * @def lh_date_initializer(year, month, day)
 * @brief Brace-enclosed initializer for ::lh_date_t.
 *
 * @param year  Year (`0`–::LH_DATE_YEAR_MAX).
 * @param month Month (::LH_DATE_MONTH_MIN–::LH_DATE_MONTH_MAX).
 * @param day   Day of month (::LH_DATE_DAY_MIN and up).
 *
 * Example usage:
 * @code{.c}
 * static lh_date_t date = lh_date_initializer(2026, 9, 16);
 * @endcode
 */
#define lh_date_initializer(year, month, day) lh_initializer(year, month, day)

#endif /* LH_DATE_INITIALIZER_H */
