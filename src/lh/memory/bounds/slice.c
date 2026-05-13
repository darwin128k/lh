#include <lh/memory/bounds/slice/initializer.h>
#include <lh/memory/bounds/slice.h>
#include <lh/util/return.h>
#include <lh/memory/view/slice.h>
#include <lh/attribute/static.h>
#include <lh/util/algorithm.h>
#include <lh/runtime/assert.h>
#include <lh/optional/ref.h>
#include <lh/runtime/try.h>

lh_void
lh_memory_bounds_slice_unpack(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_runtime_assert_ref(self);

    lh_optional_ref(begin)
    {
        lh_ptr_deref(begin) = self->first;
    }

    lh_optional_ref(end)
    {
        lh_ptr_deref(end) = self->second;
    }
}

lh_ptr
lh_memory_bounds_slice_get_begin(const lh_memory_bounds_slice_t *self)
{
    lh_ptr begin;
    lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_null);
    return begin;
}

lh_ptr
lh_memory_bounds_slice_get_data(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_begin(self);
}

lh_ptr
lh_memory_bounds_slice_get_end(const lh_memory_bounds_slice_t *self)
{
    lh_ptr end;
    lh_memory_bounds_slice_unpack(self, lh_null, lh_addr_of(end));
    return end;
}

lh_memory_view_slice_flags_t
lh_memory_bounds_slice_get_flags(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_flags(s);
}

lh_bool_t
lh_memory_bounds_slice_is_uninitialized(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_view_slice_flags_uninitialized;
}

lh_bool_t
lh_memory_bounds_slice_is_initialized(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_view_slice_flags_initialized;
}

lh_memory_view_slice_direction_t
lh_memory_bounds_slice_get_direction(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_direction(s);
}

lh_bool_t
lh_memory_bounds_slice_is_forward(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_view_slice_direction_forward;
}

lh_bool_t
lh_memory_bounds_slice_is_backward(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_view_slice_direction_backward;
}

lh_bool_t
lh_memory_bounds_slice_is_valid(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_is_forward(self);
}

lh_void
lh_memory_bounds_slice_unpack_v(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid(self),
                          lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_unpack(self, begin, end);
}

lh_ptr
lh_memory_bounds_slice_get_begin_v(const lh_memory_bounds_slice_t *self)
{
    lh_ptr begin;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_null);
    return begin;
}

lh_ptr
lh_memory_bounds_slice_get_end_v(const lh_memory_bounds_slice_t *self)
{
    lh_ptr end;
    lh_memory_bounds_slice_unpack_v(self, lh_null, lh_addr_of(end));
    return end;
}

lh_usize_t
lh_memory_bounds_slice_get_size(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_size(s);
}

lh_bool_t
lh_memory_bounds_slice_is_empty(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_is_empty(s);
}

lh_bool_t
lh_memory_bounds_slice_is_valid_offset(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_is_valid_offset(s, offset);
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_begin(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_offset_from_begin(s, ptr);
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_end(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_offset_from_end(s, ptr);
}

lh_bool_t
lh_memory_bounds_slice_contains_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_contains_ptr(s, ptr);
}

lh_bool_t
lh_memory_bounds_slice_contains_range(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                      const lh_ptr end)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_contains_range(s, begin, end);
}

lh_bool_t
lh_memory_bounds_slice_contains(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    const lh_memory_view_slice_t *o = lh_ptr_ccast(lh_memory_view_slice_t, other);
    return lh_memory_view_slice_contains(s, o);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_begin(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_begin(self), offset);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_end(self),
                                       lh_math_neg(lh_type_cast(lh_soffset_t, offset)));
}

lh_ptr
lh_memory_bounds_slice_get_ptr_by_offset(const lh_memory_bounds_slice_t *self, lh_soffset_t offset)
{
    if (lh_math_is_negative(offset))
    {
        return lh_memory_bounds_slice_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(offset))));
    }

    return lh_memory_bounds_slice_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_begin(const lh_memory_bounds_slice_t *self,
                                            lh_uoffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_value_from_begin(s, offset);
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_value_from_end(s, offset);
}

lh_byte_t
lh_memory_bounds_slice_get_value_by_offset(const lh_memory_bounds_slice_t *self,
                                           lh_soffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_value_by_offset(s, offset);
}

lh_byte_t
lh_memory_bounds_slice_get_first_value(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_first_value(s);
}

lh_byte_t
lh_memory_bounds_slice_get_last_value(const lh_memory_bounds_slice_t *self)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_last_value(s);
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                           lh_soffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_get_offset_from_ptr(s, ptr, offset);
}

const lh_ptr
lh_memory_bounds_slice_seek_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                lh_soffset_t offset)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_seek_ptr(s, ptr, offset);
}

const lh_ptr
lh_memory_bounds_slice_next_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    return lh_memory_bounds_slice_seek_ptr(self, ptr, 1);
}

const lh_ptr
lh_memory_bounds_slice_prev_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    return lh_memory_bounds_slice_seek_ptr(self, ptr, -1);
}

lh_byte_t
lh_memory_bounds_slice_seek_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_seek_value(s, ptr);
}

lh_byte_t
lh_memory_bounds_slice_next_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_next_value(s, ptr);
}

lh_byte_t
lh_memory_bounds_slice_prev_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_prev_value(s, ptr);
}

lh_bool_t
lh_memory_bounds_slice_overlaps_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_overlaps_of(s, begin, end);
}

lh_bool_t
lh_memory_bounds_slice_overlaps(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    const lh_memory_view_slice_t *o = lh_ptr_ccast(lh_memory_view_slice_t, other);
    return lh_memory_view_slice_overlaps(s, o);
}

lh_bool_t
lh_memory_bounds_slice_overlaps_v(const lh_memory_bounds_slice_t *self,
                                  const lh_memory_bounds_slice_t *other)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    const lh_memory_view_slice_t *o = lh_ptr_ccast(lh_memory_view_slice_t, other);
    return lh_memory_view_slice_overlaps_v(s, o);
}

lh_bool_t
lh_memory_bounds_slice_multiple_of(const lh_memory_bounds_slice_t *self, lh_usize_t alignment)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_multiple_of(s, alignment);
}

lh_bool_t
lh_memory_bounds_slice_aligned_is_begin_aligned(const lh_memory_bounds_slice_t *self,
                                                lh_usize_t align)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_aligned_is_begin_aligned(s, align);
}

lh_bool_t
lh_memory_bounds_slice_is_aligned(const lh_memory_bounds_slice_t *self, lh_usize_t align)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_is_aligned(s, align);
}

lh_bool_t
lh_memory_bounds_slice_equals_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end)
{
    const lh_memory_view_slice_t *s = lh_ptr_ccast(lh_memory_view_slice_t, self);
    return lh_memory_view_slice_equals_of(s, begin, end);
}

lh_bool_t
lh_memory_bounds_slice_equals(const lh_memory_bounds_slice_t *self,
                              const lh_memory_bounds_slice_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_slice_equals_of(self, other_begin, other_end);
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_slice_set(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    lh_runtime_assert_ref(self);

    self->first = begin;
    self->second = end;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_slice_assign(lh_memory_bounds_slice_t *self, const lh_memory_bounds_slice_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    lh_memory_bounds_slice_set(self, other_begin, other_end);
}

lh_void
lh_memory_bounds_slice_clear(lh_memory_bounds_slice_t *self)
{
    const lh_memory_bounds_slice_t s = lh_memory_bounds_slice_initializer_empty();
    lh_memory_bounds_slice_assign(self, lh_addr_of(s));
}

lh_void
lh_memory_bounds_slice_assign_v(lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid(other),
                          lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_assign(self, other);
}

lh_void
lh_memory_bounds_slice_set_v(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_slice_t s = lh_memory_bounds_slice_initializer(begin, end);
    lh_memory_bounds_slice_assign_v(self, lh_addr_of(s));
}

LH_ATTRIBUTE_STATIC
lh_memory_bounds_slice_t
lh_memory_bounds_slice_make(lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_slice_t slice = lh_memory_bounds_slice_initializer(begin, end);
    return slice;
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_v(lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_slice_t slice = lh_memory_bounds_slice_make(begin, end);
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid(lh_addr_of(slice)),
                          lh_runtime_error_code_invalid_range);
    return slice;
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_by_size(lh_ptr begin, lh_usize_t size)
{
    lh_runtime_assert_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_runtime_assert_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    return lh_memory_bounds_slice_make_v(
        begin, lh_ptr_add_by_offset_unsafe(lh_void, begin, lh_math_sub_one(size)));
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_empty(lh_void)
{
    const lh_memory_bounds_slice_t slice = lh_memory_bounds_slice_initializer_empty();
    return slice;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_slice_swap(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_runtime_assert_ref(self);
    lh_runtime_assert_ref(other);

    lh_algorithm_swap(lh_memory_bounds_slice_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_slice_swap_v_other(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid(other),
                          lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_swap(self, other);
}

lh_void
lh_memory_bounds_slice_swap_v(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_is_valid(self),
                          lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_swap_v_other(self, other);
}

lh_void
lh_memory_bounds_slice_set_by_size(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_runtime_assert_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_runtime_assert_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    lh_ptr end = lh_ptr_add_by_offset_unsafe(lh_void, begin, lh_math_sub_one(size));
    lh_memory_bounds_slice_set(self, begin, end);
}

lh_void
lh_memory_bounds_slice_swap_and_clear(lh_memory_bounds_slice_t *self,
                                      lh_memory_bounds_slice_t *other)
{
    lh_memory_bounds_slice_clear(self);

    if (lh_math_ne(self, other))
    {
        lh_memory_bounds_slice_swap_v_other(self, other);
    }
}

lh_void
lh_memory_bounds_slice_set_value(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset,
                                 lh_byte_t value)
{
    lh_ptr ptr = lh_memory_bounds_slice_get_ptr_from_begin(self, offset);
    lh_ptr_deref(lh_ptr_cast(lh_byte_t, ptr)) = value;
}

lh_void
lh_memory_bounds_slice_init(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    lh_memory_bounds_slice_set_v(self, begin, end);
}

lh_void
lh_memory_bounds_slice_init_by_size(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_memory_bounds_slice_set_by_size(self, begin, size);
}

lh_void
lh_memory_bounds_slice_init_empty(lh_memory_bounds_slice_t *self)
{
    lh_memory_bounds_slice_clear(self);
}

lh_void
lh_memory_bounds_slice_init_by_other(lh_memory_bounds_slice_t *self,
                                     const lh_memory_bounds_slice_t *other)
{
    lh_memory_bounds_slice_assign_v(self, other);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_take_first(lh_ptr begin, lh_ptr end, lh_usize_t n)
{
    if (lh_math_is_zero(n))
    {
        return lh_memory_bounds_slice_make_empty();
    }

    const lh_memory_bounds_slice_t base = lh_memory_bounds_slice_make_v(begin, end);
    const lh_usize_t total = lh_memory_bounds_slice_get_size(lh_addr_of(base));

    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_bounds_slice_make_by_size(begin, n);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_take_last(lh_ptr begin, lh_ptr end, lh_usize_t n)
{
    if (lh_math_is_zero(n))
    {
        return lh_memory_bounds_slice_make_empty();
    }

    const lh_memory_bounds_slice_t base = lh_memory_bounds_slice_make_v(begin, end);
    const lh_usize_t total = lh_memory_bounds_slice_get_size(lh_addr_of(base));
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);

    lh_ptr data = lh_ptr_sub_by_offset_unsafe(lh_void, end, lh_math_sub_one(n));
    return lh_memory_bounds_slice_make_by_size(data, n);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_from_begin(const lh_memory_bounds_slice_t *self, lh_usize_t size)
{
    return lh_memory_bounds_slice_take_first(lh_memory_bounds_slice_get_begin(self),
                                             lh_memory_bounds_slice_get_end(self), size);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_from_end(const lh_memory_bounds_slice_t *self, lh_usize_t size)
{
    return lh_memory_bounds_slice_take_last(lh_memory_bounds_slice_get_begin(self),
                                            lh_memory_bounds_slice_get_end(self), size);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_between(const lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    lh_runtime_assert_ifn(lh_memory_bounds_slice_contains_range(self, begin, end),
                          lh_runtime_error_code_out_of_range);
    return lh_memory_bounds_slice_make_v(begin, end);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_make_from_offset(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset,
                                        lh_usize_t size)
{
    const lh_usize_t total = lh_memory_bounds_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(offset, total), lh_runtime_error_code_out_of_range);

    if (lh_math_is_zero(size))
    {
        return lh_memory_bounds_slice_make_empty();
    }

    lh_runtime_assert_ifn(lh_math_le(size, lh_math_sub(total, offset)),
                          lh_runtime_error_code_out_of_range);

    return lh_memory_bounds_slice_make_by_size(
        lh_memory_bounds_slice_get_ptr_from_begin(self, offset), size);
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_drop_first(const lh_memory_bounds_slice_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_bounds_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_bounds_slice_make_from_offset(self, n, lh_math_sub(total, n));
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_drop_last(const lh_memory_bounds_slice_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_bounds_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_bounds_slice_make_from_begin(self, lh_math_sub(total, n));
}

lh_memory_bounds_slice_t
lh_memory_bounds_slice_trim(const lh_memory_bounds_slice_t *self, lh_usize_t left, lh_usize_t right)
{
    const lh_usize_t total = lh_memory_bounds_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(left, total), lh_runtime_error_code_out_of_range);

    const lh_usize_t rest = lh_math_sub(total, left);
    lh_runtime_assert_ifn(lh_math_le(right, rest), lh_runtime_error_code_out_of_range);

    return lh_memory_bounds_slice_make_from_offset(self, left, lh_math_sub(rest, right));
}
