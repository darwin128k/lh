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
 */
#define lh_date_initializer(year, month, day) lh_initializer(year, month, day)

#endif /* LH_DATE_INITIALIZER_H */
