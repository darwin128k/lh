#include <lh/str/format/base64.h>
#include <lh/char/map.h>
#include <lh/assert.h>
#include <lh/math.h>

/** The 64 digits, in value order. */
static const lh_char_t m_base64_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

lh_usize_t
lh_str_ptr_format_base64_size(lh_usize_t data_size)
{
    return lh_math_mul(lh_math_div(lh_math_add(data_size, 2U), 3U), 4U);
}

lh_void
lh_str_ptr_format_base64_block(lh_byte_cptr data, lh_usize_t count, lh_str_ptr str)
{
    const lh_u32_t b0 = data[0];
    const lh_u32_t b1 = lh_math_gt(count, 1U) ? data[1] : 0U;
    const lh_u32_t b2 = lh_math_gt(count, 2U) ? data[2] : 0U;
    const lh_u32_t n = lh_bit_or(lh_bit_or(lh_bit_shl(b0, 16U), lh_bit_shl(b1, 8U)), b2);

    lh_assert_runtime_if(lh_math_is_zero(count) || lh_math_gt(count, 3U),
                         lh_runtime_error_code_invalid_argument);
    str[0] = m_base64_alphabet[lh_bit_and(lh_bit_shr(n, 18U), 63U)];
    str[1] = m_base64_alphabet[lh_bit_and(lh_bit_shr(n, 12U), 63U)];
    str[2] = lh_math_gt(count, 1U) ? m_base64_alphabet[lh_bit_and(lh_bit_shr(n, 6U), 63U)] : lh_char_map_equals;
    str[3] = lh_math_gt(count, 2U) ? m_base64_alphabet[lh_bit_and(n, 63U)] : lh_char_map_equals;
}

lh_usize_t
lh_str_ptr_format_base64(const lh_ptr data, lh_usize_t data_size, lh_str_ptr str,
                         lh_usize_t str_size)
{
    const lh_byte_cptr bytes = lh_ptr_rcast(const lh_byte_t, data);
    const lh_usize_t size = lh_str_ptr_format_base64_size(data_size);
    lh_usize_t i = 0U;
    lh_usize_t o = 0U;

    lh_assert_runtime_ref(str);
    if (lh_math_gt(size, str_size))
    {
        return 0U;
    }
    for (; lh_math_lt(i, data_size); i = lh_math_add(i, 3U), o = lh_math_add(o, 4U))
    {
        lh_str_ptr_format_base64_block(bytes + i, lh_math_min(lh_math_sub(data_size, i), 3U),
                                       str + o);
    }
    return size;
}
