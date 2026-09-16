/**
 * @file date.h
 * @brief Calendar date value type (`year/month/day`).
 *
 * Pure value — no clock, no OS. Gregorian calendar on parse. Fields are
 * ::lh_date_year_t, ::lh_date_month_t, and ::lh_date_day_t.
 *
 * Brace init: ::lh_date_initializer in `lh/date/initializer.h`.
 */

#ifndef LH_DATE_H
#define LH_DATE_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/date/day.h>
#include <lh/date/month.h>
#include <lh/date/view/fields.h>
#include <lh/date/week/index.h>
#include <lh/date/year.h>
#include <lh/size.h>
#include <lh/str/ptr.h>

/**
 * @def LH_DATE_TEXT_MAX
 * @brief Longest `YYYY/MM/DD` text, excluding NUL (`65535/12/31` is 11).
 */
#define LH_DATE_TEXT_MAX 11U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_date
 * @brief Gregorian calendar date.
 *
 * Fields are injected via ::lh_date_fields.
 */
struct lh_date
{
    lh_date_fields(lh_date_year_t, lh_date_month_t, lh_date_day_t);
};

/**
 * @typedef lh_date_t
 * @brief Alias for `struct lh_date`.
 */
typedef struct lh_date lh_date_t;

/**
 * @brief Write individual components into a date struct.
 *
 * Each pointer is optional: pass ::lh_null to leave that field unchanged.
 *
 * @param self  Date to modify (not null).
 * @param year  New year, or ::lh_null to skip.
 * @param month New month, or ::lh_null to skip.
 * @param day   New day, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_date_pack(lh_date_t *self, const lh_date_year_t *year, const lh_date_month_t *month,
             const lh_date_day_t *day);

/**
 * @brief Read individual components out of a date struct.
 *
 * Each pointer is optional: pass ::lh_null to skip that field.
 *
 * @param self  Date to read (not null).
 * @param year  Output for year, or ::lh_null to skip.
 * @param month Output for month, or ::lh_null to skip.
 * @param day   Output for day, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_date_unpack(const lh_date_t *self, lh_date_year_t *year, lh_date_month_t *month,
               lh_date_day_t *day);

/**
 * @brief Copy @p other into @p self.
 *
 * @param self  Destination (not null).
 * @param other Source (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_date_assign(lh_date_t *self, const lh_date_t *other);

/**
 * @brief Replace @p self with @p year, @p month, and @p day.
 *
 * Equivalent to ::lh_date_pack with all three pointers provided.
 *
 * @param self  Date to modify (not null).
 * @param year  Year (`0`–::LH_DATE_YEAR_MAX).
 * @param month Month (::LH_DATE_MONTH_MIN–::LH_DATE_MONTH_MAX).
 * @param day   Day of month (::LH_DATE_DAY_MIN and up).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_date_set(lh_date_t *self, lh_date_year_t year, lh_date_month_t month, lh_date_day_t day);

/**
 * @brief Add @p value years. Delegates to ::lh_date_year_add, then clamps the day
 *        if it is past the new month length (e.g. 29 Feb → 28 Feb).
 *
 * @param self  Date to update (not null).
 * @param value Years to add (any ::lh_uint_t).
 *
 * @return Year-radix overflow from ::lh_date_year_add. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_year(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value years. Delegates to ::lh_date_year_sub, then clamps the day.
 *
 * @param self  Date to update (not null).
 * @param value Years to subtract (any ::lh_uint_t).
 *
 * @return Year-radix units borrowed. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_year(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add @p value months. Delegates to ::lh_date_month_add, then ::lh_date_add_year,
 *        then clamps the day.
 *
 * @param self  Date to update (not null).
 * @param value Months to add (any ::lh_uint_t).
 *
 * @return Year-radix overflow. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_month(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value months. Delegates to ::lh_date_month_sub, then
 *        ::lh_date_sub_year, then clamps the day.
 *
 * @param self  Date to update (not null).
 * @param value Months to subtract (any ::lh_uint_t).
 *
 * @return Year-radix units borrowed. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_month(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add @p value calendar days. Overflowing a month calls ::lh_date_add_month
 *        and sets the day to ::LH_DATE_DAY_MIN.
 *
 * If the current day is not a valid calendar day (`left` is `0`), the loop
 * stops and @p value is not fully applied.
 *
 * @param self  Date to update (not null).
 * @param value Days to add (any ::lh_uint_t).
 *
 * @return Year-radix overflow from month/year carry. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_day(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value calendar days. Borrowing a month calls ::lh_date_sub_month
 *        and sets the day to that month's last day.
 *
 * A stored day below ::LH_DATE_DAY_MIN is treated as ::LH_DATE_DAY_MIN so the
 * loop does not hang.
 *
 * @param self  Date to update (not null).
 * @param value Days to subtract (any ::lh_uint_t).
 *
 * @return Year-radix units borrowed. `0` if the year stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_day(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add days, then months, then years (::lh_date_add_day / `_month` / `_year`).
 *
 * @param self  Date to update (not null).
 * @param year  Years to add.
 * @param month Months to add.
 * @param day   Days to add.
 *
 * @return Sum of year-radix overflow from the three steps.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day);

/**
 * @brief Subtract years, then months, then days (::lh_date_sub_year / `_month` / `_day`).
 *
 * Inverse field order of ::lh_date_add_custom.
 *
 * @param self  Date to update (not null).
 * @param year  Years to subtract.
 * @param month Months to subtract.
 * @param day   Days to subtract.
 *
 * @return Sum of year-radix units borrowed from the three steps.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day);

/**
 * @brief Add @p other as a duration. Delegates to ::lh_date_add_custom
 *        with other's year, month, and day as unsigned counts (not a calendar delta).
 *
 * @param self  Date to update (not null).
 * @param other Duration (not null).
 *
 * @return Same as ::lh_date_add_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add(lh_date_t *self, const lh_date_t *other);

/**
 * @brief Subtract @p other as a duration. Delegates to ::lh_date_sub_custom.
 *
 * @param self  Date to update (not null).
 * @param other Duration (not null).
 *
 * @return Same as ::lh_date_sub_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub(lh_date_t *self, const lh_date_t *other);

/**
 * @brief Days in @p self's month. Delegates to ::lh_date_days_in_month.
 *
 * @param self Date to read (not null).
 * @return `28`–`31`, or `0` if the month is not `1…12`.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_max_days(const lh_date_t *self);

/**
 * @brief Days after today until month end. Delegates to ::lh_date_days_left.
 *
 * @param self Date to read (not null).
 * @return `dim - day`, or `0` if the date is not a valid calendar day.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_left_days(const lh_date_t *self);

/**
 * @brief Days until the next month starts (includes today).
 *        Delegates to ::lh_date_days_left_with_today.
 *
 * @param self Date to read (not null).
 * @return `dim - day + 1`, or `0` if the date is not a valid calendar day.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_left_days_with_today(const lh_date_t *self);

/**
 * @brief Return the year of @p self.
 *
 * @param self Date to read (not null).
 * @return Stored year.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_year_t
lh_date_get_year(const lh_date_t *self);

/**
 * @brief Return the month of @p self.
 *
 * @param self Date to read (not null).
 * @return Stored month (`1`–`12` when valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_date_get_month(const lh_date_t *self);

/**
 * @brief Return the day of month of @p self.
 *
 * @param self Date to read (not null).
 * @return Stored day of month.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_get_day(const lh_date_t *self);

/**
 * @brief True if @p self and @p other hold the same year, month, and day.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_equals(const lh_date_t *self, const lh_date_t *other);

/**
 * @brief True if @p self is not earlier than @p minimum.
 *
 * Order: year, then month, then day.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_is_at_least(const lh_date_t *self, const lh_date_t *minimum);

/**
 * @brief True if @p self is strictly earlier than @p other.
 *
 * Same field order as ::lh_date_is_at_least.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_is_less(const lh_date_t *self, const lh_date_t *other);

/**
 * @brief True if @p self is strictly later than @p other.
 *
 * Same field order as ::lh_date_is_at_least.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_is_greater(const lh_date_t *self, const lh_date_t *other);

/**
 * @brief Parse `Y/M/D` (slashes; leading zeros on month/day allowed).
 *
 * All of @p str_size must be consumed. Rejects impossible calendar days.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed date on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid date.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_date_parse(lh_str_cptr str, lh_usize_t str_size, lh_date_t *out);

/**
 * @brief Format @p self as `YYYY/MM/DD` (month and day zero-padded).
 *
 * No NUL terminator. A buffer of ::LH_DATE_TEXT_MAX + 1 always has room
 * to add one after the returned length.
 *
 * @param self     Date to format (not null).
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written, or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_date_format(const lh_date_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_DATE_H */
