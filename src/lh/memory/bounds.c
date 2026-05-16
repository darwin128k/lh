#include <lh/memory/bounds.h>
#include <lh/memory/bounds/initializer.h>
#include <lh/attribute/static.h>
#include <lh/util/algorithm.h>
#include <lh/util/interval.h>
#include <lh/runtime/throw.h>
#include <lh/optional/ref.h>
#include <lh/runtime/try.h>
#include <lh/util/return.h>
#include <lh/memory/raw.h>
#include <lh/cast/const.h>
#include <lh/assert.h>

lh_void
lh_memory_bounds_unpack(const lh_memory_bounds_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_memory_bounds_slice_unpack(self, begin, end);
}

lh_ptr
lh_memory_bounds_get_begin(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_slice_get_begin(self);
}

lh_ptr
lh_memory_bounds_get_data(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_begin(self);
}

lh_ptr
lh_memory_bounds_get_end(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_slice_get_end(self);
}

lh_bool_t
lh_memory_bounds_is_uninitialized(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_slice_is_uninitialized(self);
}

lh_bool_t
lh_memory_bounds_is_initialized(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_slice_is_initialized(self);
}

lh_memory_view_slice_direction_t
lh_memory_bounds_get_direction(const lh_memory_bounds_t *self)
{
    lh_memory_view_slice_direction_t direction = lh_memory_view_slice_direction_unknown;
    if (lh_memory_bounds_is_initialized(self))
    {
        lh_void *begin, *end;
        lh_memory_bounds_unpack(self, lh_addr_of(begin), lh_addr_of(end));

        if (lh_interval_ropen_is_valid(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end)))
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
lh_memory_bounds_is_forward(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_direction(self) == lh_memory_view_slice_direction_forward;
}

lh_bool_t
lh_memory_bounds_is_backward(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_direction(self) == lh_memory_view_slice_direction_backward;
}

lh_bool_t
lh_memory_bounds_is_valid(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_is_forward(self);
}

lh_void
lh_memory_bounds_unpack_v(const lh_memory_bounds_t *self, lh_ptr *begin, lh_ptr *end)
{
    lh_assert_runtime_ref(lh_memory_bounds_is_valid(self));
    lh_memory_bounds_unpack(self, begin, end);
}

lh_ptr
lh_memory_bounds_get_begin_v(const lh_memory_bounds_t *self)
{
    lh_ptr begin;
    lh_memory_bounds_unpack_v(self, lh_addr_of(begin), lh_null);
    return begin;
}

lh_ptr
lh_memory_bounds_get_end_v(const lh_memory_bounds_t *self)
{
    lh_ptr end;
    lh_memory_bounds_unpack_v(self, lh_null, lh_addr_of(end));
    return end;
}

lh_usize_t
lh_memory_bounds_get_size(const lh_memory_bounds_t *self)
{
    lh_void *begin, *end;
    lh_memory_bounds_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_ropen_get_size(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_bounds_is_empty(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_is_uninitialized(self) ||
           lh_math_is_zero(lh_memory_bounds_get_size(self));
}

lh_bool_t
lh_memory_bounds_is_valid_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_memory_bounds_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_begin(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(ptr, lh_memory_bounds_get_begin(self));
    lh_assert_runtime_ref(lh_memory_bounds_is_valid_offset(self, offset));
    return offset;
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_end(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    lh_assert_runtime_ref(lh_memory_bounds_contains_ptr(self, ptr));
    return lh_math_sub_one(lh_ptr_udiff(lh_memory_bounds_get_end(self), ptr));
}

lh_bool_t
lh_memory_bounds_contains_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    lh_void *begin, *end;
    lh_memory_bounds_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_ropen_contains_value(begin, end, ptr);
}

lh_bool_t
lh_memory_bounds_contains_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_void *self_begin, *self_end;
    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_ropen_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t
lh_memory_bounds_contains(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_contains_of(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_get_ptr_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ref(lh_memory_bounds_is_valid_offset(self, offset));
    return lh_ptr_add_by_offset_unsafe(lh_void, lh_memory_bounds_get_begin(self), offset);
}

lh_ptr
lh_memory_bounds_get_ptr_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ref(lh_memory_bounds_is_valid_offset(self, offset));
    return lh_ptr_add_by_offset_unsafe(
        lh_void, lh_memory_bounds_get_end(self),
        lh_math_neg(lh_type_cast(lh_soffset_t, lh_math_add_one(offset))));
}

lh_ptr
lh_memory_bounds_get_first_ptr(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_ptr_from_begin(self, 0);
}

lh_ptr
lh_memory_bounds_get_last_ptr(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_ptr_from_end(self, 0);
}

lh_ptr
lh_memory_bounds_get_ptr_by_offset(const lh_memory_bounds_t *self, lh_soffset_t offset)
{
    if (lh_math_is_negative(offset))
    {
        return lh_memory_bounds_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(offset))));
    }

    return lh_memory_bounds_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
}

lh_byte_t
lh_memory_bounds_get_value_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_bounds_get_ptr_from_begin(self, offset)));
}

lh_byte_t
lh_memory_bounds_get_value_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_bounds_get_ptr_from_end(self, offset)));
}

lh_byte_t
lh_memory_bounds_get_value_by_offset(const lh_memory_bounds_t *self, lh_soffset_t offset)
{
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, lh_memory_bounds_get_ptr_by_offset(self, offset)));
}

lh_byte_t
lh_memory_bounds_get_first_value(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_bounds_get_last_value(const lh_memory_bounds_t *self)
{
    return lh_memory_bounds_get_value_from_end(self, 0);
}

lh_uoffset_t
lh_memory_bounds_get_offset_from_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr,
                                     lh_soffset_t offset)
{
    if (lh_ptr_is_null(ptr))
    {
        return lh_memory_bounds_get_offset_from_begin(
            self, lh_memory_bounds_get_ptr_by_offset(self, offset));
    }

    const lh_usize_t size = lh_memory_bounds_get_size(self);
    const lh_uoffset_t ptr_offset = lh_memory_bounds_get_offset_from_begin(self, ptr);

    if (lh_math_is_negative(offset))
    {
        const lh_uoffset_t abs_offset = lh_type_cast(lh_uoffset_t, lh_math_neg(offset));
        lh_assert_runtime_if(lh_interval_ropen_is_sub_overflow(ptr_offset, abs_offset, 0, size),
                             lh_runtime_error_code_underflow);
        return lh_math_sub(ptr_offset, abs_offset);
    }

    lh_assert_runtime_if(
        lh_interval_ropen_is_add_overflow(ptr_offset, lh_type_cast(lh_uoffset_t, offset), 0, size),
        lh_runtime_error_code_overflow);

    return lh_math_add(ptr_offset, lh_type_cast(lh_uoffset_t, offset));
}

const lh_ptr
lh_memory_bounds_seek_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr, lh_soffset_t offset)
{
    lh_runtime_try(e)
    {
        lh_uoffset_t cur = lh_memory_bounds_get_offset_from_ptr(self, ptr, offset);
        ptr = lh_memory_bounds_get_ptr_from_begin(self, cur);
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
lh_memory_bounds_next_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    return lh_memory_bounds_seek_ptr(self, ptr, 1);
}

const lh_ptr
lh_memory_bounds_prev_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    return lh_memory_bounds_seek_ptr(self, ptr, -1);
}

lh_byte_t
lh_memory_bounds_seek_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_ptr seek_ptr = lh_memory_bounds_seek_ptr(self, ptr, 0);
    lh_assert_runtime_ifn(lh_ptr_is_set(seek_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, seek_ptr));
}

lh_byte_t
lh_memory_bounds_next_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_ptr next_ptr = lh_memory_bounds_next_ptr(self, ptr);
    lh_assert_runtime_ifn(lh_ptr_is_set(next_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, next_ptr));
}

lh_byte_t
lh_memory_bounds_prev_value(const lh_memory_bounds_t *self, const lh_ptr ptr)
{
    const lh_ptr prev_ptr = lh_memory_bounds_prev_ptr(self, ptr);
    lh_assert_runtime_ifn(lh_ptr_is_set(prev_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(lh_byte_t, prev_ptr));
}

lh_bool_t
lh_memory_bounds_overlaps_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_void *self_begin, *self_end;
    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_ropen_overlaps_range(lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(self_end),
                                            lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_bounds_overlaps_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end)
{
    return lh_memory_bounds_overlaps_of(self, begin, end);
}

lh_bool_t
lh_memory_bounds_overlaps(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_void *other_begin, *other_end;
    lh_memory_bounds_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_overlaps_of(self, other_begin, other_end);
}

lh_bool_t
lh_memory_bounds_overlaps_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_assert_runtime_ifn(lh_memory_bounds_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_bounds_overlaps(self, other);
}

lh_bool_t
lh_memory_bounds_multiple_of(const lh_memory_bounds_t *self, lh_usize_t alignment)
{
    lh_assert_runtime_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    const lh_usize_t size = lh_memory_bounds_get_size(self);
    return lh_math_is_zero(lh_math_mod(size, alignment));
}

lh_bool_t
lh_memory_bounds_is_multiple_of(const lh_memory_bounds_t *self, lh_usize_t multiple)
{
    return lh_memory_bounds_multiple_of(self, multiple);
}

lh_bool_t
lh_memory_bounds_aligned_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align)
{
    const lh_ptr begin = lh_memory_bounds_get_begin_v(self);
    lh_assert_runtime_ifn(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t
lh_memory_bounds_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align)
{
    return lh_memory_bounds_aligned_is_begin_aligned(self, align);
}

lh_bool_t
lh_memory_bounds_is_aligned(const lh_memory_bounds_t *self, lh_usize_t align)
{
    const lh_bool_t is_begin_aligned = lh_memory_bounds_aligned_is_begin_aligned(self, align);
    const lh_ptr end = lh_memory_bounds_get_end_v(self);
    return is_begin_aligned && lh_ptr_is_aligned(end, align);
}

lh_bool_t
lh_memory_bounds_equals_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_bounds_slice_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_bounds_equals_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_bounds_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_bounds_equals(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    return lh_memory_bounds_slice_equals(self, other);
}

lh_ptr
lh_memory_bounds_copy_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_copy(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_copy(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_copy_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_move_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_move(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_move(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_move_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_find_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_find(self_begin, self_end, begin, end));
}

lh_ptr
lh_memory_bounds_find(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_find_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_rfind_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_rfind(self_begin, self_end, begin, end));
}

lh_ptr
lh_memory_bounds_rfind(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_rfind_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_compare_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_compare(self_begin, self_end, begin, end));
}

lh_ptr
lh_memory_bounds_compare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_compare_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_rcompare_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_cast_const(lh_ptr, lh_memory_raw_rcompare(self_begin, self_end, begin, end));
}

lh_ptr
lh_memory_bounds_rcompare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_rcompare_range(self, other_begin, other_end);
}

lh_ptr
lh_memory_bounds_fill(lh_memory_bounds_t *self, lh_byte_t value)
{
    lh_ptr begin;
    lh_ptr end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_raw_set(begin, end, value);
}

lh_ptr
lh_memory_bounds_fill_pattern_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_ptr self_begin;
    lh_ptr self_end;

    lh_memory_bounds_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_set_pattern(self_begin, self_end, begin, end);
}

lh_ptr
lh_memory_bounds_fill_pattern(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_ptr other_begin;
    lh_ptr other_end;

    lh_memory_bounds_unpack_v(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    return lh_memory_bounds_fill_pattern_range(self, other_begin, other_end);
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_set(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    lh_assert_runtime_ref(self);

    self->first = begin;
    self->second = end;
}

lh_void
lh_memory_bounds_assign(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_void *other_begin, *other_end;
    lh_memory_bounds_unpack(other, lh_addr_of(other_begin), lh_addr_of(other_end));
    lh_memory_bounds_set(self, other_begin, other_end);
}

lh_void
lh_memory_bounds_clear(lh_memory_bounds_t *self)
{
    const lh_memory_bounds_t s = lh_memory_bounds_empty_initializer();
    lh_memory_bounds_assign(self, lh_addr_of(s));
}

lh_void
lh_memory_bounds_assign_v(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_assert_runtime_ifn(lh_memory_bounds_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_assign(self, other);
}

lh_void
lh_memory_bounds_set_v(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_initializer(begin, end);
    lh_memory_bounds_assign_v(self, lh_addr_of(bounds));
}

LH_ATTRIBUTE_STATIC
lh_memory_bounds_t
lh_memory_bounds_make(lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_initializer(begin, end);
    return bounds;
}

lh_memory_bounds_t
lh_memory_bounds_make_v(lh_ptr begin, lh_ptr end)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_make(begin, end);
    lh_assert_runtime_ifn(lh_memory_bounds_is_valid(lh_addr_of(bounds)),
                          lh_runtime_error_code_invalid_range);
    return bounds;
}

lh_memory_bounds_t
lh_memory_bounds_make_by_size(lh_ptr begin, lh_usize_t size)
{
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    return lh_memory_bounds_make_v(begin, lh_ptr_add_by_offset_unsafe(lh_void, begin, size));
}

lh_memory_bounds_t
lh_memory_bounds_make_empty(lh_void)
{
    const lh_memory_bounds_t bounds = lh_memory_bounds_empty_initializer();
    return bounds;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_swap(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    lh_algorithm_swap(lh_memory_bounds_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_bounds_swap_v_other(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_assert_runtime_ifn(lh_memory_bounds_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_swap(self, other);
}

lh_void
lh_memory_bounds_swap_v(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_assert_runtime_ifn(lh_memory_bounds_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_bounds_swap_v_other(self, other);
}

lh_void
lh_memory_bounds_set_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    lh_ptr end = lh_ptr_add_by_offset_unsafe(lh_void, begin, size);
    lh_memory_bounds_set(self, begin, end);
}

lh_void
lh_memory_bounds_swap_and_clear(lh_memory_bounds_t *self, lh_memory_bounds_t *other)
{
    lh_memory_bounds_clear(self);

    if (lh_math_ne(self, other))
    {
        lh_memory_bounds_swap_v_other(self, other);
    }
}

lh_void
lh_memory_bounds_set_value(const lh_memory_bounds_t *self, lh_uoffset_t offset, lh_byte_t value)
{
    lh_ptr ptr = lh_memory_bounds_get_ptr_from_begin(self, offset);
    lh_ptr_deref(lh_ptr_cast(lh_byte_t, ptr)) = value;
}

lh_void
lh_memory_bounds_init(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end)
{
    lh_memory_bounds_set_v(self, begin, end);
}

lh_void
lh_memory_bounds_init_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size)
{
    lh_memory_bounds_set_by_size(self, begin, size);
}

lh_void
lh_memory_bounds_init_empty(lh_memory_bounds_t *self)
{
    lh_memory_bounds_clear(self);
}

lh_void
lh_memory_bounds_init_by_other(lh_memory_bounds_t *self, const lh_memory_bounds_t *other)
{
    lh_memory_bounds_assign_v(self, other);
}

lh_memory_bounds_slice_t
lh_memory_bounds_make_slice(const lh_memory_bounds_t *self)
{
    if (lh_memory_bounds_is_uninitialized(self))
    {
        return lh_memory_bounds_slice_make_empty();
    }

    lh_void *begin, *end;
    lh_memory_bounds_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_bounds_slice_make_v(begin, lh_ptr_sub_by_offset_unsafe(lh_void, end, 1U));
}
