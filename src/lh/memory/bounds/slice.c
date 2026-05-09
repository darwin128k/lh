#include <lh/memory/bounds/slice/initializer.h>
#include <lh/memory/bounds/slice.h>
#include <lh/runtime/check/ref.h>
#include <lh/util/algorithm.h>
#include <lh/util/interval.h>
#include <lh/runtime/return/if.h>
#include <lh/runtime/throw.h>
#include <lh/optional/ref.h>
#include <lh/runtime/try.h>
#include <lh/util/ptr.h>

lh_void
lh_memory_bounds_slice_unpack(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_runtime_check_ref(self);

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
lh_memory_bounds_slice_get_end(const lh_memory_bounds_slice_t *self)
{
    lh_ptr end;
    lh_memory_bounds_slice_unpack(self, lh_null, lh_addr_of(end));
    return end;
}

lh_memory_bounds_slice_flags_t
lh_memory_bounds_slice_get_flags(const lh_memory_bounds_slice_t *self)
{
    lh_memory_bounds_slice_flags_t flags = lh_memory_bounds_slice_flags_uninitialized;

    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

    if (lh_ptr_is_set(begin))
    {
        lh_bit_set(flags, lh_memory_bounds_slice_flags_initialized_begin);
    }

    if (lh_ptr_is_set(end))
    {
        lh_bit_set(flags, lh_memory_bounds_slice_flags_initialized_end);
    }

    return flags;
}

lh_bool_t
lh_memory_bounds_slice_is_uninitialized(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_bounds_slice_flags_uninitialized;
}

lh_bool_t
lh_memory_bounds_slice_is_initialized(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_flags(self) == lh_memory_bounds_slice_flags_initialized;
}

lh_memory_bounds_slice_direction_t
lh_memory_bounds_slice_get_direction(const lh_memory_bounds_slice_t *self)
{
    lh_memory_bounds_slice_direction_t direction = lh_memory_bounds_slice_direction_unknown;
    if (lh_memory_bounds_slice_is_initialized(self))
    {
        lh_void *begin, *end;
        lh_memory_bounds_slice_unpack(self, lh_addr_of(begin), lh_addr_of(end));

        if (lh_interval_closed_is_valid(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end)))
        {
            direction = lh_memory_bounds_slice_direction_forward;
        }
        else
        {
            direction = lh_memory_bounds_slice_direction_backward;
        }
    }
    return direction;
}

lh_bool_t
lh_memory_bounds_slice_is_forward(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_bounds_slice_direction_forward;
}

lh_bool_t
lh_memory_bounds_slice_is_backward(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_direction(self) == lh_memory_bounds_slice_direction_backward;
}

lh_bool_t
lh_memory_bounds_slice_is_valid(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_is_forward(self);
}

lh_void
lh_memory_bounds_slice_unpack_v(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid(self), lh_runtime_error_code_invalid_range);
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
    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_get_size(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_bounds_slice_is_empty(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_is_uninitialized(self) ||
           lh_math_is_zero(lh_memory_bounds_slice_get_size(self));
}

lh_bool_t
lh_memory_bounds_slice_is_valid_offset(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_memory_bounds_slice_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_begin(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(ptr, lh_memory_bounds_slice_get_begin(self));
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return offset;
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_end(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(lh_memory_bounds_slice_get_end(self), ptr);
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return offset;
}

lh_bool_t
lh_memory_bounds_slice_contains_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    lh_void *begin, *end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_closed_contains_value(begin, end, ptr);
}

lh_bool_t
lh_memory_bounds_slice_contains_range(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                      const lh_ptr end)
{
    lh_void *self_begin, *self_end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_closed_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t
lh_memory_bounds_slice_contains(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_slice_contains_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_begin(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_begin(self), offset);
}

lh_ptr
lh_memory_bounds_slice_get_ptr_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, offset),
                     lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_slice_get_end(self),
                                       lh_math_neg(lh_type_cast(lh_soffset_t, offset)));
}

lh_ptr
lh_memory_bounds_slice_get_ptr_by_offset(const lh_memory_bounds_slice_t *self, lh_soffset_t offset)
{
    if (lh_math_ge(offset, 0))
    {
        return lh_memory_bounds_slice_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
    }
    else
    {
        return lh_memory_bounds_slice_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(offset))));
    }
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_begin(const lh_memory_bounds_slice_t *self,
                                            lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr_from_begin(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_value_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr_from_end(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_value_by_offset(const lh_memory_bounds_slice_t *self,
                                           lh_soffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(lh_byte_t, lh_memory_bounds_slice_get_ptr_by_offset(self, offset)));
}

lh_byte_t
lh_memory_bounds_slice_get_begin_value(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_bounds_slice_get_end_value(const lh_memory_bounds_slice_t *self)
{
    return lh_memory_bounds_slice_get_value_from_end(self, 0);
}

lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                           lh_soffset_t offset)
{
    if (lh_ptr_is_null(ptr))
    {
        return lh_memory_bounds_slice_get_offset_from_begin(
            self, lh_memory_bounds_slice_get_ptr_by_offset(self, offset));
    }

    const lh_uoffset_t ptr_offset = lh_memory_bounds_slice_get_offset_from_begin(self, ptr);
    if (lh_math_is_negative(offset))
    {
        const lh_uoffset_t abs_offset = lh_type_cast(lh_uoffset_t, lh_math_neg(offset));
        lh_runtime_check_if(lh_interval_closed_is_sub_overflow(ptr_offset, abs_offset,
                                                               LH_UOFFSET_T_MIN, LH_UOFFSET_T_MAX),
                            lh_runtime_error_code_underflow);
        return lh_math_sub(ptr_offset, abs_offset);
    }

    lh_runtime_check_if(
        lh_interval_closed_is_add_overflow(ptr_offset, offset, LH_UOFFSET_T_MIN, LH_UOFFSET_T_MAX),
        lh_runtime_error_code_overflow);

    const lh_uoffset_t target_offset = lh_math_add(ptr_offset, offset);
    lh_runtime_check(lh_memory_bounds_slice_is_valid_offset(self, target_offset),
                     lh_runtime_error_code_overflow);

    return target_offset;
}

const lh_ptr
lh_memory_bounds_slice_seek_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                lh_soffset_t offset)
{
    lh_runtime_try(e)
    {
        offset = lh_memory_bounds_slice_get_offset_from_ptr(self, ptr, offset);
        ptr = lh_memory_bounds_slice_get_ptr_from_begin(self, offset);
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
    const lh_ptr seek_ptr = lh_memory_bounds_slice_seek_ptr(self, ptr, 0);
    lh_runtime_check(lh_ptr_is_set(seek_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, seek_ptr));
}

lh_byte_t
lh_memory_bounds_slice_next_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_ptr next_ptr = lh_memory_bounds_slice_next_ptr(self, ptr);
    lh_runtime_check(lh_ptr_is_set(next_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, next_ptr));
}

lh_byte_t
lh_memory_bounds_slice_prev_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr)
{
    const lh_ptr prev_ptr = lh_memory_bounds_slice_prev_ptr(self, ptr);
    lh_runtime_check(lh_ptr_is_set(prev_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, prev_ptr));
}

lh_bool_t
lh_memory_bounds_slice_overlaps_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end)
{
    lh_void *self_begin, *self_end;
    lh_memory_bounds_slice_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_closed_overlaps_range(lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(self_end),
                                             lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_bounds_slice_overlaps(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_slice_overlaps_of(self, other_begin, other_end);
}

lh_bool_t
lh_memory_bounds_slice_overlaps_v(const lh_memory_bounds_slice_t *self,
                                  const lh_memory_bounds_slice_t *other)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid(other), lh_runtime_error_code_invalid_range);

    return lh_memory_bounds_slice_overlaps(self, other);
}

lh_bool_t
lh_memory_bounds_slice_multiple_of(const lh_memory_bounds_slice_t *self, lh_usize_t alignment)
{
    lh_runtime_check_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    const lh_usize_t size = lh_memory_bounds_slice_get_size(self);
    return lh_math_is_zero(lh_math_mod(size, alignment));
}

lh_bool_t
lh_memory_bounds_slice_aligned_is_begin_aligned(const lh_memory_bounds_slice_t *self,
                                                lh_usize_t align)
{
    const lh_ptr begin = lh_memory_bounds_slice_get_begin_v(self);
    lh_runtime_check(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t
lh_memory_bounds_slice_is_aligned(const lh_memory_bounds_slice_t *self, lh_usize_t align)
{
    const lh_bool_t is_begin_aligned = lh_memory_bounds_slice_aligned_is_begin_aligned(self, align);
    const lh_ptr end = lh_memory_bounds_slice_get_end_v(self);
    return is_begin_aligned && lh_ptr_is_aligned(end, align);
}

lh_bool_t
lh_memory_bounds_slice_equals_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end)
{
    lh_void *self_begin, *self_end;
    lh_memory_bounds_slice_unpack(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_ptr_eq(self_begin, begin) && lh_ptr_eq(self_end, end);
}

lh_bool_t
lh_memory_bounds_slice_equals(const lh_memory_bounds_slice_t *self,
                              const lh_memory_bounds_slice_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_slice_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_slice_equals_of(self, other_begin, other_end);
}

LH_ATTRIBUTE_BUILTIN
lh_void
lh_memory_bounds_slice_set(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    lh_runtime_check_ref(self);

    self->first = begin;
    self->second = end;
}

LH_ATTRIBUTE_BUILTIN
lh_void
lh_memory_bounds_slice_assign(lh_memory_bounds_slice_t *self, const lh_memory_bounds_slice_t *other)
{
    lh_runtime_return_if(lh_math_eq(self, other));

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
    lh_runtime_check(lh_memory_bounds_slice_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_assign(self, other);
}

lh_void
lh_memory_bounds_slice_set_v(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_slice_t s = lh_memory_bounds_slice_initializer(begin, end);
    lh_memory_bounds_slice_assign_v(self, lh_addr_of(s));
}

LH_ATTRIBUTE_BUILTIN
lh_void
lh_memory_bounds_slice_swap(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_runtime_return_if(lh_math_eq(self, other));

    lh_runtime_check_ref(self);
    lh_runtime_check_ref(other);

    lh_algorithm_swap(lh_memory_bounds_slice_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

LH_ATTRIBUTE_BUILTIN
lh_void
lh_memory_bounds_slice_swap_v_other(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_swap(self, other);
}

lh_void
lh_memory_bounds_slice_swap_v(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other)
{
    lh_runtime_check(lh_memory_bounds_slice_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_slice_swap_v_other(self, other);
}

lh_void
lh_memory_bounds_slice_set_by_size(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_runtime_check(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_runtime_check_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

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
