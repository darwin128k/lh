/**
 * @file timestamp.h
 * @brief A point in time as a single scalar (::lh_timestamp_t).
 *
 * Seconds since the Unix epoch (1970-01-01 00:00:00 UTC) — the same model
 * as C's own `time_t`, but a fixed-width ::lh_s64_t so the value's size
 * does not depend on platform/compiler (unlike `time_t`, which is 32 or
 * 64 bits depending on both).
 *
 * This is the kernel: all calendar math lives here. ::lh_date_t /
 * ::lh_time_t / ::lh_datetime_t are plain field-structs — a scalar
 * timestamp is what actually gets stored, compared, and sent over the
 * wire (a database column, a filesystem's mtime, a socket payload), so
 * this type exposes everything needed to work with a point in time
 * directly, without a round trip through a calendar representation:
 * pulling out a single component, shifting by seconds/days/months/years,
 * comparing, diffing.
 */

#ifndef LH_TIMESTAMP_H
#define LH_TIMESTAMP_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/datetime.h>
#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_timestamp_t
 * @brief Unix seconds since the epoch. Alias for ::lh_s64_t.
 */
typedef lh_s64_t lh_timestamp_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Floor division: rounds toward negative infinity, unlike `/`.
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `floor(a / b)`.
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_floor_div(lh_s64_t a, lh_s64_t b);

/**
 * @brief Floor modulo: always has the sign of @p b, unlike `%`.
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `a - b * lh_timestamp_floor_div(a, b)`.
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_floor_mod(lh_s64_t a, lh_s64_t b);

/**
 * @brief Days in @p year (365 or 366).
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_days_in_year(lh_date_year_t year);

/**
 * @brief Days in @p month of @p year (28–31). Delegates to ::lh_date_max_days.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_timestamp_days_in_month(lh_date_year_t year, lh_date_month_t month);

/**
 * @brief Days from 1970-01-01 to @p year-01-01 (negative when @p year < 1970).
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_days_before_year(lh_date_year_t year);

/**
 * @brief Add @p months to @p date in place, wrapping the month/year via
 *        ::lh_date_month_add / ::lh_date_year_add, then clamping the day
 *        if it no longer fits (e.g. 31 Jan + 1 month → 28/29 Feb).
 *
 * @param date   Date to update (not null).
 * @param months Months to add.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_timestamp_date_add_months(lh_date_t *date, lh_uint_t months);

/**
 * @brief Subtract @p months from @p date in place. Mirror of
 *        ::lh_timestamp_date_add_months.
 *
 * @param date   Date to update (not null).
 * @param months Months to subtract.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_timestamp_date_sub_months(lh_date_t *date, lh_uint_t months);

/**
 * @brief Convert @p self (midnight on that date) to a scalar timestamp.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_from_date(const lh_date_t *self);

/**
 * @brief Convert @p self (time of day, no date) to seconds since midnight.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_from_time(const lh_time_t *self);

/**
 * @brief Convert @p self to a scalar timestamp.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_from_datetime(const lh_datetime_t *self);

/**
 * @brief Whole days from the epoch to @p self, floored (negative before 1970).
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_get_days(lh_timestamp_t self);

/**
 * @brief Seconds since midnight of @p self's day (`0`–`86399`).
 */
LH_ATTRIBUTE_SYMBOL
lh_s64_t
lh_timestamp_get_seconds_of_day(lh_timestamp_t self);

/**
 * @brief Calendar date of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_t
lh_timestamp_get_date(lh_timestamp_t self);

/**
 * @brief Time of day of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_time_t
lh_timestamp_get_time(lh_timestamp_t self);

/**
 * @brief Convert @p self into its date and time-of-day components.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_timestamp_to_datetime(lh_timestamp_t self, lh_datetime_t *out);

/**
 * @brief Year of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_year_t
lh_timestamp_get_year(lh_timestamp_t self);

/**
 * @brief Month of @p self (`1`–`12`).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_timestamp_get_month(lh_timestamp_t self);

/**
 * @brief Day of month of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_timestamp_get_day(lh_timestamp_t self);

/**
 * @brief Hour of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_time_hour_t
lh_timestamp_get_hour(lh_timestamp_t self);

/**
 * @brief Minute of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_time_minute_t
lh_timestamp_get_minute(lh_timestamp_t self);

/**
 * @brief Second of @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_time_second_t
lh_timestamp_get_second(lh_timestamp_t self);

/**
 * @brief Add @p seconds to @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_add_seconds(lh_timestamp_t self, lh_timestamp_t seconds);

/**
 * @brief Subtract @p seconds from @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_sub_seconds(lh_timestamp_t self, lh_timestamp_t seconds);

/**
 * @brief Add @p days to @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_add_days(lh_timestamp_t self, lh_s64_t days);

/**
 * @brief Subtract @p days from @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_sub_days(lh_timestamp_t self, lh_s64_t days);

/**
 * @brief Add @p months to @p self. Delegates to ::lh_timestamp_date_add_months
 *        on @p self's date; time of day is unchanged.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_add_months(lh_timestamp_t self, lh_uint_t months);

/**
 * @brief Subtract @p months from @p self. Mirror of ::lh_timestamp_add_months.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_sub_months(lh_timestamp_t self, lh_uint_t months);

/**
 * @brief Add @p years to @p self. Delegates to ::lh_timestamp_add_months.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_add_years(lh_timestamp_t self, lh_uint_t years);

/**
 * @brief Subtract @p years from @p self. Delegates to ::lh_timestamp_sub_months.
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_sub_years(lh_timestamp_t self, lh_uint_t years);

/**
 * @brief Seconds from @p other to @p self (`self - other`; negative if
 *        @p other is later).
 */
LH_ATTRIBUTE_SYMBOL
lh_timestamp_t
lh_timestamp_diff(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self and @p other are the same point in time.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_timestamp_equals(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self is not earlier than @p minimum.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_timestamp_is_at_least(lh_timestamp_t self, lh_timestamp_t minimum);

/**
 * @brief True if @p self is strictly earlier than @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_timestamp_is_less(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self is strictly later than @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_timestamp_is_greater(lh_timestamp_t self, lh_timestamp_t other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMESTAMP_H */
