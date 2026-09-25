/**
 * @file minute.h
 * @brief Minute-of-hour value type and its own operations.
 *
 * Not a ::lh_time_t. Closed interval `[0, ::LH_TIME_MINUTE_MAX]`. ::lh_time_t
 * is meant to delegate wrap here.
 *
 * Brace init: ::lh_time_minute_initializer in `lh/time/minute/initializer.h`.
 * Runtime init: ::lh_time_minute_init. Overflow into hours: ::lh_time_minute_set
 * / ::lh_time_minute_add / ::lh_time_minute_sub.
 */

#ifndef LH_TIME_MINUTE_H
#define LH_TIME_MINUTE_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

/**
 * @typedef lh_time_minute_t
 * @brief Minute of hour (`0`–::LH_TIME_MINUTE_MAX).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_time_minute_t;

/**
 * @def LH_TIME_MINUTE_MAX
 * @brief Last minute of an hour (closed upper bound).
 */
#define LH_TIME_MINUTE_MAX 59U

/**
 * @def LH_TIME_MINUTE_RADIX
 * @brief Minutes per hour — ::lh_interval_closed_get_size of `[0, ::LH_TIME_MINUTE_MAX]`.
 */
#define LH_TIME_MINUTE_RADIX lh_interval_closed_get_size(0U, LH_TIME_MINUTE_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write an in-range minute into @p self. Does not wrap.
 *
 * @param self  Minute to initialize (not null).
 * @param value Value in `[0, ::LH_TIME_MINUTE_MAX]`.
 *
 * For a large count that may overflow into hours, use ::lh_time_minute_set.
 */
void
lh_time_minute_init(lh_time_minute_t *self, lh_time_minute_t value);

/**
 * @brief Write @p value into @p self, wrapping on the closed minute interval.
 *
 * @p value may be any ::lh_uint_t. @p self receives the remainder
 * (`value % ::LH_TIME_MINUTE_RADIX`).
 *
 * @param self  Minute to write (not null).
 * @param value Minutes, possibly larger than one hour.
 *
 * @return Whole hours of overflow (`value / radix`). `0` if it fit.
 */
lh_uint_t
lh_time_minute_set(lh_time_minute_t *self, lh_uint_t value);

/**
 * @brief Add @p value to @p self, wrapping on the closed minute interval.
 *
 * @param self  Minute to update (not null).
 * @param value Minutes to add (any ::lh_uint_t).
 *
 * @return Whole hours of overflow. `0` if the sum stayed in range.
 */
lh_uint_t
lh_time_minute_add(lh_time_minute_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value from @p self, wrapping on the closed minute interval.
 *
 * @param self  Minute to update (not null).
 * @param value Minutes to subtract (any ::lh_uint_t).
 *
 * @return Whole hours borrowed. `0` if the difference stayed in range.
 */
lh_uint_t
lh_time_minute_sub(lh_time_minute_t *self, lh_uint_t value);

/**
 * @brief Read the stored minute (`0`–::LH_TIME_MINUTE_MAX).
 *
 * @param self Minute to read (not null).
 * @return Stored minute (`0`–::LH_TIME_MINUTE_MAX).
 */
lh_time_minute_t
lh_time_minute_get(const lh_time_minute_t *self);

/**
 * @brief True if @p self and @p other hold the same minute.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
lh_bool_t
lh_time_minute_equals(const lh_time_minute_t *self, const lh_time_minute_t *other);

/**
 * @brief True if @p self is not less than @p minimum.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
lh_bool_t
lh_time_minute_is_at_least(const lh_time_minute_t *self, const lh_time_minute_t *minimum);

/**
 * @brief True if @p self is strictly less than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
lh_bool_t
lh_time_minute_is_less(const lh_time_minute_t *self, const lh_time_minute_t *other);

/**
 * @brief True if @p self is strictly greater than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
lh_bool_t
lh_time_minute_is_greater(const lh_time_minute_t *self, const lh_time_minute_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIME_MINUTE_H */
