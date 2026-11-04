/**
 * @file decoder.c
 * @brief Incremental gzip header decoding.
 */

#include <lh/core/compress/gzip/header/decoder.h>
#include <lh/core/hash/crc.h>
#include <lh/core/assert.h>
#include <lh/core/math.h>

lh_usize_t
lh_compress_gzip_header_decoder_get_size(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_compress_gzip_header_decoder_set_size(lh_compress_gzip_header_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->size = value;
}

lh_usize_t
lh_compress_gzip_header_decoder_get_limit(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->limit;
}

lh_void
lh_compress_gzip_header_decoder_set_limit(lh_compress_gzip_header_decoder_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->limit = value;
}

lh_usize_t
lh_compress_gzip_header_decoder_get_remaining(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->remaining;
}

lh_void
lh_compress_gzip_header_decoder_set_remaining(lh_compress_gzip_header_decoder_t *self,
                                              lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->remaining = value;
}

lh_u32_t
lh_compress_gzip_header_decoder_get_crc(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->crc;
}

lh_void
lh_compress_gzip_header_decoder_set_crc(lh_compress_gzip_header_decoder_t *self, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    self->crc = value;
}

lh_u16_t
lh_compress_gzip_header_decoder_get_value(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_void
lh_compress_gzip_header_decoder_set_value(lh_compress_gzip_header_decoder_t *self, lh_u16_t value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}

lh_u8_t
lh_compress_gzip_header_decoder_get_flags(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flags;
}

lh_void
lh_compress_gzip_header_decoder_set_flags(lh_compress_gzip_header_decoder_t *self, lh_u8_t value)
{
    lh_assert_runtime_ref(self);
    self->flags = value;
}

lh_u8_t
lh_compress_gzip_header_decoder_get_fixed(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fixed;
}

lh_void
lh_compress_gzip_header_decoder_set_fixed(lh_compress_gzip_header_decoder_t *self, lh_u8_t value)
{
    lh_assert_runtime_ref(self);
    self->fixed = value;
}

lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_get_phase(const lh_compress_gzip_header_decoder_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_compress_gzip_header_decoder_set_phase(lh_compress_gzip_header_decoder_t *self,
                                          lh_compress_gzip_header_decoder_phase_t value)
{
    lh_assert_runtime_ref(self);
    self->phase = value;
}

lh_void
lh_compress_gzip_header_decoder_init(lh_compress_gzip_header_decoder_t *self, lh_usize_t limit)
{
    lh_compress_gzip_header_decoder_set_size(self, 0U);
    lh_compress_gzip_header_decoder_set_limit(self, limit);
    lh_compress_gzip_header_decoder_set_remaining(self, 0U);
    lh_compress_gzip_header_decoder_set_crc(self, 0U);
    lh_compress_gzip_header_decoder_set_value(self, 0U);
    lh_compress_gzip_header_decoder_set_flags(self, 0U);
    lh_compress_gzip_header_decoder_set_fixed(self, 0U);
    lh_compress_gzip_header_decoder_set_phase(self, lh_compress_gzip_header_decoder_phase_fixed);
}

lh_void
lh_compress_gzip_header_decoder_next(lh_compress_gzip_header_decoder_t *self)
{
    const lh_u8_t flags = lh_compress_gzip_header_decoder_get_flags(self);
    lh_u8_t mask = 0U;
    lh_compress_gzip_header_decoder_phase_t phase = lh_compress_gzip_header_decoder_phase_done;
    if (!lh_math_is_zero(lh_bit_and(flags, 4U)))
    {
        mask = 4U;
        phase = lh_compress_gzip_header_decoder_phase_extra_low;
    }
    else if (!lh_math_is_zero(lh_bit_and(flags, 8U)))
    {
        mask = 8U;
        phase = lh_compress_gzip_header_decoder_phase_name;
    }
    else if (!lh_math_is_zero(lh_bit_and(flags, 16U)))
    {
        mask = 16U;
        phase = lh_compress_gzip_header_decoder_phase_comment;
    }
    else if (!lh_math_is_zero(lh_bit_and(flags, 2U)))
    {
        mask = 2U;
        phase = lh_compress_gzip_header_decoder_phase_crc_low;
    }
    lh_compress_gzip_header_decoder_set_flags(
        self, lh_cast_static(lh_u8_t, lh_bit_and(flags, lh_bit_not(mask))));
    lh_compress_gzip_header_decoder_set_phase(self, phase);
}

lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_try_feed(lh_compress_gzip_header_decoder_t *self,
                                         const lh_byte_t *input, lh_usize_t size,
                                         lh_usize_t *consumed)
{
    lh_usize_t i = 0U;
    while (lh_math_lt(i, size))
    {
        const lh_compress_gzip_header_decoder_phase_t phase =
            lh_compress_gzip_header_decoder_get_phase(self);
        if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_done) ||
            lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_error))
        {
            break;
        }
        const lh_usize_t count = lh_compress_gzip_header_decoder_get_size(self);
        if (lh_math_ge(count, lh_compress_gzip_header_decoder_get_limit(self)))
        {
            lh_compress_gzip_header_decoder_set_phase(self,
                                                      lh_compress_gzip_header_decoder_phase_error);
            break;
        }
        const lh_byte_t byte = input[i];
        i = lh_math_add_one(i);
        lh_compress_gzip_header_decoder_set_size(self, lh_math_add_one(count));
        if (lh_math_ne(phase, lh_compress_gzip_header_decoder_phase_crc_low) &&
            lh_math_ne(phase, lh_compress_gzip_header_decoder_phase_crc_high))
        {
            lh_compress_gzip_header_decoder_set_crc(
                self,
                lh_hash_crc32_update(lh_compress_gzip_header_decoder_get_crc(self), &byte, 1U));
        }
        if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_fixed))
        {
            const lh_u8_t index = lh_compress_gzip_header_decoder_get_fixed(self);
            if ((lh_math_eq(index, 0U) && lh_math_ne(byte, LH_COMPRESS_GZIP_HEADER_ID1)) ||
                (lh_math_eq(index, 1U) && lh_math_ne(byte, LH_COMPRESS_GZIP_HEADER_ID2)) ||
                (lh_math_eq(index, 2U) &&
                 lh_math_ne(byte, LH_COMPRESS_GZIP_HEADER_METHOD_DEFLATE)) ||
                (lh_math_eq(index, 3U) && !lh_math_is_zero(lh_bit_and(byte, 224U))))
            {
                lh_compress_gzip_header_decoder_set_phase(
                    self, lh_compress_gzip_header_decoder_phase_error);
                break;
            }
            if (lh_math_eq(index, 3U))
            {
                lh_compress_gzip_header_decoder_set_flags(self, byte);
            }
            lh_compress_gzip_header_decoder_set_fixed(self, lh_math_add_one(index));
            if (lh_math_eq(index, lh_math_sub(LH_COMPRESS_GZIP_HEADER_SIZE, 1U)))
            {
                lh_compress_gzip_header_decoder_next(self);
            }
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_extra_low))
        {
            lh_compress_gzip_header_decoder_set_value(self, byte);
            lh_compress_gzip_header_decoder_set_phase(
                self, lh_compress_gzip_header_decoder_phase_extra_high);
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_extra_high))
        {
            const lh_u16_t length = lh_bit_or(lh_compress_gzip_header_decoder_get_value(self),
                                              lh_bit_shl(lh_cast_static(lh_u16_t, byte), 8U));
            lh_compress_gzip_header_decoder_set_remaining(self, length);
            if (lh_math_is_zero(length))
            {
                lh_compress_gzip_header_decoder_next(self);
            }
            else
            {
                lh_compress_gzip_header_decoder_set_phase(
                    self, lh_compress_gzip_header_decoder_phase_extra);
            }
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_extra))
        {
            const lh_usize_t remaining =
                lh_math_sub_one(lh_compress_gzip_header_decoder_get_remaining(self));
            lh_compress_gzip_header_decoder_set_remaining(self, remaining);
            if (lh_math_is_zero(remaining))
            {
                lh_compress_gzip_header_decoder_next(self);
            }
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_name) ||
                 lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_comment))
        {
            if (lh_math_is_zero(byte))
            {
                lh_compress_gzip_header_decoder_next(self);
            }
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_crc_low))
        {
            lh_compress_gzip_header_decoder_set_value(self, byte);
            lh_compress_gzip_header_decoder_set_phase(
                self, lh_compress_gzip_header_decoder_phase_crc_high);
        }
        else if (lh_math_eq(phase, lh_compress_gzip_header_decoder_phase_crc_high))
        {
            const lh_u16_t expected = lh_bit_or(lh_compress_gzip_header_decoder_get_value(self),
                                                lh_bit_shl(lh_cast_static(lh_u16_t, byte), 8U));
            lh_compress_gzip_header_decoder_set_phase(
                self, lh_math_eq(expected,
                                 lh_bit_and(lh_compress_gzip_header_decoder_get_crc(self), 65535U))
                          ? lh_compress_gzip_header_decoder_phase_done
                          : lh_compress_gzip_header_decoder_phase_error);
        }
        else
        {
            lh_compress_gzip_header_decoder_set_phase(self,
                                                      lh_compress_gzip_header_decoder_phase_error);
            break;
        }
    }
    *consumed = i;
    return lh_compress_gzip_header_decoder_get_phase(self);
}

lh_compress_gzip_header_decoder_phase_t
lh_compress_gzip_header_decoder_feed(lh_compress_gzip_header_decoder_t *self,
                                     const lh_byte_t *input, lh_usize_t size, lh_usize_t *consumed)
{
    const lh_compress_gzip_header_decoder_phase_t phase =
        lh_compress_gzip_header_decoder_try_feed(self, input, size, consumed);
    lh_runtime_check_ifn(lh_math_ne(phase, lh_compress_gzip_header_decoder_phase_error),
                         lh_runtime_error_code_invalid_range);
    return phase;
}
