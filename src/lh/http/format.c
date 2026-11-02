/**
 * @file format.c
 * @brief Serialize HTTP/1 response status and fields.
 */

#include <lh/http/format.h>

#include <lh/assert.h>
#include <lh/char/digit.h>
#include <lh/http/status.h>
#include <lh/str/format/text.h>
#include <lh/str/lit.h>
#include <lh/util/interval.h>

lh_bool_t
lh_http_format_try_status_line(const lh_http_response_t *response, lh_str_ptr out,
                               lh_usize_t capacity, lh_usize_t *offset)
{
    const lh_u16_t status = lh_http_response_get_status(response);
    const lh_u8_t minor = lh_http_response_get_version_minor(response);
    if (!lh_http_status_is_valid(status) || !lh_interval_closed_contains_value(0U, 9U, minor))
    {
        return lh_bool_false;
    }
    lh_char_t digits[LH_STR_FORMAT_UINT_DIGITS_MAX];
    const lh_usize_t n = lh_str_ptr_format_uint(status, digits, sizeof(digits));
    const lh_char_t crlf[] = {lh_char_map_cr, lh_char_map_lf};
    const lh_str_view_t *reason = lh_http_response_get_reason(response);
    const lh_usize_t reason_len = lh_str_view_get_size(reason);

    return lh_str_ptr_format_text_put_text(out, capacity, offset, lh_str_lit("HTTP/1.")) &&
           lh_str_ptr_format_text_put_run(out, capacity, offset, lh_char_from_digit(minor), 1U) &&
           lh_str_ptr_format_text_put_run(out, capacity, offset, lh_char_map_space, 1U) &&
           lh_str_ptr_format_text_put_text(out, capacity, offset, digits, n) &&
           lh_str_ptr_format_text_put_run(out, capacity, offset, lh_char_map_space, 1U) &&
           (lh_math_is_zero(reason_len) ||
            lh_str_ptr_format_text_put_text(out, capacity, offset, lh_str_view_get_data(reason),
                                            reason_len)) &&
           lh_str_ptr_format_text_put_text(out, capacity, offset, crlf, sizeof(crlf));
}

lh_void
lh_http_format_status_line(const lh_http_response_t *response, lh_str_ptr out, lh_usize_t capacity,
                           lh_usize_t *offset)
{
    lh_runtime_check_ifn(lh_http_format_try_status_line(response, out, capacity, offset),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_http_format_try_header(const lh_http_header_t *header, lh_str_ptr out, lh_usize_t capacity,
                          lh_usize_t *offset)
{
    const lh_str_view_t *name = lh_http_header_get_name(header);
    const lh_str_view_t *value = lh_http_header_get_value(header);
    const lh_usize_t value_len = lh_str_view_get_size(value);
    const lh_char_t crlf[] = {lh_char_map_cr, lh_char_map_lf};

    return lh_str_ptr_format_text_put_text(out, capacity, offset, lh_str_view_get_data(name),
                                           lh_str_view_get_size(name)) &&
           lh_str_ptr_format_text_put_run(out, capacity, offset, lh_char_map_colon, 1U) &&
           lh_str_ptr_format_text_put_run(out, capacity, offset, lh_char_map_space, 1U) &&
           (lh_math_is_zero(value_len) ||
            lh_str_ptr_format_text_put_text(out, capacity, offset, lh_str_view_get_data(value),
                                            value_len)) &&
           lh_str_ptr_format_text_put_text(out, capacity, offset, crlf, sizeof(crlf));
}

lh_void
lh_http_format_header(const lh_http_header_t *header, lh_str_ptr out, lh_usize_t capacity,
                      lh_usize_t *offset)
{
    lh_runtime_check_ifn(lh_http_format_try_header(header, out, capacity, offset),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_http_format_try_response_head(const lh_http_response_t *response,
                                 const lh_http_header_t *headers, lh_str_ptr out,
                                 lh_usize_t capacity, lh_usize_t *offset)
{
    if (!lh_http_format_try_status_line(response, out, capacity, offset))
    {
        return lh_bool_false;
    }
    const lh_usize_t count = lh_http_response_get_header_count(response);
    for (lh_usize_t i = 0U; i < count; i = lh_math_add_one(i))
    {
        if (!lh_http_format_try_header(lh_addr_of(headers[i]), out, capacity, offset))
        {
            return lh_bool_false;
        }
    }
    const lh_char_t crlf[] = {lh_char_map_cr, lh_char_map_lf};
    return lh_str_ptr_format_text_put_text(out, capacity, offset, crlf, sizeof(crlf));
}

lh_void
lh_http_format_response_head(const lh_http_response_t *response, const lh_http_header_t *headers,
                             lh_str_ptr out, lh_usize_t capacity, lh_usize_t *offset)
{
    lh_runtime_check_ifn(lh_http_format_try_response_head(response, headers, out, capacity, offset),
                         lh_runtime_error_code_invalid_range);
}
