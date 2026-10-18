#include <lh/str/format/uint.h>
#include <lh/math.h>
#include <lh/char/digit.h>
#include <lh/memory/std.h>

lh_usize_t
lh_str_ptr_format_uint(lh_uint_t value, lh_str_ptr str, lh_usize_t str_size)
{
    lh_char_t digits[LH_STR_FORMAT_UINT_DIGITS_MAX];
    lh_usize_t digit_count = 0;

    lh_assert_runtime_ref(str);

    do
    {
        digits[digit_count] =
            lh_char_from_digit(lh_char_digit_extract(lh_addr_of(value), LH_CHAR_DIGIT_RADIX));
        digit_count = lh_math_add_one(digit_count);
    } while (lh_math_gt(value, 0U));

    if (lh_math_gt(digit_count, str_size))
    {
        return 0;
    }

    lh_memory_std_copy_rev(str, digits, digit_count);
    return digit_count;
}
