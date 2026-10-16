/**
 * @file centre.h
 * @brief Centre an extent in a room, and the leftover slack, on ::lh_math_scalar_t.
 *
 * Pixel layout centres with a half-up bias: `(room - own + 1) / 2`, then never
 * negative. The leftover half-pixel of an odd slack goes to the far side (right
 * / bottom), matching ::lh_ui_scalar_centre and the UI text / layout callers.
 *
 * Force-inline (like `lh/math/floor.h`): no `.c` file.
 */

#ifndef LH_MATH_SCALAR_CENTRE_H
#define LH_MATH_SCALAR_CENTRE_H

#include <lh/attribute/force_inline.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/ops.h>
#include <lh/math/scalar.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @def lh_math_slack(room, own)
 * @brief `max(0, room - own)` for any arithmetic type; the one slack formula
 *        (::lh_math_scalar_slack, ::lh_ui_scalar_slack). Arguments are
 *        evaluated more than once.
 */
#define lh_math_slack(room, own) lh_math_max(lh_math_sub((room), (own)), 0)

/**
 * @def lh_math_centre_half(room, own)
 * @brief `(room - own + 1) / 2` for any arithmetic type, before truncation and
 *        the zero clamp: the one centring formula (::lh_math_scalar_centre,
 *        ::lh_ui_scalar_centre).
 */
#define lh_math_centre_half(room, own) lh_math_div(lh_math_add(lh_math_sub((room), (own)), 1), 2)

/**
 * @brief Non-negative leftover room: `max(0, room - own)`.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_math_scalar_t
lh_math_scalar_slack(lh_math_scalar_t room, lh_math_scalar_t own)
{
    return lh_math_slack(room, own);
}

/**
 * @brief Offset that centres @p own in @p room: `max(0, (room - own + 1) / 2)`.
 *
 * Truncating integer division; the `+ 1` hands an odd leftover pixel to the far
 * side. Equivalent to `(lh_math_scalar_slack(room, own) + 1) / 2` when the
 * dividend stays non-negative.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_math_scalar_t
lh_math_scalar_centre(lh_math_scalar_t room, lh_math_scalar_t own)
{
    return lh_math_max(lh_math_centre_half(room, own), 0);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_MATH_SCALAR_CENTRE_H */
