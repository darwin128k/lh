#include <lh/str/parse/hex.h>
#include <lh/assert.h>
#include <lh/char/xdigit.h>
#include <lh/util/addr.h>

lh_bool_t
lh_str_ptr_parse_hex(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_uint_t value = 0;
    lh_usize_t pos = 0;

    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size == 0)
    {
        return lh_bool_false;
    }

    while (pos < str_size && lh_char_is_xdigit(str[pos]))
    {
        if (!lh_char_xdigit_accumulate(lh_addr_of(value), lh_char_to_xdigit(str[pos])))
        {
            return lh_bool_false;
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
