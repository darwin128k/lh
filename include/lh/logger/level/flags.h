/**
 * @file flags.h
 * @brief Bitmask of which ::lh_logger_level_t values a logger will emit.
 *
 * Each flag is `lh_bit_mask` of the matching level ordinal, so the central
 * ::lh_logger_log can test `lh_bit_disjoint(self->flags, lh_bit_mask(level))`
 * and skip the slot callback when the level is filtered out.
 */

#ifndef LH_LOGGER_LEVEL_FLAGS_H
#define LH_LOGGER_LEVEL_FLAGS_H

#include <lh/byte.h>
#include <lh/logger/level.h>
#include <lh/util/bit.h>

/**
 * @def lh_logger_level_flags_none
 * @brief No levels enabled — every record is dropped.
 */
#define lh_logger_level_flags_none 0

/**
 * @def lh_logger_level_flags_emergency
 * @brief Enable ::lh_logger_level_emergency.
 */
#define lh_logger_level_flags_emergency lh_bit_mask(lh_logger_level_emergency)

/**
 * @def lh_logger_level_flags_alert
 * @brief Enable ::lh_logger_level_alert.
 */
#define lh_logger_level_flags_alert lh_bit_mask(lh_logger_level_alert)

/**
 * @def lh_logger_level_flags_critical
 * @brief Enable ::lh_logger_level_critical.
 */
#define lh_logger_level_flags_critical lh_bit_mask(lh_logger_level_critical)

/**
 * @def lh_logger_level_flags_error
 * @brief Enable ::lh_logger_level_error.
 */
#define lh_logger_level_flags_error lh_bit_mask(lh_logger_level_error)

/**
 * @def lh_logger_level_flags_warning
 * @brief Enable ::lh_logger_level_warning.
 */
#define lh_logger_level_flags_warning lh_bit_mask(lh_logger_level_warning)

/**
 * @def lh_logger_level_flags_notice
 * @brief Enable ::lh_logger_level_notice.
 */
#define lh_logger_level_flags_notice lh_bit_mask(lh_logger_level_notice)

/**
 * @def lh_logger_level_flags_info
 * @brief Enable ::lh_logger_level_info.
 */
#define lh_logger_level_flags_info lh_bit_mask(lh_logger_level_info)

/**
 * @def lh_logger_level_flags_debug
 * @brief Enable ::lh_logger_level_debug.
 */
#define lh_logger_level_flags_debug lh_bit_mask(lh_logger_level_debug)

/**
 * @def lh_logger_level_flags_all
 * @brief Every PSR-3 level enabled.
 */
#define lh_logger_level_flags_all                                                                  \
    (lh_logger_level_flags_emergency | lh_logger_level_flags_alert |                               \
     lh_logger_level_flags_critical | lh_logger_level_flags_error |                                \
     lh_logger_level_flags_warning | lh_logger_level_flags_notice | lh_logger_level_flags_info |   \
     lh_logger_level_flags_debug)

/**
 * @typedef lh_logger_level_flags_t
 * @brief Bitmask of enabled levels; eight bits, one per ::lh_logger_level_t.
 */
typedef lh_byte_t lh_logger_level_flags_t;

#endif /* LH_LOGGER_LEVEL_FLAGS_H */
