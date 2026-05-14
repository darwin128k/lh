#include <lh/memory/typed.h>
#include <lh/memory/typed/initializer.h>
#include <lh/attribute/static.h>
#include <lh/util/algorithm.h>
#include <lh/util/math.h>
#include <lh/util/return.h>
#include <lh/optional/ref.h>
#include <lh/assert.h>

lh_void
lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       lh_usize_t *type_size)
{
    lh_assert_runtime_ref(self);
    lh_memory_bounds_unpack(&self->bounds, begin, end);
    lh_optional_ref(type_size) *type_size = self->type_size;
}

const lh_memory_bounds_t *
lh_memory_typed_get_bounds(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return &self->bounds;
}

lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_get_begin(&self->bounds);
}

lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_get_end(&self->bounds);
}

lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return self->type_size;
}

lh_bool_t
lh_memory_typed_is_uninitialized(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_is_uninitialized(&self->bounds);
}

lh_bool_t
lh_memory_typed_is_initialized(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_is_initialized(&self->bounds);
}

lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_memory_bounds_is_valid(&self->bounds) &&
           !lh_math_is_zero(self->type_size) &&
           lh_math_is_zero(lh_math_mod(lh_memory_bounds_get_size(&self->bounds), self->type_size));
}

lh_bool_t
lh_memory_typed_is_invalid(const lh_memory_typed_t *self)
{
    return !lh_memory_typed_is_valid(self);
}

lh_void
lh_memory_typed_unpack_v(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                         lh_usize_t *type_size)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_typed_unpack(self, begin, end, type_size);
}

lh_ptr
lh_memory_typed_get_begin_v(const lh_memory_typed_t *self)
{
    lh_ptr begin;
    lh_memory_typed_unpack_v(self, lh_addr_of(begin), lh_null, lh_null);
    return begin;
}

lh_ptr
lh_memory_typed_get_end_v(const lh_memory_typed_t *self)
{
    lh_ptr end;
    lh_memory_typed_unpack_v(self, lh_null, lh_addr_of(end), lh_null);
    return end;
}

lh_usize_t
lh_memory_typed_get_byte_size(const lh_memory_typed_t *self)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(self), lh_runtime_error_code_invalid_range);
    return lh_memory_bounds_get_size(&self->bounds);
}

lh_usize_t
lh_memory_typed_get_count(const lh_memory_typed_t *self)
{
    lh_assert_runtime_ref(self);
    lh_assert_runtime_if(lh_math_is_zero(self->type_size), lh_runtime_error_code_invalid_argument);
    const lh_usize_t byte_size = lh_memory_bounds_get_size(&self->bounds);
    lh_assert_runtime_if(lh_math_ne(lh_math_mod(byte_size, self->type_size), 0),
                         lh_runtime_error_code_size_not_multiple_of_type_size);
    return lh_math_div(byte_size, self->type_size);
}

lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self)
{
    return lh_memory_typed_is_uninitialized(self) ||
           (lh_memory_typed_is_valid(self) && lh_math_is_zero(lh_memory_typed_get_count(self)));
}

lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(self), lh_runtime_error_code_invalid_range);
    return index < lh_memory_typed_get_count(self);
}

lh_ptr
lh_memory_typed_get_element_ptr_from_begin(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid_index(self, index),
                             lh_runtime_error_code_out_of_range);
    return lh_memory_bounds_get_ptr_from_begin(&self->bounds,
                                               lh_math_mul(index, self->type_size));
}

lh_ptr
lh_memory_typed_get_element_ptr_from_end(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid_index(self, index),
                             lh_runtime_error_code_out_of_range);
    const lh_usize_t from_begin = lh_math_sub(lh_math_sub_one(lh_memory_typed_get_count(self)),
                                              index);
    return lh_memory_bounds_get_ptr_from_begin(&self->bounds,
                                               lh_math_mul(from_begin, self->type_size));
}

lh_ptr
lh_memory_typed_get_first_element_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_element_ptr_from_begin(self, 0);
}

lh_ptr
lh_memory_typed_get_last_element_ptr(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_element_ptr_from_end(self, 0);
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    self->bounds    = other->bounds;
    self->type_size = other->type_size;
}

lh_void
lh_memory_typed_clear(lh_memory_typed_t *self)
{
    const lh_memory_typed_t empty = lh_memory_typed_initializer(lh_null, lh_null, 0);
    lh_memory_typed_assign(self, lh_addr_of(empty));
}

lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_typed_assign(self, other);
}

LH_ATTRIBUTE_STATIC
lh_memory_typed_t
lh_memory_typed_make(lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    const lh_memory_typed_t typed = lh_memory_typed_initializer(begin, end, type_size);
    return typed;
}

lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    const lh_memory_typed_t typed = lh_memory_typed_make_v(begin, end, type_size);
    lh_memory_typed_assign(self, lh_addr_of(typed));
}

lh_void
lh_memory_typed_set_by_count(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t count,
                              lh_usize_t type_size)
{
    const lh_memory_typed_t typed = lh_memory_typed_make_by_count(begin, count, type_size);
    lh_memory_typed_assign(self, lh_addr_of(typed));
}

lh_void
lh_memory_typed_swap_v(lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(other), lh_runtime_error_code_invalid_range);

    lh_return_if(lh_math_eq(self, other));

    lh_algorithm_swap(lh_memory_typed_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    lh_memory_typed_set_v(self, begin, end, type_size);
}

lh_void
lh_memory_typed_init_by_count(lh_memory_typed_t *self, lh_ptr begin, lh_usize_t count,
                               lh_usize_t type_size)
{
    lh_memory_typed_set_by_count(self, begin, count, type_size);
}

lh_void
lh_memory_typed_init_empty(lh_memory_typed_t *self, lh_usize_t type_size)
{
    const lh_memory_typed_t empty = lh_memory_typed_empty_initializer(type_size);
    lh_memory_typed_assign(self, lh_addr_of(empty));
}

lh_void
lh_memory_typed_init_by_other(lh_memory_typed_t *self, const lh_memory_typed_t *other)
{
    lh_memory_typed_assign_v(self, other);
}

lh_memory_typed_t
lh_memory_typed_make_v(lh_ptr begin, lh_ptr end, lh_usize_t type_size)
{
    const lh_memory_typed_t typed = lh_memory_typed_make(begin, end, type_size);
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(lh_addr_of(typed)),
                             lh_runtime_error_code_invalid_range);
    return typed;
}

lh_memory_typed_t
lh_memory_typed_make_by_count(lh_ptr begin, lh_usize_t count, lh_usize_t type_size)
{
    lh_assert_runtime_if(lh_math_is_zero(type_size), lh_runtime_error_code_invalid_argument);
    const lh_usize_t byte_size = lh_math_mul(count, type_size);
    const lh_memory_bounds_t bounds = lh_memory_bounds_make_by_size(begin, byte_size);
    return lh_memory_typed_make(lh_memory_bounds_get_begin(lh_addr_of(bounds)),
                                lh_memory_bounds_get_end(lh_addr_of(bounds)),
                                type_size);
}

lh_memory_typed_t
lh_memory_typed_make_empty(lh_usize_t type_size)
{
    const lh_memory_typed_t empty = lh_memory_typed_empty_initializer(type_size);
    return empty;
}

lh_memory_typed_t
lh_memory_typed_make_by_other(const lh_memory_typed_t *other)
{
    lh_assert_runtime_if_not(lh_memory_typed_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_ptr_deref(other);
}
