/**
 * @file day.h
 * @brief Day-of-month type and month-length helpers.
 *
 * Lengths go through ::lh_date_month_to_index and ::lh_date_year_is_leap —
 * not a second copy of the calendar table logic on ::lh_date_t.
 */

#ifndef LH_DATE_DAY_H
#define LH_DATE_DAY_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/date/month.h>
#include <lh/date/year.h>
#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_date_day_t
 * @brief Day of month (`1`–`28`/`29`/`30`/`31` depending on month).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_day_t;

/**
 * @def LH_DATE_DAY_MIN
 * @brief First day of a month.
 */
#define LH_DATE_DAY_MIN 1U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Days in @p month of @p year (`0` if @p month is not `1…12`).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_in_month(lh_date_year_t year, lh_date_month_t month);

/**
 * @brief Days after @p day until month end (`dim - day`). `0` if invalid.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_left(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

/**
 * @brief ::lh_date_days_left plus today (how many days until the next month starts).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_left_with_today(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_DAY_H */
