/**
 * @file round.h
 * @brief ::lh_math_fscalar_t to a whole ::lh_s32_t, rounded down or up.
 *
 * Plain casts and one compare: no libm (`floorf` / `ceilf`), so it builds
 * freestanding. Force-inline, like `lh/math/floor.h`.
 */

#ifndef LH_MATH_FSCALAR_ROUND_H
#define LH_MATH_FSCALAR_ROUND_H

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/fscalar.h>
#include <lh/numeric/fixed/types.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Largest whole number not above @p v.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_s32_t
lh_math_fscalar_floor_s32(lh_math_fscalar_t v)
{
    const lh_s32_t i = lh_cast_static(lh_s32_t, v);
    return lh_cast_static(lh_math_fscalar_t, i) > v ? i - 1 : i;
}

/**
 * @brief Smallest whole number not below @p v.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_s32_t
lh_math_fscalar_ceil_s32(lh_math_fscalar_t v)
{
    const lh_s32_t i = lh_cast_static(lh_s32_t, v);
    return lh_cast_static(lh_math_fscalar_t, i) < v ? i + 1 : i;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_FSCALAR_ROUND_H */
