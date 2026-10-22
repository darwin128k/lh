#include <lh/real/bits.h>

#if LH_REAL_ENABLED

#    include <lh/cast/static.h>
#    include <lh/math/ops.h>

#    if LH_LIBRARY_OPTION_REAL == LH_REAL_DOUBLE
typedef lh_u64_t lh_real_word_t;
#    else
typedef lh_u32_t lh_real_word_t;
#    endif

/* Reading a union through another member than the one written is how C
   reinterprets bits without breaking the aliasing rules. */
union lh_real_pun
{
    lh_real_t value;
    lh_real_word_t word;
};

lh_u64_t
lh_real_to_bits(lh_real_t value)
{
    union lh_real_pun pun;

    pun.value = value;
    return pun.word;
}

lh_real_t
lh_real_from_bits(lh_u64_t bits)
{
    union lh_real_pun pun;

    pun.word = lh_cast_static(lh_real_word_t, bits);
    return pun.value;
}

lh_bool_t
lh_real_bits_is_negative(lh_u64_t bits)
{
    return lh_cast_static(lh_bool_t, !lh_math_is_zero(lh_math_bit_shr(bits, LH_REAL_SIGN_SHIFT)));
}

lh_u32_t
lh_real_bits_get_exponent(lh_u64_t bits)
{
    return lh_cast_static(lh_u32_t, lh_math_bit_and(lh_math_bit_shr(bits, LH_REAL_MANTISSA_BITS),
                                                    LH_REAL_EXPONENT_MAX));
}

lh_u64_t
lh_real_bits_get_fraction(lh_u64_t bits)
{
    return lh_math_bit_and(bits, lh_math_sub_one(lh_math_bit_shl(1ULL, LH_REAL_MANTISSA_BITS)));
}

lh_u64_t
lh_real_bits_get_magnitude(lh_u64_t bits)
{
    return lh_math_bit_and(bits, lh_math_sub_one(lh_math_bit_shl(1ULL, LH_REAL_SIGN_SHIFT)));
}

lh_bool_t
lh_real_bits_is_finite(lh_u64_t bits)
{
    return lh_cast_static(lh_bool_t,
                          lh_math_ne(lh_real_bits_get_exponent(bits), LH_REAL_EXPONENT_MAX));
}

lh_u64_t
lh_real_bits_make(lh_bool_t negative, lh_u32_t exponent, lh_u64_t fraction)
{
    return lh_math_bit_or(
        lh_math_bit_or(
            lh_math_bit_shl(lh_cast_static(lh_u64_t, negative ? 1U : 0U), LH_REAL_SIGN_SHIFT),
            lh_math_bit_shl(lh_cast_static(lh_u64_t, exponent), LH_REAL_MANTISSA_BITS)),
        fraction);
}

#endif /* LH_REAL_ENABLED */
