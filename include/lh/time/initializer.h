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
 */
#define lh_time_initializer(hour, minute, second) lh_initializer(hour, minute, second)

#endif /* LH_TIME_INITIALIZER_H */
