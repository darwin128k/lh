#include <lh/char/digit.h>
#include <lh/assert.h>
#include <lh/runtime/error.h>
#include <lh/util/char.h>
#include <lh/util/numeric.h>

lh_bool_t
lh_char_is_digit(lh_char_t ch)
{
    return (ch >= '0' && ch <= '9') ? lh_bool_true : lh_bool_false;
}

lh_uchar_t
lh_char_to_digit(lh_char_t ch)
{
    lh_assert_runtime_if(!lh_char_is_digit(ch),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    return (lh_uchar_t)(lh_char_ord(ch) - lh_char_ord('0'));
}

lh_char_t
lh_char_from_digit(lh_uchar_t digit)
{
    lh_assert_runtime_if(digit > 9U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));
    return lh_char_ord_to(lh_char_t, lh_char_ord('0') + digit);
}

lh_bool_t
lh_char_digit_accumulate(lh_uint_t *value, lh_uchar_t digit)
{
    lh_assert_runtime_ref(value);
    lh_assert_runtime_if(digit > 9U,
                         lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_argument));

    if (*value > (lh_numeric_limit_max(lh_uint_t) - digit) / LH_CHAR_DIGIT_RADIX)
    {
        return lh_bool_false; /* would overflow lh_uint_t */
    }
    *value = *value * LH_CHAR_DIGIT_RADIX + digit;
    return lh_bool_true;
}

lh_uchar_t
lh_char_digit_extract(lh_uint_t *value, lh_uint_t radix)
{
    lh_uchar_t digit;

    lh_assert_runtime_ref(value);

    digit = (lh_uchar_t)(*value % radix);
    *value /= radix;
    return digit;
}
