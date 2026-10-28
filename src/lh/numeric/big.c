#include <lh/numeric/big.h>
#include <lh/math.h>
#include <lh/assert.h>

lh_u32_t
lh_numeric_big_get_word(const lh_numeric_big_t *self, lh_usize_t index)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(lh_math_lt(index, LH_NUMERIC_BIG_WORDS), lh_runtime_error_code_out_of_range);
    return self->words[index];
}

lh_void
lh_numeric_big_set_word(lh_numeric_big_t *self, lh_usize_t index, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ifn(lh_math_lt(index, LH_NUMERIC_BIG_WORDS), lh_runtime_error_code_out_of_range);
    self->words[index] = value;
}

lh_void
lh_numeric_big_set_u32(lh_numeric_big_t *self, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    lh_numeric_big_set_word(self, 0, value);
    for (lh_usize_t i = 1U; lh_math_lt(i, LH_NUMERIC_BIG_WORDS); i = lh_math_add_one(i))
    {
        lh_numeric_big_set_word(self, i, 0U);
    }
}

lh_void
lh_numeric_big_mul_u32(lh_numeric_big_t *self, lh_u32_t k)
{
    lh_u64_t carry = 0U;
    lh_usize_t hi = LH_NUMERIC_BIG_WORDS;

    lh_assert_runtime_ref(self);

    /* Skip leading zero words so common float ratios stay in 1–2 limbs. */
    while (lh_math_gt(hi, 0U) && lh_math_is_zero(lh_numeric_big_get_word(self, lh_math_sub_one(hi))))
    {
        hi = lh_math_sub_one(hi);
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, hi); i = lh_math_add_one(i))
    {
        const lh_u64_t p =
            lh_math_add(lh_math_mul(lh_cast_static(lh_u64_t, lh_numeric_big_get_word(self, i)), lh_cast_static(lh_u64_t, k)),
                        carry);

        lh_numeric_big_set_word(self, i, lh_cast_static(lh_u32_t, lh_bit_and(p, 0xFFFFFFFFU)));
        carry = lh_bit_shr(p, 32U);
    }
    if (lh_math_ne(carry, 0U) && lh_math_lt(hi, LH_NUMERIC_BIG_WORDS))
    {
        lh_numeric_big_set_word(self, hi, lh_cast_static(lh_u32_t, carry));
        for (lh_usize_t i = lh_math_add_one(hi); lh_math_lt(i, LH_NUMERIC_BIG_WORDS);
             i = lh_math_add_one(i))
        {
            lh_numeric_big_set_word(self, i, 0U);
        }
    }
    else if (lh_math_lt(hi, LH_NUMERIC_BIG_WORDS))
    {
        for (lh_usize_t i = hi; lh_math_lt(i, LH_NUMERIC_BIG_WORDS); i = lh_math_add_one(i))
        {
            lh_numeric_big_set_word(self, i, 0U);
        }
    }
}

lh_void
lh_numeric_big_shl(lh_numeric_big_t *self, lh_usize_t bits)
{
    const lh_usize_t words = lh_math_div(bits, 32U);
    const lh_usize_t rest = lh_math_mod(bits, 32U);

    lh_assert_runtime_ref(self);
    for (lh_usize_t i = LH_NUMERIC_BIG_WORDS; lh_math_gt(i, 0U); i = lh_math_sub_one(i))
    {
        const lh_usize_t to = lh_math_sub_one(i);
        lh_u32_t v = 0U;

        if (lh_math_ge(to, words))
        {
            v = lh_bit_shl(lh_numeric_big_get_word(self, lh_math_sub(to, words)), rest);
            if (lh_math_ne(rest, 0U) && lh_math_gt(to, words))
            {
                lh_bit_set(v, lh_bit_shr(lh_numeric_big_get_word(self, lh_math_sub_one(lh_math_sub(to, words))),
                                         lh_math_sub(32U, rest)));
            }
        }
        lh_numeric_big_set_word(self, to, v);
    }
}

lh_sint_t
lh_numeric_big_compare(const lh_numeric_big_t *a, const lh_numeric_big_t *b)
{
    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    for (lh_usize_t i = LH_NUMERIC_BIG_WORDS; lh_math_gt(i, 0U); i = lh_math_sub_one(i))
    {
        const lh_u32_t x = lh_numeric_big_get_word(a, lh_math_sub_one(i));
        const lh_u32_t y = lh_numeric_big_get_word(b, lh_math_sub_one(i));

        if (lh_math_ne(x, y))
        {
            return lh_math_lt(x, y) ? -1 : 1;
        }
    }
    return 0;
}

lh_void
lh_numeric_big_sub(lh_numeric_big_t *a, const lh_numeric_big_t *b)
{
    lh_u64_t borrow = 0U;

    lh_assert_runtime_ref(a);
    lh_assert_runtime_ref(b);
    for (lh_usize_t i = 0U; lh_math_lt(i, LH_NUMERIC_BIG_WORDS); i = lh_math_add_one(i))
    {
        const lh_u64_t x = lh_cast_static(lh_u64_t, lh_numeric_big_get_word(a, i));
        const lh_u64_t y = lh_math_add(lh_cast_static(lh_u64_t, lh_numeric_big_get_word(b, i)), borrow);

        if (lh_math_ge(x, y))
        {
            lh_numeric_big_set_word(a, i, lh_cast_static(lh_u32_t, lh_math_sub(x, y)));
            borrow = 0U;
        }
        else
        {
            lh_numeric_big_set_word(a, i, lh_cast_static(lh_u32_t, lh_math_sub(lh_math_add(x, 0x100000000ULL), y)));
            borrow = 1U;
        }
    }
}
