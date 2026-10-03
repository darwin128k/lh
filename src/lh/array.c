#include <lh/array.h>
#include <lh/memory.h>
#include <lh/memory/std.h>
#include <lh/math.h>
#include <lh/util/return.h>
#include <lh/util/ptr.h>
#include <lh/config.h>
#include <lh/assert.h>

lh_memory_typed_allocated_t *
lh_array_get_typed(lh_array_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->typed);
}

const lh_memory_typed_allocated_t *
lh_array_get_typed_as_const(const lh_array_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->typed);
}

lh_usize_t
lh_array_get_capacity(const lh_array_t *self)
{
    return lh_memory_typed_get_size(lh_array_get_typed_as_const(self));
}

lh_usize_t
lh_array_get_size(const lh_array_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_usize_t
lh_array_get_type_size(const lh_array_t *self)
{
    return lh_memory_typed_get_type_size(lh_array_get_typed_as_const(self));
}

lh_ptr
lh_array_get_begin(const lh_array_t *self)
{
    return lh_memory_typed_get_begin(lh_array_get_typed_as_const(self));
}

lh_ptr
lh_array_get_data(const lh_array_t *self)
{
    return lh_array_get_begin(self);
}

lh_ptr
lh_array_get_end(const lh_array_t *self)
{
    const lh_usize_t used_bytes =
        lh_math_mul(lh_array_get_size(self), lh_array_get_type_size(self));
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_array_get_begin(self), used_bytes);
}

lh_bool_t
lh_array_is_empty(const lh_array_t *self)
{
    return lh_math_is_zero(lh_array_get_size(self));
}

lh_void
lh_array_clear(lh_array_t *self)
{
    lh_assert_runtime_ref(self);
    self->size = 0;
}

lh_void
lh_array_init(lh_array_t *self, lh_usize_t type_size)
{
    lh_memory_typed_init_empty(lh_array_get_typed(self), type_size);
    self->size = 0;
}

lh_void
lh_array_deinit(lh_array_t *self)
{
    lh_memory_typed_allocated_clear(lh_array_get_typed(self));
    self->size = 0;
}

lh_void
lh_array_reserve(lh_array_t *self, lh_usize_t min_capacity)
{
    lh_memory_typed_allocated_t *typed = lh_array_get_typed(self);
    lh_return_if(lh_memory_typed_get_size(typed) >= min_capacity);
    lh_memory_typed_allocated_resize(typed, min_capacity);
}

lh_usize_t
lh_array_get_grown_capacity(lh_usize_t capacity, lh_usize_t min_capacity)
{
    lh_return_if(capacity >= min_capacity, capacity);

    const lh_usize_t policy_capacity =
        lh_math_is_zero(capacity) ? LH_LIBRARY_OPTION_ARRAY_INITIAL_CAPACITY
                                  : lh_math_mul(capacity, LH_LIBRARY_OPTION_ARRAY_GROWTH_FACTOR);
    return lh_math_max(policy_capacity, min_capacity);
}

lh_void
lh_array_insert_of(lh_array_t *self, lh_uindex_t index, const lh_ptr values, lh_usize_t count)
{
    const lh_usize_t size = lh_array_get_size(self);
    lh_assert_runtime_ifn(index <= size, lh_runtime_error_code_out_of_range);
    lh_return_if(lh_math_is_zero(count));

    const lh_usize_t new_size = lh_math_add(size, count);

    const lh_usize_t capacity = lh_array_get_capacity(self);
    if (capacity < new_size)
    {
        lh_array_reserve(self, lh_array_get_grown_capacity(capacity, new_size));
    }

    if (index < size)
    {
        lh_memory_typed_move_within(lh_addr_of(self->typed), lh_math_add(index, count), index,
                                    lh_math_sub(size, index));
    }

    lh_memory_typed_set_values(lh_addr_of(self->typed), index, values, count);

    self->size = new_size;
}

lh_uindex_t
lh_array_push_back_of(lh_array_t *self, const lh_ptr values, lh_usize_t count)
{
    const lh_usize_t index = lh_array_get_size(self);
    lh_array_insert_of(self, index, values, count);
    return index;
}

lh_uindex_t
lh_array_push_back(lh_array_t *self, const lh_ptr value)
{
    /* self not re-checked here: lh_array_get_size(self) right below does it.
     * value still needs its own check — nothing downstream validates it. */
    lh_assert_runtime_ref(value);

    /*
     * Fast path for the common case (room already reserved): avoid
     * lh_array_push_back_of / lh_array_insert_of's generic machinery, which
     * re-derives capacity through lh_memory_typed_get_size (and, through it,
     * validity + a division) several times over for a single element. Every
     * value used below still comes from the public, bounds-checked accessors
     * — this only removes the redundant re-derivation, not the checks
     * themselves. Falls back to the general path whenever growth is needed.
     */
    const lh_usize_t size = lh_array_get_size(self);
    const lh_usize_t capacity = lh_array_get_capacity(self);

    if (size >= capacity)
    {
        return lh_array_push_back_of(self, value, 1);
    }

    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_ptr dst = lh_ptr_add_by_offset_unsafe(lh_void, lh_array_get_begin(self),
                                             lh_math_mul(size, type_size));
    lh_memory_std_copy(dst, value, type_size);
    self->size = lh_math_add_one(size);
    return size;
}

lh_bool_t
lh_array_is_valid_index(const lh_array_t *self, lh_uindex_t index)
{
    return index < lh_array_get_size(self);
}

lh_ptr
lh_array_get_ptr(const lh_array_t *self, lh_uindex_t index)
{
    /* lh_memory_typed_get_ptr_from_begin would re-validate index against the typed block's
     * own capacity via lh_memory_typed_get_size (division + a validity check of its own) —
     * redundant, since lh_array_is_valid_index (a plain field compare against self->size)
     * already guarantees index < size <= capacity. Compute the pointer directly instead. */
    lh_assert_runtime_ifn(lh_array_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_array_get_begin(self),
                                       lh_math_mul(index, lh_array_get_type_size(self)));
}

lh_void
lh_array_pop_back(lh_array_t *self, lh_ptr dst)
{
    lh_assert_runtime_if(lh_array_is_empty(self), lh_runtime_error_code_out_of_range);

    const lh_usize_t last_index = lh_math_sub_one(lh_array_get_size(self));
    if (lh_ptr_is_set(dst))
    {
        lh_memory_typed_get_value_into(lh_addr_of(self->typed), last_index, dst);
    }
    self->size = last_index;
}

lh_void
lh_array_insert(lh_array_t *self, lh_uindex_t index, const lh_ptr value)
{
    lh_array_insert_of(self, index, value, 1);
}

lh_void
lh_array_erase(lh_array_t *self, lh_uindex_t index, lh_ptr dst)
{
    lh_assert_runtime_ifn(lh_array_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);

    if (lh_ptr_is_set(dst))
    {
        lh_memory_typed_get_value_into(lh_addr_of(self->typed), index, dst);
    }

    lh_array_erase_of(self, index, 1);
}

lh_void
lh_array_erase_of(lh_array_t *self, lh_uindex_t index, lh_usize_t count)
{
    const lh_usize_t size = lh_array_get_size(self);
    lh_assert_runtime_ifn(index <= size && count <= lh_math_sub(size, index),
                          lh_runtime_error_code_out_of_range);
    lh_return_if(lh_math_is_zero(count));

    const lh_uindex_t tail_index = lh_math_add(index, count);
    const lh_usize_t tail_count = lh_math_sub(size, tail_index);
    if (lh_math_is_positive(tail_count))
    {
        lh_memory_typed_move_within(lh_addr_of(self->typed), index, tail_index, tail_count);
    }

    self->size = lh_math_sub(size, count);
}

lh_void
lh_array_resize(lh_array_t *self, lh_usize_t n)
{
    if (n > lh_array_get_size(self))
    {
        lh_array_reserve(self, n);
    }
    lh_assert_runtime_ref(self);
    self->size = n;
}

lh_void
lh_array_assign(lh_array_t *self, const lh_array_t *other)
{
    lh_assert_runtime_ref(other);
    if (self == other)
    {
        return;
    }
    lh_assert_runtime_if(lh_array_get_type_size(self) != lh_array_get_type_size(other),
                         lh_runtime_error_code_invalid_argument);
    lh_array_clear(self);
    lh_array_push_back_of(self, lh_array_get_data(other), lh_array_get_size(other));
}

lh_void
lh_array_append(lh_array_t *self, const lh_array_t *other)
{
    lh_assert_runtime_if(lh_array_get_type_size(self) != lh_array_get_type_size(other),
                         lh_runtime_error_code_invalid_argument);

    const lh_usize_t count = lh_array_get_size(other);
    lh_return_if(lh_math_is_zero(count));

    /* Grow first, then read other's data: when other is self, growing moves the
     * block, and the source pointer must point into the moved one. */
    const lh_usize_t capacity = lh_array_get_capacity(self);
    lh_array_reserve(self, lh_array_get_grown_capacity(
                               capacity, lh_math_add(lh_array_get_size(self), count)));
    lh_array_push_back_of(self, lh_array_get_data(other), count);
}

lh_void
lh_array_merge(lh_array_t *self, lh_array_t *other)
{
    lh_assert_runtime_ref(other);
    lh_return_if(self == other);
    lh_assert_runtime_if(lh_array_get_type_size(self) != lh_array_get_type_size(other),
                         lh_runtime_error_code_invalid_argument);

    if (lh_array_is_empty(self))
    {
        lh_memory_typed_allocated_exchange(lh_array_get_typed(self), lh_array_get_typed(other));
        self->size = other->size;
        other->size = 0;
        return;
    }

    lh_array_append(self, other);
    lh_array_clear(other);
}

lh_uindex_t
lh_array_index_of(const lh_array_t *self, const lh_ptr value)
{
    lh_assert_runtime_ref(value);
    lh_return_if(lh_array_is_empty(self), LH_ARRAY_INVALID);

    const lh_usize_t type_size = lh_array_get_type_size(self);
    const lh_ptr begin = lh_array_get_begin(self);
    const lh_ptr found =
        lh_memory_find_step(begin, lh_math_mul(lh_array_get_size(self), type_size), value,
                            type_size, type_size);
    lh_return_if(lh_ptr_is_null(found), LH_ARRAY_INVALID);
    return lh_math_div(lh_math_sub(lh_ptr_to_uaddr(found), lh_ptr_to_uaddr(begin)), type_size);
}

lh_bool_t
lh_array_contains(const lh_array_t *self, const lh_ptr value)
{
    return lh_array_index_of(self, value) != LH_ARRAY_INVALID;
}

lh_void
lh_array_fill(lh_array_t *self, const lh_ptr value)
{
    lh_assert_runtime_ref(value);
    lh_return_if(lh_array_is_empty(self));

    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_memory_set_pattern(lh_array_get_begin(self),
                          lh_math_mul(lh_array_get_size(self), type_size), value, type_size);
}

lh_void
lh_array_reverse(lh_array_t *self)
{
    const lh_usize_t size = lh_array_get_size(self);
    lh_return_if(size < 2U);

    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_uchar_t *lo = lh_ptr_cast(lh_uchar_t, lh_array_get_begin(self));
    lh_uchar_t *hi = lo + lh_math_mul(lh_math_sub_one(size), type_size);
    for (; lo < hi; hi -= type_size)
    {
        for (lh_usize_t i = 0; i < type_size; ++i, ++lo)
        {
            const lh_uchar_t byte = *lo;
            *lo = hi[i];
            hi[i] = byte;
        }
    }
}

lh_void
lh_array_sort(lh_array_t *self, lh_array_cmp_cb cmp, lh_ptr context)
{
    lh_assert_runtime_ref(cmp);
    const lh_usize_t size = lh_array_get_size(self);
    lh_return_if(size < 2U);

    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_memory_typed_allocated_t scratch;
    lh_memory_typed_init_empty(lh_addr_of(scratch), type_size);
    lh_memory_typed_allocated_resize(lh_addr_of(scratch), size);

    /* Bottom-up merge sort, ping-ponging runs between the array and scratch. */
    lh_uchar_t *const data = lh_ptr_cast(lh_uchar_t, lh_array_get_begin(self));
    lh_uchar_t *src = data;
    lh_uchar_t *dst = lh_ptr_cast(lh_uchar_t, lh_memory_typed_get_begin(lh_addr_of(scratch)));

    for (lh_usize_t run = 1U; run < size; run = lh_math_mul(run, 2U))
    {
        for (lh_uindex_t lo = 0; lo < size; lo = lh_math_add(lo, lh_math_mul(run, 2U)))
        {
            const lh_uindex_t mid = lh_math_min(lh_math_add(lo, run), size);
            const lh_uindex_t hi = lh_math_min(lh_math_add(mid, run), size);
            lh_uindex_t i = lo;
            lh_uindex_t j = mid;

            for (lh_uindex_t k = lo; k < hi; ++k)
            {
                /* Left wins ties: the sort is stable. */
                const lh_bool_t take_right =
                    i == mid || (j < hi && cmp(src + lh_math_mul(i, type_size),
                                               src + lh_math_mul(j, type_size), context) > 0);
                const lh_uindex_t from = take_right ? j++ : i++;
                lh_memory_std_copy(dst + lh_math_mul(k, type_size),
                                   src + lh_math_mul(from, type_size), type_size);
            }
        }

        lh_uchar_t *const swap = src;
        src = dst;
        dst = swap;
    }

    if (src != data)
    {
        lh_memory_std_copy(data, src, lh_math_mul(size, type_size));
    }
    lh_memory_typed_allocated_clear(lh_addr_of(scratch));
}

lh_void
lh_array_unique(lh_array_t *self, lh_array_cmp_cb cmp, lh_ptr context)
{
    lh_assert_runtime_ref(cmp);
    const lh_usize_t size = lh_array_get_size(self);
    lh_return_if(size < 2U);

    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_uchar_t *const data = lh_ptr_cast(lh_uchar_t, lh_array_get_begin(self));
    lh_usize_t kept = 1U;

    for (lh_uindex_t i = 1U; i < size; ++i)
    {
        lh_uchar_t *const value = data + lh_math_mul(i, type_size);
        lh_uchar_t *const last = data + lh_math_mul(lh_math_sub_one(kept), type_size);
        if (cmp(last, value, context) == 0)
        {
            continue;
        }
        if (kept != i)
        {
            lh_memory_std_copy(last + type_size, value, type_size);
        }
        kept = lh_math_add_one(kept);
    }

    self->size = kept;
}

lh_void
lh_array_filter(lh_array_t *self, lh_array_pred_cb pred, lh_ptr context)
{
    lh_assert_runtime_ref(pred);
    const lh_usize_t size = lh_array_get_size(self);
    const lh_usize_t type_size = lh_array_get_type_size(self);
    lh_uchar_t *const data = lh_ptr_cast(lh_uchar_t, lh_array_get_begin(self));
    lh_usize_t kept = 0U;

    for (lh_uindex_t i = 0U; i < size; ++i)
    {
        lh_uchar_t *const value = data + lh_math_mul(i, type_size);
        if (!pred(value, context))
        {
            continue;
        }
        if (kept != i)
        {
            lh_memory_std_copy(data + lh_math_mul(kept, type_size), value, type_size);
        }
        kept = lh_math_add_one(kept);
    }

    self->size = kept;
}
