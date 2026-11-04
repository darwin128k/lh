/**
 * @file deflate.c
 * @brief Bounded DEFLATE decoder and block-wise encoder.
 */

#include <lh/core/compress/deflate.h>
#include <lh/core/bit/reverse.h>
#include <lh/core/memory.h>
#include <lh/core/assert.h>
#include <lh/core/math.h>
#include <lh/core/size/limits.h>
#include <lh/core/util/interval.h>

static const lh_u16_t lh_compress_deflate_length_base[29] = {
    3,  4,  5,  6,  7,  8,  9,  10, 11,  13,  15,  17,  19,  23, 27,
    31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};

static const lh_byte_t lh_compress_deflate_length_extra[29] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};

static const lh_u16_t lh_compress_deflate_distance_base[30] = {
    1,   2,   3,   4,   5,   7,    9,    13,   17,   25,   33,   49,   65,    97,    129,
    193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

static const lh_byte_t lh_compress_deflate_distance_extra[30] = {
    0, 0, 0, 0, 1, 1, 2, 2,  3,  3,  4,  4,  5,  5,  6,
    6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

lh_uint_t
lh_compress_deflate_get_length_base(lh_uint_t index)
{
    return lh_compress_deflate_length_base[index];
}

lh_uint_t
lh_compress_deflate_get_length_extra(lh_uint_t index)
{
    return lh_compress_deflate_length_extra[index];
}

lh_uint_t
lh_compress_deflate_get_distance_base(lh_uint_t code)
{
    return lh_compress_deflate_distance_base[code];
}

lh_uint_t
lh_compress_deflate_get_distance_extra(lh_uint_t code)
{
    return lh_compress_deflate_distance_extra[code];
}

lh_uint_t
lh_compress_deflate_get_length_index(lh_uint_t length)
{
    /* Past the first eight lengths, every four indices double the range: the
     * top set bit of length - 3 picks the group, the two bits below it the
     * index within the group. Only 258 leaves the pattern. */
    const lh_u32_t x = lh_cast_static(lh_u32_t, lh_math_sub(length, 3U));
    if (lh_math_lt(x, 8U))
    {
        return x;
    }
    if (lh_math_eq(length, 258U))
    {
        return 28U;
    }
    const lh_uint_t top = lh_cast_static(lh_uint_t, lh_bit_scan_reverse_u32(x));
    return lh_math_add(lh_math_mul(lh_math_sub(top, 1U), 4U),
                       lh_bit_and(lh_bit_shr(x, lh_math_sub(top, 2U)), 3U));
}

lh_uint_t
lh_compress_deflate_get_distance_code(lh_uint_t distance)
{
    /* Past the first four distances, every two codes double the range: the
     * top set bit of distance - 1 picks the pair, the bit below it the code. */
    const lh_u32_t x = lh_cast_static(lh_u32_t, lh_math_sub(distance, 1U));
    if (lh_math_lt(x, 4U))
    {
        return x;
    }
    const lh_uint_t top = lh_cast_static(lh_uint_t, lh_bit_scan_reverse_u32(x));
    return lh_math_add(lh_math_mul(top, 2U), lh_bit_and(lh_bit_shr(x, lh_math_sub(top, 1U)), 1U));
}

lh_uint_t
lh_compress_deflate_get_fixed_length(lh_uint_t symbol)
{
    if (lh_math_lt(symbol, 144U))
    {
        return 8U;
    }
    if (lh_math_lt(symbol, 256U))
    {
        return 9U;
    }
    return lh_math_lt(symbol, 280U) ? 7U : 8U;
}

lh_void
lh_compress_deflate_fill_fixed_lengths(lh_byte_t *lit_lengths, lh_byte_t *dist_lengths)
{
    for (lh_uint_t i = 0U; lh_math_lt(i, LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES);
         i = lh_math_add_one(i))
    {
        lit_lengths[i] = lh_cast_static(lh_byte_t, lh_compress_deflate_get_fixed_length(i));
    }
    lh_memory_std_set(dist_lengths, LH_COMPRESS_DEFLATE_FIXED_DISTANCE_BITS,
                      LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES);
}

lh_bool_t
lh_compress_deflate_try_bits(const lh_byte_t *input, lh_usize_t size, lh_usize_t *position,
                             lh_uint_t count, lh_uint_t *value)
{
    lh_usize_t pos = *position;
    if (lh_interval_closed_is_add_overflow(pos, count, 0U, LH_USIZE_T_MAX))
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
lh_compress_deflate_is_tree_valid(const lh_u16_t *counts, lh_bool_t allow_single,
                                  lh_bool_t allow_empty)
{
    lh_int_t left = 1;
    lh_uint_t symbols = 0U;
    for (lh_uint_t len = 1U; lh_math_le(len, 15U); len = lh_math_add_one(len))
    {
        left = lh_math_sub(lh_math_mul(left, 2), counts[len]);
        if (lh_math_lt(left, 0))
        {
            return lh_bool_false;
        }
        symbols = lh_math_add(symbols, counts[len]);
    }
    if (lh_math_is_zero(symbols))
    {
        return allow_empty;
    }
    if (lh_math_is_zero(left))
    {
        return lh_bool_true;
    }
    return allow_single && lh_math_eq(symbols, 1U) && lh_math_eq(counts[1], 1U);
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
lh_compress_deflate_try_stored_header(const lh_byte_t *input, lh_usize_t input_size,
                                      lh_usize_t *position, lh_uint_t *length)
{
    lh_usize_t pos = *position;
    if (lh_interval_closed_is_add_overflow(pos, 7U, 0U, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }
    pos = lh_math_add(pos, lh_compress_deflate_get_pad_bits(pos));
    lh_uint_t value;
    if (!lh_compress_deflate_try_bits(input, input_size, &pos, 16U, &value))
    {
        return lh_bool_false;
    }
    lh_uint_t inverse;
    if (!lh_compress_deflate_try_bits(input, input_size, &pos, 16U, &inverse) ||
        lh_math_ne(lh_bit_xor(value, inverse), 65535U))
    {
        return lh_bool_false;
    }
    *position = pos;
    *length = value;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_stored_header(const lh_byte_t *input, lh_usize_t input_size,
                                  lh_usize_t *position, lh_uint_t *length)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_stored_header(input, input_size, position, length),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_stored(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    lh_usize_t pos = *position;
    const lh_usize_t used = *written;
    if (lh_math_gt(used, capacity))
    {
        return lh_bool_false;
    }
    lh_uint_t length;
    if (!lh_compress_deflate_try_stored_header(input, input_size, &pos, &length))
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
lh_compress_deflate_try_fixed(lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                              lh_u16_t *dist_symbols)
{
    lh_byte_t lit_lengths[LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES],
        dist_lengths[LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES];
    lh_compress_deflate_fill_fixed_lengths(lit_lengths, dist_lengths);
    if (!lh_compress_deflate_try_build(lit_lengths, LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES,
                                       lit_counts, lit_symbols) ||
        !lh_compress_deflate_try_build(dist_lengths, LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES,
                                       dist_counts, dist_symbols))
    {
        return lh_bool_false;
    }
    return lh_bool_true;
}

lh_void
lh_compress_deflate_fixed(lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                          lh_u16_t *dist_symbols)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_fixed(lit_counts, lit_symbols, dist_counts, dist_symbols),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_dynamic(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                                lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                                lh_u16_t *dist_symbols)
{
    const lh_byte_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    lh_usize_t pos = *position;
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
    if (lh_math_gt(hlit, LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS))
    {
        return lh_bool_false;
    }
    lh_byte_t code_lengths[19];
    lh_memory_std_set(code_lengths, 0U, sizeof(code_lengths));
    lh_byte_t lengths[320];
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
    if (!lh_compress_deflate_try_build(code_lengths, 19U, code_counts, code_symbols) ||
        !lh_compress_deflate_is_tree_valid(code_counts, lh_bool_false, lh_bool_false))
    {
        return lh_bool_false;
    }
    const lh_uint_t total = lh_math_add(hlit, hdist);
    for (lh_uint_t i = 0U; lh_math_lt(i, total);)
    {
        lh_uint_t symbol;
        if (!lh_compress_deflate_try_symbol(input, input_size, &pos, code_counts, code_symbols,
                                            &symbol))
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
        const lh_uint_t bits = lh_math_eq(symbol, 16U) ? 2U : (lh_math_eq(symbol, 17U) ? 3U : 7U);
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
        !lh_compress_deflate_is_tree_valid(lit_counts, lh_bool_true, lh_bool_false) ||
        !lh_compress_deflate_try_build(lengths + hlit, hdist, dist_counts, dist_symbols) ||
        !lh_compress_deflate_is_tree_valid(dist_counts, lh_bool_true, lh_bool_true))
    {
        return lh_bool_false;
    }
    *position = pos;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_dynamic(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                            lh_u16_t *lit_counts, lh_u16_t *lit_symbols, lh_u16_t *dist_counts,
                            lh_u16_t *dist_symbols)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_dynamic(input, input_size, position, lit_counts,
                                                         lit_symbols, dist_counts, dist_symbols),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_copy_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written,
                                   lh_usize_t distance, lh_usize_t length)
{
    const lh_usize_t used = *written;
    if (lh_math_gt(used, capacity) || lh_math_is_zero(distance) ||
        lh_math_gt(distance, LH_COMPRESS_DEFLATE_WINDOW_SIZE) || lh_math_gt(distance, used) ||
        lh_math_gt(length, lh_math_sub(capacity, used)))
    {
        return lh_bool_false;
    }
    if (!lh_math_is_zero(length))
    {
        lh_memory_set_pattern(output + used, length, output + lh_math_sub(used, distance),
                              distance);
    }
    *written = lh_math_add(used, length);
    return lh_bool_true;
}

lh_void
lh_compress_deflate_copy_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written,
                               lh_usize_t distance, lh_usize_t length)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_copy_match(output, capacity, written, distance, length),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_match(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                              lh_uint_t symbol, const lh_u16_t *dist_counts,
                              const lh_u16_t *dist_symbols, lh_uint_t *length, lh_uint_t *distance)
{
    if (lh_math_lt(symbol, LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL) ||
        lh_math_ge(symbol, LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS))
    {
        return lh_bool_false;
    }
    lh_usize_t pos = *position;
    const lh_uint_t li = lh_math_sub(symbol, LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL);
    lh_uint_t extra;
    if (!lh_compress_deflate_try_bits(input, input_size, &pos,
                                      lh_compress_deflate_get_length_extra(li), &extra))
    {
        return lh_bool_false;
    }
    const lh_uint_t result_length = lh_math_add(lh_compress_deflate_get_length_base(li), extra);
    lh_uint_t distance_symbol;
    if (!lh_compress_deflate_try_symbol(input, input_size, &pos, dist_counts, dist_symbols,
                                        &distance_symbol) ||
        lh_math_ge(distance_symbol, LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS) ||
        !lh_compress_deflate_try_bits(input, input_size, &pos,
                                      lh_compress_deflate_get_distance_extra(distance_symbol),
                                      &extra))
    {
        return lh_bool_false;
    }
    *length = result_length;
    *distance = lh_math_add(lh_compress_deflate_get_distance_base(distance_symbol), extra);
    *position = pos;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_match(const lh_byte_t *input, lh_usize_t input_size, lh_usize_t *position,
                          lh_uint_t symbol, const lh_u16_t *dist_counts,
                          const lh_u16_t *dist_symbols, lh_uint_t *length, lh_uint_t *distance)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_match(input, input_size, position, symbol,
                                                       dist_counts, dist_symbols, length, distance),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_decode(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                               lh_usize_t capacity, lh_usize_t *consumed, lh_usize_t *written)
{
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
        lh_u16_t lit_counts[16], lit_symbols[LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES],
            dist_counts[16], dist_symbols[LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES];
        if (lh_math_eq(type, 1U))
        {
            if (!lh_compress_deflate_try_fixed(lit_counts, lit_symbols, dist_counts, dist_symbols))
            {
                return lh_bool_false;
            }
        }
        else if (!lh_compress_deflate_try_dynamic(input, input_size, &pos, lit_counts, lit_symbols,
                                                  dist_counts, dist_symbols))
        {
            return lh_bool_false;
        }
        for (;;)
        {
            lh_uint_t symbol;
            if (!lh_compress_deflate_try_symbol(input, input_size, &pos, lit_counts, lit_symbols,
                                                &symbol))
            {
                return lh_bool_false;
            }
            if (lh_math_lt(symbol, LH_COMPRESS_DEFLATE_END_OF_BLOCK))
            {
                if (lh_math_ge(used, capacity))
                {
                    return lh_bool_false;
                }
                output[used] = lh_cast_static(lh_byte_t, symbol);
                used = lh_math_add_one(used);
                continue;
            }
            if (lh_math_eq(symbol, LH_COMPRESS_DEFLATE_END_OF_BLOCK))
            {
                break;
            }
            lh_uint_t length;
            lh_uint_t distance;
            if (!lh_compress_deflate_try_match(input, input_size, &pos, symbol, dist_counts,
                                               dist_symbols, &length, &distance))
            {
                return lh_bool_false;
            }
            if (!lh_compress_deflate_try_copy_match(output, capacity, &used, distance, length))
            {
                return lh_bool_false;
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

lh_bool_t
lh_compress_deflate_try_put_bits(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                 lh_uint_t count, lh_uint_t value)
{
    lh_usize_t pos = *position;
    if (lh_interval_closed_is_add_overflow(pos, count, 0U, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }
    const lh_usize_t end = lh_math_add(pos, count);
    if (!lh_math_is_zero(count) && lh_math_ge(lh_math_div(lh_math_sub(end, 1U), 8U), capacity))
    {
        return lh_bool_false;
    }
    const lh_uint_t shift = lh_cast_static(lh_uint_t, lh_math_mod(pos, 8U));
    lh_u32_t mask = lh_bit_shl(lh_math_sub(lh_bit_mask(count), 1U), shift);
    lh_u32_t bits = lh_bit_and(lh_bit_shl(lh_cast_static(lh_u32_t, value), shift), mask);
    for (lh_usize_t at = lh_math_div(pos, 8U); !lh_math_is_zero(mask); at = lh_math_add_one(at))
    {
        output[at] =
            lh_cast_static(lh_byte_t, lh_bit_or(lh_bit_and(output[at], lh_bit_not(mask)), bits));
        mask = lh_bit_shr(mask, 8U);
        bits = lh_bit_shr(bits, 8U);
    }
    *position = end;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_put_bits(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                             lh_uint_t count, lh_uint_t value)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_put_bits(output, capacity, position, count, value),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_codes(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *codes)
{
    if (lh_math_gt(size, LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES))
    {
        return lh_bool_false;
    }
    lh_u16_t counts[16], symbols[LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES];
    if (!lh_compress_deflate_try_build(lengths, size, counts, symbols))
    {
        return lh_bool_false;
    }
    lh_u16_t next[16];
    next[0] = 0U;
    lh_uint_t code = 0U;
    for (lh_uint_t len = 1U; lh_math_le(len, 15U); len = lh_math_add_one(len))
    {
        const lh_uint_t shorter = lh_math_eq(len, 1U) ? 0U : counts[lh_math_sub(len, 1U)];
        code = lh_bit_shl(lh_math_add(code, shorter), 1U);
        next[len] = lh_cast_static(lh_u16_t, code);
    }
    for (lh_usize_t i = 0U; lh_math_lt(i, size); i = lh_math_add_one(i))
    {
        const lh_uint_t len = lengths[i];
        if (lh_math_is_zero(len))
        {
            codes[i] = 0U;
            continue;
        }
        codes[i] = lh_cast_static(lh_u16_t, lh_bit_reverse_u32(next[len], len));
        next[len] = lh_cast_static(lh_u16_t, lh_math_add_one(next[len]));
    }
    return lh_bool_true;
}

lh_void
lh_compress_deflate_codes(const lh_byte_t *lengths, lh_usize_t size, lh_u16_t *codes)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_codes(lengths, size, codes),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_put_symbol(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                   const lh_u16_t *codes, const lh_byte_t *lengths,
                                   lh_uint_t symbol)
{
    if (lh_math_is_zero(lengths[symbol]))
    {
        return lh_bool_false;
    }
    return lh_compress_deflate_try_put_bits(output, capacity, position, lengths[symbol],
                                            codes[symbol]);
}

lh_void
lh_compress_deflate_put_symbol(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                               const lh_u16_t *codes, const lh_byte_t *lengths, lh_uint_t symbol)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_put_symbol(output, capacity, position, codes, lengths, symbol),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_put_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                                  const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                                  const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                                  lh_uint_t length, lh_uint_t distance)
{
    if (lh_math_lt(length, LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH) ||
        lh_math_gt(length, LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH) || lh_math_is_zero(distance) ||
        lh_math_gt(distance, LH_COMPRESS_DEFLATE_WINDOW_SIZE))
    {
        return lh_bool_false;
    }
    const lh_uint_t index = lh_compress_deflate_get_length_index(length);
    const lh_uint_t code = lh_compress_deflate_get_distance_code(distance);
    lh_usize_t pos = *position;
    if (!lh_compress_deflate_try_put_symbol(
            output, capacity, &pos, lit_codes, lit_lengths,
            lh_math_add(index, LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL)) ||
        !lh_compress_deflate_try_put_bits(
            output, capacity, &pos, lh_compress_deflate_get_length_extra(index),
            lh_math_sub(length, lh_compress_deflate_get_length_base(index))) ||
        !lh_compress_deflate_try_put_symbol(output, capacity, &pos, dist_codes, dist_lengths,
                                            code) ||
        !lh_compress_deflate_try_put_bits(
            output, capacity, &pos, lh_compress_deflate_get_distance_extra(code),
            lh_math_sub(distance, lh_compress_deflate_get_distance_base(code))))
    {
        return lh_bool_false;
    }
    *position = pos;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_put_match(lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position,
                              const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                              const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                              lh_uint_t length, lh_uint_t distance)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_put_match(output, capacity, position, lit_codes,
                                                           lit_lengths, dist_codes, dist_lengths,
                                                           length, distance),
                         lh_runtime_error_code_invalid_range);
}

lh_uint_t
lh_compress_deflate_get_pad_bits(lh_usize_t position)
{
    return lh_cast_static(lh_uint_t, lh_math_mod(lh_math_sub(8U, lh_math_mod(position, 8U)), 8U));
}

lh_bool_t
lh_compress_deflate_try_put_stored(const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                                   lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position)
{
    if (lh_math_gt(input_size, LH_COMPRESS_DEFLATE_STORED_MAX))
    {
        return lh_bool_false;
    }
    lh_usize_t pos = *position;
    const lh_uint_t length = lh_cast_static(lh_uint_t, input_size);
    if (!lh_compress_deflate_try_put_bits(output, capacity, &pos, 3U, final ? 1U : 0U) ||
        !lh_compress_deflate_try_put_bits(output, capacity, &pos,
                                          lh_compress_deflate_get_pad_bits(pos), 0U) ||
        !lh_compress_deflate_try_put_bits(output, capacity, &pos, 16U, length) ||
        !lh_compress_deflate_try_put_bits(output, capacity, &pos, 16U,
                                          lh_bit_xor(length, LH_COMPRESS_DEFLATE_STORED_MAX)))
    {
        return lh_bool_false;
    }
    const lh_usize_t at = lh_math_div(pos, 8U);
    if (lh_math_gt(input_size, lh_math_sub(capacity, at)))
    {
        return lh_bool_false;
    }
    if (!lh_math_is_zero(input_size))
    {
        lh_memory_std_copy(output + at, input, input_size);
    }
    *position = lh_math_add(pos, lh_math_mul(input_size, 8U));
    return lh_bool_true;
}

lh_void
lh_compress_deflate_put_stored(const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_put_stored(input, input_size, final, output, capacity, position),
        lh_runtime_error_code_invalid_range);
}

lh_usize_t
lh_compress_deflate_get_block_count(lh_usize_t size, lh_usize_t block_size)
{
    const lh_usize_t whole = lh_math_div(size, block_size);
    if (lh_math_is_zero(size) || !lh_math_is_zero(lh_math_mod(size, block_size)))
    {
        return lh_math_add_one(whole);
    }
    return whole;
}

lh_bool_t
lh_compress_deflate_try_blocks_bound(lh_usize_t size, lh_usize_t block_size, lh_usize_t *bound)
{
    const lh_usize_t overhead = lh_math_mul(lh_compress_deflate_get_block_count(size, block_size),
                                            LH_COMPRESS_DEFLATE_STORED_OVERHEAD);
    if (lh_interval_closed_is_add_overflow(size, overhead, 0U, LH_USIZE_T_MAX))
    {
        return lh_bool_false;
    }
    *bound = lh_math_add(size, overhead);
    return lh_bool_true;
}

lh_void
lh_compress_deflate_blocks_bound(lh_usize_t size, lh_usize_t block_size, lh_usize_t *bound)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_blocks_bound(size, block_size, bound),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_stored_bound(lh_usize_t size, lh_usize_t *bound)
{
    return lh_compress_deflate_try_blocks_bound(size, LH_COMPRESS_DEFLATE_STORED_MAX, bound);
}

lh_void
lh_compress_deflate_stored_bound(lh_usize_t size, lh_usize_t *bound)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_stored_bound(size, bound),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_encode_stored(const lh_byte_t *input, lh_usize_t input_size,
                                      lh_byte_t *output, lh_usize_t capacity, lh_usize_t *written)
{
    lh_usize_t pos = 0U;
    lh_usize_t done = 0U;
    do
    {
        const lh_usize_t chunk =
            lh_math_min(lh_math_sub(input_size, done), LH_COMPRESS_DEFLATE_STORED_MAX);
        const lh_usize_t next = lh_math_add(done, chunk);
        if (!lh_compress_deflate_try_put_stored(input + done, chunk, lh_math_eq(next, input_size),
                                                output, capacity, &pos))
        {
            return lh_bool_false;
        }
        done = next;
    } while (lh_math_lt(done, input_size));
    *written = lh_math_div(pos, 8U);
    return lh_bool_true;
}

lh_void
lh_compress_deflate_encode_stored(const lh_byte_t *input, lh_usize_t input_size, lh_byte_t *output,
                                  lh_usize_t capacity, lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_encode_stored(input, input_size, output, capacity, written),
        lh_runtime_error_code_invalid_range);
}

lh_void
lh_compress_deflate_count_symbols(const lh_compress_deflate_tokens_t *tokens, lh_usize_t *lit_freq,
                                  lh_usize_t *dist_freq)
{
    lh_memory_std_set(lit_freq, 0U,
                      lh_math_mul(LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS, sizeof(*lit_freq)));
    lh_memory_std_set(dist_freq, 0U,
                      lh_math_mul(LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS, sizeof(*dist_freq)));
    const lh_usize_t count = lh_compress_deflate_tokens_get_count(tokens);
    for (lh_usize_t i = 0U; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        if (lh_compress_deflate_tokens_is_literal(tokens, i))
        {
            const lh_uint_t literal = lh_compress_deflate_tokens_get_literal(tokens, i);
            lit_freq[literal] = lh_math_add_one(lit_freq[literal]);
            continue;
        }
        const lh_uint_t symbol = lh_math_add(
            lh_compress_deflate_get_length_index(lh_compress_deflate_tokens_get_length(tokens, i)),
            LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL);
        lit_freq[symbol] = lh_math_add_one(lit_freq[symbol]);
        const lh_uint_t code = lh_compress_deflate_get_distance_code(
            lh_compress_deflate_tokens_get_distance(tokens, i));
        dist_freq[code] = lh_math_add_one(dist_freq[code]);
    }
    lit_freq[LH_COMPRESS_DEFLATE_END_OF_BLOCK] = 1U;
}

lh_usize_t
lh_compress_deflate_get_codes_cost(const lh_byte_t *lit_lengths, const lh_byte_t *dist_lengths,
                                   const lh_usize_t *lit_freq, const lh_usize_t *dist_freq)
{
    lh_usize_t bits = 0U;
    for (lh_uint_t s = 0U; lh_math_lt(s, LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS);
         s = lh_math_add_one(s))
    {
        lh_uint_t cost = lit_lengths[s];
        if (lh_math_ge(s, LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL))
        {
            cost = lh_math_add(cost, lh_compress_deflate_get_length_extra(
                                         lh_math_sub(s, LH_COMPRESS_DEFLATE_FIRST_LENGTH_SYMBOL)));
        }
        bits = lh_math_add(bits, lh_math_mul(lit_freq[s], cost));
    }
    for (lh_uint_t d = 0U; lh_math_lt(d, LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS);
         d = lh_math_add_one(d))
    {
        const lh_uint_t cost =
            lh_math_add(dist_lengths[d], lh_compress_deflate_get_distance_extra(d));
        bits = lh_math_add(bits, lh_math_mul(dist_freq[d], cost));
    }
    return bits;
}

lh_usize_t
lh_compress_deflate_get_stored_cost(lh_usize_t size, lh_usize_t position)
{
    const lh_usize_t header =
        lh_math_add(3U, lh_compress_deflate_get_pad_bits(lh_math_add(position, 3U)));
    return lh_math_add(lh_math_add(header, 32U), lh_math_mul(size, 8U));
}

lh_bool_t
lh_compress_deflate_try_put_tokens(const lh_compress_deflate_tokens_t *tokens,
                                   const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                                   const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                                   lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position)
{
    lh_usize_t pos = *position;
    const lh_usize_t count = lh_compress_deflate_tokens_get_count(tokens);
    for (lh_usize_t i = 0U; lh_math_lt(i, count); i = lh_math_add_one(i))
    {
        if (lh_compress_deflate_tokens_is_literal(tokens, i))
        {
            if (!lh_compress_deflate_try_put_symbol(
                    output, capacity, &pos, lit_codes, lit_lengths,
                    lh_compress_deflate_tokens_get_literal(tokens, i)))
            {
                return lh_bool_false;
            }
            continue;
        }
        if (!lh_compress_deflate_try_put_match(output, capacity, &pos, lit_codes, lit_lengths,
                                               dist_codes, dist_lengths,
                                               lh_compress_deflate_tokens_get_length(tokens, i),
                                               lh_compress_deflate_tokens_get_distance(tokens, i)))
        {
            return lh_bool_false;
        }
    }
    if (!lh_compress_deflate_try_put_symbol(output, capacity, &pos, lit_codes, lit_lengths,
                                            LH_COMPRESS_DEFLATE_END_OF_BLOCK))
    {
        return lh_bool_false;
    }
    *position = pos;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_put_tokens(const lh_compress_deflate_tokens_t *tokens,
                               const lh_u16_t *lit_codes, const lh_byte_t *lit_lengths,
                               const lh_u16_t *dist_codes, const lh_byte_t *dist_lengths,
                               lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_put_tokens(tokens, lit_codes, lit_lengths,
                                                            dist_codes, dist_lengths, output,
                                                            capacity, position),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_collect(lh_compress_deflate_matcher_t *matcher,
                                lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                                lh_usize_t input_size, lh_usize_t begin, lh_usize_t *end)
{
    if (lh_math_gt(input_size, LH_COMPRESS_DEFLATE_MATCHER_INPUT_LIMIT) ||
        lh_math_gt(begin, input_size))
    {
        return lh_bool_false;
    }
    const lh_usize_t limit = lh_math_add(
        begin, lh_math_min(lh_math_sub(input_size, begin), LH_COMPRESS_DEFLATE_STORED_MAX));
    lh_compress_deflate_tokens_reset(tokens);
    lh_usize_t at = begin;
    while (lh_math_lt(at, limit) && !lh_compress_deflate_tokens_is_full(tokens))
    {
        lh_usize_t length = 1U;
        lh_usize_t distance = 0U;
        if (lh_compress_deflate_matcher_try_find(matcher, input, limit, at, &length, &distance))
        {
            lh_compress_deflate_tokens_push_match(tokens, lh_cast_static(lh_uint_t, length),
                                                  lh_cast_static(lh_uint_t, distance));
        }
        else
        {
            lh_compress_deflate_tokens_push_literal(tokens, input[at]);
        }
        for (lh_usize_t left = length; !lh_math_is_zero(left); left = lh_math_sub(left, 1U))
        {
            lh_compress_deflate_matcher_insert(matcher, input, input_size, at);
            at = lh_math_add_one(at);
        }
    }
    *end = at;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_collect(lh_compress_deflate_matcher_t *matcher,
                            lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                            lh_usize_t input_size, lh_usize_t begin, lh_usize_t *end)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_collect(matcher, tokens, input, input_size, begin, end),
        lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_put_block(const lh_compress_deflate_tokens_t *tokens,
                                  const lh_byte_t *input, lh_usize_t input_size, lh_bool_t final,
                                  lh_byte_t *output, lh_usize_t capacity, lh_usize_t *position)
{
    if (lh_math_gt(input_size, LH_COMPRESS_DEFLATE_STORED_MAX))
    {
        return lh_bool_false;
    }
    lh_byte_t lit_lengths[LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES],
        dist_lengths[LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES];
    lh_compress_deflate_fill_fixed_lengths(lit_lengths, dist_lengths);
    lh_usize_t lit_freq[LH_COMPRESS_DEFLATE_LITERAL_SYMBOLS],
        dist_freq[LH_COMPRESS_DEFLATE_DISTANCE_SYMBOLS];
    lh_compress_deflate_count_symbols(tokens, lit_freq, dist_freq);
    const lh_usize_t fixed = lh_math_add(
        3U, lh_compress_deflate_get_codes_cost(lit_lengths, dist_lengths, lit_freq, dist_freq));
    if (lh_math_lt(lh_compress_deflate_get_stored_cost(input_size, *position), fixed))
    {
        return lh_compress_deflate_try_put_stored(input, input_size, final, output, capacity,
                                                  position);
    }
    lh_u16_t lit_codes[LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES],
        dist_codes[LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES];
    lh_compress_deflate_codes(lit_lengths, LH_COMPRESS_DEFLATE_FIXED_LITERAL_CODES, lit_codes);
    lh_compress_deflate_codes(dist_lengths, LH_COMPRESS_DEFLATE_FIXED_DISTANCE_CODES, dist_codes);
    lh_usize_t pos = *position;
    if (!lh_compress_deflate_try_put_bits(output, capacity, &pos, 3U, final ? 3U : 2U) ||
        !lh_compress_deflate_try_put_tokens(tokens, lit_codes, lit_lengths, dist_codes,
                                            dist_lengths, output, capacity, &pos))
    {
        return lh_bool_false;
    }
    *position = pos;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_put_block(const lh_compress_deflate_tokens_t *tokens, const lh_byte_t *input,
                              lh_usize_t input_size, lh_bool_t final, lh_byte_t *output,
                              lh_usize_t capacity, lh_usize_t *position)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_put_block(tokens, input, input_size, final, output,
                                                           capacity, position),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_bound(lh_usize_t size, lh_usize_t *bound)
{
    return lh_compress_deflate_try_blocks_bound(size, LH_COMPRESS_DEFLATE_TOKENS_LIMIT, bound);
}

lh_void
lh_compress_deflate_bound(lh_usize_t size, lh_usize_t *bound)
{
    lh_runtime_check_ifn(lh_compress_deflate_try_bound(size, bound),
                         lh_runtime_error_code_invalid_range);
}

lh_bool_t
lh_compress_deflate_try_encode(lh_compress_deflate_encoder_t *encoder, const lh_byte_t *input,
                               lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                               lh_usize_t *written)
{
    lh_compress_deflate_matcher_t *matcher = lh_compress_deflate_encoder_get_matcher_mut(encoder);
    lh_compress_deflate_tokens_t *tokens = lh_compress_deflate_encoder_get_tokens_mut(encoder);
    lh_compress_deflate_matcher_reset(matcher);
    lh_usize_t pos = 0U;
    lh_usize_t begin = 0U;
    do
    {
        lh_usize_t end;
        if (!lh_compress_deflate_try_collect(matcher, tokens, input, input_size, begin, &end) ||
            !lh_compress_deflate_try_put_block(tokens, input + begin, lh_math_sub(end, begin),
                                               lh_math_eq(end, input_size), output, capacity, &pos))
        {
            return lh_bool_false;
        }
        begin = end;
    } while (lh_math_lt(begin, input_size));
    if (!lh_compress_deflate_try_put_bits(output, capacity, &pos,
                                          lh_compress_deflate_get_pad_bits(pos), 0U))
    {
        return lh_bool_false;
    }
    *written = lh_math_div(pos, 8U);
    return lh_bool_true;
}

lh_void
lh_compress_deflate_encode(lh_compress_deflate_encoder_t *encoder, const lh_byte_t *input,
                           lh_usize_t input_size, lh_byte_t *output, lh_usize_t capacity,
                           lh_usize_t *written)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_try_encode(encoder, input, input_size, output, capacity, written),
        lh_runtime_error_code_invalid_range);
}
