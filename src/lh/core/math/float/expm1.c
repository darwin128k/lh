/**
 * @file expm1.c
 * @brief Implementation of `lh/core/math/float/expm1.h`.
 */

#include <lh/core/math/float/exp.h>
#include <lh/core/math/float/expm1.h>
#include <lh/core/math/float/finite.h>

/* Below this the difference e^x - 1 is taken from the series itself, where it
   keeps every digit of x. */
#define LH_MATH_FLOAT_EXPM1_SERIES_MAX 0.5f

lh_float_t
lh_math_float_expm1(lh_float_t x)
{
    lh_float_t series;

    if (x != x)
    {
        return x; /* A NaN stays a NaN. */
    }
    if (x > LH_MATH_FLOAT_EXPM1_SERIES_MAX || x < -LH_MATH_FLOAT_EXPM1_SERIES_MAX)
    {
        lh_float_t exponential = lh_math_float_exp(x);

        if (!lh_math_float_is_finite(exponential))
        {
            return exponential;
        }
        return exponential - 1.0f;
    }

    /* e^x - 1 = x * (1 + x * (1/2! + x * (1/3! + ...))), the series to the
       twelfth power, which is below the last digit of a float at half a
       turn. */
    series = 1.0f / 479001600.0f;
    series = 1.0f / 39916800.0f + x * series;
    series = 1.0f / 3628800.0f + x * series;
    series = 1.0f / 362880.0f + x * series;
    series = 1.0f / 40320.0f + x * series;
    series = 1.0f / 5040.0f + x * series;
    series = 1.0f / 720.0f + x * series;
    series = 1.0f / 120.0f + x * series;
    series = 1.0f / 24.0f + x * series;
    series = 1.0f / 6.0f + x * series;
    series = 0.5f + x * series;

    return x * (1.0f + x * series);
}
