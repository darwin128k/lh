/**
 * @file tan.c
 * @brief Implementation of `lh/math/float/tan.h`.
 */

#include <lh/math/float/sin_cos.h>
#include <lh/math/float/tan.h>

lh_float_t
lh_math_float_tan(lh_float_t x)
{
    lh_float_t sine;
    lh_float_t cosine;
    lh_math_float_sin_cos(x, &sine, &cosine);
    return sine / cosine;
}
