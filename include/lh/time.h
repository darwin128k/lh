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
 * @brief Write individual components into a time struct.
 *
 * Each pointer is optional: pass ::lh_null to leave that field unchanged.
 *
 * @param self   Time to modify (not null).
 * @param hour   New hour, or ::lh_null to skip.
 * @param minute New minute, or ::lh_null to skip.
 * @param second New second, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_pack(lh_time_t *self, const lh_time_hour_t *hour, const lh_time_minute_t *minute,
             const lh_time_second_t *second);

/**
 * @brief Read individual components out of a time struct.
 *
 * Each pointer is optional: pass ::lh_null to skip that field.
 *
 * @param self   Time to read (not null).
 * @param hour   Output for hour, or ::lh_null to skip.
 * @param minute Output for minute, or ::lh_null to skip.
 * @param second Output for second, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_unpack(const lh_time_t *self, lh_time_hour_t *hour, lh_time_minute_t *minute,
               lh_time_second_t *second);

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
 * Equivalent to ::lh_time_pack with all three pointers provided.
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
 * @brief Return the hour of @p self.
 *
 * @param self Time to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_hour_t
lh_time_get_hour(const lh_time_t *self);

/**
 * @brief Return the minute of @p self.
 *
 * @param self Time to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_minute_t
lh_time_get_minute(const lh_time_t *self);

/**
 * @brief Return the second of @p self.
 *
 * @param self Time to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_second_t
lh_time_get_second(const lh_time_t *self);

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
