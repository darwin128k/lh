/**
 * @file time.h
 * @brief Time-of-day value type (`hour:minute:second`).
 *
 * Pure value — no clock, no OS, no timezone.
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

LH_ATTRIBUTE_SYMBOL
void
lh_time_pack(lh_time_t *self, const lh_time_hour_t *hour, const lh_time_minute_t *minute,
             const lh_time_second_t *second);

LH_ATTRIBUTE_SYMBOL
void
lh_time_unpack(const lh_time_t *self, lh_time_hour_t *hour, lh_time_minute_t *minute,
               lh_time_second_t *second);

LH_ATTRIBUTE_SYMBOL
void
lh_time_assign(lh_time_t *self, const lh_time_t *other);

LH_ATTRIBUTE_SYMBOL
void
lh_time_set(lh_time_t *self, lh_time_hour_t hour, lh_time_minute_t minute, lh_time_second_t second);

LH_ATTRIBUTE_SYMBOL
lh_time_hour_t
lh_time_get_hour(const lh_time_t *self);

LH_ATTRIBUTE_SYMBOL
lh_time_minute_t
lh_time_get_minute(const lh_time_t *self);

LH_ATTRIBUTE_SYMBOL
lh_time_second_t
lh_time_get_second(const lh_time_t *self);

LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_equals(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief True if @p self is not earlier than @p minimum (hour, then minute, then second).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_at_least(const lh_time_t *self, const lh_time_t *minimum);

/**
 * @brief True if @p self is strictly earlier than @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_less(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief True if @p self is strictly later than @p other.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_is_greater(const lh_time_t *self, const lh_time_t *other);

/**
 * @brief Parse `H:M:S` (colons; leading zeros allowed).
 *
 * All of @p str_size must be consumed.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_parse(lh_str_cptr str, lh_usize_t str_size, lh_time_t *out);

/**
 * @brief Format @p self as `HH:MM:SS` (zero-padded).
 *
 * No NUL terminator. Returns 0 if @p str_size is too small.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_time_format(const lh_time_t *self, lh_str_ptr str, lh_usize_t str_size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIME_H */
