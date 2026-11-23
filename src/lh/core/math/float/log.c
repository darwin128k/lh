/**
 * @file log.c
 * @brief Implementation of `lh/core/math/float/log.h`.
 */

#include <lh/core/math/float/bits.h>
#include <lh/core/math/float/finite.h>
#include <lh/core/math/float/ln2.h>
#include <lh/core/math/float/log.h>

/* 2^64: a subnormal number times it is a normal one, exactly. */
#define LH_MATH_FLOAT_LOG_SCALE 18446744073709551616.0f

/* Around one the logarithm is a small difference of two large numbers; there
   the series around one answers it, and its argument stays below a third. */
#define LH_MATH_FLOAT_LOG_NEAR_ONE_MIN 0.5f
#define LH_MATH_FLOAT_LOG_NEAR_ONE_MAX 2.0f

lh_float_t
lh_math_float_log(lh_float_t x)
{
    lh_u32_t   bits;
    lh_u32_t   exponent;
    lh_u32_t   fraction;
    lh_float_t mantissa;
    lh_float_t whole;
    lh_float_t z;
    lh_float_t series;
    lh_float_t part;

    if (x != x)
    {
        return x; /* A NaN stays a NaN. */
    }
    if (x < 0.0f)
    {
        return lh_math_float_from_bits(LH_MATH_FLOAT_NAN_BITS);
    }
    if (x == 0.0f)
    {
        return lh_math_float_from_bits(LH_MATH_FLOAT_INFINITY_BITS
                                       | (1U << LH_MATH_FLOAT_SIGN_SHIFT));
    }
    if (!lh_math_float_is_finite(x))
    {
        return x; /* An infinity is its own logarithm. */
    }

    if (x >= LH_MATH_FLOAT_LOG_NEAR_ONE_MIN && x <= LH_MATH_FLOAT_LOG_NEAR_ONE_MAX)
    {
        /* The series around one: nothing cancels here. */
        whole = 0.0f;
        z = (x - 1.0f) / (x + 1.0f);
    }
    else
    {
        /* The number apart: the exponent gives the whole turns of ln(2), the
           fraction the mantissa between one and two. */
        bits = lh_math_float_to_bits(x);
        exponent = (bits >> LH_MATH_FLOAT_MANTISSA_BITS) & LH_MATH_FLOAT_EXPONENT_MAX;
        if (exponent == 0U)
        {
            /* A subnormal: scale it up to a normal number and take the scale
               out of the answer. */
            x *= LH_MATH_FLOAT_LOG_SCALE;
            bits = lh_math_float_to_bits(x);
            exponent = (bits >> LH_MATH_FLOAT_MANTISSA_BITS) & LH_MATH_FLOAT_EXPONENT_MAX;
            whole = -64.0f;
        }
        else
        {
            whole = 0.0f;
        }
        whole += (lh_float_t) exponent - (lh_float_t) LH_MATH_FLOAT_EXPONENT_BIAS;
        fraction = bits & ((1U << LH_MATH_FLOAT_MANTISSA_BITS) - 1U);
        mantissa = 1.0f + (lh_float_t) fraction * (1.0f / 8388608.0f);
        z = (mantissa - 1.0f) / (mantissa + 1.0f);
    }

    /* ln(x) = 2 * (z + z^3/3 + z^5/5 + ...), with z below a third. */
    series = 1.0f / 15.0f;
    series = 1.0f / 13.0f + z * z * series;
    series = 1.0f / 11.0f + z * z * series;
    series = 1.0f / 9.0f + z * z * series;
    series = 1.0f / 7.0f + z * z * series;
    series = 1.0f / 5.0f + z * z * series;
    series = 1.0f / 3.0f + z * z * series;
    part = 2.0f * z * (1.0f + z * z * series);

    /* The whole turns of ln(2), the high part exact. */
    return (whole * LH_MATH_FLOAT_LN2_HI + part) + whole * LH_MATH_FLOAT_LN2_LO;
}
