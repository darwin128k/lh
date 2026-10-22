#include <lh/real/grisu.h>

#if LH_REAL_ENABLED

#    include <lh/assert.h>
#    include <lh/cast/static.h>
#    include <lh/char/digit.h>
#    include <lh/math/ops.h>
#    include <lh/real/cached.h>

/* 10^0 .. 10^19: every power a 64-bit integer holds. */
static const lh_u64_t m_real_grisu_pow10[20] = {1ULL,
                                                10ULL,
                                                100ULL,
                                                1000ULL,
                                                10000ULL,
                                                100000ULL,
                                                1000000ULL,
                                                10000000ULL,
                                                100000000ULL,
                                                1000000000ULL,
                                                10000000000ULL,
                                                100000000000ULL,
                                                1000000000000ULL,
                                                10000000000000ULL,
                                                100000000000000ULL,
                                                1000000000000000ULL,
                                                10000000000000000ULL,
                                                100000000000000000ULL,
                                                1000000000000000000ULL,
                                                10000000000000000000ULL};

lh_u32_t
lh_real_grisu_count_digits(lh_u32_t value)
{
    lh_u32_t count = 1U;

    while (lh_math_ge(value, 10U))
    {
        value = lh_math_div(value, 10U);
        count = lh_math_add_one(count);
    }
    return count;
}

lh_void
lh_real_grisu_round(lh_char_t *digits, lh_usize_t count, lh_u64_t delta, lh_u64_t rest,
                    lh_u64_t ten_kappa, lh_u64_t distance)
{
    /* One step down is ten_kappa closer to the exact value; take it while it
       stays above the lower boundary and lands nearer than where we are. */
    while (lh_math_lt(rest, distance) && lh_math_ge(lh_math_sub(delta, rest), ten_kappa) &&
           (lh_math_lt(lh_math_add(rest, ten_kappa), distance) ||
            lh_math_gt(lh_math_sub(distance, rest),
                       lh_math_sub(lh_math_add(rest, ten_kappa), distance))))
    {
        digits[lh_math_sub_one(count)] =
            lh_cast_static(lh_char_t, digits[lh_math_sub_one(count)] - 1);
        rest = lh_math_add(rest, ten_kappa);
    }
}

lh_void
lh_real_grisu_digits(lh_real_diy_t w, lh_real_diy_t upper, lh_u64_t delta, lh_char_t *digits,
                     lh_usize_t *count, lh_s32_t *k)
{
    /* upper.e is in [-60, -32]: the integer part of upper fits 32 bits, and
       the fraction has |e| bits. */
    const lh_s32_t shift = lh_math_neg(upper.e);
    const lh_u64_t one = lh_math_bit_shl(1ULL, shift);
    const lh_u64_t distance = lh_math_sub(upper.f, w.f);
    lh_u32_t integral = lh_cast_static(lh_u32_t, lh_math_bit_shr(upper.f, shift));
    lh_u64_t fraction = lh_math_bit_and(upper.f, lh_math_sub_one(one));
    lh_s32_t kappa = lh_cast_static(lh_s32_t, lh_real_grisu_count_digits(integral));
    lh_usize_t n = 0U;

    lh_assert_runtime_ref(digits);
    lh_assert_runtime_ref(count);
    lh_assert_runtime_ref(k);

    while (lh_math_gt(kappa, 0))
    {
        const lh_u32_t divisor =
            lh_cast_static(lh_u32_t, m_real_grisu_pow10[lh_math_sub_one(kappa)]);
        const lh_u32_t digit = lh_math_div(integral, divisor);
        lh_u64_t rest;

        integral = lh_math_mod(integral, divisor);
        if (!lh_math_is_zero(digit) || !lh_math_is_zero(n))
        {
            digits[n] = lh_char_from_digit(lh_cast_static(lh_byte_t, digit));
            n = lh_math_add_one(n);
        }
        kappa = lh_math_sub_one(kappa);
        rest = lh_math_add(lh_math_bit_shl(lh_cast_static(lh_u64_t, integral), shift), fraction);
        if (lh_math_le(rest, delta))
        {
            lh_ptr_deref(k) = lh_math_add(lh_ptr_deref(k), kappa);
            lh_real_grisu_round(digits, n, delta, rest,
                                lh_math_bit_shl(m_real_grisu_pow10[kappa], shift), distance);
            lh_ptr_deref(count) = n;
            return;
        }
    }

    for (;;)
    {
        lh_u32_t digit;

        fraction = lh_math_mul(fraction, 10U);
        delta = lh_math_mul(delta, 10U);
        digit = lh_cast_static(lh_u32_t, lh_math_bit_shr(fraction, shift));
        if (!lh_math_is_zero(digit) || !lh_math_is_zero(n))
        {
            digits[n] = lh_char_from_digit(lh_cast_static(lh_byte_t, digit));
            n = lh_math_add_one(n);
        }
        fraction = lh_math_bit_and(fraction, lh_math_sub_one(one));
        kappa = lh_math_sub_one(kappa);
        if (lh_math_lt(fraction, delta))
        {
            const lh_s32_t index = lh_math_neg(kappa);

            lh_ptr_deref(k) = lh_math_add(lh_ptr_deref(k), kappa);
            lh_real_grisu_round(
                digits, n, delta, fraction, one,
                lh_math_lt(index, 20) ? lh_math_mul(distance, m_real_grisu_pow10[index]) : 0U);
            lh_ptr_deref(count) = n;
            return;
        }
    }
}

lh_void
lh_real_grisu(lh_real_t value, lh_char_t *digits, lh_usize_t *count, lh_s32_t *k)
{
    lh_real_diy_t minus;
    lh_real_diy_t plus;
    lh_real_diy_t power;
    lh_real_diy_t w;
    lh_usize_t index;

    lh_assert_runtime_ref(k);
    lh_real_diy_get_boundaries(value, lh_addr_of(minus), lh_addr_of(plus));
    index = lh_real_cached_find(plus.e);
    power = lh_real_cached_get(index);
    lh_ptr_deref(k) = lh_math_neg(lh_real_cached_get_k(index));

    w = lh_real_diy_mul(lh_real_diy_normalize(lh_real_diy_from_real(value)), power);
    plus = lh_real_diy_mul(plus, power);
    minus = lh_real_diy_mul(minus, power);
    /* One unit in from each boundary: the products are rounded, and a digit
       on the boundary itself might read back as the neighbour. */
    plus.f = lh_math_sub_one(plus.f);
    minus.f = lh_math_add_one(minus.f);
    lh_real_grisu_digits(w, plus, lh_math_sub(plus.f, minus.f), digits, count, k);
}

#endif /* LH_REAL_ENABLED */
