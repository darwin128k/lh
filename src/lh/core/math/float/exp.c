/**
 * @file exp.c
 * @brief Implementation of `lh/core/math/float/exp.h`.
 */

#include <lh/core/math/float/bits.h>
#include <lh/core/math/float/exp.h>
#include <lh/core/math/float/ln2.h>
#include <lh/core/math/float/round.h>

/* Above this the exponential does not fit in a float: e^88.8 is past
   FLT_MAX. Below this it is smaller than the smallest float. */
#define LH_MATH_FLOAT_EXP_MAX 88.8f
#define LH_MATH_FLOAT_EXP_MIN (-104.0f)

lh_float_t
lh_math_float_exp(lh_float_t x)
{
    lh_int_t   turns;
    lh_float_t remainder;
    lh_float_t series;
    lh_float_t scale;
    lh_float_t extra;

    if (x != x)
    {
        return x; /* A NaN stays a NaN. */
    }
    if (x > LH_MATH_FLOAT_EXP_MAX)
    {
        return lh_math_float_from_bits(LH_MATH_FLOAT_INFINITY_BITS);
    }
    if (x < LH_MATH_FLOAT_EXP_MIN)
    {
        return 0.0f;
    }

    /* The nearest whole number of turns of ln(2). */
    turns = lh_math_float_floor_to_int(x / LH_MATH_FLOAT_LN2 + 0.5f);

    /* What is left of the argument: exact in the high part, rounded in the
       low one. */
    remainder = (x - (lh_float_t) turns * LH_MATH_FLOAT_LN2_HI)
                - (lh_float_t) turns * LH_MATH_FLOAT_LN2_LO;

    /* e^remainder: the series to the seventh power, which is below the last
       digit of a float for half a turn. */
    series = 1.0f / 5040.0f;
    series = 1.0f / 720.0f + remainder * series;
    series = 1.0f / 120.0f + remainder * series;
    series = 1.0f / 24.0f + remainder * series;
    series = 1.0f / 6.0f + remainder * series;
    series = 0.5f + remainder * series;
    series = 1.0f + remainder * series;
    series = 1.0f + remainder * series;

    /* The whole turns: a power of two, from the bits of the format. */
    extra = 1.0f;
    if (turns >= 128)
    {
        /* 2^128 is not a float. The last turn is a factor of two on top of
           2^127, and the product overflows only when the answer is too big. */
        scale = lh_math_float_from_bits(254U << LH_MATH_FLOAT_MANTISSA_BITS);
        series *= 2.0f;
    }
    else if (turns >= -126)
    {
        scale = lh_math_float_from_bits((lh_u32_t) (turns + LH_MATH_FLOAT_EXPONENT_BIAS)
                                        << LH_MATH_FLOAT_MANTISSA_BITS);
    }
    else
    {
        /* Below the smallest normal float the result is a subnormal: the
           factor of two comes last, so that it does not vanish on the way. */
        scale = lh_math_float_from_bits((lh_u32_t) (turns + 253)
                                        << LH_MATH_FLOAT_MANTISSA_BITS);
        extra = LH_FLOAT_T_MIN;
    }

    return series * scale * extra;
}
