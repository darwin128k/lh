/**
 * @file decoder.c
 * @brief Incremental chunked transfer decoder.
 */

#include <lh/http/chunked/decoder.h>
#include <lh/assert.h>
#include <lh/char/map.h>
#include <lh/memory/std.h>

lh_str_ptr
lh_http_chunked_decoder_get_line(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line;
}

lh_void
lh_http_chunked_decoder_set_line(lh_http_chunked_decoder_t *self, lh_str_ptr value)
{
    lh_assert_runtime_ref(self);
    self->line = value;
}

lh_usize_t
lh_http_chunked_decoder_get_line_capacity(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_capacity;
}

lh_void
lh_http_chunked_decoder_set_line_capacity(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->line_capacity = value;
}

lh_usize_t
lh_http_chunked_decoder_get_line_size(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->line_size;
}

lh_void
lh_http_chunked_decoder_set_line_size(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->line_size = value;
}

lh_usize_t
lh_http_chunked_decoder_get_remaining(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->remaining;
}

lh_void
lh_http_chunked_decoder_set_remaining(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->remaining = value;
}

lh_usize_t
lh_http_chunked_decoder_get_decoded(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->decoded;
}

lh_void
lh_http_chunked_decoder_set_decoded(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->decoded = value;
}

lh_usize_t
lh_http_chunked_decoder_get_decoded_limit(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->decoded_limit;
}

lh_void
lh_http_chunked_decoder_set_decoded_limit(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->decoded_limit = value;
}

lh_usize_t
lh_http_chunked_decoder_get_metadata(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metadata;
}

lh_void
lh_http_chunked_decoder_set_metadata(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->metadata = value;
}

lh_usize_t
lh_http_chunked_decoder_get_metadata_limit(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metadata_limit;
}

lh_void
lh_http_chunked_decoder_set_metadata_limit(lh_http_chunked_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->metadata_limit = value;
}

lh_http_chunked_decoder_phase_t
lh_http_chunked_decoder_get_phase(const lh_http_chunked_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_http_chunked_decoder_set_phase(lh_http_chunked_decoder_t *self,
                                  lh_http_chunked_decoder_phase_t value)
{
    lh_assert_runtime_ref(self);
    self->phase = value;
}

lh_void
lh_http_chunked_decoder_init(lh_http_chunked_decoder_t *self, lh_str_ptr line,
                             lh_usize_t line_capacity, lh_usize_t decoded_limit,
                             lh_usize_t metadata_limit)
{
    lh_http_chunked_decoder_set_line(self, line);
    lh_http_chunked_decoder_set_line_capacity(self, line_capacity);
    lh_http_chunked_decoder_set_line_size(self, 0U);
    lh_http_chunked_decoder_set_remaining(self, 0U);
    lh_http_chunked_decoder_set_decoded(self, 0U);
    lh_http_chunked_decoder_set_decoded_limit(self, decoded_limit);
    lh_http_chunked_decoder_set_metadata(self, 0U);
    lh_http_chunked_decoder_set_metadata_limit(self, metadata_limit);
    lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_size);
}

lh_bool_t
lh_http_chunked_decoder_try_finish_line(lh_http_chunked_decoder_t *self)
{
    lh_str_view_t line;
    lh_str_view_init_by_size(lh_addr_of(line), lh_http_chunked_decoder_get_line(self),
                             lh_http_chunked_decoder_get_line_size(self));
    const lh_http_chunked_decoder_phase_t phase = lh_http_chunked_decoder_get_phase(self);
    if (lh_math_eq(phase, lh_http_chunked_decoder_phase_size_lf))
    {
        lh_usize_t size;
        if (!lh_http_chunked_try_parse_size(line, lh_addr_of(size)) ||
            lh_math_gt(size, lh_math_sub(lh_http_chunked_decoder_get_decoded_limit(self),
                                         lh_http_chunked_decoder_get_decoded(self))))
        {
            return lh_bool_false;
        }
        lh_http_chunked_decoder_set_remaining(self, size);
        lh_http_chunked_decoder_set_phase(self, lh_math_is_zero(size)
                                                    ? lh_http_chunked_decoder_phase_trailer
                                                    : lh_http_chunked_decoder_phase_data);
    }
    else if (lh_math_eq(phase, lh_http_chunked_decoder_phase_trailer_lf))
    {
        if (lh_str_view_is_empty(lh_addr_of(line)))
        {
            lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_done);
        }
        else
        {
            lh_http_parser_t parser;
            lh_http_parser_init(lh_addr_of(parser));
            lh_http_header_t field;
            if (lh_math_ne(lh_http_parser_parse_field(lh_addr_of(parser), line, 0U,
                                                      lh_str_view_get_size(lh_addr_of(line)),
                                                      lh_addr_of(field)),
                           lh_http_parse_complete))
            {
                return lh_bool_false;
            }
            lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_trailer);
        }
    }
    else
    {
        return lh_bool_false;
    }
    lh_http_chunked_decoder_set_line_size(self, 0U);
    return lh_bool_true;
}

lh_void
lh_http_chunked_decoder_finish_line(lh_http_chunked_decoder_t *self)
{
    lh_runtime_check_ifn(lh_http_chunked_decoder_try_finish_line(self),
                         lh_runtime_error_code_invalid_range);
}

lh_http_parse_status_t
lh_http_chunked_decoder_feed(lh_http_chunked_decoder_t *self, lh_str_view_t input,
                             lh_str_ptr output, lh_usize_t capacity, lh_usize_t *consumed,
                             lh_usize_t *written)
{
    const lh_str_cptr text = lh_str_view_get_data(lh_addr_of(input));
    const lh_usize_t size = lh_str_view_get_size(lh_addr_of(input));
    lh_usize_t in = 0U;
    lh_usize_t out = 0U;
    while (lh_math_lt(in, size))
    {
        const lh_http_chunked_decoder_phase_t phase = lh_http_chunked_decoder_get_phase(self);
        if (lh_math_eq(phase, lh_http_chunked_decoder_phase_done) ||
            lh_math_eq(phase, lh_http_chunked_decoder_phase_error))
        {
            break;
        }
        if (lh_math_eq(phase, lh_http_chunked_decoder_phase_data))
        {
            const lh_usize_t amount =
                lh_math_min(lh_http_chunked_decoder_get_remaining(self),
                            lh_math_min(lh_math_sub(size, in), lh_math_sub(capacity, out)));
            if (lh_math_is_zero(amount))
            {
                break;
            }
            lh_memory_std_copy(output + out, text + in, amount);
            in = lh_math_add(in, amount);
            out = lh_math_add(out, amount);
            lh_http_chunked_decoder_set_decoded(
                self, lh_math_add(lh_http_chunked_decoder_get_decoded(self), amount));
            lh_http_chunked_decoder_set_remaining(
                self, lh_math_sub(lh_http_chunked_decoder_get_remaining(self), amount));
            if (lh_math_is_zero(lh_http_chunked_decoder_get_remaining(self)))
            {
                lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_data_cr);
            }
            continue;
        }
        if (lh_math_ge(lh_http_chunked_decoder_get_metadata(self),
                       lh_http_chunked_decoder_get_metadata_limit(self)))
        {
            lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_error);
            break;
        }
        const lh_char_t ch = text[in];
        in = lh_math_add_one(in);
        lh_http_chunked_decoder_set_metadata(
            self, lh_math_add_one(lh_http_chunked_decoder_get_metadata(self)));
        if (lh_math_eq(phase, lh_http_chunked_decoder_phase_size) ||
            lh_math_eq(phase, lh_http_chunked_decoder_phase_trailer))
        {
            if (lh_math_eq(ch, lh_char_map_cr))
            {
                lh_http_chunked_decoder_set_phase(
                    self, lh_math_eq(phase, lh_http_chunked_decoder_phase_size)
                              ? lh_http_chunked_decoder_phase_size_lf
                              : lh_http_chunked_decoder_phase_trailer_lf);
            }
            else if (lh_math_eq(ch, lh_char_map_lf) ||
                     lh_math_ge(lh_http_chunked_decoder_get_line_size(self),
                                lh_http_chunked_decoder_get_line_capacity(self)))
            {
                lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_error);
            }
            else
            {
                const lh_usize_t length = lh_http_chunked_decoder_get_line_size(self);
                lh_memory_std_copy(lh_http_chunked_decoder_get_line(self) + length, lh_addr_of(ch),
                                   1U);
                lh_http_chunked_decoder_set_line_size(self, lh_math_add_one(length));
            }
        }
        else if (lh_math_eq(phase, lh_http_chunked_decoder_phase_size_lf) ||
                 lh_math_eq(phase, lh_http_chunked_decoder_phase_trailer_lf))
        {
            if (lh_math_ne(ch, lh_char_map_lf) || !lh_http_chunked_decoder_try_finish_line(self))
            {
                lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_error);
            }
        }
        else if (lh_math_eq(phase, lh_http_chunked_decoder_phase_data_cr))
        {
            lh_http_chunked_decoder_set_phase(self, lh_math_eq(ch, lh_char_map_cr)
                                                        ? lh_http_chunked_decoder_phase_data_lf
                                                        : lh_http_chunked_decoder_phase_error);
        }
        else if (lh_math_eq(phase, lh_http_chunked_decoder_phase_data_lf))
        {
            lh_http_chunked_decoder_set_phase(self, lh_math_eq(ch, lh_char_map_lf)
                                                        ? lh_http_chunked_decoder_phase_size
                                                        : lh_http_chunked_decoder_phase_error);
        }
        else
        {
            lh_http_chunked_decoder_set_phase(self, lh_http_chunked_decoder_phase_error);
        }
    }
    lh_ptr_deref(consumed) = in;
    lh_ptr_deref(written) = out;
    if (lh_math_eq(lh_http_chunked_decoder_get_phase(self), lh_http_chunked_decoder_phase_done))
    {
        return lh_http_parse_complete;
    }
    if (lh_math_eq(lh_http_chunked_decoder_get_phase(self), lh_http_chunked_decoder_phase_error))
    {
        return lh_http_parse_error;
    }
    return lh_http_parse_incomplete;
}
