/**
 * @file encoding.c
 * @brief Validate request content coding.
 */

#include <lh/core/http/content/encoding.h>
#include <lh/core/http/header/name.h>
#include <lh/core/http/header/value.h>
#include <lh/core/assert.h>

lh_bool_t
lh_http_content_encoding_try_is_gzip(const lh_http_header_t *headers, lh_usize_t count,
                                     lh_bool_t *out)
{
    const lh_str_view_t name = lh_http_header_name_content_encoding;
    const lh_http_header_t *field = lh_http_header_find(headers, count, &name);
    if (lh_null_eq(field))
    {
        *out = lh_bool_false;
        return lh_bool_true;
    }
    const lh_usize_t next = lh_math_add_one(lh_cast_static(lh_usize_t, field - headers));
    if (lh_null_ne(lh_http_header_find(headers + next, lh_math_sub(count, next), &name)))
    {
        return lh_bool_false;
    }
    const lh_str_view_t gzip = lh_http_header_value_gzip;
    const lh_str_view_t identity = lh_http_header_value_identity;
    if (lh_str_view_equals(lh_http_header_get_value(field), &gzip, lh_bool_true))
    {
        *out = lh_bool_true;
        return lh_bool_true;
    }
    if (lh_str_view_equals(lh_http_header_get_value(field), &identity, lh_bool_true))
    {
        *out = lh_bool_false;
        return lh_bool_true;
    }
    return lh_bool_false;
}

lh_bool_t
lh_http_content_encoding_is_gzip(const lh_http_header_t *headers, lh_usize_t count)
{
    lh_bool_t result = lh_bool_false;
    lh_runtime_check_ifn(lh_http_content_encoding_try_is_gzip(headers, count, &result),
                         lh_runtime_error_code_invalid_range);
    return result;
}
