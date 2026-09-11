#include <lh/str/format/hex.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/util/addr.h>

/* lh_uint_t is 32-bit; 8 hex digits is its longest form. */
#define LH_STR_FORMAT_HEX_DIGITS_MAX 8U

lh_usize_t
lh_str_ptr_format_hex(lh_uint_t value, lh_bool_t uppercase, lh_str_ptr str, lh_usize_t str_size)
{
    lh_str_cptr alphabet = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    lh_char_t digits[LH_STR_FORMAT_HEX_DIGITS_MAX];
    lh_usize_t digit_count = 0;
    lh_usize_t i;

    lh_assert_runtime_ref(str);

    do
    {
        digits[digit_count++] =
            alphabet[lh_char_digit_extract(lh_addr_of(value), LH_STR_FORMAT_HEX_RADIX)];
    } while (value > 0U);

    if (digit_count > str_size)
    {
        return 0;
    }

    for (i = 0; i < digit_count; i++)
    {
        str[i] = digits[digit_count - 1U - i];
    }
    return digit_count;
}
