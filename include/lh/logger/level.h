/**
 * @file level.h
 * @brief Log record severity (PSR-3 names, ordinal 0-7).
 *
 * This is the kind of a message — what it *is*. Which kinds a logger
 * actually emits is a separate bitmask in `lh/logger/level/flags.h`
 * (::lh_logger_level_flags_t); the flag for a level is
 * `lh_bit_mask` of that level's ordinal.
 */

#ifndef LH_LOGGER_LEVEL_H
#define LH_LOGGER_LEVEL_H

#include <lh/byte.h>

/**
 * @def lh_logger_level_emergency
 * @brief System unusable.
 */
#define lh_logger_level_emergency 0

/**
 * @def lh_logger_level_alert
 * @brief Action must be taken immediately.
 */
#define lh_logger_level_alert 1

/**
 * @def lh_logger_level_critical
 * @brief Critical conditions.
 */
#define lh_logger_level_critical 2

/**
 * @def lh_logger_level_error
 * @brief Runtime errors.
 */
#define lh_logger_level_error 3

/**
 * @def lh_logger_level_warning
 * @brief Exceptional occurrences that are not errors.
 */
#define lh_logger_level_warning 4

/**
 * @def lh_logger_level_notice
 * @brief Normal but significant events.
 */
#define lh_logger_level_notice 5

/**
 * @def lh_logger_level_info
 * @brief Interesting events.
 */
#define lh_logger_level_info 6

/**
 * @def lh_logger_level_debug
 * @brief Detailed debug information.
 */
#define lh_logger_level_debug 7

/**
 * @typedef lh_logger_level_t
 * @brief Log severity; one of the `lh_logger_level_*` constants (0-7).
 */
typedef lh_byte_t lh_logger_level_t;

#endif /* LH_LOGGER_LEVEL_H */
