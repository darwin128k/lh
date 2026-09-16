/**
 * @file index.h
 * @brief Day-of-week index (`0` = Monday … `6` = Sunday).
 */

#ifndef LH_DATE_WEEK_INDEX_H
#define LH_DATE_WEEK_INDEX_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_date_week_index_t
 * @brief Weekday index.
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_week_index_t;

#define LH_DATE_WEEK_INDEX_MONDAY 0U
#define LH_DATE_WEEK_INDEX_TUESDAY 1U
#define LH_DATE_WEEK_INDEX_WEDNESDAY 2U
#define LH_DATE_WEEK_INDEX_THURSDAY 3U
#define LH_DATE_WEEK_INDEX_FRIDAY 4U
#define LH_DATE_WEEK_INDEX_SATURDAY 5U
#define LH_DATE_WEEK_INDEX_SUNDAY 6U
#define LH_DATE_WEEK_INDEX_MAX LH_DATE_WEEK_INDEX_SUNDAY

#endif /* LH_DATE_WEEK_INDEX_H */
