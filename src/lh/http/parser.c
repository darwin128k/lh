#include <lh/http/parser.h>
#include <lh/char/digit.h>
#include <lh/http/char.h>
#include <lh/http/version.h>
#include <lh/util/return.h>

lh_http_parse_status_t
lh_http_parser_fail(lh_http_parser_t *self, lh_http_error_t error, lh_usize_t offset)
{
    self->error = error;
    self->error_offset = offset;
    return lh_http_parse_error;
}

lh_bool_t
lh_http_parser_begin(lh_http_parser_t *self, lh_str_view_t input, const lh_void *output,
                     const lh_http_header_t *headers, lh_usize_t capacity)
{
    lh_return_if(lh_null_eq(self), lh_bool_false);
    self->consumed = 0;
    self->error_offset = 0;
    self->error = lh_http_error_none;
    if (lh_null_eq(output) || (lh_null_eq(input.begin) && input.size) ||
        (lh_null_eq(headers) && capacity) || !self->head_size_max || !self->line_size_max)
    {
        self->error = lh_http_error_argument;
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_http_parse_status_t
lh_http_parser_find_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t start,
                         lh_usize_t *end)
{
    const lh_char_t *text = input.begin;
    const lh_usize_t limit = lh_math_min(input.size, self->head_size_max);
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
        if (i - start >= self->line_size_max)
        {
            return lh_http_parser_fail(self, lh_http_error_line_size, i);
        }
    }
    if (input.size >= self->head_size_max)
    {
        return lh_http_parser_fail(self, lh_http_error_head_size, self->head_size_max);
    }
    return lh_http_parse_incomplete;
}

lh_http_parse_status_t
lh_http_parser_parse_request_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                  lh_http_request_t *request)
{
    const lh_char_t *text = input.begin;
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
    lh_str_view_t token;
    lh_str_view_from_offset(&input, version + 1, end - version - 1, &token);
    lh_return_ifn(lh_http_parse_version(token, &request->version_minor),
                  lh_http_parser_fail(self, lh_http_error_start_line, version));
    lh_str_view_from_offset(&input, 0, method, &request->method);
    lh_str_view_from_offset(&input, target, version - target, &request->target);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_parser_parse_status_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t end,
                                 lh_http_response_t *response)
{
    const lh_char_t *text = input.begin;
    lh_return_if(end < 13, lh_http_parser_fail(self, lh_http_error_start_line, 0));
    lh_str_view_t token;
    lh_str_view_from_offset(&input, 0, 8, &token);
    if (!lh_http_parse_version(token, &response->version_minor) || text[8] != lh_char_map_space || text[9] < lh_char_map_digit_1 || text[9] > lh_char_map_digit_5 || !lh_char_is_digit(text[10]) ||
        !lh_char_is_digit(text[11]) || text[12] != lh_char_map_space)
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
    response->status = (lh_u16_t)(lh_char_to_digit(text[9]) * 100 +
                                  lh_char_to_digit(text[10]) * 10 + lh_char_to_digit(text[11]));
    lh_str_view_from_offset(&input, 13, end - 13, &response->reason);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_parser_parse_field(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t begin,
                           lh_usize_t end, lh_http_header_t *header)
{
    const lh_char_t *text = input.begin;
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
    lh_str_view_from_offset(&input, begin, colon - begin, &header->name);
    lh_str_view_from_offset(&input, colon + 1, end - colon - 1, &header->value);
    /* Field bytes are checked above, so the only whitespace left is SP and HTAB. */
    lh_str_view_trim(&header->value);
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
            self->consumed = end + 2;
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

lh_void
lh_http_parser_init(lh_http_parser_t *self)
{
    lh_return_if(lh_null_eq(self));
    self->head_size_max = 8192;
    self->line_size_max = 4096;
    self->consumed = 0;
    self->error_offset = 0;
    self->error = lh_http_error_none;
}

lh_void
lh_http_parser_set_limits(lh_http_parser_t *self, lh_usize_t head_size_max,
                          lh_usize_t line_size_max)
{
    lh_return_if(lh_null_eq(self));
    self->head_size_max = head_size_max;
    self->line_size_max = line_size_max;
}

lh_usize_t
lh_http_parser_get_consumed(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->consumed : 0;
}

lh_http_error_t
lh_http_parser_get_error(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->error : lh_http_error_argument;
}

lh_usize_t
lh_http_parser_get_error_offset(const lh_http_parser_t *self)
{
    return lh_null_ne(self) ? self->error_offset : 0;
}

lh_http_parse_status_t
lh_http_parser_request(lh_http_parser_t *self, lh_str_view_t input, lh_http_request_t *request,
                       lh_http_header_t *headers, lh_usize_t header_capacity)
{
    lh_return_ifn(lh_http_parser_begin(self, input, request, headers, header_capacity),
                  lh_http_parse_error);
    lh_usize_t end;
    lh_http_parse_status_t status = lh_http_parser_find_line(self, input, 0, &end);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_request_line(self, input, end, request);
    lh_return_if(status != lh_http_parse_complete, status);
    return lh_http_parser_parse_fields(self, input, end + 2, headers, header_capacity,
                                       &request->header_count);
}

lh_http_parse_status_t
lh_http_parser_response(lh_http_parser_t *self, lh_str_view_t input, lh_http_response_t *response,
                        lh_http_header_t *headers, lh_usize_t header_capacity)
{
    lh_return_ifn(lh_http_parser_begin(self, input, response, headers, header_capacity),
                  lh_http_parse_error);
    lh_usize_t end;
    lh_http_parse_status_t status = lh_http_parser_find_line(self, input, 0, &end);
    lh_return_if(status != lh_http_parse_complete, status);
    status = lh_http_parser_parse_status_line(self, input, end, response);
    lh_return_if(status != lh_http_parse_complete, status);
    return lh_http_parser_parse_fields(self, input, end + 2, headers, header_capacity,
                                       &response->header_count);
}
