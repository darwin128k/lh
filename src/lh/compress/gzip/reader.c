/**
 * @file reader.c
 * @brief Streaming gzip reader implementation.
 */

#include <lh/compress/gzip/reader.h>
#include <lh/compress/gzip.h>
#include <lh/hash/crc.h>
#include <lh/assert.h>
#include <lh/math.h>
#include <lh/size/limits.h>

lh_io_reader_t *
lh_compress_gzip_reader_get_source(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->source;
}

lh_void
lh_compress_gzip_reader_set_source(lh_compress_gzip_reader_t *self, lh_io_reader_t *value)
{
    lh_assert_runtime_ref(self);
    self->source = value;
}

lh_compress_gzip_header_decoder_t *
lh_compress_gzip_reader_get_header(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->header;
}

lh_void
lh_compress_gzip_reader_set_header(lh_compress_gzip_reader_t *self,
                                   lh_compress_gzip_header_decoder_t *value)
{
    lh_assert_runtime_ref(self);
    self->header = value;
}

lh_compress_deflate_reader_t *
lh_compress_gzip_reader_get_deflate(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->deflate;
}

lh_void
lh_compress_gzip_reader_set_deflate(lh_compress_gzip_reader_t *self,
                                    lh_compress_deflate_reader_t *value)
{
    lh_assert_runtime_ref(self);
    self->deflate = value;
}

lh_byte_t *
lh_compress_gzip_reader_get_scratch(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->scratch;
}

lh_void
lh_compress_gzip_reader_set_scratch(lh_compress_gzip_reader_t *self, lh_byte_t *value)
{
    lh_assert_runtime_ref(self);
    self->scratch = value;
}

lh_byte_t *
lh_compress_gzip_reader_get_history(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->history;
}

lh_void
lh_compress_gzip_reader_set_history(lh_compress_gzip_reader_t *self, lh_byte_t *value)
{
    lh_assert_runtime_ref(self);
    self->history = value;
}

lh_usize_t
lh_compress_gzip_reader_get_limit(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->limit;
}

lh_void
lh_compress_gzip_reader_set_limit(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->limit = value;
}

lh_usize_t
lh_compress_gzip_reader_get_metadata_limit(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metadata_limit;
}

lh_void
lh_compress_gzip_reader_set_metadata_limit(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->metadata_limit = value;
}

lh_usize_t
lh_compress_gzip_reader_get_metadata(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metadata;
}

lh_void
lh_compress_gzip_reader_set_metadata(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->metadata = value;
}

lh_usize_t
lh_compress_gzip_reader_get_total(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->total;
}

lh_void
lh_compress_gzip_reader_set_total(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->total = value;
}

lh_usize_t
lh_compress_gzip_reader_get_member_size(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->member_size;
}

lh_void
lh_compress_gzip_reader_set_member_size(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->member_size = value;
}

lh_u32_t
lh_compress_gzip_reader_get_crc(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->crc;
}

lh_void
lh_compress_gzip_reader_set_crc(lh_compress_gzip_reader_t *self, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    self->crc = value;
}

lh_usize_t
lh_compress_gzip_reader_get_trailer_size(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->trailer_size;
}

lh_void
lh_compress_gzip_reader_set_trailer_size(lh_compress_gzip_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->trailer_size = value;
}

lh_compress_gzip_reader_phase_t
lh_compress_gzip_reader_get_phase(const lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_compress_gzip_reader_set_phase(lh_compress_gzip_reader_t *self,
                                  lh_compress_gzip_reader_phase_t value)
{
    lh_assert_runtime_ref(self);
    self->phase = value;
}

lh_byte_t *
lh_compress_gzip_reader_get_trailer(lh_compress_gzip_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->trailer;
}

lh_void
lh_compress_gzip_reader_init(lh_compress_gzip_reader_t *self, lh_io_reader_t *source,
                             lh_compress_gzip_header_decoder_t *header,
                             lh_compress_deflate_reader_t *deflate, lh_byte_t *scratch,
                             lh_byte_t *history, lh_usize_t limit, lh_usize_t metadata_limit)
{
    lh_compress_gzip_reader_set_source(self, source);
    lh_compress_gzip_reader_set_header(self, header);
    lh_compress_gzip_reader_set_deflate(self, deflate);
    lh_compress_gzip_reader_set_scratch(self, scratch);
    lh_compress_gzip_reader_set_history(self, history);
    lh_compress_gzip_reader_set_limit(self, limit);
    lh_compress_gzip_reader_set_metadata_limit(self, metadata_limit);
    lh_compress_gzip_reader_set_metadata(self, 0U);
    lh_compress_gzip_reader_set_total(self, 0U);
    lh_compress_gzip_reader_set_member_size(self, 0U);
    lh_compress_gzip_reader_set_crc(self, 0U);
    lh_compress_gzip_reader_set_trailer_size(self, 0U);
    lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_header);
    lh_compress_gzip_header_decoder_init(header, metadata_limit);
}

lh_bool_t
lh_compress_gzip_reader_try_header_byte(lh_compress_gzip_reader_t *self, lh_byte_t value)
{
    lh_usize_t consumed;
    const lh_compress_gzip_header_decoder_phase_t phase = lh_compress_gzip_header_decoder_try_feed(
        lh_compress_gzip_reader_get_header(self), &value, 1U, &consumed);
    lh_compress_gzip_reader_set_metadata(
        self, lh_math_add(lh_compress_gzip_reader_get_metadata(self), consumed));
    if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_error))
    {
        return lh_bool_false;
    }
    if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_done))
    {
        lh_compress_gzip_reader_set_member_size(self, 0U);
        lh_compress_gzip_reader_set_crc(self, 0U);
        lh_compress_deflate_reader_init(
            lh_compress_gzip_reader_get_deflate(self), lh_compress_gzip_reader_get_source(self),
            lh_compress_gzip_reader_get_scratch(self), LH_COMPRESS_DEFLATE_READER_INPUT_SIZE,
            lh_compress_gzip_reader_get_history(self),
            lh_math_sub(lh_compress_gzip_reader_get_limit(self),
                        lh_compress_gzip_reader_get_total(self)));
        lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_deflate);
    }
    return lh_bool_true;
}

lh_void
lh_compress_gzip_reader_header_byte(lh_compress_gzip_reader_t *self, lh_byte_t value)
{
    lh_runtime_check_ifn(lh_compress_gzip_reader_try_header_byte(self, value),
                         lh_runtime_error_code_invalid_range);
}

lh_ssize_t
lh_compress_gzip_reader_try_step_header(lh_compress_gzip_reader_t *self)
{
    const lh_compress_gzip_reader_phase_t phase = lh_compress_gzip_reader_get_phase(self);
    lh_byte_t byte;
    const lh_ssize_t got = lh_io_reader_read(lh_compress_gzip_reader_get_source(self), &byte, 1U);
    if (lh_math_eq(got, LH_IO_PENDING))
    {
        return LH_IO_PENDING;
    }
    if (lh_math_is_zero(got) && lh_math_eq(phase, lh_compress_gzip_reader_phase_next))
    {
        lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_done);
        return 0;
    }
    if (lh_math_ne(got, 1))
    {
        return LH_IO_ERROR;
    }
    if (lh_math_eq(phase, lh_compress_gzip_reader_phase_next))
    {
        lh_compress_gzip_header_decoder_init(
            lh_compress_gzip_reader_get_header(self),
            lh_math_sub(lh_compress_gzip_reader_get_metadata_limit(self),
                        lh_compress_gzip_reader_get_metadata(self)));
        lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_header);
    }
    return lh_compress_gzip_reader_try_header_byte(self, byte) ? 0 : LH_IO_ERROR;
}

lh_ssize_t
lh_compress_gzip_reader_try_step_deflate(lh_compress_gzip_reader_t *self, lh_ptr output,
                                         lh_usize_t size)
{
    const lh_ssize_t got =
        lh_compress_deflate_reader_read(lh_compress_gzip_reader_get_deflate(self), output, size);
    if (lh_math_gt(got, 0))
    {
        const lh_usize_t n = lh_cast_static(lh_usize_t, got);
        lh_compress_gzip_reader_set_crc(
            self, lh_hash_crc32_update(lh_compress_gzip_reader_get_crc(self), output, n));
        lh_compress_gzip_reader_set_member_size(
            self, lh_math_add(lh_compress_gzip_reader_get_member_size(self), n));
        return got;
    }
    if (lh_math_lt(got, 0))
    {
        return got;
    }
    lh_compress_gzip_reader_set_trailer_size(self, 0U);
    lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_trailer);
    return 0;
}

lh_ssize_t
lh_compress_gzip_reader_try_step_trailer(lh_compress_gzip_reader_t *self)
{
    const lh_usize_t used = lh_compress_gzip_reader_get_trailer_size(self);
    const lh_usize_t metadata = lh_compress_gzip_reader_get_metadata(self);
    const lh_usize_t room = lh_math_sub(lh_compress_gzip_reader_get_metadata_limit(self), metadata);
    if (lh_math_is_zero(room))
    {
        return LH_IO_ERROR;
    }
    const lh_usize_t want = lh_math_min(lh_math_sub(LH_COMPRESS_GZIP_TRAILER_SIZE, used), room);
    const lh_ssize_t got =
        lh_io_reader_read(lh_compress_gzip_reader_get_source(self),
                          lh_compress_gzip_reader_get_trailer(self) + used, want);
    if (lh_math_eq(got, LH_IO_PENDING))
    {
        return LH_IO_PENDING;
    }
    if (lh_math_le(got, 0))
    {
        return LH_IO_ERROR;
    }
    const lh_usize_t n = lh_cast_static(lh_usize_t, got);
    lh_compress_gzip_reader_set_trailer_size(self, lh_math_add(used, n));
    lh_compress_gzip_reader_set_metadata(self, lh_math_add(metadata, n));
    if (lh_math_eq(lh_math_add(used, n), LH_COMPRESS_GZIP_TRAILER_SIZE))
    {
        if (!lh_compress_gzip_try_trailer_values(lh_compress_gzip_reader_get_trailer(self),
                                                 LH_COMPRESS_GZIP_TRAILER_SIZE,
                                                 lh_compress_gzip_reader_get_crc(self),
                                                 lh_compress_gzip_reader_get_member_size(self)))
        {
            return LH_IO_ERROR;
        }
        lh_compress_gzip_reader_set_total(
            self, lh_math_add(lh_compress_gzip_reader_get_total(self),
                              lh_compress_gzip_reader_get_member_size(self)));
        lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_next);
    }
    return 0;
}

lh_ssize_t
lh_compress_gzip_reader_try_step(lh_compress_gzip_reader_t *self, lh_ptr output, lh_usize_t size)
{
    const lh_compress_gzip_reader_phase_t phase = lh_compress_gzip_reader_get_phase(self);
    lh_ssize_t result;
    if (lh_math_eq(phase, lh_compress_gzip_reader_phase_header) ||
        lh_math_eq(phase, lh_compress_gzip_reader_phase_next))
    {
        result = lh_compress_gzip_reader_try_step_header(self);
    }
    else if (lh_math_eq(phase, lh_compress_gzip_reader_phase_deflate))
    {
        result = lh_compress_gzip_reader_try_step_deflate(self, output, size);
    }
    else if (lh_math_eq(phase, lh_compress_gzip_reader_phase_trailer))
    {
        result = lh_compress_gzip_reader_try_step_trailer(self);
    }
    else
    {
        result = lh_math_eq(phase, lh_compress_gzip_reader_phase_done) ? 0 : LH_IO_ERROR;
    }
    if (lh_math_lt(result, 0) && lh_math_ne(result, LH_IO_PENDING))
    {
        lh_compress_gzip_reader_set_phase(self, lh_compress_gzip_reader_phase_error);
        return LH_IO_ERROR;
    }
    return result;
}

lh_ssize_t
lh_compress_gzip_reader_read(lh_self_ptr context, lh_ptr output, lh_usize_t size)
{
    lh_compress_gzip_reader_t *self = lh_ptr_cast(lh_compress_gzip_reader_t, context);
    if (lh_math_is_zero(size))
    {
        return 0;
    }
    const lh_usize_t capacity = lh_math_min(size, lh_cast_static(lh_usize_t, LH_SSIZE_T_MAX));
    while (lh_math_ne(lh_compress_gzip_reader_get_phase(self), lh_compress_gzip_reader_phase_done))
    {
        const lh_ssize_t result = lh_compress_gzip_reader_try_step(self, output, capacity);
        if (!lh_math_is_zero(result))
        {
            return result;
        }
    }
    return 0;
}

lh_void
lh_compress_gzip_reader_get_reader(lh_compress_gzip_reader_t *self, lh_io_reader_t *output)
{
    lh_io_reader_init(output, lh_compress_gzip_reader_read, self);
}

lh_ssize_t
lh_compress_gzip_reader_step_header(lh_compress_gzip_reader_t *self)
{
    const lh_ssize_t result = lh_compress_gzip_reader_try_step_header(self);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_gzip_reader_step_deflate(lh_compress_gzip_reader_t *self, lh_ptr output,
                                     lh_usize_t size)
{
    const lh_ssize_t result = lh_compress_gzip_reader_try_step_deflate(self, output, size);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_gzip_reader_step_trailer(lh_compress_gzip_reader_t *self)
{
    const lh_ssize_t result = lh_compress_gzip_reader_try_step_trailer(self);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_gzip_reader_step(lh_compress_gzip_reader_t *self, lh_ptr output, lh_usize_t size)
{
    const lh_ssize_t result = lh_compress_gzip_reader_try_step(self, output, size);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}
