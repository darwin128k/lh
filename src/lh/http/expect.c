/**
 * @file expect.c
 * @brief Parse HTTP request expectations.
 */

#include <lh/http/expect.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/http/header/name.h>
#include <lh/http/header/value.h>
#include <lh/str/split/next.h>

lh_bool_t
lh_http_expect_try_get_continue(const lh_http_header_t *headers, lh_usize_t count, lh_bool_t *out)
{
    if (lh_math_is_zero(count))
    {
        lh_ptr_deref(out) = lh_bool_false;
        return lh_bool_true;
    }
    const lh_str_view_t name = lh_http_header_name_expect;
    const lh_str_view_t expected = lh_http_header_value_continue;
    const lh_char_t comma = lh_char_map_comma;
    lh_usize_t offset = 0U;
    lh_bool_t found = lh_bool_false;
    for (;;)
    {
        const lh_http_header_t *field =
            lh_http_header_find(headers + offset, count - offset, lh_addr_of(name));
        if (lh_null_eq(field))
        {
            lh_ptr_deref(out) = found;
            return lh_bool_true;
        }
        offset = lh_math_add_one(lh_cast_static(lh_usize_t, field - headers));
        const lh_str_view_t *value = lh_http_header_get_value(field);
        lh_usize_t pos = 0U;
        lh_bool_t delimiter;
        lh_bool_t member = lh_bool_false;
        lh_str_view_t part;
        while (lh_str_view_split_next_of(value, lh_addr_of(comma), 1U, lh_addr_of(pos),
                                         lh_addr_of(part), lh_addr_of(delimiter)))
        {
            lh_str_view_trim(lh_addr_of(part));
            if (lh_str_view_is_empty(lh_addr_of(part)))
            {
                continue;
            }
            if (!lh_str_view_equals(lh_addr_of(part), lh_addr_of(expected), lh_bool_true))
            {
                return lh_bool_false;
            }
            member = lh_bool_true;
            found = lh_bool_true;
        }
        if (!member)
        {
            return lh_bool_false;
        }
    }
}

lh_bool_t
lh_http_expect_get_continue(const lh_http_header_t *headers, lh_usize_t count)
{
    lh_bool_t value = lh_bool_false;
    lh_runtime_check_ifn(lh_http_expect_try_get_continue(headers, count, lh_addr_of(value)),
                         lh_runtime_error_code_invalid_range);
    return value;
}
