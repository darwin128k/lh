/**
 * @file bits.c
 * @brief Implementation of `lh/core/math/float/bits.h`.
 */

#include <lh/core/math/float/bits.h>

/* Reading a union through another member than the one written is how C
   reinterprets bits without breaking the aliasing rules. */
union lh_math_float_pun
{
    lh_float_t value;
    lh_u32_t   word;
};

lh_u32_t
lh_math_float_to_bits(lh_float_t value)
{
    union lh_math_float_pun pun;

    pun.value = value;
    return pun.word;
}

lh_float_t
lh_math_float_from_bits(lh_u32_t bits)
{
    union lh_math_float_pun pun;

    pun.word = bits;
    return pun.value;
}
