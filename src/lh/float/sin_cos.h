/**
 * @file sin_cos.h
 * @brief Library-private: single-precision sine and cosine without libm.
 *
 * `__builtin_sinf` / `__builtin_cosf` are not instructions on any target lh
 * builds for: the compiler emits calls to libm, which a freestanding build
 * does not have. So lh computes both itself, in float only:
 *
 * 1. Range reduction: `x = k * pi/2 + r` with `|r| <= pi/4`. `pi/2` is split
 *    into three floats (Cody-Waite) so `x - k * pi/2` loses no precision for
 *    `|k|` up to 2^16, i.e. `|x|` up to about 1e5 radians; beyond that the
 *    result degrades. Angles in a game or UI stay far below that.
 * 2. On `[-pi/4, pi/4]`, the minimax polynomials of Cephes' `sinf` / `cosf`
 *    (error about 1 ulp).
 * 3. The quadrant `k mod 4` picks which of `sin r`, `cos r` and their
 *    negations is the answer.
 */

#ifndef LH_SRC_FLOAT_SIN_COS_H
#define LH_SRC_FLOAT_SIN_COS_H

#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/float.h>
#include <lh/numeric/types.h>
#include <lh/void.h>

/**
 * @brief Both `sin(x)` and `cos(x)` of @p x radians, sharing one range
 *        reduction.
 */
LH_ATTRIBUTE_STATIC
lh_void
lh_float_sin_cos(lh_float_t x, lh_float_t *sin_out, lh_float_t *cos_out)
{
    const lh_float_t quarter_turns = x * 0.636619772f; /* x / (pi/2) */
    const lh_float_t rounded = quarter_turns >= 0.0f ? quarter_turns + 0.5f : quarter_turns - 0.5f;
    const lh_int_t k = lh_cast_static(lh_int_t, rounded);
    const lh_float_t kf = lh_cast_static(lh_float_t, k);
    const lh_float_t r =
        ((x - kf * 1.5703125f) - kf * 4.837512969970703125e-4f) - kf * 7.54978995489188216e-8f;
    const lh_float_t z = r * r;

    const lh_float_t s =
        r + r * z * (-1.6666654611e-1f + z * (8.3321608736e-3f + z * -1.9515295891e-4f));
    const lh_float_t c =
        1.0f - 0.5f * z +
        z * z * (4.166664568298827e-2f + z * (-1.388731625493765e-3f + z * 2.443315711809948e-5f));

    switch (k & 3)
    {
    case 0:
        *sin_out = s;
        *cos_out = c;
        break;
    case 1:
        *sin_out = c;
        *cos_out = -s;
        break;
    case 2:
        *sin_out = -s;
        *cos_out = -c;
        break;
    default:
        *sin_out = -c;
        *cos_out = s;
        break;
    }
}

#endif /* LH_SRC_FLOAT_SIN_COS_H */
