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
 * @brief Add @p value years. Delegates to ::lh_date_year_add, then clamps the day.
 *
 * @return Year-radix overflow.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_year(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value years. Delegates to ::lh_date_year_sub, then clamps the day.
 *
 * @return Year-radix units borrowed.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_year(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add @p value months. Delegates to ::lh_date_month_add, then ::lh_date_add_year.
 *
 * @return Year-radix overflow.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_month(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value months. Delegates to ::lh_date_month_sub, then ::lh_date_sub_year.
 *
 * @return Year-radix units borrowed.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_month(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add @p value calendar days. Overflowing a month calls ::lh_date_add_month.
 *
 * @return Year-radix overflow.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_day(lh_date_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value calendar days. Borrowing a month calls ::lh_date_sub_month.
 *
 * @return Year-radix units borrowed.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_day(lh_date_t *self, lh_uint_t value);

/**
 * @brief Add days, then months, then years (::lh_date_add_day / `_month` / `_year`).
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day);

/**
 * @brief Subtract years, then months, then days.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub_custom(lh_date_t *self, lh_uint_t year, lh_uint_t month, lh_uint_t day);

/**
 * @brief Add @p other as a duration. Delegates to ::lh_date_add_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_add(lh_date_t *self, const lh_date_t *other);

/**
 * @brief Subtract @p other as a duration. Delegates to ::lh_date_sub_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_date_sub(lh_date_t *self, const lh_date_t *other);

/**
 * @brief Days in @p self's month. Delegates to ::lh_date_days_in_month.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_max_days(const lh_date_t *self);

/**
 * @brief Days after today until month end. Delegates to ::lh_date_days_left.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_left_days(const lh_date_t *self);

/**
 * @brief Days until the next month starts. Delegates to ::lh_date_days_left_with_today.
 */
LH_ATTRIBUTE_SYMBOL
lh_date_day_t
lh_date_left_days_with_today(const lh_date_t *self);

/**
 * @brief Return the year of @p self.
 *
 * @param self Date to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_year_t
lh_date_get_year(const lh_date_t *self);

/**
 * @brief Return the month of @p self.
 *
 * @param self Date to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_date_month_t
lh_date_get_month(const lh_date_t *self);

/**
 * @brief Return the day of month of @p self.
 *
 * @param self Date to read (not null).
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
