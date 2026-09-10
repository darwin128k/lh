#include <lh/str/parse/uint.h>
#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/util/addr.h>

lh_bool_t
lh_str_ptr_parse_uint(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size == 0)
    {
        return lh_bool_false;
    }

    while (pos < str_size && lh_char_is_digit(str[pos]))
    {
        if (pos == 1 && str[0] == '0')
        {
            return lh_bool_false; /* leading zero, e.g. "0080" */
        }
        if (!lh_char_digit_accumulate(lh_addr_of(value), lh_char_to_digit(str[pos])))
        {
            return lh_bool_false; /* would overflow lh_uint_t itself */
        }
        if (value > max)
        {
            return lh_bool_false;
        }
        pos++;
    }

    if (pos != str_size)
    {
        return lh_bool_false;
    }

    *out = value;
    return lh_bool_true;
}
