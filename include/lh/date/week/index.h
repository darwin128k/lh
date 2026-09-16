/**
 * @file index.h
 * @brief Day-of-week index (`0` = Monday … `6` = Sunday).
 *
 * Naming constants only — no wrap helpers. ISO-like Monday-first numbering.
 */

#ifndef LH_DATE_WEEK_INDEX_H
#define LH_DATE_WEEK_INDEX_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_date_week_index_t
 * @brief Weekday index in `[0, ::LH_DATE_WEEK_INDEX_MAX]`.
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_week_index_t;

/**
 * @def LH_DATE_WEEK_INDEX_MONDAY
 * @brief Monday (`0`).
 */
#define LH_DATE_WEEK_INDEX_MONDAY 0U

/**
 * @def LH_DATE_WEEK_INDEX_TUESDAY
 * @brief Tuesday (`1`).
 */
#define LH_DATE_WEEK_INDEX_TUESDAY 1U

/**
 * @def LH_DATE_WEEK_INDEX_WEDNESDAY
 * @brief Wednesday (`2`).
 */
#define LH_DATE_WEEK_INDEX_WEDNESDAY 2U

/**
 * @def LH_DATE_WEEK_INDEX_THURSDAY
 * @brief Thursday (`3`).
 */
#define LH_DATE_WEEK_INDEX_THURSDAY 3U

/**
 * @def LH_DATE_WEEK_INDEX_FRIDAY
 * @brief Friday (`4`).
 */
#define LH_DATE_WEEK_INDEX_FRIDAY 4U

/**
 * @def LH_DATE_WEEK_INDEX_SATURDAY
 * @brief Saturday (`5`).
 */
#define LH_DATE_WEEK_INDEX_SATURDAY 5U

/**
 * @def LH_DATE_WEEK_INDEX_SUNDAY
 * @brief Sunday (`6`).
 */
#define LH_DATE_WEEK_INDEX_SUNDAY 6U

/**
 * @def LH_DATE_WEEK_INDEX_MAX
 * @brief Last weekday index. Same as ::LH_DATE_WEEK_INDEX_SUNDAY.
 */
#define LH_DATE_WEEK_INDEX_MAX LH_DATE_WEEK_INDEX_SUNDAY

#endif /* LH_DATE_WEEK_INDEX_H */
