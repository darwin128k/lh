/**
 * @file reader.c
 * @brief Incremental HTTP message body reader.
 */

#include <lh/core/http/body/reader.h>
#include <lh/core/assert.h>
#include <lh/core/memory/std.h>

lh_io_reader_t *
lh_http_body_reader_get_source(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->source;
}

lh_void
lh_http_body_reader_set_source(lh_http_body_reader_t *self, lh_io_reader_t *value)
{
    lh_assert_runtime_ref(self);
    self->source = value;
}

lh_http_chunked_decoder_t *
lh_http_body_reader_get_decoder(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->decoder;
}

lh_void
lh_http_body_reader_set_decoder(lh_http_body_reader_t *self, lh_http_chunked_decoder_t *value)
{
    lh_assert_runtime_ref(self);
    self->decoder = value;
}

lh_str_ptr
lh_http_body_reader_get_wire(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire;
}

lh_void
lh_http_body_reader_set_wire(lh_http_body_reader_t *self, lh_str_ptr value)
{
    lh_assert_runtime_ref(self);
    self->wire = value;
}

lh_usize_t
lh_http_body_reader_get_wire_capacity(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire_capacity;
}

lh_void
lh_http_body_reader_set_wire_capacity(lh_http_body_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->wire_capacity = value;
}

lh_usize_t
lh_http_body_reader_get_wire_size(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire_size;
}

lh_void
lh_http_body_reader_set_wire_size(lh_http_body_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->wire_size = value;
}

lh_usize_t
lh_http_body_reader_get_wire_pos(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire_pos;
}

lh_void
lh_http_body_reader_set_wire_pos(lh_http_body_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->wire_pos = value;
}

lh_usize_t
lh_http_body_reader_get_remaining(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->remaining;
}

lh_void
lh_http_body_reader_set_remaining(lh_http_body_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->remaining = value;
}

lh_bool_t
lh_http_body_reader_get_failed(const lh_http_body_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->failed;
}

lh_void
lh_http_body_reader_set_failed(lh_http_body_reader_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->failed = value;
}

lh_void
lh_http_body_reader_init_length(lh_http_body_reader_t *self, lh_io_reader_t *source,
                                lh_str_ptr wire, lh_usize_t capacity, lh_usize_t available,
                                lh_usize_t length)
{
    lh_http_body_reader_set_source(self, source);
    lh_http_body_reader_set_decoder(self, lh_null);
    lh_http_body_reader_set_wire(self, wire);
    lh_http_body_reader_set_wire_capacity(self, capacity);
    lh_http_body_reader_set_wire_size(self, available);
    lh_http_body_reader_set_wire_pos(self, 0U);
    lh_http_body_reader_set_remaining(self, length);
    lh_http_body_reader_set_failed(self, lh_bool_false);
}

lh_void
lh_http_body_reader_init_chunked(lh_http_body_reader_t *self, lh_io_reader_t *source,
                                 lh_str_ptr wire, lh_usize_t capacity, lh_usize_t available,
                                 lh_http_chunked_decoder_t *decoder)
{
    lh_http_body_reader_init_length(self, source, wire, capacity, available, 0U);
    lh_http_body_reader_set_decoder(self, decoder);
}

lh_ssize_t
lh_http_body_reader_refill(lh_http_body_reader_t *self)
{
    const lh_ssize_t n =
        lh_io_reader_read(lh_http_body_reader_get_source(self), lh_http_body_reader_get_wire(self),
                          lh_http_body_reader_get_wire_capacity(self));
    if (lh_math_gt(n, 0))
    {
        lh_http_body_reader_set_wire_pos(self, 0U);
        lh_http_body_reader_set_wire_size(self, lh_cast_static(lh_usize_t, n));
    }
    return n;
}

lh_ssize_t
lh_http_body_reader_read(lh_self_ptr context, lh_ptr out, lh_usize_t size)
{
    lh_http_body_reader_t *self = lh_ptr_cast(lh_http_body_reader_t, context);
    if (lh_math_is_zero(size))
    {
        return 0;
    }
    if (lh_http_body_reader_get_failed(self))
    {
        return LH_IO_ERROR;
    }
    const lh_usize_t capacity = lh_math_min(size, lh_cast_static(lh_usize_t, LH_SSIZE_T_MAX));
    for (;;)
    {
        lh_http_chunked_decoder_t *decoder = lh_http_body_reader_get_decoder(self);
        if (lh_null_eq(decoder) && lh_math_is_zero(lh_http_body_reader_get_remaining(self)))
        {
            return 0;
        }
        if (lh_null_ne(decoder) && lh_math_eq(lh_http_chunked_decoder_get_phase(decoder),
                                              lh_http_chunked_decoder_phase_done))
        {
            return 0;
        }
        if (lh_math_eq(lh_http_body_reader_get_wire_pos(self),
                       lh_http_body_reader_get_wire_size(self)))
        {
            const lh_ssize_t got = lh_http_body_reader_refill(self);
            if (lh_math_eq(got, LH_IO_PENDING))
            {
                return LH_IO_PENDING;
            }
            if (lh_math_le(got, 0))
            {
                lh_http_body_reader_set_failed(self, lh_bool_true);
                return LH_IO_ERROR;
            }
        }
        const lh_usize_t pos = lh_http_body_reader_get_wire_pos(self);
        const lh_usize_t available = lh_math_sub(lh_http_body_reader_get_wire_size(self), pos);
        const lh_str_cptr wire = lh_http_body_reader_get_wire(self) + pos;
        if (lh_null_eq(decoder))
        {
            const lh_usize_t n = lh_math_min(
                capacity, lh_math_min(available, lh_http_body_reader_get_remaining(self)));
            lh_memory_std_copy(out, wire, n);
            lh_http_body_reader_set_wire_pos(self, lh_math_add(pos, n));
            lh_http_body_reader_set_remaining(
                self, lh_math_sub(lh_http_body_reader_get_remaining(self), n));
            return lh_cast_static(lh_ssize_t, n);
        }
        lh_str_view_t input;
        lh_str_view_init_by_size(lh_addr_of(input), wire, available);
        lh_usize_t consumed;
        lh_usize_t written;
        const lh_http_parse_status_t status =
            lh_http_chunked_decoder_feed(decoder, input, lh_ptr_cast(lh_char_t, out), capacity,
                                         lh_addr_of(consumed), lh_addr_of(written));
        lh_http_body_reader_set_wire_pos(self, lh_math_add(pos, consumed));
        if (lh_math_eq(status, lh_http_parse_error))
        {
            lh_http_body_reader_set_failed(self, lh_bool_true);
        }
        if (!lh_math_is_zero(written))
        {
            return lh_cast_static(lh_ssize_t, written);
        }
        if (lh_math_eq(status, lh_http_parse_error))
        {
            return LH_IO_ERROR;
        }
        if (lh_math_eq(status, lh_http_parse_complete))
        {
            return 0;
        }
    }
}

lh_void
lh_http_body_reader_get_reader(lh_http_body_reader_t *self, lh_io_reader_t *out)
{
    lh_io_reader_init(out, lh_http_body_reader_read, self);
}

lh_void
lh_http_body_reader_get_unread(const lh_http_body_reader_t *self, lh_str_view_t *out)
{
    const lh_usize_t pos = lh_http_body_reader_get_wire_pos(self);
    lh_str_view_init_by_size(out, lh_http_body_reader_get_wire(self) + pos,
                             lh_math_sub(lh_http_body_reader_get_wire_size(self), pos));
}
