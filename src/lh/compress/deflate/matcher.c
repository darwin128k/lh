/**
 * @file matcher.c
 * @brief DEFLATE hash-chain match finder.
 */

#include <lh/compress/deflate/matcher.h>
#include <lh/memory/std.h>
#include <lh/assert.h>
#include <lh/math.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

lh_usize_t
lh_compress_deflate_matcher_get_chain_limit(const lh_compress_deflate_matcher_t *self)
{
    lh_assert_runtime_ref(self);
    return self->chain_limit;
}

lh_void
lh_compress_deflate_matcher_set_chain_limit(lh_compress_deflate_matcher_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->chain_limit = value;
}

const lh_u32_t *
lh_compress_deflate_matcher_get_head(const lh_compress_deflate_matcher_t *self)
{
    lh_assert_runtime_ref(self);
    return self->head;
}

lh_u32_t *
lh_compress_deflate_matcher_get_head_mut(lh_compress_deflate_matcher_t *self)
{
    lh_assert_runtime_ref(self);
    return self->head;
}

const lh_u32_t *
lh_compress_deflate_matcher_get_prev(const lh_compress_deflate_matcher_t *self)
{
    lh_assert_runtime_ref(self);
    return self->prev;
}

lh_u32_t *
lh_compress_deflate_matcher_get_prev_mut(lh_compress_deflate_matcher_t *self)
{
    lh_assert_runtime_ref(self);
    return self->prev;
}

lh_usize_t
lh_compress_deflate_matcher_get_hash(const lh_byte_t *at)
{
    const lh_usize_t value =
        lh_bit_xor(lh_bit_xor(lh_bit_shl(lh_cast_static(lh_usize_t, at[0]), 10U),
                              lh_bit_shl(lh_cast_static(lh_usize_t, at[1]), 5U)),
                   lh_cast_static(lh_usize_t, at[2]));
    return lh_bit_and(value, lh_math_sub(LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE, 1U));
}

lh_usize_t
lh_compress_deflate_matcher_get_match_size(const lh_byte_t *input, lh_usize_t size,
                                           lh_usize_t candidate, lh_usize_t position)
{
    const lh_usize_t limit =
        lh_math_min(lh_math_sub(size, position), LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH);
    const lh_ptr diff = lh_memory_std_compare(input + candidate, input + position, limit);
    if (lh_null_eq(diff))
    {
        return limit;
    }
    return lh_addr_diff(lh_ptr_to_uaddr(diff), lh_ptr_to_uaddr(input + candidate));
}

lh_void
lh_compress_deflate_matcher_reset(lh_compress_deflate_matcher_t *self)
{
    lh_memory_std_set(lh_compress_deflate_matcher_get_head_mut(self), 0U,
                      LH_COMPRESS_DEFLATE_MATCHER_HASH_SIZE * sizeof(lh_u32_t));
}

lh_void
lh_compress_deflate_matcher_insert(lh_compress_deflate_matcher_t *self, const lh_byte_t *input,
                                   lh_usize_t size, lh_usize_t position)
{
    if (lh_math_lt(lh_math_sub(size, position), LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH))
    {
        return;
    }
    lh_u32_t *head = lh_compress_deflate_matcher_get_head_mut(self);
    const lh_usize_t hash = lh_compress_deflate_matcher_get_hash(input + position);
    const lh_usize_t slot = lh_bit_and(position, lh_math_sub(LH_COMPRESS_DEFLATE_WINDOW_SIZE, 1U));
    lh_compress_deflate_matcher_get_prev_mut(self)[slot] = head[hash];
    head[hash] = lh_cast_static(lh_u32_t, lh_math_add_one(position));
}

lh_bool_t
lh_compress_deflate_matcher_try_find(const lh_compress_deflate_matcher_t *self,
                                     const lh_byte_t *input, lh_usize_t size, lh_usize_t position,
                                     lh_usize_t *length, lh_usize_t *distance)
{
    if (lh_math_lt(lh_math_sub(size, position), LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH))
    {
        return lh_bool_false;
    }
    const lh_u32_t *prev = lh_compress_deflate_matcher_get_prev(self);
    const lh_usize_t limit = lh_compress_deflate_matcher_get_chain_limit(self);
    lh_usize_t entry = lh_compress_deflate_matcher_get_head(
        self)[lh_compress_deflate_matcher_get_hash(input + position)];
    lh_usize_t best_length = 0U;
    lh_usize_t best_distance = 0U;
    for (lh_usize_t step = 0U; lh_math_lt(step, limit) && !lh_math_is_zero(entry);
         step = lh_math_add_one(step))
    {
        const lh_usize_t candidate = lh_math_sub(entry, 1U);
        const lh_usize_t back = lh_math_sub(position, candidate);
        if (lh_math_ge(candidate, position) || lh_math_gt(back, LH_COMPRESS_DEFLATE_WINDOW_SIZE))
        {
            break;
        }
        const lh_usize_t found =
            lh_compress_deflate_matcher_get_match_size(input, size, candidate, position);
        if (lh_math_gt(found, best_length))
        {
            best_length = found;
            best_distance = back;
            if (lh_math_eq(found, LH_COMPRESS_DEFLATE_MATCHER_MAX_MATCH))
            {
                break;
            }
        }
        const lh_usize_t next =
            prev[lh_bit_and(candidate, lh_math_sub(LH_COMPRESS_DEFLATE_WINDOW_SIZE, 1U))];
        if (lh_math_ge(next, entry))
        {
            break;
        }
        entry = next;
    }
    if (lh_math_lt(best_length, LH_COMPRESS_DEFLATE_MATCHER_MIN_MATCH))
    {
        return lh_bool_false;
    }
    *length = best_length;
    *distance = best_distance;
    return lh_bool_true;
}

lh_void
lh_compress_deflate_matcher_init(lh_compress_deflate_matcher_t *self, lh_usize_t chain_limit)
{
    lh_compress_deflate_matcher_set_chain_limit(self, chain_limit);
    lh_compress_deflate_matcher_reset(self);
}

lh_void
lh_compress_deflate_matcher_find(const lh_compress_deflate_matcher_t *self, const lh_byte_t *input,
                                 lh_usize_t size, lh_usize_t position, lh_usize_t *length,
                                 lh_usize_t *distance)
{
    lh_runtime_check_ifn(
        lh_compress_deflate_matcher_try_find(self, input, size, position, length, distance),
        lh_runtime_error_code_invalid_range);
}
