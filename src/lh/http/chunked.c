/**
 * @file chunked.c
 * @brief HTTP/1 chunked transfer parsing and in-place decoding.
 */

#include <lh/http/chunked.h>
#include <lh/char/map.h>
#include <lh/assert.h>
#include <lh/memory/std.h>
#include <lh/http/cursor.h>
#include <lh/str/parse/radix.h>
#include <lh/util/interval.h>

lh_bool_t
lh_http_chunked_are_extensions_valid(lh_str_view_t text)
{
    lh_str_cursor_t cursor;
    lh_str_cursor_init(lh_addr_of(cursor), text);
    while (!lh_str_cursor_is_end(lh_addr_of(cursor)))
    {
        lh_http_cursor_skip_ows(lh_addr_of(cursor));
        if (!lh_str_cursor_skip_char(lh_addr_of(cursor), lh_char_map_semicolon))
        {
            return lh_bool_false;
        }
        lh_http_cursor_skip_ows(lh_addr_of(cursor));
        if (lh_math_is_zero(lh_http_cursor_skip_token(lh_addr_of(cursor))))
        {
            return lh_bool_false;
        }
        const lh_usize_t after_name = lh_str_cursor_get_pos(lh_addr_of(cursor));
        lh_http_cursor_skip_ows(lh_addr_of(cursor));
        if (!lh_str_cursor_skip_char(lh_addr_of(cursor), lh_char_map_equals))
        {
            lh_str_cursor_set_pos(lh_addr_of(cursor), after_name);
            continue;
        }
        lh_http_cursor_skip_ows(lh_addr_of(cursor));
        if (lh_math_eq(lh_str_cursor_get_char(lh_addr_of(cursor)), lh_char_map_dquot))
        {
            if (!lh_http_cursor_try_skip_quoted(lh_addr_of(cursor)))
            {
                return lh_bool_false;
            }
        }
        else
        {
            if (lh_math_is_zero(lh_http_cursor_skip_token(lh_addr_of(cursor))))
            {
                return lh_bool_false;
            }
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_http_chunked_try_parse_size(lh_str_view_t line, lh_usize_t *out)
{
    const lh_str_cptr text = lh_str_view_get_data(lh_addr_of(line));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(line));
    if (lh_math_is_zero(size))
    {
        return lh_bool_false;
    }
    const lh_usize_t digits = lh_str_ptr_count_radix_digits(text, size, 16U);
    lh_ullong_t value;
    if (lh_math_is_zero(digits) ||
        !lh_str_ptr_parse_radix_ullong(
            text, digits, 16U, lh_cast_static(lh_ullong_t, LH_USIZE_T_MAX), lh_addr_of(value)))
    {
        return lh_bool_false;
    }
    lh_str_view_t extensions;
    lh_str_view_from_offset(lh_addr_of(line), digits, lh_math_sub(size, digits),
                            lh_addr_of(extensions));
    if (!lh_http_chunked_are_extensions_valid(extensions))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_cast_static(lh_usize_t, value);
    return lh_bool_true;
}

lh_usize_t
lh_http_chunked_parse_size(lh_str_view_t line)
{
    lh_usize_t size = 0U;
    lh_runtime_check_ifn(lh_http_chunked_try_parse_size(line, lh_addr_of(size)),
                         lh_runtime_error_code_invalid_range);
    return size;
}

lh_http_parse_status_t
lh_http_chunked_parse_part(lh_str_view_t input, lh_usize_t offset, lh_usize_t *data_offset,
                           lh_usize_t *data_size, lh_usize_t *next)
{
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(input));
    if (lh_math_gt(offset, size))
    {
        return lh_http_parse_error;
    }
    lh_http_parser_t parser;
    lh_http_parser_init(lh_addr_of(parser));
    lh_http_parser_set_limits(lh_addr_of(parser), LH_USIZE_T_MAX, LH_USIZE_T_MAX);
    lh_usize_t end;
    const lh_http_parse_status_t status =
        lh_http_parser_find_line(lh_addr_of(parser), input, offset, lh_addr_of(end));
    if (lh_math_ne(status, lh_http_parse_complete))
    {
        return status;
    }
    lh_str_view_t line;
    lh_str_view_from_offset(lh_addr_of(input), offset, lh_math_sub(end, offset), lh_addr_of(line));
    lh_usize_t length;
    if (!lh_http_chunked_try_parse_size(line, lh_addr_of(length)))
    {
        return lh_http_parse_error;
    }
    const lh_usize_t start = lh_math_add(end, 2U);
    const lh_usize_t left = lh_math_sub(size, start);
    if (!lh_math_is_zero(length))
    {
        if (lh_math_le(left, length))
        {
            return lh_http_parse_incomplete;
        }
        const lh_str_cptr text = lh_str_view_get_data(lh_addr_of(input));
        if (lh_math_ne(text[lh_math_add(start, length)], lh_char_map_cr))
        {
            return lh_http_parse_error;
        }
        if (lh_math_lt(lh_math_sub(left, length), 2U))
        {
            return lh_http_parse_incomplete;
        }
        if (lh_math_ne(text[lh_math_add(lh_math_add(start, length), 1U)], lh_char_map_lf))
        {
            return lh_http_parse_error;
        }
    }
    lh_ptr_deref(data_offset) = start;
    lh_ptr_deref(data_size) = length;
    lh_ptr_deref(next) =
        lh_math_is_zero(length) ? start : lh_math_add(lh_math_add(start, length), 2U);
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_chunked_parse_trailers(lh_str_view_t input, lh_usize_t offset, lh_usize_t *next)
{
    if (lh_math_gt(offset, lh_str_view_get_size(lh_addr_of(input))))
    {
        return lh_http_parse_error;
    }
    lh_http_parser_t parser;
    lh_http_parser_init(lh_addr_of(parser));
    lh_http_parser_set_limits(lh_addr_of(parser), LH_USIZE_T_MAX, LH_USIZE_T_MAX);
    for (;;)
    {
        lh_usize_t end;
        const lh_http_parse_status_t status =
            lh_http_parser_find_line(lh_addr_of(parser), input, offset, lh_addr_of(end));
        if (lh_math_ne(status, lh_http_parse_complete))
        {
            return status;
        }
        if (lh_math_eq(end, offset))
        {
            lh_ptr_deref(next) = lh_math_add(end, 2U);
            return lh_http_parse_complete;
        }
        lh_http_header_t trailer;
        if (lh_math_ne(lh_http_parser_parse_field(lh_addr_of(parser), input, offset, end,
                                                  lh_addr_of(trailer)),
                       lh_http_parse_complete))
        {
            return lh_http_parse_error;
        }
        offset = lh_math_add(end, 2U);
    }
}

lh_http_parse_status_t
lh_http_chunked_scan(lh_str_view_t input, lh_usize_t *consumed, lh_usize_t *decoded)
{
    lh_usize_t offset = 0U;
    lh_usize_t total = 0U;
    for (;;)
    {
        lh_usize_t data;
        lh_usize_t length;
        lh_usize_t next;
        const lh_http_parse_status_t status = lh_http_chunked_parse_part(
            input, offset, lh_addr_of(data), lh_addr_of(length), lh_addr_of(next));
        if (lh_math_ne(status, lh_http_parse_complete))
        {
            return status;
        }
        offset = next;
        if (lh_math_is_zero(length))
        {
            break;
        }
        if (lh_interval_closed_is_add_overflow(total, length, 0U, LH_USIZE_T_MAX))
        {
            return lh_http_parse_error;
        }
        total = lh_math_add(total, length);
    }
    lh_usize_t next;
    const lh_http_parse_status_t trailers =
        lh_http_chunked_parse_trailers(input, offset, lh_addr_of(next));
    if (lh_math_ne(trailers, lh_http_parse_complete))
    {
        return trailers;
    }
    lh_ptr_deref(consumed) = next;
    lh_ptr_deref(decoded) = total;
    return lh_http_parse_complete;
}

lh_http_parse_status_t
lh_http_chunked_decode(lh_str_ptr data, lh_usize_t size, lh_usize_t *consumed, lh_usize_t *decoded)
{
    lh_usize_t wire = 0U;
    lh_str_view_t input;
    lh_str_view_init_by_size(lh_addr_of(input), data, size);
    lh_usize_t output;
    const lh_http_parse_status_t status =
        lh_http_chunked_scan(input, lh_addr_of(wire), lh_addr_of(output));
    if (lh_math_ne(status, lh_http_parse_complete))
    {
        return status;
    }
    lh_usize_t offset = 0U;
    lh_usize_t written = 0U;
    for (;;)
    {
        lh_usize_t start;
        lh_usize_t length;
        lh_usize_t next;
        const lh_http_parse_status_t part = lh_http_chunked_parse_part(
            input, offset, lh_addr_of(start), lh_addr_of(length), lh_addr_of(next));
        if (lh_math_ne(part, lh_http_parse_complete))
        {
            return lh_http_parse_error;
        }
        if (lh_math_is_zero(length))
        {
            break;
        }
        lh_memory_std_move(data + written, data + start, length);
        written = lh_math_add(written, length);
        offset = next;
    }
    lh_ptr_deref(consumed) = wire;
    lh_ptr_deref(decoded) = output;
    return lh_http_parse_complete;
}
