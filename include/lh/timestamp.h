/**
 * @file timestamp.h
 * @brief A point in time as a single scalar (::lh_timestamp_t).
 *
 * Seconds since the Unix epoch (1970-01-01 00:00:00 UTC) — the same model
 * as C's own `time_t`, but a fixed-width ::lh_s64_t so the value's size
 * does not depend on platform/compiler (unlike `time_t`, which is 32 or
 * 64 bits depending on both).
 *
 * This is a derived, thin scalar view — not the kernel. All calendar/clock
 * math lives on the components: ::lh_date_t (::lh_date_days_since_epoch /
 * ::lh_date_from_epoch_days, ::lh_date_add_month, …) and ::lh_time_t
 * (::lh_time_seconds_of_day / ::lh_time_from_seconds_of_day, …). This type
 * only converts seconds ↔ components and does pure scalar-seconds
 * arithmetic (add/sub seconds, diff, compare) — it exists because a scalar
 * is what actually gets stored, compared, and sent over the wire (a
 * database column, a filesystem's mtime, a socket payload), not because it
 * owns the calendar rules.
 */

#ifndef LH_TIMESTAMP_H
#define LH_TIMESTAMP_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/datetime.h>
#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_timestamp_t
 * @brief Unix seconds since the epoch. Alias for ::lh_s64_t.
 */
typedef lh_s64_t lh_timestamp_t;

/**
 * @def LH_TIMESTAMP_SECONDS_PER_MINUTE
 * @brief Seconds in a minute.
 */
#define LH_TIMESTAMP_SECONDS_PER_MINUTE 60LL

/**
 * @def LH_TIMESTAMP_SECONDS_PER_HOUR
 * @brief Seconds in an hour.
 */
#define LH_TIMESTAMP_SECONDS_PER_HOUR 3600LL

/**
 * @def LH_TIMESTAMP_SECONDS_PER_DAY
 * @brief Seconds in a day.
 */
#define LH_TIMESTAMP_SECONDS_PER_DAY 86400LL

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Floor division: rounds toward negative infinity, unlike `/`.
 *        Delegates to ::lh_math_floor_div (`lh/util/math/floor.h`).
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `floor(a / b)`.
 */
lh_s64_t
lh_timestamp_floor_div(lh_s64_t a, lh_s64_t b);

/**
 * @brief Floor modulo: always has the sign of @p b, unlike `%`.
 *        Delegates to ::lh_math_floor_mod.
 *
 * @param a Dividend.
 * @param b Divisor (nonzero, any sign).
 * @return `a - b * lh_timestamp_floor_div(a, b)`.
 */
lh_s64_t
lh_timestamp_floor_mod(lh_s64_t a, lh_s64_t b);

/**
 * @brief Convert @p self (midnight on that date) to a scalar timestamp.
 *        Delegates to ::lh_date_days_since_epoch.
 */
lh_timestamp_t
lh_timestamp_from_date(const lh_date_t *self);

/**
 * @brief Convert @p self (time of day, no date) to seconds since midnight.
 *        Delegates to ::lh_time_seconds_of_day.
 */
lh_timestamp_t
lh_timestamp_from_time(const lh_time_t *self);

/**
 * @brief Convert @p self to a scalar timestamp.
 */
lh_timestamp_t
lh_timestamp_from_datetime(const lh_datetime_t *self);

/**
 * @brief Whole days from the epoch to @p self, floored (negative before 1970).
 */
lh_s64_t
lh_timestamp_get_days(lh_timestamp_t self);

/**
 * @brief Seconds since midnight of @p self's day (`0`–`86399`).
 */
lh_s64_t
lh_timestamp_get_seconds_of_day(lh_timestamp_t self);

/**
 * @brief Calendar date of @p self. Delegates to ::lh_date_from_epoch_days.
 */
lh_date_t
lh_timestamp_get_date(lh_timestamp_t self);

/**
 * @brief Time of day of @p self. Delegates to ::lh_time_from_seconds_of_day.
 */
lh_time_t
lh_timestamp_get_time(lh_timestamp_t self);

/**
 * @brief Convert @p self into its date and time-of-day components.
 */
void
lh_timestamp_to_datetime(lh_timestamp_t self, lh_datetime_t *out);

/**
 * @brief Year of @p self.
 */
lh_date_year_t
lh_timestamp_get_year(lh_timestamp_t self);

/**
 * @brief Month of @p self (`1`–`12`).
 */
lh_date_month_t
lh_timestamp_get_month(lh_timestamp_t self);

/**
 * @brief Day of month of @p self.
 */
lh_date_day_t
lh_timestamp_get_day(lh_timestamp_t self);

/**
 * @brief Hour of @p self.
 */
lh_time_hour_t
lh_timestamp_get_hour(lh_timestamp_t self);

/**
 * @brief Minute of @p self.
 */
lh_time_minute_t
lh_timestamp_get_minute(lh_timestamp_t self);

/**
 * @brief Second of @p self.
 */
lh_time_second_t
lh_timestamp_get_second(lh_timestamp_t self);

/**
 * @brief Add @p seconds to @p self.
 */
lh_timestamp_t
lh_timestamp_add_seconds(lh_timestamp_t self, lh_timestamp_t seconds);

/**
 * @brief Subtract @p seconds from @p self.
 */
lh_timestamp_t
lh_timestamp_sub_seconds(lh_timestamp_t self, lh_timestamp_t seconds);

/**
 * @brief Add @p days to @p self.
 */
lh_timestamp_t
lh_timestamp_add_days(lh_timestamp_t self, lh_s64_t days);

/**
 * @brief Subtract @p days from @p self.
 */
lh_timestamp_t
lh_timestamp_sub_days(lh_timestamp_t self, lh_s64_t days);

/**
 * @brief Add @p months to @p self. Delegates to ::lh_date_add_month on
 *        @p self's date; time of day is unchanged.
 */
lh_timestamp_t
lh_timestamp_add_months(lh_timestamp_t self, lh_uint_t months);

/**
 * @brief Subtract @p months from @p self. Mirror of ::lh_timestamp_add_months,
 *        via ::lh_date_sub_month.
 */
lh_timestamp_t
lh_timestamp_sub_months(lh_timestamp_t self, lh_uint_t months);

/**
 * @brief Add @p years to @p self. Delegates to ::lh_date_add_year on
 *        @p self's date; time of day is unchanged.
 */
lh_timestamp_t
lh_timestamp_add_years(lh_timestamp_t self, lh_uint_t years);

/**
 * @brief Subtract @p years from @p self. Mirror of ::lh_timestamp_add_years,
 *        via ::lh_date_sub_year.
 */
lh_timestamp_t
lh_timestamp_sub_years(lh_timestamp_t self, lh_uint_t years);

/**
 * @brief Seconds from @p other to @p self (`self - other`; negative if
 *        @p other is later).
 */
lh_timestamp_t
lh_timestamp_diff(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self and @p other are the same point in time.
 */
lh_bool_t
lh_timestamp_equals(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self is not earlier than @p minimum.
 */
lh_bool_t
lh_timestamp_is_at_least(lh_timestamp_t self, lh_timestamp_t minimum);

/**
 * @brief True if @p self is strictly earlier than @p other.
 */
lh_bool_t
lh_timestamp_is_less(lh_timestamp_t self, lh_timestamp_t other);

/**
 * @brief True if @p self is strictly later than @p other.
 */
lh_bool_t
lh_timestamp_is_greater(lh_timestamp_t self, lh_timestamp_t other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMESTAMP_H */
