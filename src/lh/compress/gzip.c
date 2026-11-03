/**
 * @file gzip.c
 * @brief Gzip member validation and decoding.
 */

#include <lh/compress/gzip.h>
#include <lh/compress/gzip/header/decoder.h>
#include <lh/hash/crc.h>
#include <lh/bit/endian.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>
#include <lh/size/limits.h>
#include <lh/util/interval.h>

lh_bool_t
lh_compress_gzip_try_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed)
{
    lh_compress_gzip_header_decoder_t decoder;
    lh_compress_gzip_header_decoder_init(&decoder, input_size);
    lh_usize_t used;
    if (lh_math_ne(lh_compress_gzip_header_decoder_try_feed(&decoder, input, input_size, &used),
                   lh_compress_gzip_header_decoder_phase_done))
    {
        return lh_bool_false;
    }
    *consumed = used;
    return lh_bool_true;
}

lh_void
lh_compress_gzip_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_header(input, input_size, consumed),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_trailer_values(const lh_byte_t *trailer, lh_usize_t trailer_size, lh_u32_t crc,
                                    lh_usize_t decoded_size)
{
    if (lh_math_lt(trailer_size, LH_COMPRESS_GZIP_TRAILER_SIZE))
    {
        return lh_bool_false;
    }
    return lh_math_eq(lh_bit_unpack_le32(trailer), crc) &&
           lh_math_eq(lh_bit_unpack_le32(trailer + 4U), lh_cast_static(lh_u32_t, decoded_size));
}

lh_void
lh_compress_gzip_trailer_values(const lh_byte_t *trailer, lh_usize_t trailer_size, lh_u32_t crc,
                                lh_usize_t decoded_size)
{
    lh_runtime_check_ifn(
        lh_compress_gzip_try_trailer_values(trailer, trailer_size, crc, decoded_size),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_trailer(const lh_byte_t *trailer, lh_usize_t trailer_size,
                             const lh_byte_t *decoded, lh_usize_t decoded_size)
{
    return lh_compress_gzip_try_trailer_values(trailer, trailer_size,
                                               lh_hash_crc32(decoded, decoded_size), decoded_size);
}

lh_void
lh_compress_gzip_trailer(const lh_byte_t *trailer, lh_usize_t trailer_size,
                         const lh_byte_t *decoded, lh_usize_t decoded_size)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_trailer(trailer, trailer_size, decoded, decoded_size),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                            lh_usize_t capacity, lh_usize_t *written)
{
    lh_usize_t pos = 0U;
    lh_usize_t total = 0U;
    if (lh_math_is_zero(input_size))
    {
        return lh_bool_false;
    }
    while (lh_math_lt(pos, input_size))
    {
        lh_usize_t header_size;
        if (!lh_compress_gzip_try_header(input + pos, lh_math_sub(input_size, pos), &header_size))
        {
            return lh_bool_false;
        }
        pos = lh_math_add(pos, header_size);
        lh_usize_t consumed, decoded;
        if (!lh_compress_deflate_try_decode(input + pos, lh_math_sub(input_size, pos),
                                            output + total, lh_math_sub(capacity, total), &consumed,
                                            &decoded))
        {
            return lh_bool_false;
        }
        pos = lh_math_add(pos, consumed);
        if (!lh_compress_gzip_try_trailer(input + pos, lh_math_sub(input_size, pos), output + total,
                                          decoded))
        {
            return lh_bool_false;
        }
        total = lh_math_add(total, decoded);
        pos = lh_math_add(pos, LH_COMPRESS_GZIP_TRAILER_SIZE);
    }
    *written = total;
    return lh_bool_true;
}

lh_void
lh_compress_gzip_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                        lh_usize_t capacity, lh_usize_t *written)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_decode(input, input_size, output, capacity, written),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_put_header(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    const lh_usize_t used = *written;
    if (lh_math_gt(used, capacity) ||
        lh_math_lt(lh_math_sub(capacity, used), LH_COMPRESS_GZIP_HEADER_SIZE))
    {
        return lh_bool_false;
    }
    lh_byte_t *at = output + used;
    lh_memory_std_set(at, 0U, LH_COMPRESS_GZIP_HEADER_SIZE);
    at[0] = LH_COMPRESS_GZIP_HEADER_ID1;
    at[1] = LH_COMPRESS_GZIP_HEADER_ID2;
    at[2] = LH_COMPRESS_GZIP_HEADER_METHOD_DEFLATE;
    at[lh_math_sub(LH_COMPRESS_GZIP_HEADER_SIZE, 1U)] = LH_COMPRESS_GZIP_HEADER_OS_UNKNOWN;
    *written = lh_math_add(used, LH_COMPRESS_GZIP_HEADER_SIZE);
    return lh_bool_true;
}

lh_void
lh_compress_gzip_put_header(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_put_header(output, capacity, written),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_put_trailer(lh_u32_t crc, lh_usize_t decoded_size, lh_byte_t *output,
                                 lh_usize_t capacity, lh_usize_t *written)
{
    const lh_usize_t used = *written;
    if (lh_math_gt(used, capacity) ||
        lh_math_lt(lh_math_sub(capacity, used), LH_COMPRESS_GZIP_TRAILER_SIZE))
    {
        return lh_bool_false;
    }
    lh_bit_pack_le32(crc, output + used);
    lh_bit_pack_le32(lh_cast_static(lh_u32_t, decoded_size), output + lh_math_add(used, 4U));
    *written = lh_math_add(used, LH_COMPRESS_GZIP_TRAILER_SIZE);
    return lh_bool_true;
}

lh_void
lh_compress_gzip_put_trailer(lh_u32_t crc, lh_usize_t decoded_size, lh_byte_t *output,
                             lh_usize_t capacity, lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_gzip_try_put_trailer(crc, decoded_size, output, capacity, written),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_bound(lh_usize_t size, lh_usize_t *bound)
{
    lh_usize_t stored;
    const lh_usize_t frame =
        lh_math_add(LH_COMPRESS_GZIP_HEADER_SIZE, LH_COMPRESS_GZIP_TRAILER_SIZE);
    if (!lh_compress_deflate_try_stored_bound(size, &stored) ||
        lh_interval_closed_is_add_overflow(stored, frame, 0U, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }
    *bound = lh_math_add(stored, frame);
    return lh_bool_true;
}

lh_void
lh_compress_gzip_bound(lh_usize_t size, lh_usize_t *bound)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_bound(size, bound),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                            lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                            lh_usize_t *written)
{
    lh_usize_t used = 0U;
    if (!lh_compress_gzip_try_put_header(output, capacity, &used) ||
        lh_math_lt(lh_math_sub(capacity, used), LH_COMPRESS_GZIP_TRAILER_SIZE))
    {
        return lh_bool_false;
    }
    lh_usize_t body;
    if (!lh_compress_deflate_try_encode(
            matcher, input, input_size, output + used,
            lh_math_sub(lh_math_sub(capacity, used), LH_COMPRESS_GZIP_TRAILER_SIZE), &body))
    {
        return lh_bool_false;
    }
    used = lh_math_add(used, body);
    if (!lh_compress_gzip_try_put_trailer(lh_hash_crc32(input, input_size), input_size, output,
                                          capacity, &used))
    {
        return lh_bool_false;
    }
    *written = used;
    return lh_bool_true;
}

lh_void
lh_compress_gzip_encode(lh_compress_deflate_matcher_t *matcher, const lh_byte_t *input,
                        lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                        lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_gzip_try_encode(matcher, input, input_size, output, capacity, written),
        lh_runtime_error_code_invalid_range);
}
