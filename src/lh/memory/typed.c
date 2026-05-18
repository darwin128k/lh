#include <lh/memory/typed.h>
#include <lh/util/math.h>
#include <lh/optional/ref.h>
#include <lh/assert.h>

lh_void
lh_memory_typed_unpack_bounds(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_memory_bounds_unpack(lh_ptr_ccast(lh_memory_bounds_t, self), begin, end);
}

lh_void
lh_memory_typed_unpack(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                       lh_usize_t *type_size)
{

    lh_memory_typed_unpack_bounds(self, begin, end);
    lh_optional_ref(type_size)
    {
        lh_ptr_deref(type_size) = self->type_size;
    }
}

lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self)
{
    lh_ptr begin;
    lh_memory_typed_unpack(self, lh_addr_of(begin), lh_null, lh_null);
    return begin;
}

lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self)
{
    lh_ptr end;
    lh_memory_typed_unpack(self, lh_null, lh_addr_of(end), lh_null);
    return end;
}

lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self)
{
    lh_usize_t type_size;
    lh_memory_typed_unpack(self, lh_null, lh_null, lh_addr_of(type_size));
    return type_size;
}

lh_bool_t
lh_memory_typed_is_uninitialized(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_is_uninitialized(lh_ptr_ccast(lh_memory_bounds_t, self));
}

lh_bool_t
lh_memory_typed_is_initialized(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_is_initialized(lh_ptr_ccast(lh_memory_bounds_t, self));
}

lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self, lh_usize_t alignment)
{
    return lh_memory_bounds_is_multiple_of(lh_ptr_ccast(lh_memory_bounds_t, self), alignment);
}

lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self)
{
    if (lh_memory_bounds_is_valid(lh_ptr_ccast(lh_memory_bounds_t, self)))
    {
        lh_usize_t type_size = lh_memory_typed_get_type_size(self);
        return lh_memory_typed_is_multiple_of(self, type_size);
    }
    return 0;
}

lh_void
lh_memory_typed_unpack_v(const lh_memory_typed_t *self, lh_ptr *begin, lh_ptr *end,
                         lh_usize_t *type_size)
{
    lh_assert_runtime_ifn(lh_memory_typed_is_valid(self),
                          lh_runtime_error_make_by_code(lh_runtime_error_code_invalid_range));
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
lh_memory_typed_get_type_size_v(const lh_memory_typed_t *self)
{
    lh_usize_t type_size;
    lh_memory_typed_unpack_v(self, lh_null, lh_null, lh_addr_of(type_size));
    return type_size;
}

lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self)
{
    return lh_memory_bounds_get_size(lh_ptr_ccast(lh_memory_bounds_t, self));
}

lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self)
{
    lh_usize_t size_of_bytes = lh_memory_typed_get_size_of_bytes(self);
    lh_usize_t type_size = lh_memory_typed_get_type_size_v(self);
    return lh_math_div(size_of_bytes, type_size);
}

lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self)
{
    return lh_memory_typed_is_uninitialized(self) ||
           lh_math_is_zero(lh_memory_typed_get_size(self));
}

lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_usize_t index)
{
    return index < lh_memory_typed_get_size(self);
}

lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_memory_bounds_get_ptr_from_begin(lh_ptr_ccast(lh_memory_bounds_t, self),
                                               index * type_size);
}

lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_usize_t index)
{
    lh_usize_t type_size = lh_memory_typed_get_type_size(self);
    return lh_memory_bounds_get_ptr_from_end(lh_ptr_ccast(lh_memory_bounds_t, self),
                                             index * type_size);
}

lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_ssize_t index)
{
    if (lh_math_is_negative(index))
    {
        return lh_memory_typed_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(index))));
    }

    return lh_memory_typed_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, index));
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

lh_byte_t
lh_memory_typed_get_value_from_begin(const lh_memory_typed_t *self, lh_usize_t index)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr_from_begin(self, index)));
}

lh_byte_t
lh_memory_typed_get_value_from_end(const lh_memory_typed_t *self, lh_usize_t index)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr_from_end(self, index)));
}

lh_byte_t
lh_memory_typed_get_value(const lh_memory_typed_t *self, lh_ssize_t index)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_typed_get_ptr(self, index)));
}

lh_byte_t
lh_memory_typed_get_first_value(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_typed_get_last_value(const lh_memory_typed_t *self)
{
    return lh_memory_typed_get_value_from_end(self, 0);
}

lh_void
lh_memory_typed_retype(lh_memory_typed_t *self, lh_usize_t new_type_size)
{
    self->type_size = new_type_size;
}

lh_void
lh_memory_typed_set_by_bounds(lh_memory_typed_t *self, const lh_memory_bounds_t *bounds)
{
}

lh_void
lh_memory_typed_unpack_to_other(const lh_memory_typed_t *self, lh_memory_typed_t *other)
{
    lh_memory_bounds_assign(lh_addr_of(other->bounds), lh_addr_of(self->bounds));
    other->type_size = self->type_size;
}