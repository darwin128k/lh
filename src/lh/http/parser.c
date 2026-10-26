#include <lh/http/parser.h>

static lh_http_parse_status_t
lh_http_parser_fail(lh_http_parser_t *self, lh_http_error_t error, lh_usize_t offset)
{
    self->error = error;
    self->error_offset = offset;
    return lh_http_parse_error;
}

static lh_str_view_t
lh_http_view(const lh_char_t *text, lh_usize_t size)
{
    lh_str_view_t view = lh_str_view_initializer(text, size);
    return view;
}

static lh_u8_t
lh_http_token(lh_u8_t c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
    {
        return 1;
    }
    switch (c)
    {
    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '\'':
    case '*':
    case '+':
    case '-':
    case '.':
    case '^':
    case '_':
    case '`':
    case '|':
    case '~':
        return 1;
    default:
        return 0;
    }
}

static lh_u8_t
lh_http_value_byte(lh_u8_t c)
{
    return c == '\t' || (c >= 0x20 && c != 0x7f);
}

/* Find a physical line without looking beyond either the view or head budget. */
static lh_http_parse_status_t
lh_http_line(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t start, lh_usize_t *end)
{
    const lh_char_t *text = input.begin;
    lh_usize_t limit = input.size < self->head_size_max ? input.size : self->head_size_max;
    for (lh_usize_t i = start; i < limit; ++i)
    {
        if (text[i] == '\n')
        {
            return lh_http_parser_fail(self, lh_http_error_line_ending, i);
        }
        if (text[i] == '\r')
        {
            if (i + 1 == limit)
            {
                break;
            }
            if (text[i + 1] != '\n')
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

static lh_u8_t
lh_http_version(const lh_char_t *text, lh_usize_t length, lh_u8_t *minor)
{
    if (length != 8 || text[0] != 'H' || text[1] != 'T' || text[2] != 'T' || text[3] != 'P' ||
        text[4] != '/' || text[5] != '1' || text[6] != '.' || text[7] < '0' || text[7] > '9')
    {
        return 0;
    }
    *minor = (lh_u8_t)(text[7] - '0');
    return 1;
}

static lh_http_parse_status_t
lh_http_headers(lh_http_parser_t *self, lh_str_view_t input, lh_usize_t offset,
                lh_http_header_t *headers, lh_usize_t capacity, lh_usize_t *count)
{
    const lh_char_t *text = input.begin;
    lh_usize_t number = 0;
    for (;;)
    {
        lh_usize_t end;
        lh_http_parse_status_t status = lh_http_line(self, input, offset, &end);
        if (status != lh_http_parse_complete)
        {
            return status;
        }
        if (end == offset)
        {
            self->consumed = end + 2;
            *count = number;
            return lh_http_parse_complete;
        }
        lh_usize_t colon = offset;
        while (colon < end && lh_http_token((lh_u8_t)text[colon]))
        {
            ++colon;
        }
        if (colon == offset || colon == end || text[colon] != ':')
        {
            return lh_http_parser_fail(self, lh_http_error_header, colon);
        }
        lh_usize_t begin = colon + 1, finish = end;
        for (lh_usize_t i = begin; i < end; ++i)
        {
            if (!lh_http_value_byte((lh_u8_t)text[i]))
            {
                return lh_http_parser_fail(self, lh_http_error_header, i);
            }
        }
        while (begin < finish && (text[begin] == ' ' || text[begin] == '\t'))
        {
            ++begin;
        }
        while (finish > begin && (text[finish - 1] == ' ' || text[finish - 1] == '\t'))
        {
            --finish;
        }
        if (number == capacity)
        {
            return lh_http_parser_fail(self, lh_http_error_header_capacity, offset);
        }
        headers[number].name = lh_http_view(text + offset, colon - offset);
        headers[number].value = lh_http_view(text + begin, finish - begin);
        ++number;
        offset = end + 2;
    }
}

static lh_u8_t
lh_http_prepare(lh_http_parser_t *self, lh_str_view_t input, const void *output,
                const lh_http_header_t *headers, lh_usize_t capacity)
{
    if (!self)
    {
        return 0;
    }
    self->consumed = self->error_offset = 0;
    self->error = lh_http_error_none;
    if (!output || (!input.begin && input.size) || (!headers && capacity) || !self->head_size_max ||
        !self->line_size_max)
    {
        self->error = lh_http_error_argument;
        return 0;
    }
    return 1;
}

void
lh_http_parser_init(lh_http_parser_t *self)
{
    if (!self)
    {
        return;
    }
    self->head_size_max = 8192;
    self->line_size_max = 4096;
    self->consumed = self->error_offset = 0;
    self->error = lh_http_error_none;
}

void
lh_http_parser_set_limits(lh_http_parser_t *self, lh_usize_t head_size_max,
                          lh_usize_t line_size_max)
{
    if (!self)
    {
        return;
    }
    self->head_size_max = head_size_max;
    self->line_size_max = line_size_max;
}

lh_usize_t
lh_http_parser_get_consumed(const lh_http_parser_t *self)
{
    return self ? self->consumed : 0;
}

lh_http_error_t
lh_http_parser_get_error(const lh_http_parser_t *self)
{
    return self ? self->error : lh_http_error_argument;
}

lh_usize_t
lh_http_parser_get_error_offset(const lh_http_parser_t *self)
{
    return self ? self->error_offset : 0;
}

lh_http_parse_status_t
lh_http_parser_request(lh_http_parser_t *self, lh_str_view_t input, lh_http_request_t *request,
                       lh_http_header_t *headers, lh_usize_t header_capacity)
{
    if (!lh_http_prepare(self, input, request, headers, header_capacity))
    {
        return lh_http_parse_error;
    }
    const lh_char_t *text = input.begin;
    lh_usize_t end;
    lh_http_parse_status_t status = lh_http_line(self, input, 0, &end);
    if (status != lh_http_parse_complete)
    {
        return status;
    }
    lh_usize_t method = 0;
    while (method < end && lh_http_token((lh_u8_t)text[method]))
    {
        ++method;
    }
    if (!method || method == end || text[method] != ' ')
    {
        return lh_http_parser_fail(self, lh_http_error_start_line, method);
    }
    lh_usize_t target = method + 1, version = target;
    while (version < end && text[version] != ' ')
    {
        if ((lh_u8_t)text[version] < 0x21 || (lh_u8_t)text[version] > 0x7e)
        {
            return lh_http_parser_fail(self, lh_http_error_start_line, version);
        }
        ++version;
    }
    if (version == target || version == end ||
        !lh_http_version(text + version + 1, end - version - 1, &request->version_minor))
    {
        return lh_http_parser_fail(self, lh_http_error_start_line, version);
    }
    request->method = lh_http_view(text, method);
    request->target = lh_http_view(text + target, version - target);
    return lh_http_headers(self, input, end + 2, headers, header_capacity, &request->header_count);
}

lh_http_parse_status_t
lh_http_parser_response(lh_http_parser_t *self, lh_str_view_t input, lh_http_response_t *response,
                        lh_http_header_t *headers, lh_usize_t header_capacity)
{
    if (!lh_http_prepare(self, input, response, headers, header_capacity))
    {
        return lh_http_parse_error;
    }
    const lh_char_t *text = input.begin;
    lh_usize_t end;
    lh_http_parse_status_t status = lh_http_line(self, input, 0, &end);
    if (status != lh_http_parse_complete)
    {
        return status;
    }
    if (end < 13 || !lh_http_version(text, 8, &response->version_minor) || text[8] != ' ' ||
        text[9] < '1' || text[9] > '5' || text[10] < '0' || text[10] > '9' || text[11] < '0' ||
        text[11] > '9' || text[12] != ' ')
    {
        return lh_http_parser_fail(self, lh_http_error_start_line, 0);
    }
    for (lh_usize_t i = 13; i < end; ++i)
    {
        if (!lh_http_value_byte((lh_u8_t)text[i]))
        {
            return lh_http_parser_fail(self, lh_http_error_start_line, i);
        }
    }
    response->status = (lh_u16_t)((text[9] - '0') * 100 + (text[10] - '0') * 10 + text[11] - '0');
    response->reason = lh_http_view(text + 13, end - 13);
    return lh_http_headers(self, input, end + 2, headers, header_capacity, &response->header_count);
}
