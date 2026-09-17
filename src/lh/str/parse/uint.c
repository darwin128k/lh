#include <lh/str/parse/uint.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/char/digit.h>
#include <lh/str/split/next.h>
#include <lh/util/addr.h>

lh_bool_t
lh_str_ptr_parse_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
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
        if (!lh_char_digit_accumulate(lh_addr_of(value), lh_char_to_digit(str[pos])))
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

lh_bool_t
lh_str_ptr_parse_uint(lh_str_cptr str, lh_usize_t str_size, lh_uint_t max, lh_uint_t *out)
{
    lh_assert_runtime_ref(str);
    lh_assert_runtime_ref(out);

    if (str_size > 1U && str[0] == '0')
    {
        return lh_bool_false;
    }

    return lh_str_ptr_parse_uint_digits(str, str_size, max, out);
}

lh_bool_t
lh_str_ptr_split_next_uint_digits(lh_str_cptr str, lh_usize_t str_size, lh_char_t delim,
                                  lh_usize_t *pos, lh_uint_t max, lh_uint_t *out,
                                  lh_bool_t want_delim)
{
    lh_str_cptr field;
    lh_usize_t field_size;
    lh_bool_t had_delim;

    if (!lh_str_ptr_split_next(str, str_size, delim, pos, lh_addr_of(field),
                               lh_addr_of(field_size), lh_addr_of(had_delim)))
    {
        return lh_bool_false;
    }
    if (!lh_str_ptr_parse_uint_digits(field, field_size, max, out))
    {
        return lh_bool_false;
    }

    return lh_cast_static(lh_bool_t, had_delim == want_delim);
}
