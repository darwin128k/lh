#include <lh/str/format/uint.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/util/addr.h>
#include <lh/util/algorithm.h>

/* lh_uint_t is 32-bit; "4294967295" is its longest decimal form. */
#define LH_STR_FORMAT_UINT_DIGITS_MAX 10U

lh_usize_t
lh_str_ptr_format_uint(lh_uint_t value, lh_str_ptr str, lh_usize_t str_size)
{
    lh_char_t digits[LH_STR_FORMAT_UINT_DIGITS_MAX];
    lh_usize_t digit_count = 0;
    lh_usize_t n;

    lh_assert_runtime_ref(str);

    do
    {
        digits[digit_count++] =
            lh_char_from_digit(lh_char_digit_extract(lh_addr_of(value), LH_CHAR_DIGIT_RADIX));
    } while (value > 0U);

    if (digit_count > str_size)
    {
        return 0;
    }

    /* digits[] holds the least significant digit first; str wants the most
     * significant first, i.e. exactly a reverse copy. n is a scratch copy of
     * digit_count: lh_algorithm_copy_rev consumes its count argument via
     * while(n--), and digit_count is still needed below for the return value. */
    n = digit_count;
    lh_algorithm_copy_rev(lh_char_t, str, digits, n);
    return digit_count;
}
