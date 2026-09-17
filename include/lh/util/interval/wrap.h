/**
 * @file wrap.h
 * @brief Closed-interval wrap add/sub/assign on ::lh_ullong_t.
 *
 * One implementation for year, month-index, hour, minute, and second.
 * Callers store the result back with ::lh_cast_static to their field type.
 */

#ifndef LH_UTIL_INTERVAL_WRAP_H
#define LH_UTIL_INTERVAL_WRAP_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/util/interval.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold @p value into [@p min, @p max] and write it to @p self.
 *
 * @return Whole interval lengths that overflowed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uint_t
lh_interval_closed_wrap_set(lh_ullong_t *self, lh_ullong_t value, lh_ullong_t min, lh_ullong_t max)
{
    *self = lh_interval_closed_wrap_value(value, min, max);
    return lh_cast_static(lh_uint_t, lh_interval_closed_wrap_overflow(value, min, max));
}

/**
 * @brief Add @p value to @p self, wrapping on [@p min, @p max].
 *
 * @return Whole interval lengths that overflowed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uint_t
lh_interval_closed_wrap_add(lh_ullong_t *self, lh_ullong_t value, lh_ullong_t min, lh_ullong_t max)
{
    return lh_interval_closed_wrap_set(self, *self + value, min, max);
}

/**
 * @brief Subtract @p value from @p self, wrapping on [@p min, @p max].
 *
 * @return Whole interval lengths borrowed.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_uint_t
lh_interval_closed_wrap_sub(lh_ullong_t *self, lh_ullong_t value, lh_ullong_t min, lh_ullong_t max)
{
    lh_ullong_t cur = *self;

    *self = lh_interval_closed_unsigned_sub_wrap_value(cur, value, min, max);
    return lh_cast_static(lh_uint_t,
                          lh_interval_closed_unsigned_sub_wrap_overflow(cur, value, min, max));
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_INTERVAL_WRAP_H */
