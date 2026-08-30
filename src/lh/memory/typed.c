#include <lh/memory/typed.h>
#include <lh/memory/typed/initializer.h>
#include <lh/util/return.h>
#include <lh/util/math.h>
#include <lh/util/type.h>
#include <lh/assert.h>

lh_memory_bounds_t *
lh_memory_typed_get_bounds(lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->bounds);
}

const lh_memory_bounds_t *
lh_memory_typed_get_bounds_as_const(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->bounds);
}

lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return self->type_size;
}

lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    return lh_memory_bounds_get_size(bounds);
}

lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    return lh_memory_bounds_is_multiple_of(bounds, type_size);
}

lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    return lh_memory_bounds_is_valid(bounds) && lh_memory_typed_is_multiple_of(self);
}

lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    return lh_memory_bounds_is_empty(bounds);
}

lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self)
{
    lh_return_if(lh_memory_typed_is_empty(self), 0);
    lh_assert_runtime(
        lh_memory_typed_is_valid(self),
        lh_runtime_error_make_by_code(lh_runtime_error_code_size_not_multiple_of_type_size));

    const lh_usize_t size_of_bytes = lh_memory_typed_get_size_of_bytes(self);
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_math_div(size_of_bytes, type_size);
}

lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_uindex_t index)
{
    return index < lh_memory_typed_get_size(self);
}

lh_uoffset_t
lh_memory_typed_get_offset_from_index(const lh_memory_typed_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_math_mul(index, type_size);
}

lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_uindex_t index)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    return lh_memory_bounds_get_ptr_from_begin(bounds,
                                               lh_memory_typed_get_offset_from_index(self, index));
}

lh_uindex_t
lh_memory_typed_get_last_index(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, 0),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    return lh_math_sub_one(lh_memory_typed_get_size(self));
}

lh_uindex_t
lh_memory_typed_get_index_from_end(const lh_memory_typed_t *self, lh_uindex_t index)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));

    const lh_uindex_t last_index = lh_memory_typed_get_last_index(self);
    return lh_math_sub(last_index, index);
}

lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_uindex_t index)
{
    return lh_memory_typed_get_ptr_from_begin(self, lh_memory_typed_get_index_from_end(self, index));
}

lh_ptr
lh_memory_typed_get_first_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_ptr_from_begin(self, 0);
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
        const lh_uindex_t distance_from_end =
            lh_type_cast(lh_uindex_t, lh_math_sub_one(lh_math_neg(index)));
        return lh_memory_typed_get_ptr_from_begin(
            self, lh_memory_typed_get_index_from_end(self, distance_from_end));
    }

    return lh_memory_typed_get_ptr_from_begin(self, lh_type_cast(lh_uindex_t, index));
}

lh_uindex_t
lh_memory_typed_get_index_from_offset(const lh_memory_typed_t *self, lh_uoffset_t offset)
{
    const lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    lh_assert_runtime(
        lh_math_is_zero(lh_math_mod(offset, type_size)),
        lh_runtime_error_make_by_code(lh_runtime_error_code_size_not_multiple_of_type_size));

    const lh_uindex_t index = lh_math_div(offset, type_size);
    lh_assert_runtime_ifn(lh_memory_typed_is_valid_index(self, index),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_out_of_range));
    return index;
}

lh_uindex_t
lh_memory_typed_get_index_from_ptr(const lh_memory_typed_t *self, const lh_ptr ptr)
{
    const lh_memory_bounds_t *bounds = lh_memory_typed_get_bounds_as_const(self);
    const lh_uoffset_t offset = lh_memory_bounds_get_offset_from_begin(bounds, ptr);
    return lh_memory_typed_get_index_from_offset(self, offset);
}

lh_void
lh_memory_typed_retype(lh_memory_typed_t *self, lh_usize_t type_size)
{
    lh_assert_runtime_ref(self);
    self->type_size = type_size;
}

lh_void
lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_initializer(begin, end);
    lh_memory_bounds_assign(lh_memory_typed_get_bounds(self), lh_addr_of(bounds));
    lh_memory_typed_retype(self, type_size);
}

lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_void *other_begin, *other_end;
    lh_memory_bounds_unpack(lh_memory_typed_get_bounds_as_const(other), lh_addr_of(other_begin),
                            lh_addr_of(other_end));
    lh_memory_typed_set(self, other_begin, other_end, lh_memory_typed_get_type_size(other));
}

lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid(other),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));
    lh_memory_typed_assign(self, other);
}

lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    const lh_memory_typed_t typed = lh_memory_typed_initializer(begin, end, type_size);
    lh_memory_typed_assign_v(self, lh_addr_of(typed));
}

lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_set_v(self, begin, end, type_size);
}