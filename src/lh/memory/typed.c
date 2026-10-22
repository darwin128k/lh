#include <lh/memory/typed.h>
#include <lh/memory/typed/initializer.h>
#include <lh/util/return.h>
#include <lh/math.h>
#include <lh/util/swap.h>
#include <lh/cast/const.h>
#include <lh/assert.h>

/* A typed struct starts with the fields of memory bounds, so it is its own bounds. */
lh_memory_bounds_t *
lh_memory_typed_get_bounds_mut(lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_cast(lh_memory_bounds_t, self);
}

const lh_memory_bounds_t *
lh_memory_typed_get_bounds(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_ptr_ccast(lh_memory_bounds_t, self);
}

lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return self->type_size;
}

lh_void
lh_memory_typed_set_type_size(lh_memory_typed_t *self, lh_usize_t type_size)
{
    lh_assert_runtime_ref(self);
    self->type_size = type_size;
}

lh_void
lh_memory_typed_swap(lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    lh_return_if(lh_math_eq(self, other));
    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    lh_swap(lh_memory_typed_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_get_begin(lh_memory_typed_get_bounds(self));
}

lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_get_end(lh_memory_typed_get_bounds(self));
}

lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_get_size(lh_memory_typed_get_bounds(self));
}

lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_is_empty(lh_memory_typed_get_bounds(self));
}

lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_memory_bounds_is_multiple_of(lh_memory_typed_get_bounds(self), type_size);
}

lh_void
lh_memory_typed_clear(lh_memory_typed_t *self)
{
    lh_memory_bounds_clear(lh_memory_typed_get_bounds_mut(self));
}

lh_void
lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_bounds_set(lh_memory_typed_get_bounds_mut(self), begin, end);
    lh_memory_typed_set_type_size(self, type_size);
}

lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_memory_bounds_assign(lh_memory_typed_get_bounds_mut(self),
                            lh_memory_typed_get_bounds(other));
    lh_memory_typed_set_type_size(self, lh_memory_typed_get_type_size(other));
}

lh_ptr
lh_memory_typed_get_data(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_begin(self);
}

/* The element count is not stored: no bytes is no elements, whatever type_size is. */
lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self)
{
    const lh_usize_t size_of_bytes = lh_memory_typed_get_size_of_bytes(self);
    return lh_math_is_zero(size_of_bytes)
               ? 0U
               : lh_math_div(size_of_bytes, lh_memory_typed_get_type_size(self));
}

lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_is_valid(lh_memory_typed_get_bounds(self)) &&
           lh_memory_typed_is_multiple_of(self);
}

lh_void
lh_memory_typed_init_empty(lh_memory_typed_t *self, lh_usize_t type_size)
{
    lh_memory_typed_clear(self);
    lh_memory_typed_set_type_size(self, type_size);
}

lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_uindex_t index)
{
    return index < lh_memory_typed_get_size(self);
}

lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_typed_assign(self, other);
}

lh_void
lh_memory_typed_swap_v(lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_assert_runtime_ifn(lh_memory_typed_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_typed_swap(self, other);
}

lh_uoffset_t
lh_memory_typed_get_offset_from_index(const lh_memory_typed_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);

    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_math_mul(index, type_size);
}

lh_uindex_t
lh_memory_typed_get_last_index(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, 0),
                          lh_runtime_error_code_out_of_range);

    return lh_math_sub_one(lh_memory_typed_get_size(self));
}

lh_uindex_t
lh_memory_typed_get_index_from_offset(const lh_memory_typed_t *self, lh_uoffset_t offset)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    lh_assert_runtime(lh_math_is_zero(lh_math_mod(offset, type_size)),
                      lh_runtime_error_code_size_not_multiple_of_type_size);

    const lh_uindex_t index = lh_math_div(offset, type_size);
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);
    return index;
}

lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_t typed;

    lh_memory_typed_set(lh_addr_of(typed), begin, end, type_size);
    lh_memory_typed_assign_v(self, lh_addr_of(typed));
}

lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_uindex_t index)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds(self);
    return lh_memory_bounds_get_ptr_from_begin(bounds,
                                               lh_memory_typed_get_offset_from_index(self, index));
}

lh_uindex_t
lh_memory_typed_get_index_from_end(const lh_memory_typed_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_code_out_of_range);

    const lh_uindex_t last_index = lh_memory_typed_get_last_index(self);
    return lh_math_sub(last_index, index);
}

lh_uindex_t
lh_memory_typed_get_index_from_ptr(const lh_memory_typed_t *self, const lh_ptr ptr)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds(self);
    const lh_uoffset_t offset = lh_memory_bounds_get_offset_from_begin(bounds, ptr);
    return lh_memory_typed_get_index_from_offset(self, offset);
}

lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_set_v(self, begin, end, type_size);
}

lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_uindex_t index)
{
    return lh_memory_typed_get_ptr_from_begin(self,
                                              lh_memory_typed_get_index_from_end(self, index));
}

lh_ptr
lh_memory_typed_get_first_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_ptr_from_begin(self, 0);
}

lh_void
lh_memory_typed_get_value_bounds(const lh_memory_typed_t *self, lh_uindex_t index,
                                 lh_memory_bounds_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ptr begin = lh_memory_typed_get_ptr_from_begin(self, index);
    lh_memory_bounds_init_by_size(out, begin, lh_memory_typed_get_type_size(self));
}

lh_void
lh_memory_typed_get_range_bounds(const lh_memory_typed_t *self, lh_uindex_t index, lh_usize_t count,
                                 lh_memory_bounds_t *out)
{
    lh_assert_runtime_ref(out);
    lh_assert_runtime_ifn(lh_math_le(lh_math_add(index, count), lh_memory_typed_get_size(self)),
                          lh_runtime_error_code_out_of_range);

    lh_ptr begin = lh_memory_typed_get_ptr_from_begin(self, index);
    lh_memory_bounds_init_by_size(out, begin, lh_math_mul(count, lh_memory_typed_get_type_size(self)));
}

lh_void
lh_memory_typed_get_value_into(const lh_memory_typed_t *self, lh_uindex_t index, lh_ptr dst)
{
    lh_memory_bounds_t destination;
    lh_memory_bounds_init_by_size(lh_addr_of(destination), dst, lh_memory_typed_get_type_size(self));
    lh_memory_bounds_copy_by_size(lh_addr_of(destination),
                                  lh_memory_typed_get_ptr_from_begin(self, index),
                                  lh_memory_typed_get_type_size(self));
}

lh_ptr
lh_memory_typed_get_last_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_ptr_from_end(self, 0);
}

lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_sindex_t index)
{
    if (lh_math_is_negative(index))
    {
        /* -(index + 1), not -index - 1: the negation of the minimum overflows. */
        return lh_memory_typed_get_ptr_from_end(
            self, lh_type_cast(lh_uindex_t, lh_math_neg(lh_math_add(index, 1))));
    }

    return lh_memory_typed_get_ptr_from_begin(self, lh_type_cast(lh_uindex_t, index));
}

lh_void
lh_memory_typed_set_value(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr value)
{
    lh_memory_bounds_t value_bounds;
    lh_memory_typed_get_value_bounds(self, index, lh_addr_of(value_bounds));
    lh_memory_bounds_copy_by_size(lh_addr_of(value_bounds), value,
                                  lh_memory_typed_get_type_size(self));
}

lh_bool_t
lh_memory_typed_value_equals(const lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr other)
{
    lh_memory_bounds_t value_bounds;
    lh_memory_typed_get_value_bounds(self, index, lh_addr_of(value_bounds));
    return lh_ptr_is_null(lh_memory_bounds_compare_by_size(lh_addr_of(value_bounds), other,
                                                           lh_memory_typed_get_type_size(self)));
}

lh_void
lh_memory_typed_set_values(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr values,
                           lh_usize_t count)
{
    lh_return_if(lh_math_is_zero(count));

    lh_memory_bounds_t dest;
    lh_memory_typed_get_range_bounds(self, index, count, lh_addr_of(dest));
    lh_memory_bounds_copy_by_size(lh_addr_of(dest), values,
                                  lh_math_mul(count, lh_memory_typed_get_type_size(self)));
}

lh_void
lh_memory_typed_move_within(lh_memory_typed_t *self, lh_uindex_t dst_index, lh_uindex_t src_index,
                            lh_usize_t count)
{
    lh_return_if(lh_math_is_zero(count));

    lh_memory_bounds_t dest;
    lh_memory_typed_get_range_bounds(self, dst_index, count, lh_addr_of(dest));
    lh_memory_bounds_t source;
    lh_memory_typed_get_range_bounds(self, src_index, count, lh_addr_of(source));
    lh_memory_bounds_move(lh_addr_of(dest), lh_addr_of(source));
}

lh_void
lh_memory_typed_swap_values(lh_memory_typed_t *self, lh_uindex_t i, lh_uindex_t j, lh_ptr scratch)
{
    lh_memory_typed_get_value_into(self, i, scratch);
    lh_memory_typed_set_value(self, i, lh_memory_typed_get_ptr_from_begin(self, j));
    lh_memory_typed_set_value(self, j, scratch);
}
