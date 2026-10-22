#include <lh/real/lemire.h>

#if LH_REAL_ENABLED

#    include <lh/assert.h>
#    include <lh/bit/scan.h>
#    include <lh/cast/static.h>
#    include <lh/math/ops.h>
#    include <lh/numeric/mul.h>
#    include <lh/real/bits.h>
#    include <lh/real/power5.h>

lh_s32_t
lh_real_lemire_get_power2(lh_s64_t q)
{
    /* 217706 / 2^16 is log2(10) to the precision the table range needs.
       Floor division by hand: `>>` on a negative number is not portable C. */
    const lh_s64_t scaled = lh_math_mul(lh_math_add(152170, 65536), q);
    const lh_s64_t floor =
        lh_math_is_negative(scaled)
            ? lh_math_neg(lh_math_bit_shr(lh_math_add(lh_math_neg(scaled), 65535), 16))
            : lh_math_bit_shr(scaled, 16);

    return lh_cast_static(lh_s32_t, lh_math_add(floor, 63));
}

lh_void
lh_real_lemire_multiply(lh_s64_t q, lh_u64_t w, lh_u64_t *high, lh_u64_t *low)
{
    /* Bits below the mantissa plus a guard and a rounding bit decide nothing
       unless they are all ones: only then can the next 64 bits of 5^q carry
       into them. */
    const lh_u64_t mask = lh_math_bit_shr(~0ULL, LH_REAL_MANTISSA_BITS + 3);
    lh_u64_t first_high;
    lh_u64_t first_low =
        lh_numeric_mul_u64_full(w, lh_real_power5_get_high(q), lh_addr_of(first_high));

    lh_assert_runtime_ref(high);
    lh_assert_runtime_ref(low);

    if (lh_math_eq(lh_math_bit_and(first_high, mask), mask))
    {
        lh_u64_t second_high;

        lh_numeric_mul_u64_full(w, lh_real_power5_get_low(q), lh_addr_of(second_high));
        first_low = lh_math_add(first_low, second_high);
        if (lh_math_gt(second_high, first_low))
        {
            first_high = lh_math_add_one(first_high);
        }
    }
    lh_ptr_deref(high) = first_high;
    lh_ptr_deref(low) = first_low;
}

lh_void
lh_real_lemire_compute(lh_s64_t q, lh_u64_t w, lh_u64_t *fraction, lh_s32_t *exponent)
{
    const lh_u64_t hidden = lh_math_bit_shl(1ULL, LH_REAL_MANTISSA_BITS);
    lh_u64_t high;
    lh_u64_t low;
    lh_u64_t mantissa;
    lh_s32_t lz;
    lh_s32_t upperbit;
    lh_s32_t shift;
    lh_s32_t power2;

    lh_assert_runtime_ref(fraction);
    lh_assert_runtime_ref(exponent);

    if (lh_math_is_zero(w) || lh_math_lt(q, LH_REAL_POWER5_MIN))
    {
        lh_ptr_deref(fraction) = 0U;
        lh_ptr_deref(exponent) = 0;
        return;
    }
    if (lh_math_gt(q, LH_REAL_POWER5_MAX))
    {
        lh_ptr_deref(fraction) = 0U;
        lh_ptr_deref(exponent) = LH_REAL_EXPONENT_MAX;
        return;
    }

    lz = lh_cast_static(lh_s32_t, lh_math_sub(63U, lh_bit_scan_reverse_u64(w)));
    w = lh_math_bit_shl(w, lz);
    lh_real_lemire_multiply(q, w, lh_addr_of(high), lh_addr_of(low));

    upperbit = lh_cast_static(lh_s32_t, lh_math_bit_shr(high, 63));
    shift = lh_math_sub(lh_math_add(upperbit, 64), LH_REAL_MANTISSA_BITS + 3);
    mantissa = lh_math_bit_shr(high, shift);
    power2 = lh_math_add(lh_math_sub(lh_math_add(lh_real_lemire_get_power2(q), upperbit), lz),
                         LH_REAL_EXPONENT_BIAS);

    if (lh_math_le(power2, 0))
    {
        /* Subnormal: shift into place, then round once, as the hardware would. */
        if (lh_math_ge(lh_math_add_one(lh_math_neg(power2)), 64))
        {
            lh_ptr_deref(fraction) = 0U;
            lh_ptr_deref(exponent) = 0;
            return;
        }
        mantissa = lh_math_bit_shr(mantissa, lh_math_add_one(lh_math_neg(power2)));
        mantissa = lh_math_add(mantissa, lh_math_bit_and(mantissa, 1U));
        mantissa = lh_math_bit_shr(mantissa, 1U);
        /* Rounding up may have made it the smallest normal number. */
        lh_ptr_deref(exponent) = lh_math_lt(mantissa, hidden) ? 0 : 1;
        lh_ptr_deref(fraction) = lh_math_bit_and(mantissa, lh_math_sub_one(hidden));
        return;
    }

    /* An exact tie (nothing below the rounding bit) rounds to even: drop the
       rounding bit instead of carrying it, when the mantissa is odd. */
    if (lh_math_le(low, 1U) && lh_math_ge(q, LH_REAL_LEMIRE_ROUND_EVEN_MIN) &&
        lh_math_le(q, LH_REAL_LEMIRE_ROUND_EVEN_MAX) &&
        lh_math_eq(lh_math_bit_and(mantissa, 3U), 1U) &&
        lh_math_eq(lh_math_bit_shl(mantissa, shift), high))
    {
        mantissa = lh_math_bit_and(mantissa, ~1ULL);
    }
    mantissa = lh_math_add(mantissa, lh_math_bit_and(mantissa, 1U));
    mantissa = lh_math_bit_shr(mantissa, 1U);
    if (lh_math_ge(mantissa, lh_math_bit_shl(hidden, 1U)))
    {
        /* Rounding carried out of the mantissa: 1.111… became 10.000…. */
        mantissa = hidden;
        power2 = lh_math_add_one(power2);
    }
    if (lh_math_ge(power2, LH_REAL_EXPONENT_MAX))
    {
        lh_ptr_deref(fraction) = 0U;
        lh_ptr_deref(exponent) = LH_REAL_EXPONENT_MAX;
        return;
    }
    lh_ptr_deref(fraction) = lh_math_bit_and(mantissa, lh_math_sub_one(hidden));
    lh_ptr_deref(exponent) = power2;
}

#endif /* LH_REAL_ENABLED */
