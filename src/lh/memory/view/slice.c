#include <lh/memory/view/slice.h>
#include <lh/attribute/static.h>
#include <lh/util/return.h>
#include <lh/runtime/assert.h>
#include <lh/runtime/throw.h>
#include <lh/runtime/try.h>
#include <lh/optional/ref.h>
#include <lh/util/interval.h>

lh_void
lh_memory_view_slice_unpack(const lh_memory_view_slice_t *self, const lh_ptr *begin,
                            const lh_ptr *end)
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

const lh_ptr
lh_memory_view_slice_get_begin(const lh_memory_view_slice_t *self)
{
    const lh_ptr begin;
    lh_memory_view_slice_unpack(self, lh_addr_of(begin), lh_null);
    return begin;
}

const lh_ptr
lh_memory_view_slice_get_data(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_begin(self);
}

const lh_ptr
lh_memory_view_slice_get_end(const lh_memory_view_slice_t *self)
{
    const lh_ptr end;
    lh_memory_view_slice_unpack(self, lh_null, lh_addr_of(end));
    return end;
}

lh_memory_view_slice_flags_t
lh_memory_view_slice_get_flags(const lh_memory_view_slice_t *self)
{
    lh_memory_view_slice_flags_t flags = lh_memory_view_slice_flags_uninitialized;

    const lh_void *begin, *end;
    lh_memory_view_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

    if (lh_ptr_is_set(begin))
    {
        lh_bit_set(flags, lh_memory_view_slice_flags_initialized_begin);
    }

    if (lh_ptr_is_set(end))
    {
        lh_bit_set(flags, lh_memory_view_slice_flags_initialized_end);
    }

    return flags;
}

lh_bool_t
lh_memory_view_slice_is_uninitialized(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_flags(self) == lh_memory_view_slice_flags_uninitialized;
}

lh_bool_t
lh_memory_view_slice_is_initialized(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_flags(self) == lh_memory_view_slice_flags_initialized;
}

lh_memory_view_slice_direction_t
lh_memory_view_slice_get_direction(const lh_memory_view_slice_t *self)
{
    lh_memory_view_slice_direction_t direction = lh_memory_view_slice_direction_unknown;
    if (lh_memory_view_slice_is_initialized(self))
    {
        const lh_ptr begin;
        const lh_ptr end;
        lh_memory_view_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

        if (lh_interval_closed_is_valid(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end)))
        {
            direction = lh_memory_view_slice_direction_forward;
        }
        else
        {
            direction = lh_memory_view_slice_direction_backward;
        }
    }
    return direction;
}

lh_bool_t
lh_memory_view_slice_is_forward(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_direction(self) == lh_memory_view_slice_direction_forward;
}

lh_bool_t
lh_memory_view_slice_is_backward(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_direction(self) == lh_memory_view_slice_direction_backward;
}

lh_bool_t
lh_memory_view_slice_is_valid(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_is_forward(self);
}

lh_void
lh_memory_view_slice_unpack_v(const lh_memory_view_slice_t *self, const lh_ptr *begin,
                              const lh_ptr *end)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_view_slice_unpack(self, begin, end);
}

const lh_ptr
lh_memory_view_slice_get_begin_v(const lh_memory_view_slice_t *self)
{
    const lh_ptr begin;
    lh_memory_view_slice_unpack_v(self, lh_addr_of(begin), lh_null);
    return begin;
}

const lh_ptr
lh_memory_view_slice_get_end_v(const lh_memory_view_slice_t *self)
{
    const lh_ptr end;
    lh_memory_view_slice_unpack_v(self, lh_null, lh_addr_of(end));
    return end;
}

lh_usize_t
lh_memory_view_slice_get_size(const lh_memory_view_slice_t *self)
{
    const lh_void *begin, *end;
    lh_memory_view_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_get_size(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_slice_is_empty(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_is_uninitialized(self) ||
           lh_math_is_zero(lh_memory_view_slice_get_size(self));
}

lh_bool_t
lh_memory_view_slice_is_valid_offset(const lh_memory_view_slice_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_memory_view_slice_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_uoffset_t
lh_memory_view_slice_get_offset_from_begin(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(ptr, lh_memory_view_slice_get_begin(self));
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return offset;
}

lh_uoffset_t
lh_memory_view_slice_get_offset_from_end(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(lh_memory_view_slice_get_end(self), ptr);
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return offset;
}

lh_bool_t
lh_memory_view_slice_contains_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    const lh_void *begin, *end;
    lh_memory_view_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_contains_value(begin, end, ptr);
}

lh_bool_t
lh_memory_view_slice_contains_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;
    lh_memory_view_slice_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_closed_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t
lh_memory_view_slice_contains(const lh_memory_view_slice_t *self,
                              const lh_memory_view_slice_t *other)
{
    const lh_void *begin, *end;
    lh_memory_view_slice_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_slice_contains_range(self, begin, end);
}

lh_bool_t
lh_memory_view_slice_overlaps_of(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;
    lh_memory_view_slice_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_closed_overlaps_range(lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(self_end),
                                             lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_slice_overlaps(const lh_memory_view_slice_t *self,
                              const lh_memory_view_slice_t *other)
{
    const lh_void *begin, *end;
    lh_memory_view_slice_unpack(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_slice_overlaps_of(self, begin, end);
}

lh_bool_t
lh_memory_view_slice_overlaps_v(const lh_memory_view_slice_t *self,
                                const lh_memory_view_slice_t *other)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid(other),
                          lh_runtime_error_code_invalid_range);
    return lh_memory_view_slice_overlaps(self, other);
}

lh_bool_t
lh_memory_view_slice_multiple_of(const lh_memory_view_slice_t *self, lh_usize_t alignment)
{
    lh_runtime_assert_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    const lh_usize_t size = lh_memory_view_slice_get_size(self);
    return lh_math_is_zero(lh_math_mod(size, alignment));
}

lh_bool_t
lh_memory_view_slice_aligned_is_begin_aligned(const lh_memory_view_slice_t *self, lh_usize_t align)
{
    const lh_ptr begin = lh_memory_view_slice_get_begin_v(self);
    lh_runtime_assert_ifn(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t
lh_memory_view_slice_is_aligned(const lh_memory_view_slice_t *self, lh_usize_t align)
{
    const lh_bool_t is_begin_aligned = lh_memory_view_slice_aligned_is_begin_aligned(self, align);
    const lh_ptr end = lh_memory_view_slice_get_end_v(self);
    return is_begin_aligned && lh_ptr_is_aligned(end, align);
}

lh_bool_t
lh_memory_view_slice_equals_of(const lh_memory_view_slice_t *self, const lh_ptr begin,
                               const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;
    lh_memory_view_slice_unpack(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_ptr_eq(self_begin, begin) && lh_ptr_eq(self_end, end);
}

lh_bool_t
lh_memory_view_slice_equals(const lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other)
{
    const lh_void *begin, *end;
    lh_memory_view_slice_unpack(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_slice_equals_of(self, begin, end);
}

const lh_ptr
lh_memory_view_slice_get_ptr_from_begin(const lh_memory_view_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_slice_get_begin(self), offset);
}

const lh_ptr
lh_memory_view_slice_get_ptr_from_end(const lh_memory_view_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_slice_get_end(self),
                                       lh_math_neg(lh_type_cast(lh_soffset_t, offset)));
}

const lh_ptr
lh_memory_view_slice_get_ptr_by_offset(const lh_memory_view_slice_t *self, lh_soffset_t offset)
{
    if (lh_math_is_negative(offset))
    {
        return lh_memory_view_slice_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(offset))));
    }

    return lh_memory_view_slice_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
}

lh_byte_t
lh_memory_view_slice_get_value_from_begin(const lh_memory_view_slice_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_slice_get_ptr_from_begin(self, offset)));
}

lh_byte_t
lh_memory_view_slice_get_value_from_end(const lh_memory_view_slice_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_slice_get_ptr_from_end(self, offset)));
}

lh_byte_t
lh_memory_view_slice_get_value_by_offset(const lh_memory_view_slice_t *self, lh_soffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_slice_get_ptr_by_offset(self, offset)));
}

lh_byte_t
lh_memory_view_slice_get_first_value(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_view_slice_get_last_value(const lh_memory_view_slice_t *self)
{
    return lh_memory_view_slice_get_value_from_end(self, 0);
}

lh_uoffset_t
lh_memory_view_slice_get_offset_from_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr,
                                         lh_soffset_t offset)
{
    if (lh_ptr_is_null(ptr))
    {
        return lh_memory_view_slice_get_offset_from_begin(
            self, lh_memory_view_slice_get_ptr_by_offset(self, offset));
    }

    const lh_usize_t size = lh_memory_view_slice_get_size(self);
    const lh_uoffset_t ptr_offset = lh_memory_view_slice_get_offset_from_begin(self, ptr);
    const lh_uoffset_t max_offset = lh_math_sub_one(size);

    if (lh_math_is_negative(offset))
    {
        const lh_uoffset_t abs_offset = lh_type_cast(lh_uoffset_t, lh_math_neg(offset));
        lh_runtime_assert_if(
            lh_interval_closed_is_sub_overflow(ptr_offset, abs_offset, 0, max_offset),
            lh_runtime_error_code_underflow);

        return lh_math_sub(ptr_offset, abs_offset);
    }

    lh_runtime_assert_if(lh_interval_closed_is_add_overflow(
                             ptr_offset, lh_type_cast(lh_uoffset_t, offset), 0, max_offset),
                         lh_runtime_error_code_overflow);

    return lh_math_add(ptr_offset, lh_type_cast(lh_uoffset_t, offset));
}

const lh_ptr
lh_memory_view_slice_seek_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr,
                              lh_soffset_t offset)
{
    lh_runtime_try(e)
    {
        lh_uoffset_t cur = lh_memory_view_slice_get_offset_from_ptr(self, ptr, offset);
        ptr = lh_memory_view_slice_get_ptr_from_begin(self, cur);
        lh_runtime_try_return(ptr);
    }
    lh_runtime_catch
    {
        if (!lh_exception_catch_has_code(lh_addr_of(e), lh_runtime_error_code_overflow) &&
            !lh_exception_catch_has_code(lh_addr_of(e), lh_runtime_error_code_underflow))
        {
            lh_runtime_rethrow();
        }

        return lh_null;
    }
}

const lh_ptr
lh_memory_view_slice_next_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    return lh_memory_view_slice_seek_ptr(self, ptr, 1);
}

const lh_ptr
lh_memory_view_slice_prev_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    return lh_memory_view_slice_seek_ptr(self, ptr, -1);
}

lh_byte_t
lh_memory_view_slice_seek_value(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    const lh_ptr seek_ptr = lh_memory_view_slice_seek_ptr(self, ptr, 0);
    lh_runtime_assert_ifn(lh_ptr_is_set(seek_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, seek_ptr));
}

lh_byte_t
lh_memory_view_slice_next_value(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    const lh_ptr next_ptr = lh_memory_view_slice_next_ptr(self, ptr);
    lh_runtime_assert_ifn(lh_ptr_is_set(next_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, next_ptr));
}

lh_byte_t
lh_memory_view_slice_prev_value(const lh_memory_view_slice_t *self, const lh_ptr ptr)
{
    const lh_ptr prev_ptr = lh_memory_view_slice_prev_ptr(self, ptr);
    lh_runtime_assert_ifn(lh_ptr_is_set(prev_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, prev_ptr));
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_view_slice_set(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_runtime_assert_ref(self);

    self->first = begin;
    self->second = end;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_view_slice_assign(lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    const lh_void *begin, *end;
    lh_memory_view_slice_unpack(other, lh_addr_of(begin), lh_addr_of(end));
    lh_memory_view_slice_set(self, begin, end);
}

lh_void
lh_memory_view_slice_clear(lh_memory_view_slice_t *self)
{
    const lh_memory_view_slice_t s = lh_memory_view_slice_initializer_empty();
    lh_memory_view_slice_assign(self, lh_addr_of(s));
}

lh_void
lh_memory_view_slice_assign_v(lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid(other),
                          lh_runtime_error_code_invalid_range);
    lh_memory_view_slice_assign(self, other);
}

lh_void
lh_memory_view_slice_set_v(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_slice_t s = lh_memory_view_slice_make_v(begin, end);
    lh_memory_view_slice_assign(self, lh_addr_of(s));
}

LH_ATTRIBUTE_STATIC
lh_memory_view_slice_t
lh_memory_view_slice_make(const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_initializer(begin, end);
    return slice;
}

lh_memory_view_slice_t
lh_memory_view_slice_make_v(const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_make(begin, end);
    lh_runtime_assert_ifn(lh_memory_view_slice_is_valid(lh_addr_of(slice)),
                          lh_runtime_error_code_invalid_range);
    return slice;
}

lh_memory_view_slice_t
lh_memory_view_slice_make_by_size(const lh_ptr begin, lh_usize_t size)
{
    lh_runtime_assert_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_runtime_assert_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    return lh_memory_view_slice_make_v(
        begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, lh_math_sub_one(size)));
}

lh_memory_view_slice_t
lh_memory_view_slice_make_empty(lh_void)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_initializer_empty();
    return slice;
}

lh_memory_view_slice_t
lh_memory_view_slice_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n)
{
    if (lh_math_is_zero(n))
    {
        return lh_memory_view_slice_make_empty();
    }

    const lh_memory_view_slice_t base = lh_memory_view_slice_make_v(begin, end);
    const lh_usize_t total = lh_memory_view_slice_get_size(lh_addr_of(base));

    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_view_slice_make_by_size(begin, n);
}

lh_memory_view_slice_t
lh_memory_view_slice_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n)
{
    if (lh_math_is_zero(n))
    {
        return lh_memory_view_slice_make_empty();
    }

    const lh_memory_view_slice_t base = lh_memory_view_slice_make_v(begin, end);
    const lh_usize_t total = lh_memory_view_slice_get_size(lh_addr_of(base));
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);

    const lh_ptr data = lh_ptr_sub_by_offset_unsafe(const lh_void, end, lh_math_sub_one(n));
    return lh_memory_view_slice_make_by_size(data, n);
}

lh_memory_view_slice_t
lh_memory_view_slice_make_from_begin(const lh_memory_view_slice_t *self, lh_usize_t size)
{
    return lh_memory_view_slice_take_first(lh_memory_view_slice_get_begin(self),
                                           lh_memory_view_slice_get_end(self), size);
}

lh_memory_view_slice_t
lh_memory_view_slice_make_from_end(const lh_memory_view_slice_t *self, lh_usize_t size)
{
    return lh_memory_view_slice_take_last(lh_memory_view_slice_get_begin(self),
                                          lh_memory_view_slice_get_end(self), size);
}

lh_memory_view_slice_t
lh_memory_view_slice_make_between(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                  const lh_ptr end)
{
    lh_runtime_assert_ifn(lh_memory_view_slice_contains_range(self, begin, end),
                          lh_runtime_error_code_out_of_range);
    return lh_memory_view_slice_make_v(begin, end);
}

lh_memory_view_slice_t
lh_memory_view_slice_make_from_offset(const lh_memory_view_slice_t *self, lh_uoffset_t offset,
                                      lh_usize_t size)
{
    const lh_usize_t total = lh_memory_view_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(offset, total), lh_runtime_error_code_out_of_range);

    if (lh_math_is_zero(size))
    {
        return lh_memory_view_slice_make_empty();
    }

    lh_runtime_assert_ifn(lh_math_le(size, lh_math_sub(total, offset)),
                          lh_runtime_error_code_out_of_range);

    return lh_memory_view_slice_make_by_size(lh_memory_view_slice_get_ptr_from_begin(self, offset),
                                             size);
}

lh_memory_view_slice_t
lh_memory_view_slice_drop_first(const lh_memory_view_slice_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_view_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_view_slice_make_from_offset(self, n, lh_math_sub(total, n));
}

lh_memory_view_slice_t
lh_memory_view_slice_drop_last(const lh_memory_view_slice_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_view_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_view_slice_make_from_begin(self, lh_math_sub(total, n));
}

lh_memory_view_slice_t
lh_memory_view_slice_trim(const lh_memory_view_slice_t *self, lh_usize_t left, lh_usize_t right)
{
    const lh_usize_t total = lh_memory_view_slice_get_size(self);
    lh_runtime_assert_ifn(lh_math_le(left, total), lh_runtime_error_code_out_of_range);

    const lh_usize_t rest = lh_math_sub(total, left);
    lh_runtime_assert_ifn(lh_math_le(right, rest), lh_runtime_error_code_out_of_range);

    return lh_memory_view_slice_make_from_offset(self, left, lh_math_sub(rest, right));
}

lh_void
lh_memory_view_slice_set_by_size(lh_memory_view_slice_t *self, const lh_ptr begin, lh_usize_t size)
{
    const lh_memory_view_slice_t s = lh_memory_view_slice_make_by_size(begin, size);
    lh_memory_view_slice_assign(self, lh_addr_of(s));
}

lh_void
lh_memory_view_slice_init(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_slice_set_v(self, begin, end);
}

lh_void
lh_memory_view_slice_init_by_size(lh_memory_view_slice_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_slice_set_by_size(self, begin, size);
}

lh_void
lh_memory_view_slice_init_empty(lh_memory_view_slice_t *self)
{
    lh_memory_view_slice_clear(self);
}

lh_void
lh_memory_view_slice_init_by_other(lh_memory_view_slice_t *self,
                                   const lh_memory_view_slice_t *other)
{
    lh_memory_view_slice_assign_v(self, other);
}
