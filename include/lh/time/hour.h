/**
 * @file hour.h
 * @brief Hour-of-day value type and its own operations.
 *
 * Not a ::lh_time_t. Closed interval `[0, ::LH_TIME_HOUR_MAX]` (24-hour clock).
 * ::lh_time_t is meant to delegate wrap here.
 *
 * Brace init: ::lh_time_hour_initializer in `lh/time/hour/initializer.h`.
 * Runtime init: ::lh_time_hour_init. Overflow into days: ::lh_time_hour_set
 * / ::lh_time_hour_add.
 */

#ifndef LH_TIME_HOUR_H
#define LH_TIME_HOUR_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

/**
 * @typedef lh_time_hour_t
 * @brief Hour of day (`0`–::LH_TIME_HOUR_MAX).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_time_hour_t;

/**
 * @def LH_TIME_HOUR_MAX
 * @brief Last hour of the day (closed upper bound).
 */
#define LH_TIME_HOUR_MAX 23U

/**
 * @def LH_TIME_HOUR_RADIX
 * @brief Hours per day — ::lh_interval_closed_get_size of `[0, ::LH_TIME_HOUR_MAX]`.
 */
#define LH_TIME_HOUR_RADIX lh_interval_closed_get_size(0U, LH_TIME_HOUR_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write an in-range hour into @p self. Does not wrap.
 *
 * @param self  Hour to initialize (not null).
 * @param value Value in `[0, ::LH_TIME_HOUR_MAX]`.
 *
 * For a large count that may overflow into days, use ::lh_time_hour_set.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_hour_init(lh_time_hour_t *self, lh_time_hour_t value);

/**
 * @brief Write @p value into @p self, wrapping on the closed hour interval.
 *
 * @p value may be any ::lh_uint_t. @p self receives the remainder
 * (`value % ::LH_TIME_HOUR_RADIX`).
 *
 * @param self  Hour to write (not null).
 * @param value Hours, possibly larger than one day.
 *
 * @return Whole days of overflow (`value / radix`). `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_hour_set(lh_time_hour_t *self, lh_uint_t value);

/**
 * @brief Add @p value to @p self, wrapping on the closed hour interval.
 *
 * @param self  Hour to update (not null).
 * @param value Hours to add (any ::lh_uint_t).
 *
 * @return Whole days of overflow. `0` if the sum stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_hour_add(lh_time_hour_t *self, lh_uint_t value);

/**
 * @brief Read the stored hour (`0`–::LH_TIME_HOUR_MAX).
 *
 * @param self Hour to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_hour_t
lh_time_hour_get(const lh_time_hour_t *self);

/**
 * @brief True if @p self and @p other hold the same hour.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_hour_equals(const lh_time_hour_t *self, const lh_time_hour_t *other);

/**
 * @brief True if @p self is not less than @p minimum.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_hour_is_at_least(const lh_time_hour_t *self, const lh_time_hour_t *minimum);

/**
 * @brief True if @p self is strictly less than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_hour_is_less(const lh_time_hour_t *self, const lh_time_hour_t *other);

/**
 * @brief True if @p self is strictly greater than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_hour_is_greater(const lh_time_hour_t *self, const lh_time_hour_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIME_HOUR_H */
