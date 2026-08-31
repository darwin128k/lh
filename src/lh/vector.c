#include <lh/vector.h>
#include <lh/memory/raw.h>
#include <lh/util/math.h>
#include <lh/util/return.h>
#include <lh/util/ptr.h>
#include <lh/attribute/static.h>
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
    const lh_usize_t used_bytes = lh_math_mul(lh_vector_get_size(self), lh_vector_get_type_size(self));
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

LH_ATTRIBUTE_STATIC
lh_void
lh_vector_grow_if_full(lh_vector_t *self, lh_usize_t size)
{
    lh_return_if(size != lh_vector_get_capacity(self));

    const lh_usize_t new_capacity = lh_math_is_zero(size)
                                        ? LH_LIBRARY_OPTION_VECTOR_INITIAL_CAPACITY
                                        : lh_math_mul(size, LH_LIBRARY_OPTION_VECTOR_GROWTH_FACTOR);
    lh_vector_reserve(self, new_capacity);
}

lh_void
lh_vector_push_back(lh_vector_t *self, const lh_ptr value)
{
    const lh_usize_t size = lh_vector_get_size(self);
    lh_vector_grow_if_full(self, size);

    lh_memory_typed_set_value(lh_addr_of(self->typed), size, value);
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
    lh_assert_runtime_ifn(lh_vector_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    return lh_memory_typed_get_ptr_from_begin(lh_addr_of(self->typed), index);
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
    const lh_usize_t size = lh_vector_get_size(self);
    lh_assert_runtime_ifn(index <= size,
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    lh_vector_grow_if_full(self, size);

    if (index < size)
    {
        const lh_usize_t type_size = lh_vector_get_type_size(self);
        const lh_usize_t move_bytes = lh_math_mul(lh_math_sub(size, index), type_size);
        lh_ptr src_begin = lh_vector_get_ptr(self, index);
        lh_ptr src_end = lh_ptr_add_by_offset_unsafe(lh_void, src_begin, move_bytes);
        lh_ptr dst_begin = lh_ptr_add_by_offset_unsafe(lh_void, src_begin, type_size);
        lh_ptr dst_end = lh_ptr_add_by_offset_unsafe(lh_void, dst_begin, move_bytes);
        lh_memory_raw_move(dst_begin, dst_end, src_begin, src_end);
    }

    lh_memory_typed_set_value(lh_addr_of(self->typed), index, value);
    self->size = lh_math_add_one(size);
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
        const lh_usize_t type_size = lh_vector_get_type_size(self);
        const lh_usize_t move_bytes = lh_math_mul(tail_count, type_size);
        lh_ptr dst_begin = lh_vector_get_ptr(self, index);
        lh_ptr src_begin = lh_ptr_add_by_offset_unsafe(lh_void, dst_begin, type_size);
        lh_ptr src_end = lh_ptr_add_by_offset_unsafe(lh_void, src_begin, move_bytes);
        lh_ptr dst_end = lh_ptr_add_by_offset_unsafe(lh_void, dst_begin, move_bytes);
        lh_memory_raw_move(dst_begin, dst_end, src_begin, src_end);
    }

    self->size = lh_math_sub_one(size);
}
