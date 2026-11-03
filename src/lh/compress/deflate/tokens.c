/**
 * @file tokens.c
 * @brief DEFLATE block token buffer.
 */

#include <lh/compress/deflate/tokens.h>
#include <lh/compress/deflate/matcher.h>
#include <lh/assert.h>
#include <lh/math.h>
lh_usize_t
lh_compress_deflate_tokens_get_count(const lh_compress_deflate_tokens_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_compress_deflate_tokens_set_count(lh_compress_deflate_tokens_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->count = value;
}

const lh_byte_t *
lh_compress_deflate_tokens_get_values(const lh_compress_deflate_tokens_t *self)
{
    lh_assert_runtime_ref(self);
    return self->values;
}

lh_byte_t *
lh_compress_deflate_tokens_get_values_mut(lh_compress_deflate_tokens_t *self)
{
    lh_assert_runtime_ref(self);
    return self->values;
}

const lh_u16_t *
lh_compress_deflate_tokens_get_distances(const lh_compress_deflate_tokens_t *self)
{
    lh_assert_runtime_ref(self);
    return self->distances;
}

lh_u16_t *
lh_compress_deflate_tokens_get_distances_mut(lh_compress_deflate_tokens_t *self)
{
    lh_assert_runtime_ref(self);
    return self->distances;
}

lh_void
lh_compress_deflate_tokens_reset(lh_compress_deflate_tokens_t *self)
{
    lh_compress_deflate_tokens_set_count(self, 0U);
}

lh_bool_t
lh_compress_deflate_tokens_is_full(const lh_compress_deflate_tokens_t *self)
{
    return lh_math_ge(lh_compress_deflate_tokens_get_count(self), LH_COMPRESS_DEFLATE_TOKENS_LIMIT);
}

lh_uint_t
lh_compress_deflate_tokens_get_distance(const lh_compress_deflate_tokens_t *self, lh_usize_t index)
{
    return lh_compress_deflate_tokens_get_distances(self)[index];
}

lh_uint_t
lh_compress_deflate_tokens_get_literal(const lh_compress_deflate_tokens_t *self, lh_usize_t index)
{
    return lh_compress_deflate_tokens_get_values(self)[index];
}

lh_uint_t
lh_compress_deflate_tokens_get_length(const lh_compress_deflate_tokens_t *self, lh_usize_t index)
{
    return lh_math_add(lh_compress_deflate_tokens_get_values(self)[index],
                       LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH);
}

lh_bool_t
lh_compress_deflate_tokens_is_literal(const lh_compress_deflate_tokens_t *self, lh_usize_t index)
{
    return lh_math_is_zero(lh_compress_deflate_tokens_get_distance(self, index));
}

lh_bool_t
lh_compress_deflate_tokens_try_push_literal(lh_compress_deflate_tokens_t *self, lh_byte_t literal)
{
    if (lh_compress_deflate_tokens_is_full(self))
    {
        return lh_bool_false;
    }
    const lh_usize_t count = lh_compress_deflate_tokens_get_count(self);
    lh_compress_deflate_tokens_get_values_mut(self)[count] = literal;
    lh_compress_deflate_tokens_get_distances_mut(self)[count] = 0U;
    lh_compress_deflate_tokens_set_count(self, lh_math_add_one(count));
    return lh_bool_true;
}

lh_bool_t
lh_compress_deflate_tokens_try_push_match(lh_compress_deflate_tokens_t *self, lh_uint_t length,
                                          lh_uint_t distance)
{
    if (lh_compress_deflate_tokens_is_full(self) ||
        lh_math_lt(length, LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH) ||
        lh_math_gt(length, LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH) || lh_math_is_zero(distance) ||
        lh_math_gt(distance, LH_COMPRESS_DEFLATE_WINDOW_SIZE))
    {
        return lh_bool_false;
    }
    const lh_usize_t count = lh_compress_deflate_tokens_get_count(self);
    lh_compress_deflate_tokens_get_values_mut(self)[count] =
        lh_cast_static(lh_byte_t, lh_math_sub(length, LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH));
    lh_compress_deflate_tokens_get_distances_mut(self)[count] = lh_cast_static(lh_u16_t, distance);
    lh_compress_deflate_tokens_set_count(self, lh_math_add_one(count));
    return lh_bool_true;
}

lh_void
lh_compress_deflate_tokens_push_literal(lh_compress_deflate_tokens_t *self, lh_byte_t literal)
{
    lh_runtime_check_ifn(lh_compress_deflate_tokens_try_push_literal(self, literal),
                         lh_runtime_error_code_invalid_range);
}

lh_void
lh_compress_deflate_tokens_push_match(lh_compress_deflate_tokens_t *self, lh_uint_t length,
                                      lh_uint_t distance)
{
    lh_runtime_check_ifn(lh_compress_deflate_tokens_try_push_match(self, length, distance),
                         lh_runtime_error_code_invalid_range);
}
