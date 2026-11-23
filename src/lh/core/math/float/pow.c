/**
 * @file pow.c
 * @brief Implementation of `lh/core/math/float/pow.h`.
 */

#include <lh/core/bool.h>
#include <lh/core/math/float/bits.h>
#include <lh/core/math/float/exp.h>
#include <lh/core/math/float/log.h>
#include <lh/core/math/float/pow.h>
#include <lh/core/math/float/round.h>
#include <lh/core/math/ops.h>

/* Above this the exponent is too large to be a whole number of turns of
   repeated squaring, and a negative base has no answer for it. */
#define LH_MATH_FLOAT_POW_INTEGER_MAX 1048576.0f

lh_float_t
lh_math_float_pow(lh_float_t x, lh_float_t y)
{
    lh_bool_t  negative;
    lh_bool_t  whole;
    lh_float_t base;
    lh_float_t result;
    lh_int_t   exponent;
    lh_int_t   rest;

    if (y == 0.0f)
    {
        return 1.0f;
    }
    if (x != x || y != y)
    {
        return lh_math_float_from_bits(LH_MATH_FLOAT_NAN_BITS);
    }
    if (x == 0.0f)
    {
        if (y > 0.0f)
        {
            return 0.0f;
        }
        return lh_math_float_from_bits(LH_MATH_FLOAT_INFINITY_BITS);
    }

    negative = x < 0.0f;
    base = negative ? -x : x;

    /* A whole exponent within reach: the answer comes from repeated squaring,
       and a negative base needs it to be whole. */
    whole = y <= LH_MATH_FLOAT_POW_INTEGER_MAX && y >= -LH_MATH_FLOAT_POW_INTEGER_MAX
            && y == (lh_float_t) lh_math_float_floor_to_int(y);
    if (negative && !whole)
    {
        return lh_math_float_from_bits(LH_MATH_FLOAT_NAN_BITS);
    }
    if (!whole)
    {
        return lh_math_float_exp(y * lh_math_float_log(base));
    }

    exponent = lh_math_float_floor_to_int(y);
    result = 1.0f;
    rest = exponent < 0 ? -exponent : exponent;
    while (rest > 0)
    {
        if (lh_math_bit_and(rest, 1) != 0)
        {
            result *= base;
        }
        base *= base;
        rest >>= 1;
    }
    if (exponent < 0)
    {
        result = 1.0f / result;
    }
    if (negative && lh_math_mod(exponent, 2) != 0)
    {
        result = -result;
    }

    return result;
}
