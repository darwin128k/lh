/**
 * @file deflate.c
 * @brief Bounded DEFLATE decoder.
 */

#include <lh/compress/deflate.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>
#include <lh/size/limits.h>

lh_bool_t
lh_compress_deflate_try_bits(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                             lh_uint_t count, lh_uint_t *value)
{
    lh_usize_t pos = *position;
    if (lh_math_gt(count, lh_math_sub(LH_USIZE_T_MAX, pos)))
    {
        return lh_bool_false;
    }
    lh_uint_t result = 0U;
    for (lh_uint_t i = 0U; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        if (lh_math_ge(lh_math_div(pos, 8U), size))
        {
            return lh_bool_false;
        }
        const lh_uint_t bit =
            lh_bit_and(lh_bit_shr(input[lh_math_div(pos, 8U)], lh_math_mod(pos, 8U)), 1U);
        result = lh_bit_or(result, lh_bit_shl(bit, i));
        pos = lh_math_add_one(pos);
    }
    *position = pos;
    *value = result;
    return lh_bool_true;
}

lh_bool_t
lh_compress_deflate_try_build(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *counts,
                              lh_u16_t *symbols)
{
    lh_memory_std_set(counts, 0U, 16U * sizeof(*counts));
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        if (lh_math_gt(lengths[i], 15U))
        {
            return lh_bool_false;
        }
        counts[lengths[i]] = lh_math_add_one(counts[lengths[i]]);
    }
    lh_int_t left = 1;
    for (lh_uint_t len = 1U; lh_math_le(len, 15U); len = lh_math_add_one(len))
    {
        left = lh_math_sub(lh_math_mul(left, 2), counts[len]);
        if (lh_math_lt(left, 0))
        {
            return lh_bool_false;
        }
    }
    lh_u16_t offsets[16];
    offsets[1] = 0U;
    for (lh_uint_t len = 1U; lh_math_lt(len, 15U); len = lh_math_add_one(len))
    {
        offsets[lh_math_add_one(len)] = lh_math_add(offsets[len], counts[len]);
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        const lh_uint_t len = lengths[i];
        if (!lh_math_is_zero(len))
        {
            symbols[offsets[len]] = lh_cast_static(lh_u16_t, i);
            offsets[len] = lh_math_add_one(offsets[len]);
        }
    }
    return lh_bool_true;
}

lh_bool_t
lh_compress_deflate_try_symbol(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                               const lh_u16_t *counts, const lh_u16_t *symbols, lh_uint_t *symbol)
{
    lh_usize_t pos = *position;
    lh_uint_t code = 0U;
    lh_uint_t first = 0U;
    lh_uint_t index = 0U;
    for (lh_uint_t len = 1U; lh_math_le(len, 15U); len = lh_math_add_one(len))
    {
        lh_uint_t bit;
        if (!lh_compress_deflate_try_bits(input, size, &pos, 1U, &bit))
        {
            return lh_bool_false;
        }
        code = lh_bit_or(code, bit);
        const lh_uint_t count = counts[len];
        if (lh_math_ge(code, first) && lh_math_lt(lh_math_sub(code, first), count))
        {
            *symbol = symbols[lh_math_add(index, lh_math_sub(code, first))];
            *position = pos;
            return lh_bool_true;
        }
        index = lh_math_add(index, count);
        first = lh_bit_shl(lh_math_add(first, count), 1U);
        code = lh_bit_shl(code, 1U);
    }
    return lh_bool_false;
}

lh_bool_t
lh_compress_deflate_try_stored(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    lh_usize_t pos = *position;
    const lh_usize_t used = *written;
    if (lh_math_gt(used, capacity) || lh_math_gt(pos, lh_math_sub(LH_USIZE_T_MAX, 7U)))
    {
        return lh_bool_false;
    }
    pos = lh_math_mul(lh_math_div(lh_math_add(pos, 7U), 8U), 8U);
    lh_uint_t length;
    if (!lh_compress_deflate_try_bits(input, input_size, &pos, 16U, &length))
    {
        return lh_bool_false;
    }
    lh_uint_t inverse;
    if (!lh_compress_deflate_try_bits(input, input_size, &pos, 16U, &inverse) ||
        lh_math_ne(lh_bit_xor(length, inverse), 65535U))
    {
        return lh_bool_false;
    }
    if (lh_math_gt(length, lh_math_sub(capacity, used)) ||
        lh_math_gt(length, lh_math_sub(input_size, lh_math_div(pos, 8U))) ||
        lh_math_gt(length, lh_math_div(lh_math_sub(LH_USIZE_T_MAX, pos), 8U)))
    {
        return lh_bool_false;
    }
    if (!lh_math_is_zero(length))
    {
        lh_memory_std_copy(output + used, input + lh_math_div(pos, 8U), length);
    }
    *written = lh_math_add(used, length);
    *position = lh_math_add(pos, lh_math_mul(length, 8U));
    return lh_bool_true;
}

lh_void
lh_compress_deflate_stored(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                           lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_stored(input, input_size, position, output, capacity, written),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                               lh_usize_t capacity, lh_usize_t *consumed, lh_usize_t *written)
{
    const lh_u16_t length_base[29] = {3,  4,  5,  6,   7,   8,   9,   10,  11, 13,
                                      15, 17, 19, 23,  27,  31,  35,  43,  51, 59,
                                      67, 83, 99, 115, 131, 163, 195, 227, 258};
    const lh_byte_t length_extra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2,
                                        2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    const lh_u16_t distance_base[30] = {
        1,   2,   3,   4,   5,   7,    9,    13,   17,   25,   33,   49,   65,    97,    129,
        193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
    const lh_byte_t distance_extra[30] = {0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
                                          6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
    const lh_byte_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    lh_usize_t pos = 0U;
    lh_usize_t used = 0U;
    lh_uint_t final = 0U;
    do
    {
        lh_uint_t type;
        if (!lh_compress_deflate_try_bits(input, input_size, &pos, 1U, &final) ||
            !lh_compress_deflate_try_bits(input, input_size, &pos, 2U, &type))
        {
            return lh_bool_false;
        }
        if (lh_math_is_zero(type))
        {
            if (!lh_compress_deflate_try_stored(input, input_size, &pos, output, capacity, &used))
            {
                return lh_bool_false;
            }
            continue;
        }
        if (lh_math_gt(type, 2U))
        {
            return lh_bool_false;
        }
        lh_byte_t lengths[320];
        lh_u16_t lit_counts[16], lit_symbols[288], dist_counts[16], dist_symbols[32];
        lh_memory_std_set(lengths, 0U, sizeof(lengths));
        if (lh_math_eq(type, 1U))
        {
            for (lh_uint_t i = 0U; lh_math_lt(i, 288U); i = lh_math_add_one(i))
            {
                lengths[i] = lh_math_lt(i, 144U)
                                 ? 8U
                                 : (lh_math_lt(i, 256U) ? 9U : (lh_math_lt(i, 280U) ? 7U : 8U));
            }
            for (lh_uint_t i = 288U; lh_math_lt(i, 320U); i = lh_math_add_one(i))
            {
                lengths[i] = 5U;
            }
            if (!lh_compress_deflate_try_build(lengths, 288U, lit_counts, lit_symbols) ||
                !lh_compress_deflate_try_build(lengths + 288U, 32U, dist_counts, dist_symbols))
            {
                return lh_bool_false;
            }
        }
        else
        {
            lh_uint_t hlit, hdist, hclen;
            if (!lh_compress_deflate_try_bits(input, input_size, &pos, 5U, &hlit) ||
                !lh_compress_deflate_try_bits(input, input_size, &pos, 5U, &hdist) ||
                !lh_compress_deflate_try_bits(input, input_size, &pos, 4U, &hclen))
            {
                return lh_bool_false;
            }
            hlit = lh_math_add(hlit, 257U);
            hdist = lh_math_add(hdist, 1U);
            hclen = lh_math_add(hclen, 4U);
            if (lh_math_gt(hlit, 286U))
            {
                return lh_bool_false;
            }
            lh_byte_t code_lengths[19];
            lh_memory_std_set(code_lengths, 0U, sizeof(code_lengths));
            for (lh_uint_t i = 0U; lh_math_lt(i, hclen); i = lh_math_add_one(i))
            {
                lh_uint_t n;
                if (!lh_compress_deflate_try_bits(input, input_size, &pos, 3U, &n))
                {
                    return lh_bool_false;
                }
                code_lengths[order[i]] = lh_cast_static(lh_byte_t, n);
            }
            lh_u16_t code_counts[16], code_symbols[19];
            if (!lh_compress_deflate_try_build(code_lengths, 19U, code_counts, code_symbols))
            {
                return lh_bool_false;
            }
            const lh_uint_t total = lh_math_add(hlit, hdist);
            for (lh_uint_t i = 0U; lh_math_lt(i, total);)
            {
                lh_uint_t symbol;
                if (!lh_compress_deflate_try_symbol(input, input_size, &pos, code_counts,
                                                    code_symbols, &symbol))
                {
                    return lh_bool_false;
                }
                if (lh_math_lt(symbol, 16U))
                {
                    lengths[i] = lh_cast_static(lh_byte_t, symbol);
                    i = lh_math_add_one(i);
                    continue;
                }
                if (lh_math_gt(symbol, 18U) || (lh_math_eq(symbol, 16U) && lh_math_is_zero(i)))
                {
                    return lh_bool_false;
                }
                lh_uint_t extra;
                const lh_uint_t bits =
                    lh_math_eq(symbol, 16U) ? 2U : (lh_math_eq(symbol, 17U) ? 3U : 7U);
                if (!lh_compress_deflate_try_bits(input, input_size, &pos, bits, &extra))
                {
                    return lh_bool_false;
                }
                const lh_uint_t repeat = lh_math_add(extra, lh_math_eq(symbol, 18U) ? 11U : 3U);
                if (lh_math_gt(repeat, lh_math_sub(total, i)))
                {
                    return lh_bool_false;
                }
                const lh_byte_t value = lh_math_eq(symbol, 16U) ? lengths[lh_math_sub(i, 1U)] : 0U;
                for (lh_uint_t j = 0U; lh_math_lt(j, repeat); j = lh_math_add_one(j))
                {
                    lengths[i] = value;
                    i = lh_math_add_one(i);
                }
            }
            if (lh_math_is_zero(lengths[256]) ||
                !lh_compress_deflate_try_build(lengths, hlit, lit_counts, lit_symbols) ||
                !lh_compress_deflate_try_build(lengths + hlit, hdist, dist_counts, dist_symbols))
            {
                return lh_bool_false;
            }
        }
        for (;;)
        {
            lh_uint_t symbol;
            if (!lh_compress_deflate_try_symbol(input, input_size, &pos, lit_counts, lit_symbols,
                                                &symbol))
            {
                return lh_bool_false;
            }
            if (lh_math_lt(symbol, 256U))
            {
                if (lh_math_ge(used, capacity))
                {
                    return lh_bool_false;
                }
                output[used] = lh_cast_static(lh_byte_t, symbol);
                used = lh_math_add_one(used);
                continue;
            }
            if (lh_math_eq(symbol, 256U))
            {
                break;
            }
            if (lh_math_gt(symbol, 285U))
            {
                return lh_bool_false;
            }
            const lh_uint_t li = lh_math_sub(symbol, 257U);
            lh_uint_t extra;
            if (!lh_compress_deflate_try_bits(input, input_size, &pos, length_extra[li], &extra))
            {
                return lh_bool_false;
            }
            const lh_uint_t length = lh_math_add(length_base[li], extra);
            lh_uint_t distance_symbol;
            if (!lh_compress_deflate_try_symbol(input, input_size, &pos, dist_counts, dist_symbols,
                                                &distance_symbol) ||
                lh_math_ge(distance_symbol, 30U) ||
                !lh_compress_deflate_try_bits(input, input_size, &pos,
                                              distance_extra[distance_symbol], &extra))
            {
                return lh_bool_false;
            }
            const lh_uint_t distance = lh_math_add(distance_base[distance_symbol], extra);
            if (lh_math_gt(distance, used) || lh_math_gt(length, lh_math_sub(capacity, used)))
            {
                return lh_bool_false;
            }
            for (lh_uint_t i = 0U; lh_math_lt(i, length); i = lh_math_add_one(i))
            {
                output[used] = output[lh_math_sub(used, distance)];
                used = lh_math_add_one(used);
            }
        }
    } while (lh_math_is_zero(final));
    *consumed = lh_math_div(lh_math_add(pos, 7U), 8U);
    *written = used;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                           lh_usize_t capacity, lh_usize_t *consumed, lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_decode(input, input_size, output, capacity, consumed, written),
        lh_runtime_error_code_invalid_range);
}

lh_void
lh_compress_deflate_bits(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                         lh_uint_t count, lh_uint_t *value)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_bits(input, size, position, count, value),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_compress_deflate_build(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *counts,
                          lh_u16_t *symbols)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_build(lengths, size, counts, symbols),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_compress_deflate_symbol(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                           const lh_u16_t *counts, const lh_u16_t *symbols, lh_uint_t *symbol)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_symbol(input, size, position, counts, symbols, symbol),
        lh_runtime_error_code_invalid_range);
}
