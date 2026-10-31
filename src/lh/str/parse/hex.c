#include <lh/str/parse/hex.h>
#include <lh/char/xdigit.h>
#include <lh/math.h>
#include <lh/util/ptr.h>

lh_bool_t
lh_str_ptr_parse_hex(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (lh_math_is_zero(str_size))
    {
        return lh_bool_false;
    }

    while (lh_math_lt(pos, str_size) && lh_char_is_xdigit(str[pos]))
    {
        if (!lh_char_xdigit_accumulate(lh_addr_of(value), lh_char_to_xdigit(str[pos])))
        {
            return lh_bool_false;
        }
        if (lh_math_gt(value, max))
        {
            return lh_bool_false;
        }
        pos = lh_math_add_one(pos);
    }

    if (lh_math_ne(pos, str_size))
    {
        return lh_bool_false;
    }

    lh_ptr_deref(out) = value;
    return lh_bool_true;
}

lh_usize_t
lh_str_ptr_parse_hex_prefix(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_usize_t n = 0U;

    lh_assert_runtime_ref(str);
    while (lh_math_lt(n, str_size) && lh_char_is_xdigit(str[n]))
    {
        n = lh_math_add_one(n);
    }
    if (lh_math_is_zero(n) || !lh_str_ptr_parse_hex(str, n, max, out))
    {
        return 0U;
    }
    return n;
}

lh_bool_t
lh_str_ptr_parse_hex_bytes(lh_str_cptr str, lh_usize_t str_size, lh_byte_ptr out,
                           lh_usize_t out_size)
{
    lh_usize_t i;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (lh_math_is_zero(out_size) || lh_math_lt(str_size, lh_math_mul(out_size, 2U)))
    {
        return lh_bool_false;
    }
    /* The text is checked before a byte is written: a failure leaves the
     * caller's array as it was. */
    for (i = 0U; lh_math_lt(i, out_size); i = lh_math_add_one(i))
    {
        lh_usize_t digit = lh_math_mul(i, 2U);

        if (!lh_char_is_xdigit(str[digit]) || !lh_char_is_xdigit(str[lh_math_add_one(digit)]))
        {
            return lh_bool_false;
        }
    }
    for (i = 0U; lh_math_lt(i, out_size); i = lh_math_add_one(i))
    {
        lh_usize_t digit = lh_math_mul(i, 2U);

        out[i] = (lh_byte_t)((lh_char_to_xdigit(str[digit]) << 4) |
                             lh_char_to_xdigit(str[lh_math_add_one(digit)]));
    }
    return lh_bool_true;
}
