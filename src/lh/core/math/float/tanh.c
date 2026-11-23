/**
 * @file tanh.c
 * @brief Implementation of `lh/core/math/float/tanh.h`.
 */

#include <lh/core/math/float/expm1.h>
#include <lh/core/math/float/tanh.h>

/* From here the tangent of the hyperbola is one to the last digit of a
   float: tanh(9) = 0.99999997. */
#define LH_MATH_FLOAT_TANH_ONE 9.0f

lh_float_t
lh_math_float_tanh(lh_float_t x)
{
    lh_float_t magnitude;
    lh_float_t growth;

    if (x != x)
    {
        return x; /* A NaN stays a NaN. */
    }
    if (x == 0.0f)
    {
        return x; /* Zero keeps its sign. */
    }
    magnitude = x < 0.0f ? -x : x;
    if (magnitude >= LH_MATH_FLOAT_TANH_ONE)
    {
        return x < 0.0f ? -1.0f : 1.0f;
    }

    /* tanh(x) = (e^(2x) - 1) / (e^(2x) + 1): the difference first, so that a
       small x keeps its digits. */
    growth = lh_math_float_expm1(2.0f * magnitude);

    return x < 0.0f ? -(growth / (growth + 2.0f)) : growth / (growth + 2.0f);
}
