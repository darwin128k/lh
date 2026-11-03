/**
 * @file gzip.c
 * @brief Gzip member validation and decoding.
 */

#include <lh/compress/gzip.h>
#include <lh/hash/crc.h>
#include <lh/bit/endian.h>
#include <lh/assert.h>
#include <lh/math.h>

lh_bool_t
lh_compress_gzip_try_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed)
{
    lh_usize_t pos = 0U;
    if (lh_math_lt(lh_math_sub(input_size, pos), 10U) || input[pos] != 31U ||
        input[lh_math_add(pos, 1U)] != 139U || input[lh_math_add(pos, 2U)] != 8U ||
        !lh_math_is_zero(lh_bit_and(input[lh_math_add(pos, 3U)], 224U)))
    {
        return lh_bool_false;
    }
    const lh_byte_t flags = input[lh_math_add(pos, 3U)];
    pos = lh_math_add(pos, 10U);
    if (!lh_math_is_zero(lh_bit_and(flags, 4U)))
    {
        if (lh_math_lt(lh_math_sub(input_size, pos), 2U))
        {
            return lh_bool_false;
        }
        const lh_uint_t len = lh_bit_unpack_le16(input + pos);
        pos = lh_math_add(pos, 2U);
        if (lh_math_gt(len, lh_math_sub(input_size, pos)))
        {
            return lh_bool_false;
        }
        pos = lh_math_add(pos, len);
    }
    for (lh_uint_t mask = 8U; lh_math_le(mask, 16U); mask = lh_bit_shl(mask, 1U))
    {
        if (!lh_math_is_zero(lh_bit_and(flags, mask)))
        {
            while (lh_math_lt(pos, input_size) && !lh_math_is_zero(input[pos]))
            {
                pos = lh_math_add_one(pos);
            }
            if (lh_math_ge(pos, input_size))
            {
                return lh_bool_false;
            }
            pos = lh_math_add_one(pos);
        }
    }
    if (!lh_math_is_zero(lh_bit_and(flags, 2U)))
    {
        if (lh_math_lt(lh_math_sub(input_size, pos), 2U))
        {
            return lh_bool_false;
        }
        const lh_u32_t crc = lh_hash_crc32(input, pos);
        const lh_uint_t actual = lh_bit_unpack_le16(input + pos);
        if (lh_math_ne(lh_bit_and(crc, 65535U), actual))
        {
            return lh_bool_false;
        }
        pos = lh_math_add(pos, 2U);
    }
    *consumed = pos;
    return lh_bool_true;
}

lh_void
lh_compress_gzip_header(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *consumed)
{
    lh_runtime_check_ifn(lh_compress_gzip_try_header(input, input_size, consumed),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_gzip_try_trailer(const lh_byte_t *trailer, lh_usize_t trailer_size,
                             const lh_byte_t *decoded, lh_usize_t decoded_size)
{
    if (lh_math_lt(trailer_size, 8U))
    {
        return lh_bool_false;
    }
    return lh_math_eq(lh_bit_unpack_le32(trailer), lh_hash_crc32(decoded, decoded_size)) &&
           lh_math_eq(lh_bit_unpack_le32(trailer + 4U), lh_cast_static(lh_u32_t, decoded_size));
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
        pos = lh_math_add(pos, 8U);
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
