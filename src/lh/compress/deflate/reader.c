/**
 * @file reader.c
 * @brief Incremental raw DEFLATE reader.
 */

#include <lh/compress/deflate/reader.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>
#include <lh/size/limits.h>

lh_io_reader_t *
lh_compress_deflate_reader_get_source(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->source;
}

lh_void
lh_compress_deflate_reader_set_source(lh_compress_deflate_reader_t *self, lh_io_reader_t *value)
{
    lh_assert_runtime_ref(self);
    self->source = value;
}

lh_byte_t *
lh_compress_deflate_reader_get_input(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input;
}

lh_void
lh_compress_deflate_reader_set_input(lh_compress_deflate_reader_t *self, lh_byte_t *value)
{
    lh_assert_runtime_ref(self);
    self->input = value;
}

lh_usize_t
lh_compress_deflate_reader_get_input_capacity(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input_capacity;
}

lh_void
lh_compress_deflate_reader_set_input_capacity(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->input_capacity = value;
}

lh_usize_t
lh_compress_deflate_reader_get_input_size(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->input_size;
}

lh_void
lh_compress_deflate_reader_set_input_size(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->input_size = value;
}

lh_usize_t
lh_compress_deflate_reader_get_bit_position(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bit_position;
}

lh_void
lh_compress_deflate_reader_set_bit_position(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->bit_position = value;
}

lh_byte_t *
lh_compress_deflate_reader_get_history(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->history;
}

lh_void
lh_compress_deflate_reader_set_history(lh_compress_deflate_reader_t *self, lh_byte_t *value)
{
    lh_assert_runtime_ref(self);
    self->history = value;
}

lh_usize_t
lh_compress_deflate_reader_get_history_position(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->history_position;
}

lh_void
lh_compress_deflate_reader_set_history_position(lh_compress_deflate_reader_t *self,
                                                lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->history_position = value;
}

lh_usize_t
lh_compress_deflate_reader_get_history_size(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->history_size;
}

lh_void
lh_compress_deflate_reader_set_history_size(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->history_size = value;
}

lh_usize_t
lh_compress_deflate_reader_get_decoded(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->decoded;
}

lh_void
lh_compress_deflate_reader_set_decoded(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->decoded = value;
}

lh_usize_t
lh_compress_deflate_reader_get_limit(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->limit;
}

lh_void
lh_compress_deflate_reader_set_limit(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->limit = value;
}

lh_usize_t
lh_compress_deflate_reader_get_remaining(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->remaining;
}

lh_void
lh_compress_deflate_reader_set_remaining(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->remaining = value;
}

lh_usize_t
lh_compress_deflate_reader_get_distance(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->distance;
}

lh_void
lh_compress_deflate_reader_set_distance(lh_compress_deflate_reader_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->distance = value;
}

lh_bool_t
lh_compress_deflate_reader_get_final(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->final;
}

lh_void
lh_compress_deflate_reader_set_final(lh_compress_deflate_reader_t *self, lh_bool_t value)
{
    lh_assert_runtime_ref(self);
    self->final = value;
}

lh_compress_deflate_reader_phase_t
lh_compress_deflate_reader_get_phase(const lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->phase;
}

lh_void
lh_compress_deflate_reader_set_phase(lh_compress_deflate_reader_t *self,
                                     lh_compress_deflate_reader_phase_t value)
{
    lh_assert_runtime_ref(self);
    self->phase = value;
}

lh_u16_t *
lh_compress_deflate_reader_get_lit_counts(lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lit_counts;
}

lh_u16_t *
lh_compress_deflate_reader_get_lit_symbols(lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->lit_symbols;
}

lh_u16_t *
lh_compress_deflate_reader_get_dist_counts(lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dist_counts;
}

lh_u16_t *
lh_compress_deflate_reader_get_dist_symbols(lh_compress_deflate_reader_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dist_symbols;
}

lh_void
lh_compress_deflate_reader_init(lh_compress_deflate_reader_t *self, lh_io_reader_t *source,
                                lh_byte_t *input, lh_usize_t input_capacity, lh_byte_t *history,
                                lh_usize_t limit)
{
    lh_compress_deflate_reader_set_source(self, source);
    lh_compress_deflate_reader_set_input(self, input);
    lh_compress_deflate_reader_set_input_capacity(
        self, lh_math_min(input_capacity, LH_COMPRESS_DEFLATE_READER_INPUT_SIZE));
    lh_compress_deflate_reader_set_input_size(self, 0U);
    lh_compress_deflate_reader_set_bit_position(self, 0U);
    lh_compress_deflate_reader_set_history(self, history);
    lh_compress_deflate_reader_set_history_position(self, 0U);
    lh_compress_deflate_reader_set_history_size(self, 0U);
    lh_compress_deflate_reader_set_decoded(self, 0U);
    lh_compress_deflate_reader_set_limit(self, limit);
    lh_compress_deflate_reader_set_remaining(self, 0U);
    lh_compress_deflate_reader_set_distance(self, 0U);
    lh_compress_deflate_reader_set_final(self, lh_bool_false);
    lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_block);
}

lh_void
lh_compress_deflate_reader_compact(lh_compress_deflate_reader_t *self)
{
    const lh_usize_t bit = lh_compress_deflate_reader_get_bit_position(self);
    const lh_usize_t bytes = lh_math_div(bit, 8U);
    if (!lh_math_is_zero(bytes))
    {
        const lh_usize_t rest = lh_math_sub(lh_compress_deflate_reader_get_input_size(self), bytes);
        if (!lh_math_is_zero(rest))
        {
            lh_memory_std_move(lh_compress_deflate_reader_get_input(self),
                               lh_compress_deflate_reader_get_input(self) + bytes, rest);
        }
        lh_compress_deflate_reader_set_input_size(self, rest);
        lh_compress_deflate_reader_set_bit_position(self, lh_math_mod(bit, 8U));
    }
}

lh_ssize_t
lh_compress_deflate_reader_try_refill(lh_compress_deflate_reader_t *self)
{
    lh_compress_deflate_reader_compact(self);
    const lh_usize_t size = lh_compress_deflate_reader_get_input_size(self);
    if (lh_math_ge(size, lh_compress_deflate_reader_get_input_capacity(self)))
    {
        return LH_IO_ERROR;
    }
    const lh_ssize_t got = lh_io_reader_read(lh_compress_deflate_reader_get_source(self),
                                             lh_compress_deflate_reader_get_input(self) + size, 1U);
    if (lh_math_eq(got, 1))
    {
        lh_compress_deflate_reader_set_input_size(self, lh_math_add_one(size));
        return 1;
    }
    return lh_math_eq(got, LH_IO_PENDING) ? LH_IO_PENDING : LH_IO_ERROR;
}

lh_void
lh_compress_deflate_reader_emit(lh_compress_deflate_reader_t *self, lh_byte_t *output,
                                lh_byte_t value)
{
    *output = value;
    const lh_usize_t head = lh_compress_deflate_reader_get_history_position(self);
    lh_compress_deflate_reader_get_history(self)[head] = value;
    lh_compress_deflate_reader_set_history_position(
        self, lh_math_mod(lh_math_add_one(head), LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE));
    const lh_usize_t used = lh_compress_deflate_reader_get_history_size(self);
    if (lh_math_lt(used, LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE))
    {
        lh_compress_deflate_reader_set_history_size(self, lh_math_add_one(used));
    }
    lh_compress_deflate_reader_set_decoded(
        self, lh_math_add_one(lh_compress_deflate_reader_get_decoded(self)));
}

lh_usize_t
lh_compress_deflate_reader_get_available_bits(const lh_compress_deflate_reader_t *self)
{
    return lh_math_sub(lh_math_mul(lh_compress_deflate_reader_get_input_size(self), 8U),
                       lh_compress_deflate_reader_get_bit_position(self));
}

lh_ssize_t
lh_compress_deflate_reader_try_step_block(lh_compress_deflate_reader_t *self)
{
    lh_usize_t pos = lh_compress_deflate_reader_get_bit_position(self);
    lh_uint_t header;
    if (!lh_compress_deflate_try_bits(lh_compress_deflate_reader_get_input(self),
                                      lh_compress_deflate_reader_get_input_size(self), &pos, 3U,
                                      &header))
    {
        return LH_IO_PENDING;
    }
    const lh_uint_t type = lh_bit_shr(header, 1U);
    if (lh_math_gt(type, 2U))
    {
        return LH_IO_ERROR;
    }
    if (lh_math_eq(type, 1U) &&
        !lh_compress_deflate_try_fixed(lh_compress_deflate_reader_get_lit_counts(self),
                                       lh_compress_deflate_reader_get_lit_symbols(self),
                                       lh_compress_deflate_reader_get_dist_counts(self),
                                       lh_compress_deflate_reader_get_dist_symbols(self)))
    {
        return LH_IO_ERROR;
    }
    lh_compress_deflate_reader_set_final(self, !lh_math_is_zero(lh_bit_and(header, 1U)));
    lh_compress_deflate_reader_set_bit_position(self, pos);
    lh_compress_deflate_reader_set_phase(
        self, lh_math_is_zero(type)
                  ? lh_compress_deflate_reader_phase_stored_header
                  : (lh_math_eq(type, 1U) ? lh_compress_deflate_reader_phase_codes
                                          : lh_compress_deflate_reader_phase_dynamic));
    return 0;
}

lh_ssize_t
lh_compress_deflate_reader_try_step_stored_header(lh_compress_deflate_reader_t *self)
{
    lh_usize_t pos = lh_compress_deflate_reader_get_bit_position(self);
    lh_uint_t length;
    if (!lh_compress_deflate_try_stored_header(lh_compress_deflate_reader_get_input(self),
                                               lh_compress_deflate_reader_get_input_size(self),
                                               &pos, &length))
    {
        const lh_usize_t bit = lh_compress_deflate_reader_get_bit_position(self);
        const lh_usize_t alignment = lh_compress_deflate_get_pad_bits(bit);
        return lh_math_ge(lh_compress_deflate_reader_get_available_bits(self),
                          lh_math_add(alignment, 32U))
                   ? LH_IO_ERROR
                   : LH_IO_PENDING;
    }
    lh_compress_deflate_reader_set_bit_position(self, pos);
    lh_compress_deflate_reader_set_remaining(self, length);
    lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_stored);
    return 0;
}

lh_ssize_t
lh_compress_deflate_reader_try_step_stored(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_usize_t remaining = lh_compress_deflate_reader_get_remaining(self);
    if (lh_math_is_zero(remaining))
    {
        lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_get_final(self)
                                                       ? lh_compress_deflate_reader_phase_done
                                                       : lh_compress_deflate_reader_phase_block);
        return 0;
    }
    if (lh_math_ge(lh_compress_deflate_reader_get_decoded(self),
                   lh_compress_deflate_reader_get_limit(self)))
    {
        return LH_IO_ERROR;
    }
    lh_usize_t pos = lh_compress_deflate_reader_get_bit_position(self);
    lh_uint_t byte;
    if (!lh_compress_deflate_try_bits(lh_compress_deflate_reader_get_input(self),
                                      lh_compress_deflate_reader_get_input_size(self), &pos, 8U,
                                      &byte))
    {
        return LH_IO_PENDING;
    }
    lh_compress_deflate_reader_set_bit_position(self, pos);
    lh_compress_deflate_reader_set_remaining(self, lh_math_sub_one(remaining));
    lh_compress_deflate_reader_emit(self, output, lh_cast_static(lh_byte_t, byte));
    return 1;
}

lh_ssize_t
lh_compress_deflate_reader_step_dynamic(lh_compress_deflate_reader_t *self)
{
    lh_usize_t pos = lh_compress_deflate_reader_get_bit_position(self);
    if (!lh_compress_deflate_try_dynamic(lh_compress_deflate_reader_get_input(self),
                                         lh_compress_deflate_reader_get_input_size(self), &pos,
                                         lh_compress_deflate_reader_get_lit_counts(self),
                                         lh_compress_deflate_reader_get_lit_symbols(self),
                                         lh_compress_deflate_reader_get_dist_counts(self),
                                         lh_compress_deflate_reader_get_dist_symbols(self)))
    {
        return LH_IO_PENDING;
    }
    lh_compress_deflate_reader_set_bit_position(self, pos);
    lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_codes);
    return 0;
}

lh_ssize_t
lh_compress_deflate_reader_try_step_codes(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    lh_usize_t pos = lh_compress_deflate_reader_get_bit_position(self);
    lh_uint_t symbol;
    if (!lh_compress_deflate_try_symbol(lh_compress_deflate_reader_get_input(self),
                                        lh_compress_deflate_reader_get_input_size(self), &pos,
                                        lh_compress_deflate_reader_get_lit_counts(self),
                                        lh_compress_deflate_reader_get_lit_symbols(self), &symbol))
    {
        return lh_math_ge(lh_compress_deflate_reader_get_available_bits(self), 15U) ? LH_IO_ERROR
                                                                                    : LH_IO_PENDING;
    }
    if (lh_math_lt(symbol, 256U))
    {
        if (lh_math_ge(lh_compress_deflate_reader_get_decoded(self),
                       lh_compress_deflate_reader_get_limit(self)))
        {
            return LH_IO_ERROR;
        }
        lh_compress_deflate_reader_set_bit_position(self, pos);
        lh_compress_deflate_reader_emit(self, output, lh_cast_static(lh_byte_t, symbol));
        return 1;
    }
    if (lh_math_eq(symbol, 256U))
    {
        lh_compress_deflate_reader_set_bit_position(self, pos);
        lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_get_final(self)
                                                       ? lh_compress_deflate_reader_phase_done
                                                       : lh_compress_deflate_reader_phase_block);
        return 0;
    }
    if (lh_math_gt(symbol, 285U))
    {
        return LH_IO_ERROR;
    }
    lh_uint_t length, distance;
    if (!lh_compress_deflate_try_match(lh_compress_deflate_reader_get_input(self),
                                       lh_compress_deflate_reader_get_input_size(self), &pos,
                                       symbol, lh_compress_deflate_reader_get_dist_counts(self),
                                       lh_compress_deflate_reader_get_dist_symbols(self), &length,
                                       &distance))
    {
        return lh_math_ge(lh_compress_deflate_reader_get_available_bits(self), 48U) ? LH_IO_ERROR
                                                                                    : LH_IO_PENDING;
    }
    if (lh_math_gt(distance, lh_compress_deflate_reader_get_history_size(self)) ||
        lh_math_gt(length, lh_math_sub(lh_compress_deflate_reader_get_limit(self),
                                       lh_compress_deflate_reader_get_decoded(self))))
    {
        return LH_IO_ERROR;
    }
    lh_compress_deflate_reader_set_bit_position(self, pos);
    lh_compress_deflate_reader_set_remaining(self, length);
    lh_compress_deflate_reader_set_distance(self, distance);
    lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_match);
    return 0;
}

lh_ssize_t
lh_compress_deflate_reader_step_match(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_usize_t remaining = lh_compress_deflate_reader_get_remaining(self);
    if (lh_math_is_zero(remaining))
    {
        lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_codes);
        return 0;
    }
    const lh_usize_t index =
        lh_math_mod(lh_math_sub(lh_math_add(lh_compress_deflate_reader_get_history_position(self),
                                            LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE),
                                lh_compress_deflate_reader_get_distance(self)),
                    LH_COMPRESS_DEFLATE_READER_HISTORY_SIZE);
    lh_compress_deflate_reader_emit(self, output,
                                    lh_compress_deflate_reader_get_history(self)[index]);
    lh_compress_deflate_reader_set_remaining(self, lh_math_sub_one(remaining));
    return 1;
}

lh_ssize_t
lh_compress_deflate_reader_try_step(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_compress_deflate_reader_phase_t phase = lh_compress_deflate_reader_get_phase(self);
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_block))
    {
        return lh_compress_deflate_reader_try_step_block(self);
    }
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_stored_header))
    {
        return lh_compress_deflate_reader_try_step_stored_header(self);
    }
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_stored))
    {
        return lh_compress_deflate_reader_try_step_stored(self, output);
    }
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_dynamic))
    {
        return lh_compress_deflate_reader_step_dynamic(self);
    }
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_codes))
    {
        return lh_compress_deflate_reader_try_step_codes(self, output);
    }
    if (lh_math_eq(phase, lh_compress_deflate_reader_phase_match))
    {
        return lh_compress_deflate_reader_step_match(self, output);
    }
    return lh_math_eq(phase, lh_compress_deflate_reader_phase_done) ? 0 : LH_IO_ERROR;
}

lh_ssize_t
lh_compress_deflate_reader_read(lh_self_ptr context, lh_ptr output, lh_usize_t size)
{
    lh_compress_deflate_reader_t *self = lh_ptr_cast(lh_compress_deflate_reader_t, context);
    lh_byte_t *out = lh_ptr_cast(lh_byte_t, output);
    const lh_usize_t capacity = lh_math_min(size, lh_cast_static(lh_usize_t, LH_SSIZE_T_MAX));
    lh_usize_t written = 0U;
    while (lh_math_lt(written, capacity) && lh_math_ne(lh_compress_deflate_reader_get_phase(self),
                                                       lh_compress_deflate_reader_phase_done))
    {
        lh_ssize_t result = lh_compress_deflate_reader_try_step(self, out + written);
        if (lh_math_eq(result, 1))
        {
            written = lh_math_add_one(written);
            continue;
        }
        if (lh_math_eq(result, LH_IO_PENDING))
        {
            result = lh_compress_deflate_reader_try_refill(self);
        }
        if (lh_math_lt(result, 0))
        {
            if (lh_math_ne(result, LH_IO_PENDING))
            {
                lh_compress_deflate_reader_set_phase(self, lh_compress_deflate_reader_phase_error);
            }
            return lh_math_is_zero(written) ? result : lh_cast_static(lh_ssize_t, written);
        }
    }
    return lh_cast_static(lh_ssize_t, written);
}

lh_void
lh_compress_deflate_reader_get_reader(lh_compress_deflate_reader_t *self, lh_io_reader_t *output)
{
    lh_io_reader_init(output, lh_compress_deflate_reader_read, self);
}

lh_ssize_t
lh_compress_deflate_reader_refill(lh_compress_deflate_reader_t *self)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_refill(self);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_deflate_reader_step_block(lh_compress_deflate_reader_t *self)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_step_block(self);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_deflate_reader_step_stored_header(lh_compress_deflate_reader_t *self)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_step_stored_header(self);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_deflate_reader_step_stored(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_step_stored(self, output);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_deflate_reader_step_codes(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_step_codes(self, output);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}

lh_ssize_t
lh_compress_deflate_reader_step(lh_compress_deflate_reader_t *self, lh_byte_t *output)
{
    const lh_ssize_t result = lh_compress_deflate_reader_try_step(self, output);
    lh_runtime_check_ifn(lh_math_ge(result, 0) || lh_math_eq(result, LH_IO_PENDING),
                         lh_runtime_error_code_invalid_range);
    return result;
}
