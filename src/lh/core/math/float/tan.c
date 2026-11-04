/**
 * @file tan.c
 * @brief Implementation of `lh/core/math/float/tan.h`.
 */

#include <lh/core/math/float/tan.h>

#include <lh/core/math/float/sin_cos.h>
#include <lh/core/util/addr.h>

lh_float_t
lh_math_float_tan(lh_float_t x)
{
    {
        lh_float_t sine;
        lh_float_t cosine;

        lh_math_float_sin_cos(x, lh_addr_of(sine), lh_addr_of(cosine));
        return sine / cosine;
    }
}
