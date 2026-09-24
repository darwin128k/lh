/**
 * @file time.h
 * @brief Time-of-day value type (`hour:minute:second`).
 *
 * Pure value — no clock, no OS, no timezone. Fields are ::lh_time_hour_t,
 * ::lh_time_minute_t, and ::lh_time_second_t. Wrap belongs on those types;
 * this composite is meant to delegate to them.
 *
 * Brace init: ::lh_time_initializer in `lh/time/initializer.h`.
 */

#ifndef LH_TIME_H
#define LH_TIME_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/ptr.h>
#include <lh/time/hour.h>
#include <lh/time/minute.h>
#include <lh/time/second.h>
#include <lh/time/view/fields.h>

/**
 * @def LH_TIME_TEXT_MAX
 * @brief Longest `HH:MM:SS` text, excluding NUL (`23:59:59` is 8).
 */
#define LH_TIME_TEXT_MAX 8U

/**
 * @def LH_TIME_SECONDS_PER_MINUTE
 * @brief Seconds in a minute. Alias for ::LH_TIME_SECOND_RADIX.
 */
#define LH_TIME_SECONDS_PER_MINUTE LH_TIME_SECOND_RADIX

/**
 * @def LH_TIME_SECONDS_PER_HOUR
 * @brief Seconds in an hour.
 */
#define LH_TIME_SECONDS_PER_HOUR (LH_TIME_MINUTE_RADIX * LH_TIME_SECOND_RADIX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @struct lh_time
 * @brief Time of day, 24-hour clock.
 *
 * Fields are injected via ::lh_time_fields.
 */
struct lh_time
{
    lh_time_fields(lh_time_hour_t, lh_time_minute_t, lh_time_second_t);
};

/**
 * @typedef lh_time_t
 * @brief Alias for `struct lh_time`.
 */
typedef struct lh_time lh_time_t;

/**
 * @brief Copy @p other into @p self.
 *
 * @param self  Destination (not null).
 * @param other Source (not null).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_assign(lh_time_t *self, const lh_time_t *other);

/**
 * @brief Replace @p self with @p hour, @p minute, and @p second.
 *
 * @param self   Time to modify (not null).
 * @param hour   Hour (`0`–::LH_TIME_HOUR_MAX).
 * @param minute Minute (`0`–::LH_TIME_MINUTE_MAX).
 * @param second Second (`0`–::LH_TIME_SECOND_MAX).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_set(lh_time_t *self, lh_time_hour_t hour, lh_time_minute_t minute, lh_time_second_t second);

/**
 * @brief Write only the hour field.
 *
 * @param self Time to modify (not null).
 * @param hour New hour.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_set_hour(lh_time_t *self, lh_time_hour_t hour);

/**
 * @brief Write only the minute field.
 *
 * @param self   Time to modify (not null).
 * @param minute New minute.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_set_minute(lh_time_t *self, lh_time_minute_t minute);

/**
 * @brief Write only the second field.
 *
 * @param self   Time to modify (not null).
 * @param second New second.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_set_second(lh_time_t *self, lh_time_second_t second);

/**
 * @brief Add @p value hours. Delegates to ::lh_time_hour_add.
 *
 * @param self  Time to update (not null).
 * @param value Hours to add (any ::lh_uint_t).
 *
 * @return Whole days of overflow. `0` if the hour stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_add_hour(lh_time_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value hours. Delegates to ::lh_time_hour_sub.
 *
 * @param self  Time to update (not null).
 * @param value Hours to subtract (any ::lh_uint_t).
 *
 * @return Whole days borrowed. `0` if the hour stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_sub_hour(lh_time_t *self, lh_uint_t value);

/**
 * @brief Add @p value minutes. Delegates to ::lh_time_minute_add, then
 *        ::lh_time_add_hour for the overflow.
 *
 * @param self  Time to update (not null).
 * @param value Minutes to add (any ::lh_uint_t).
 *
 * @return Whole days of overflow from the hour carry.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_add_minute(lh_time_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value minutes. Delegates to ::lh_time_minute_sub, then
 *        ::lh_time_sub_hour for the borrow.
 *
 * @param self  Time to update (not null).
 * @param value Minutes to subtract (any ::lh_uint_t).
 *
 * @return Whole days borrowed from the hour carry.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_sub_minute(lh_time_t *self, lh_uint_t value);

/**
 * @brief Add @p value seconds. Delegates to ::lh_time_second_add, then
 *        ::lh_time_add_minute for the overflow.
 *
 * @param self  Time to update (not null).
 * @param value Seconds to add (any ::lh_uint_t).
 *
 * @return Whole days of overflow from the minute/hour carry.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_add_second(lh_time_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value seconds. Delegates to ::lh_time_second_sub, then
 *        ::lh_time_sub_minute for the borrow.
 *
 * @param self  Time to update (not null).
 * @param value Seconds to subtract (any ::lh_uint_t).
 *
 * @return Whole days borrowed from the minute/hour carry.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_sub_second(lh_time_t *self, lh_uint_t value);

/**
 * @brief Add seconds, then minutes, then hours (::lh_time_add_second / `_minute` / `_hour`).
 *
 * @param self   Time to update (not null).
 * @param hour   Hours to add.
 * @param minute Minutes to add.
 * @param second Seconds to add.
 *
 * @return Sum of whole days of overflow from the three steps.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_add_custom(lh_time_t *self, lh_uint_t hour, lh_uint_t minute, lh_uint_t second);

/**
 * @brief Subtract seconds, then minutes, then hours
 *        (::lh_time_sub_second / `_minute` / `_hour`).
 *
 * Same field order as ::lh_time_add_custom.
 *
 * @param self   Time to update (not null).
 * @param hour   Hours to subtract.
 * @param minute Minutes to subtract.
 * @param second Seconds to subtract.
 *
 * @return Sum of whole days borrowed from the three steps.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_sub_custom(lh_time_t *self, lh_uint_t hour, lh_uint_t minute, lh_uint_t second);

/**
 * @brief Add @p other as a duration. Delegates to ::lh_time_add_custom
 *        with other's hour, minute, and second as unsigned counts.
 *
 * @param self  Time to update (not null).
 * @param other Duration (not null).
 *
 * @return Same as ::lh_time_add_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_add(lh_time_t *self, const lh_time_t *other);

/**
 * @brief Subtract @p other as a duration. Delegates to ::lh_time_sub_custom.
 *
 * @param self  Time to update (not null).
 * @param other Duration (not null).
 *
 * @return Same as ::lh_time_sub_custom.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_sub(lh_time_t *self, const lh_time_t *other);

/**
 * @brief Return the hour of @p self.
 *
 * @param self Time to read (not null).
 * @return Stored hour (`0`–::LH_TIME_HOUR_MAX when valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_hour_t
lh_time_get_hour(const lh_time_t *self);

/**
 * @brief Return the minute of @p self.
 *
 * @param self Time to read (not null).
 * @return Stored minute (`0`–::LH_TIME_MINUTE_MAX when valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_minute_t
lh_time_get_minute(const lh_time_t *self);

/**
 * @brief Return the second of @p self.
 *
 * @param self Time to read (not null).
 * @return Stored second (`0`–::LH_TIME_SECOND_MAX when valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_second_t
lh_time_get_second(const lh_time_t *self);

/**
 * @brief Seconds since midnight of @p self (`0`–`86399`).
 *
 * This is the ground-truth conversion the rest of the library is built on:
 * ::lh_timestamp_from_time is just this value, unchanged.
 *
 * @param self Time to read (not null).
 * @return Seconds since midnight.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_seconds_of_day(const lh_time_t *self);

/**
 * @brief Inverse of ::lh_time_seconds_of_day: the time of day @p seconds
 *        after midnight.
 *
 * @param seconds Seconds since midnight (`0`–`86399`).
 * @return Time of day.
 */
LH_ATTRIBUTE_SYMBOL
lh_time_t
lh_time_from_seconds_of_day(lh_uint_t seconds);

/**
 * @brief True if @p self and @p other hold the same hour, minute, and second.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_equals(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief True if @p self is not earlier than @p minimum.
 *
 * Order: hour, then minute, then second.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_at_least(const lh_time_t *self, const lh_time_t *minimum);

/**
 * @brief True if @p self is strictly earlier than @p other.
 *
 * Same field order as ::lh_time_is_at_least.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_less(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief True if @p self is strictly later than @p other.
 *
 * Same field order as ::lh_time_is_at_least.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_greater(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief Parse `H:M:S` (colons; leading zeros allowed).
 *
 * All of @p str_size must be consumed. Ranges: hour `0`–::LH_TIME_HOUR_MAX,
 * minute `0`–::LH_TIME_MINUTE_MAX, second `0`–::LH_TIME_SECOND_MAX.
 *
 * @param str      Buffer to parse (not required to be NUL-terminated).
 * @param str_size Number of characters available in @p str.
 * @param out      Receives the parsed time on success; untouched on failure.
 *
 * @return ::lh_bool_true if all of @p str_size was consumed as a valid time.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_parse(lh_str_cptr str, lh_usize_t str_size, lh_time_t *out);

/**
 * @brief Format @p self as `HH:MM:SS` (zero-padded).
 *
 * No NUL terminator. A buffer of ::LH_TIME_TEXT_MAX + 1 always has room
 * to add one after the returned length.
 *
 * @param self     Time to format (not null).
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters.
 *
 * @return Characters written (`8`), or 0 if @p str_size was too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_time_format(const lh_time_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIME_H */
