#include <lh/vector.h>
#include <lh/memory/std.h>
#include <lh/util/math.h>
#include <lh/util/return.h>
#include <lh/util/ptr.h>
#include <lh/config.h>
#include <lh/assert.h>

lh_usize_t
lh_vector_get_capacity(const lh_vector_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_typed_get_size(lh_addr_of(self->typed));
}

lh_usize_t
lh_vector_get_size(const lh_vector_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_usize_t
lh_vector_get_type_size(const lh_vector_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_typed_get_type_size(lh_addr_of(self->typed));
}

lh_ptr
lh_vector_get_begin(const lh_vector_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_typed_get_begin(lh_addr_of(self->typed));
}

lh_ptr
lh_vector_get_data(const lh_vector_t *self)
{
    return lh_vector_get_begin(self);
}

lh_ptr
lh_vector_get_end(const lh_vector_t *self)
{
    const lh_usize_t used_bytes =
        lh_math_mul(lh_vector_get_size(self), lh_vector_get_type_size(self));
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_vector_get_begin(self), used_bytes);
}

lh_bool_t
lh_vector_is_empty(const lh_vector_t *self)
{
    return lh_math_is_zero(lh_vector_get_size(self));
}

lh_void
lh_vector_init(lh_vector_t *self, lh_usize_t type_size)
{
    lh_assert_runtime_ref(self);
    lh_memory_typed_init_empty(lh_addr_of(self->typed), type_size);
    self->size = 0;
}

lh_void
lh_vector_reserve(lh_vector_t *self, lh_usize_t min_capacity)
{
    lh_return_if(lh_vector_get_capacity(self) >= min_capacity);
    lh_memory_typed_allocated_resize(lh_addr_of(self->typed), min_capacity);
}

lh_usize_t
lh_vector_get_grown_capacity(lh_usize_t capacity, lh_usize_t min_capacity)
{
    lh_return_if(capacity >= min_capacity, capacity);

    const lh_usize_t policy_capacity =
        lh_math_is_zero(capacity) ? LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY
                                  : lh_math_mul(capacity, LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR);
    return lh_math_max(policy_capacity, min_capacity);
}

lh_void
lh_vector_insert_of(lh_vector_t *self, lh_uindex_t index, const lh_ptr values, lh_usize_t count)
{
    const lh_usize_t size = lh_vector_get_size(self);
    lh_assert_runtime_ifn(index <= size,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    lh_return_if(lh_math_is_zero(count));

    const lh_usize_t new_size = lh_math_add(size, count);

    const lh_usize_t capacity = lh_vector_get_capacity(self);
    if (capacity < new_size)
    {
        lh_vector_reserve(self, lh_vector_get_grown_capacity(capacity, new_size));
    }

    if (index < size)
    {
        lh_memory_typed_move_within(lh_addr_of(self->typed), lh_math_add(index, count), index,
                                    lh_math_sub(size, index));
    }

    lh_memory_typed_set_values(lh_addr_of(self->typed), index, values, count);

    self->size = new_size;
}

lh_void
lh_vector_push_back_of(lh_vector_t *self, const lh_ptr values, lh_usize_t count)
{
    lh_vector_insert_of(self, lh_vector_get_size(self), values, count);
}

lh_void
lh_vector_push_back(lh_vector_t *self, const lh_ptr value)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(value);

    /*
     * Fast path for the common case (room already reserved): avoid
     * lh_vector_push_back_of / lh_vector_insert_of's generic machinery, which
     * re-derives capacity through lh_memory_typed_get_size (and, through it,
     * validity + a division) several times over for a single element. Every
     * value used below still comes from the public, bounds-checked accessors
     * — this only removes the redundant re-derivation, not the checks
     * themselves. Falls back to the general path whenever growth is needed.
     */
    const lh_usize_t size = lh_vector_get_size(self);
    const lh_usize_t capacity = lh_vector_get_capacity(self);

    if (size >= capacity)
    {
        lh_vector_push_back_of(self, value, 1);
        return;
    }

    const lh_usize_t type_size = lh_vector_get_type_size(self);
    lh_ptr dst = lh_ptr_add_by_offset_unsafe(lh_void, lh_vector_get_begin(self),
                                             lh_math_mul(size, type_size));
    lh_memory_std_copy(dst, value, type_size);
    self->size = lh_math_add_one(size);
}

lh_bool_t
lh_vector_is_valid_index(const lh_vector_t *self, lh_uindex_t index)
{
    return index < lh_vector_get_size(self);
}

lh_ptr
lh_vector_get_ptr(const lh_vector_t *self, lh_uindex_t index)
{
    /* lh_memory_typed_get_ptr_from_begin would re-validate index against the typed block's
     * own capacity via lh_memory_typed_get_size (division + a validity check of its own) —
     * redundant, since lh_vector_is_valid_index (a plain field compare against self->size)
     * already guarantees index < size <= capacity. Compute the pointer directly instead. */
    lh_assert_runtime_ifn(lh_vector_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_vector_get_begin(self),
                                       lh_math_mul(index, lh_vector_get_type_size(self)));
}

lh_void
lh_vector_pop_back(lh_vector_t *self, lh_ptr dst)
{
    lh_assert_runtime_if(lh_vector_is_empty(self),
                         lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    const lh_usize_t last_index = lh_math_sub_one(lh_vector_get_size(self));
    if (lh_ptr_is_set(dst))
    {
        lh_memory_typed_get_value_into(lh_addr_of(self->typed), last_index, dst);
    }
    self->size = last_index;
}

lh_void
lh_vector_insert(lh_vector_t *self, lh_uindex_t index, const lh_ptr value)
{
    lh_vector_insert_of(self, index, value, 1);
}

lh_void
lh_vector_erase(lh_vector_t *self, lh_uindex_t index, lh_ptr dst)
{
    lh_assert_runtime_ifn(lh_vector_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    if (lh_ptr_is_set(dst))
    {
        lh_memory_typed_get_value_into(lh_addr_of(self->typed), index, dst);
    }

    const lh_usize_t size = lh_vector_get_size(self);
    const lh_usize_t tail_count = lh_math_sub(size, lh_math_add_one(index));
    if (lh_math_is_positive(tail_count))
    {
        lh_memory_typed_move_within(lh_addr_of(self->typed), index, lh_math_add_one(index),
                                    tail_count);
    }

    self->size = lh_math_sub_one(size);
}
