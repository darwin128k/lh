/**
 * @file second.h
 * @brief Second-of-minute value type and its own operations.
 *
 * Not a ::lh_time_t. There is nothing to pack or unpack — this is one closed
 * interval `[0, ::LH_TIME_SECOND_MAX]`. ::lh_time_t is meant to delegate wrap
 * here instead of reimplementing it.
 *
 * Two ways to put a value in:
 * - brace init (::lh_time_second_initializer in `lh/time/second/initializer.h`);
 * - ::lh_time_second_init on an existing object.
 * Both take an in-range second. A count that may overflow into minutes goes
 * through ::lh_time_second_set / ::lh_time_second_add / ::lh_time_second_sub
 * (`lh_uint_t` so the amount can be larger than one minute).
 */

#ifndef LH_TIME_SECOND_H
#define LH_TIME_SECOND_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/fixed/types.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

/**
 * @typedef lh_time_second_t
 * @brief Second of minute (`0`–::LH_TIME_SECOND_MAX).
 *
 * Alias for: ::lh_u8_t
 */
typedef lh_u8_t lh_time_second_t;

/**
 * @def LH_TIME_SECOND_MAX
 * @brief Last second of a minute (closed upper bound).
 */
#define LH_TIME_SECOND_MAX 59U

/**
 * @def LH_TIME_SECOND_RADIX
 * @brief Seconds per minute — ::lh_interval_closed_get_size of `[0, ::LH_TIME_SECOND_MAX]`.
 */
#define LH_TIME_SECOND_RADIX lh_interval_closed_get_size(0U, LH_TIME_SECOND_MAX)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write an in-range second into @p self. Does not wrap.
 *
 * @param self  Second to initialize (not null).
 * @param value Value in `[0, ::LH_TIME_SECOND_MAX]`.
 *
 * For a large count that may overflow into minutes, use ::lh_time_second_set.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_time_second_init(lh_time_second_t *self, lh_time_second_t value);

/**
 * @brief Write @p value into @p self, wrapping on the closed second interval.
 *
 * @p value may be any ::lh_uint_t. @p self receives the remainder
 * (`value % ::LH_TIME_SECOND_RADIX`).
 *
 * @param self  Second to write (not null).
 * @param value Seconds, possibly larger than one minute.
 *
 * @return Whole minutes of overflow (`value / radix`). `0` if it fit.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_second_set(lh_time_second_t *self, lh_uint_t value);

/**
 * @brief Add @p value to @p self, wrapping on the closed second interval.
 *
 * @param self  Second to update (not null).
 * @param value Seconds to add (any ::lh_uint_t).
 *
 * @return Whole minutes of overflow. `0` if the sum stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_second_add(lh_time_second_t *self, lh_uint_t value);

/**
 * @brief Subtract @p value from @p self, wrapping on the closed second interval.
 *
 * @param self  Second to update (not null).
 * @param value Seconds to subtract (any ::lh_uint_t).
 *
 * @return Whole minutes borrowed. `0` if the difference stayed in range.
 */
LH_ATTRIBUTE_SYMBOL
lh_uint_t
lh_time_second_sub(lh_time_second_t *self, lh_uint_t value);

/**
 * @brief Read the stored second (`0`–::LH_TIME_SECOND_MAX).
 *
 * @param self Second to read (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_time_second_t
lh_time_second_get(const lh_time_second_t *self);

/**
 * @brief True if @p self and @p other hold the same second.
 *
 * @param self  Left (not null).
 * @param other Right (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_second_equals(const lh_time_second_t *self, const lh_time_second_t *other);

/**
 * @brief True if @p self is not less than @p minimum.
 *
 * @param self    Value under test (not null).
 * @param minimum Floor (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_second_is_at_least(const lh_time_second_t *self, const lh_time_second_t *minimum);

/**
 * @brief True if @p self is strictly less than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_second_is_less(const lh_time_second_t *self, const lh_time_second_t *other);

/**
 * @brief True if @p self is strictly greater than @p other.
 *
 * @param self  Value under test (not null).
 * @param other Bound (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_time_second_is_greater(const lh_time_second_t *self, const lh_time_second_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIME_SECOND_H */
