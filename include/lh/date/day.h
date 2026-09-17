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
 * @brief Day of month (`::LH_DATE_DAY_MIN`–::LH_DATE_DAY_MAX depending on month).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_date_day_t;

/**
 * @def LH_DATE_DAY_MIN
 * @brief First day of a month.
 */
#define LH_DATE_DAY_MIN 1U

/**
 * @def LH_DATE_DAY_FEBRUARY
 * @brief Days in February in a common year.
 */
#define LH_DATE_DAY_FEBRUARY 28U

/**
 * @def LH_DATE_DAY_FEBRUARY_LEAP
 * @brief Days in February in a Gregorian leap year.
 */
#define LH_DATE_DAY_FEBRUARY_LEAP 29U

/**
 * @def LH_DATE_DAY_SHORT
 * @brief Days in April, June, September, and November.
 */
#define LH_DATE_DAY_SHORT 30U

/**
 * @def LH_DATE_DAY_MAX
 * @brief Longest month (January, March, May, July, August, October, December).
 *
 * Also the parse-time upper bound before ::lh_date_days_in_month.
 */
#define LH_DATE_DAY_MAX 31U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Days in @p month of @p year.
 *
 * February is ::LH_DATE_DAY_FEBRUARY_LEAP when ::lh_date_year_is_leap(@p year).
 *
 * @param year  Calendar year (used only for February).
 * @param month Calendar month (`1`–`12`).
 *
 * @return ::LH_DATE_DAY_FEBRUARY–::LH_DATE_DAY_MAX, or `0` if @p month is not in
 *         `[::LH_DATE_MONTH_MIN, ::LH_DATE_MONTH_MAX]`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_in_month(lh_date_year_t year, lh_date_month_t month);

/**
 * @brief ::lh_date_days_in_month if @p day is a valid day of that month, else `0`.
 *
 * Shared by ::lh_date_days_left and ::lh_date_days_left_with_today.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_in_month_if_valid(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

/**
 * @brief Days after @p day until month end (`dim - day`), not counting today.
 *
 * @param year  Calendar year.
 * @param month Calendar month.
 * @param day   Day of month (::LH_DATE_DAY_MIN and up).
 *
 * @return Remaining days, or `0` if the triple is not a valid calendar day.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_left(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

/**
 * @brief Days until the next month starts, including today (`dim - day + 1`).
 *
 * @param year  Calendar year.
 * @param month Calendar month.
 * @param day   Day of month (::LH_DATE_DAY_MIN and up).
 *
 * @return Count including today, or `0` if the triple is not a valid calendar day.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_days_left_with_today(lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_DAY_H */
