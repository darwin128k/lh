/**
 * @file sin_cos.c
 * @brief Implementation of `lh/math/float/sin_cos.h`.
 */

#include <lh/cast/static.h>
#include <lh/math/float/sin_cos.h>
#include <lh/numeric/types.h>
#include <lh/util/ptr.h>

lh_void
lh_math_float_sin_cos(lh_float_t x, lh_float_t *sin_out, lh_float_t *cos_out)
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
        lh_ptr_deref(sin_out) = s;
        lh_ptr_deref(cos_out) = c;
        break;
    case 1:
        lh_ptr_deref(sin_out) = c;
        lh_ptr_deref(cos_out) = lh_math_neg(s);
        break;
    case 2:
        lh_ptr_deref(sin_out) = lh_math_neg(s);
        lh_ptr_deref(cos_out) = lh_math_neg(c);
        break;
    default:
        lh_ptr_deref(sin_out) = lh_math_neg(c);
        lh_ptr_deref(cos_out) = s;
        break;
    }
}
