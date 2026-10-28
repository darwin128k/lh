#include <lh/http/parser.h>
#include <lh/char/digit.h>
#include <lh/http/char.h>
#include <lh/http/version.h>
#include <lh/util/return.h>
#include <lh/assert.h>

/* -- Level 1: the fields ------------------------------------------------------ */

lh_usize_t
lh_http_parser_get_head_size_max(const lh_http_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return self->head_size_max;
}

lh_void
lh_http_parser_set_head_size_max(lh_http_parser_t *self, lh_usize_t head_size_max)
{
    lh_assert_runtime_ref(self);
    self->head_size_max = head_size_max;
}

lh_usize_t
lh_http_parser_get_line_size_max(const lh_http_parser_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_size_max;
}

lh_void
lh_http_parser_set_line_size_max(lh_http_parser_t *self, lh_usize_t line_size_max)
{
    lh_assert_runtime_ref(self);
    self->line_size_max = line_size_max;
}

lh_usize_t
lh_http_parser_get_consumed(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->consumed : 0;
}

lh_void
lh_http_parser_set_consumed(lh_http_parser_t *self, lh_usize_t consumed)
{
    lh_assert_runtime_ref(self);
    self->consumed = consumed;
}

lh_usize_t
lh_http_parser_get_error_offset(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->error_offset : 0;
}

lh_void
lh_http_parser_set_error_offset(lh_http_parser_t *self, lh_usize_t error_offset)
{
    lh_assert_runtime_ref(self);
    self->error_offset = error_offset;
}

lh_http_error_t
lh_http_parser_get_error(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->error : lh_http_error_argument;
}

lh_void
lh_http_parser_set_error(lh_http_parser_t *self, lh_http_error_t error)
{
    lh_assert_runtime_ref(self);
    self->error = error;
}

/* -- Level 2 and up ----------------------------------------------------------- */

lh_void
lh_http_parser_set_limits(lh_http_parser_t *self, lh_usize_t head_size_max,
                          lh_usize_t line_size_max)
{
    lh_return_if(lh_null_eq(self));
    lh_http_parser_set_head_size_max(self, head_size_max);
    lh_http_parser_set_line_size_max(self, line_size_max);
}

lh_http_parse_status_t
lh_http_parser_fail(lh_http_parser_t *self, lh_http_error_t error, lh_usize_t offset)
{
    lh_http_parser_set_error(self, error);
    lh_http_parser_set_error_offset(self, offset);
    return lh_http_parse_error;
}

lh_bool_t
lh_http_parser_begin(lh_http_parser_t *self, lh_str_view_t input, const lh_void *output,
                     const lh_http_header_t *headers, lh_usize_t capacity)
{
    lh_return_if(lh_null_eq(self), lh_bool_false);
    lh_http_parser_set_consumed(self, 0);
    lh_http_parser_set_error_offset(self, 0);
    lh_http_parser_set_error(self, lh_http_error_none);
    if (lh_null_eq(output) || !lh_memory_view_is_valid(&input) || (lh_null_eq(headers) && capacity) ||
        !lh_http_parser_get_head_size_max(self) || !lh_http_parser_get_line_size_max(self))
    {
        lh_http_parser_set_error(self, lh_http_error_argument);
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_http_parser_init(lh_http_parser_t *self)
{
    lh_return_if(lh_null_eq(self));
    lh_http_parser_set_limits(self, 8192, 4096);
    lh_http_parser_set_consumed(self, 0);
    lh_http_parser_set_error_offset(self, 0);
    lh_http_parser_set_error(self, lh_http_error_none);
}

lh_http_parse_status_t
lh_http_parser_find_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t start,
                         lh_usize_t *end)
{
    const lh_char_t *text = lh_str_view_get_data(&input);
    const lh_usize_t size = lh_str_view_get_size(&input);
    const lh_usize_t head_size_max = lh_http_parser_get_head_size_max(self);
    const lh_usize_t limit = lh_math_min(size, head_size_max);
    for (lh_usize_t i = start; i < limit; ++i)
    {
        if (text[i] == lh_char_map_lf)
        {
            return lh_http_parser_fail(self, lh_http_error_line_ending, i);
        }
        if (text[i] == lh_char_map_cr)
        {
            if (i + 1 == limit)
            {
                break;
            }
            if (text[i + 1] != lh_char_map_lf)
            {
                return lh_http_parser_fail(self, lh_http_error_line_ending, i + 1);
            }
            *end = i;
            return lh_http_parse_complete;
        }
        if (i - start >= lh_http_parser_get_line_size_max(self))
        {
            return lh_http_parser_fail(self, lh_http_error_line_size, i);
        }
    }
    if (size >= head_size_max)
    {
        return lh_http_parser_fail(self, lh_http_error_head_size, head_size_max);
    }
    return lh_http_parse_incomplete;
}

lh_http_parse_status_t
lh_http_parser_parse_request_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                  lh_http_request_t *request)
{
    const lh_char_t *text = lh_str_view_get_data(&input);
    lh_usize_t method = 0;
    while (method < end && lh_http_is_token_char(text[method]))
    {
        ++method;
    }
    if (!method || method == end || text[method] != lh_char_map_space)
    {
        return lh_http_parser_fail(self, lh_http_error_start_line, method);
    }
    const lh_usize_t target = method + 1;
    lh_usize_t version = target;
    while (version < end && text[version] != lh_char_map_space)
    {
        if (!lh_http_is_target_char(text[version]))
        {
            return lh_http_parser_fail(self, lh_http_error_start_line, version);
        }
        ++version;
    }
    lh_return_if(version == target || version == end,
                 lh_http_parser_fail(self, lh_http_error_start_line, version));
    lh_str_view_t part;
    lh_u8_t minor;
    lh_str_view_from_offset(&input, version + 1, end - version - 1, &part);
    lh_return_ifn(lh_http_parse_version(part, &minor),
                  lh_http_parser_fail(self, lh_http_error_start_line, version));
    lh_http_request_set_version_minor(request, minor);
    lh_str_view_from_offset(&input, 0, method, &part);
    lh_http_request_set_method(request, part);
    lh_str_view_from_offset(&input, target, version - target, &part);
    lh_http_request_set_target(request, part);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_parser_parse_status_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                 lh_http_response_t *response)
{
    const lh_char_t *text = lh_str_view_get_data(&input);
    lh_return_if(end < 13, lh_http_parser_fail(self, lh_http_error_start_line, 0));
    lh_str_view_t part;
    lh_u8_t minor;
    lh_str_view_from_offset(&input, 0, 8, &part);
    if (!lh_http_parse_version(part, &minor) || text[8] != lh_char_map_space ||
        text[9] < lh_char_map_digit_1 || text[9] > lh_char_map_digit_5 ||
        !lh_char_is_digit(text[10]) || !lh_char_is_digit(text[11]) ||
        text[12] != lh_char_map_space)
    {
        return lh_http_parser_fail(self, lh_http_error_start_line, 0);
    }
    for (lh_usize_t i = 13; i < end; ++i)
    {
        if (!lh_http_is_field_char(text[i]))
        {
            return lh_http_parser_fail(self, lh_http_error_start_line, i);
        }
    }
    lh_http_response_set_version_minor(response, minor);
    lh_http_response_set_status(response,
                                (lh_u16_t)(lh_char_to_digit(text[9]) * 100 +
                                           lh_char_to_digit(text[10]) * 10 +
                                           lh_char_to_digit(text[11])));
    lh_str_view_from_offset(&input, 13, end - 13, &part);
    lh_http_response_set_reason(response, part);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_parser_parse_field(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t begin,
                           lh_usize_t end, lh_http_header_t *header)
{
    const lh_char_t *text = lh_str_view_get_data(&input);
    lh_usize_t colon = begin;
    while (colon < end && lh_http_is_token_char(text[colon]))
    {
        ++colon;
    }
    if (colon == begin || colon == end || text[colon] != lh_char_map_colon)
    {
        return lh_http_parser_fail(self, lh_http_error_header, colon);
    }
    for (lh_usize_t i = colon + 1; i < end; ++i)
    {
        if (!lh_http_is_field_char(text[i]))
        {
            return lh_http_parser_fail(self, lh_http_error_header, i);
        }
    }
    lh_str_view_t part;
    lh_str_view_from_offset(&input, begin, colon - begin, &part);
    lh_http_header_set_name(header, part);
    lh_str_view_from_offset(&input, colon + 1, end - colon - 1, &part);
    /* Field bytes are checked above, so the only whitespace left is SP and HTAB. */
    lh_str_view_trim(&part);
    lh_http_header_set_value(header, part);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_parser_parse_fields(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t offset,
                            lh_http_header_t *headers, lh_usize_t capacity, lh_usize_t *count)
{
    lh_usize_t number = 0;
    for (;;)
    {
        lh_usize_t end;
        lh_http_parse_status_t status = lh_http_parser_find_line(self, input, offset, &end);
        lh_return_if(status != lh_http_parse_complete, status);
        if (end == offset)
        {
            lh_http_parser_set_consumed(self, end + 2);
            *count = number;
            return lh_http_parse_complete;
        }
        lh_http_header_t field;
        status = lh_http_parser_parse_field(self, input, offset, end, &field);
        lh_return_if(status != lh_http_parse_complete, status);
        if (number == capacity)
        {
            return lh_http_parser_fail(self, lh_http_error_header_capacity, offset);
        }
        headers[number++] = field;
        offset = end + 2;
    }
}

lh_http_parse_status_t
lh_http_parser_request(lh_http_parser_t *self, lh_str_view_t input, lh_http_request_t *request,
                       lh_http_header_t *headers, lh_usize_t header_capacity)
{
    lh_return_ifn(lh_http_parser_begin(self, input, request, headers, header_capacity),
                  lh_http_parse_error);
    lh_usize_t end;
    lh_usize_t count;
    lh_http_parse_status_t status = lh_http_parser_find_line(self, input, 0, &end);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_request_line(self, input, end, request);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_fields(self, input, end + 2, headers, header_capacity, &count);
    lh_return_if(status != lh_http_parse_complete, status);
    lh_http_request_set_header_count(request, count);
    return status;
}

lh_http_parse_status_t
lh_http_parser_response(lh_http_parser_t *self, lh_str_view_t input, lh_http_response_t *response,
                        lh_http_header_t *headers, lh_usize_t header_capacity)
{
    lh_return_ifn(lh_http_parser_begin(self, input, response, headers, header_capacity),
                  lh_http_parse_error);
    lh_usize_t end;
    lh_usize_t count;
    lh_http_parse_status_t status = lh_http_parser_find_line(self, input, 0, &end);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_status_line(self, input, end, response);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_fields(self, input, end + 2, headers, header_capacity, &count);
    lh_return_if(status != lh_http_parse_complete, status);
    lh_http_response_set_header_count(response, count);
    return status;
}
