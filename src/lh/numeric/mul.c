#include <lh/numeric/mul.h>
#include <lh/assert.h>
#include <lh/bit.h>
#include <lh/cast/static.h>
#include <lh/compiler/type.h>
#include <lh/math/ops.h>

lh_u64_t
lh_numeric_mul_u64_full_halves(lh_u64_t a, lh_u64_t b, lh_u64_t *high)
{
    /* Schoolbook on 32-bit halves: a = ah:al, b = bh:bl. */
    const lh_u64_t al = lh_bit_get_low_u32(a);
    const lh_u64_t ah = lh_bit_get_high_u32(a);
    const lh_u64_t bl = lh_bit_get_low_u32(b);
    const lh_u64_t bh = lh_bit_get_high_u32(b);
    const lh_u64_t ll = lh_math_mul(al, bl);
    const lh_u64_t lh = lh_math_mul(al, bh);
    const lh_u64_t hl = lh_math_mul(ah, bl);
    const lh_u64_t hh = lh_math_mul(ah, bh);
    /* The middle column cannot overflow: each of its three parts is below 2^32. */
    const lh_u64_t middle = lh_math_add(
        lh_math_add(lh_math_bit_shr(ll, 32U), lh_bit_get_low_u32(lh)), lh_bit_get_low_u32(hl));

    lh_assert_runtime_ref(high);
    lh_ptr_deref(high) = lh_math_add(
        lh_math_add(hh, lh_math_add(lh_math_bit_shr(lh, 32U), lh_math_bit_shr(hl, 32U))),
        lh_math_bit_shr(middle, 32U));
    return lh_math_bit_or(lh_math_bit_shl(middle, 32U), lh_bit_get_low_u32(ll));
}

lh_u64_t
lh_numeric_mul_u64_full(lh_u64_t a, lh_u64_t b, lh_u64_t *high)
{
#if LH_COMPILER_TYPE_IS_GCC_LIKE && defined(__SIZEOF_INT128__)
    const unsigned __int128 product = lh_math_mul(lh_cast_static(unsigned __int128, a), b);

    lh_assert_runtime_ref(high);
    lh_ptr_deref(high) = lh_cast_static(lh_u64_t, lh_math_bit_shr(product, 64));
    return lh_cast_static(lh_u64_t, product);
#else
    return lh_numeric_mul_u64_full_halves(a, b, high);
#endif
}
