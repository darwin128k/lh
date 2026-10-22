#include <lh/real/diy.h>

#if LH_REAL_ENABLED

#    include <lh/assert.h>
#    include <lh/bit/scan.h>
#    include <lh/cast/static.h>
#    include <lh/math/ops.h>
#    include <lh/numeric/mul.h>
#    include <lh/real/bits.h>

lh_real_diy_t
lh_real_diy_make(lh_u64_t f, lh_s32_t e)
{
    lh_real_diy_t self;

    self.f = f;
    self.e = e;
    return self;
}

lh_real_diy_t
lh_real_diy_from_real(lh_real_t value)
{
    const lh_u64_t bits = lh_real_to_bits(value);
    const lh_u32_t biased = lh_real_bits_get_exponent(bits);
    const lh_u64_t fraction = lh_real_bits_get_fraction(bits);
    /* What turns the stored exponent into the power of two of the integer significand. */
    const lh_s32_t offset = LH_REAL_EXPONENT_BIAS + LH_REAL_MANTISSA_BITS;

    if (lh_math_is_zero(biased))
    {
        /* Subnormal: no implicit 1, and the exponent of the smallest normal. */
        return lh_real_diy_make(fraction, lh_math_sub(1, offset));
    }
    return lh_real_diy_make(lh_math_add(fraction, lh_math_bit_shl(1ULL, LH_REAL_MANTISSA_BITS)),
                            lh_math_sub(lh_cast_static(lh_s32_t, biased), offset));
}

lh_real_diy_t
lh_real_diy_normalize(lh_real_diy_t self)
{
    const lh_s32_t shift =
        lh_cast_static(lh_s32_t, lh_math_sub(63U, lh_bit_scan_reverse_u64(self.f)));

    return lh_real_diy_make(lh_math_bit_shl(self.f, shift), lh_math_sub(self.e, shift));
}

lh_real_diy_t
lh_real_diy_normalize_boundary(lh_real_diy_t self)
{
    /* A boundary is a significand with one more bit below: bring it up to that
       width first, then to the top of the 64 bits. */
    const lh_u64_t top = lh_math_bit_shl(1ULL, LH_REAL_MANTISSA_BITS + 1);
    const lh_s32_t rest = 64 - LH_REAL_MANTISSA_BITS - 2;

    while (lh_math_is_zero(lh_math_bit_and(self.f, top)))
    {
        self.f = lh_math_bit_shl(self.f, 1U);
        self.e = lh_math_sub_one(self.e);
    }
    return lh_real_diy_make(lh_math_bit_shl(self.f, rest), lh_math_sub(self.e, rest));
}

lh_void
lh_real_diy_get_boundaries(lh_real_t value, lh_real_diy_t *minus, lh_real_diy_t *plus)
{
    const lh_real_diy_t v = lh_real_diy_from_real(value);
    const lh_u64_t hidden = lh_math_bit_shl(1ULL, LH_REAL_MANTISSA_BITS);
    const lh_real_diy_t upper = lh_real_diy_normalize_boundary(
        lh_real_diy_make(lh_math_add_one(lh_math_bit_shl(v.f, 1U)), lh_math_sub_one(v.e)));
    /* At a power of two the number below is half as far away, so the lower
       halfway point is a quarter step down instead of a half. */
    lh_real_diy_t lower =
        lh_math_eq(v.f, hidden)
            ? lh_real_diy_make(lh_math_sub_one(lh_math_bit_shl(v.f, 2U)), lh_math_sub(v.e, 2))
            : lh_real_diy_make(lh_math_sub_one(lh_math_bit_shl(v.f, 1U)), lh_math_sub_one(v.e));

    lh_assert_runtime_ref(minus);
    lh_assert_runtime_ref(plus);
    lower.f = lh_math_bit_shl(lower.f, lh_math_sub(lower.e, upper.e));
    lower.e = upper.e;
    lh_ptr_deref(minus) = lower;
    lh_ptr_deref(plus) = upper;
}

lh_real_diy_t
lh_real_diy_sub(lh_real_diy_t a, lh_real_diy_t b)
{
    return lh_real_diy_make(lh_math_sub(a.f, b.f), a.e);
}

lh_real_diy_t
lh_real_diy_mul(lh_real_diy_t a, lh_real_diy_t b)
{
    lh_u64_t high;
    const lh_u64_t low = lh_numeric_mul_u64_full(a.f, b.f, lh_addr_of(high));

    /* Round to nearest on the bit below the kept 64. */
    return lh_real_diy_make(lh_math_add(high, lh_math_bit_shr(low, 63U)),
                            lh_math_add(lh_math_add(a.e, b.e), 64));
}

#endif /* LH_REAL_ENABLED */
