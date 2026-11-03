/**
 * @file framing.c
 * @brief HTTP request transfer coding and Content-Length framing.
 */

#include <lh/http/framing.h>
#include <lh/http/header/name.h>
#include <lh/http/header/value.h>

#include <lh/assert.h>
#include <lh/str/parse/ullong.h>

lh_bool_t
lh_http_framing_try_is_chunked(const lh_http_header_t *headers, lh_usize_t count, lh_bool_t *out)
{
    const lh_str_view_t transfer_name = lh_http_header_name_transfer_encoding;
    const lh_http_header_t *field = lh_http_header_find(headers, count, lh_addr_of(transfer_name));
    if (lh_null_eq(field))
    {
        lh_ptr_deref(out) = lh_bool_false;
        return lh_bool_true;
    }
    const lh_usize_t next = lh_math_add_one(lh_cast_static(lh_usize_t, field - headers));
    const lh_str_view_t length_name = lh_http_header_name_content_length;
    const lh_str_view_t coding = lh_http_header_value_chunked;
    if (lh_null_ne(lh_http_header_find(headers + next, lh_math_sub(count, next),
                                       lh_addr_of(transfer_name))) ||
        lh_null_ne(lh_http_header_find(headers, count, lh_addr_of(length_name))) ||
        !lh_str_view_equals(lh_http_header_get_value(field), lh_addr_of(coding), lh_bool_true))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_bool_true;
    return lh_bool_true;
}

lh_bool_t
lh_http_framing_is_chunked(const lh_http_header_t *headers, lh_usize_t count)
{
    lh_bool_t chunked = lh_bool_false;
    lh_runtime_check_ifn(lh_http_framing_try_is_chunked(headers, count, lh_addr_of(chunked)),
                         lh_runtime_error_code_invalid_range);
    return chunked;
}

lh_bool_t
lh_http_framing_try_get_content_length(const lh_http_header_t *headers, lh_usize_t count,
                                       lh_usize_t *out)
{
    const lh_str_view_t transfer_name = lh_http_header_name_transfer_encoding;
    if (lh_http_header_find(headers, count, lh_addr_of(transfer_name)) != lh_null)
    {
        return lh_bool_false;
    }
    const lh_str_view_t length_name = lh_http_header_name_content_length;
    const lh_http_header_t *field = lh_http_header_find(headers, count, lh_addr_of(length_name));
    if (field == lh_null)
    {
        lh_ptr_deref(out) = 0U;
        return lh_bool_true;
    }
    const lh_usize_t index = lh_cast_static(lh_usize_t, field - headers);
    const lh_usize_t next = lh_math_add_one(index);
    if (lh_http_header_find(headers + next, lh_math_sub(count, next), lh_addr_of(length_name)) !=
        lh_null)
    {
        return lh_bool_false;
    }
    const lh_str_view_t *value = lh_http_header_get_value(field);
    if (lh_str_view_is_empty(value))
    {
        return lh_bool_false;
    }
    lh_ullong_t length;
    if (!lh_str_ptr_parse_ullong_digits(lh_str_view_get_data(value), lh_str_view_get_size(value),
                                        lh_cast_static(lh_ullong_t, LH_USIZE_T_MAX),
                                        lh_addr_of(length)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_cast_static(lh_usize_t, length);
    return lh_bool_true;
}

lh_usize_t
lh_http_framing_get_content_length(const lh_http_header_t *headers, lh_usize_t count)
{
    lh_usize_t length = 0U;
    lh_runtime_check_ifn(lh_http_framing_try_get_content_length(headers, count, lh_addr_of(length)),
                         lh_runtime_error_code_invalid_range);
    return length;
}
