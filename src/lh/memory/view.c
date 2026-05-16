#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/util/return.h>
#include <lh/attribute/static.h>
#include <lh/util/algorithm.h>
#include <lh/util/interval.h>
#include <lh/runtime/throw.h>
#include <lh/optional/ref.h>
#include <lh/runtime/try.h>
#include <lh/memory/raw.h>
#include <lh/assert.h>

lh_void
lh_memory_view_unpack(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end)
{
    lh_memory_view_slice_unpack(self, begin, end);
}

const lh_ptr
lh_memory_view_get_begin(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_begin(self);
}

const lh_ptr
lh_memory_view_get_data(const lh_memory_view_t *self)
{
    return lh_memory_view_get_begin(self);
}

const lh_ptr
lh_memory_view_get_end(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_end(self);
}

lh_memory_view_slice_direction_t
lh_memory_view_get_direction(const lh_memory_view_t *self)
{
    lh_memory_view_slice_direction_t direction = lh_memory_view_slice_direction_unknown;
    if (lh_memory_view_is_initialized(self))
    {
        const lh_ptr begin;
        const lh_ptr end;
        lh_memory_view_unpack(self, lh_addr_of(begin), lh_addr_of(end));

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
lh_memory_view_is_uninitialized(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_is_uninitialized(self);
}

lh_bool_t
lh_memory_view_is_initialized(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_is_initialized(self);
}

lh_bool_t
lh_memory_view_is_forward(const lh_memory_view_t *self)
{
    return lh_memory_view_get_direction(self) == lh_memory_view_slice_direction_forward;
}

lh_bool_t
lh_memory_view_is_backward(const lh_memory_view_t *self)
{
    return lh_memory_view_get_direction(self) == lh_memory_view_slice_direction_backward;
}

lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self)
{
    return lh_memory_view_is_forward(self);
}

lh_void
lh_memory_view_unpack_v(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_view_unpack(self, begin, end);
}

const lh_ptr
lh_memory_view_get_begin_v(const lh_memory_view_t *self)
{
    const lh_ptr begin;
    lh_memory_view_unpack_v(self, lh_addr_of(begin), lh_null);
    return begin;
}

const lh_ptr
lh_memory_view_get_end_v(const lh_memory_view_t *self)
{
    const lh_ptr end;
    lh_memory_view_unpack_v(self, lh_null, lh_addr_of(end));
    return end;
}

lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self)
{
    const lh_void *begin, *end;
    lh_memory_view_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_ropen_get_size(lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self)
{
    return lh_memory_view_is_uninitialized(self) || lh_math_is_zero(lh_memory_view_get_size(self));
}

lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_memory_view_get_size(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_uoffset_t
lh_memory_view_get_offset_from_begin(const lh_memory_view_t *self, const lh_ptr ptr)
{
    lh_uoffset_t offset = lh_ptr_udiff(ptr, lh_memory_view_get_begin(self));
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return offset;
}

lh_uoffset_t
lh_memory_view_get_offset_from_end(const lh_memory_view_t *self, const lh_ptr ptr)
{
    lh_assert_runtime_ifn(lh_memory_view_contains_ptr(self, ptr),
                          lh_runtime_error_code_out_of_range);
    return lh_math_sub_one(lh_ptr_udiff(lh_memory_view_get_end(self), ptr));
}

lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_void *begin, *end;
    lh_memory_view_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_interval_ropen_contains_value(begin, end, ptr);
}

lh_bool_t
lh_memory_view_contains_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;
    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_ropen_contains_range(self_begin, self_end, begin, end);
}

lh_bool_t
lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;
    lh_memory_view_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_contains_of(self, begin, end);
}

const lh_ptr
lh_memory_view_get_ptr_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_get_begin(self), offset);
}

const lh_ptr
lh_memory_view_get_ptr_from_end(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(
        const lh_void, lh_memory_view_get_end(self),
        lh_math_neg(lh_type_cast(lh_soffset_t, lh_math_add_one(offset))));
}

const lh_ptr
lh_memory_view_get_first_ptr(const lh_memory_view_t *self)
{
    return lh_memory_view_get_ptr_from_begin(self, 0);
}

const lh_ptr
lh_memory_view_get_last_ptr(const lh_memory_view_t *self)
{
    return lh_memory_view_get_ptr_from_end(self, 0);
}

const lh_ptr
lh_memory_view_get_ptr_by_offset(const lh_memory_view_t *self, lh_soffset_t offset)
{
    if (lh_math_is_negative(offset))
    {
        return lh_memory_view_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_sub_one(lh_math_neg(offset))));
    }

    return lh_memory_view_get_ptr_from_begin(self, lh_type_cast(lh_uoffset_t, offset));
}

lh_byte_t
lh_memory_view_get_value_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_get_ptr_from_begin(self, offset)));
}

lh_byte_t
lh_memory_view_get_value_from_end(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_get_ptr_from_end(self, offset)));
}

lh_byte_t
lh_memory_view_get_value_by_offset(const lh_memory_view_t *self, lh_soffset_t offset)
{
    return lh_ptr_deref(
        lh_ptr_cast(const lh_byte_t, lh_memory_view_get_ptr_by_offset(self, offset)));
}

lh_byte_t
lh_memory_view_get_first_value(const lh_memory_view_t *self)
{
    return lh_memory_view_get_value_from_begin(self, 0);
}

lh_byte_t
lh_memory_view_get_last_value(const lh_memory_view_t *self)
{
    return lh_memory_view_get_value_from_end(self, 0);
}

lh_uoffset_t
lh_memory_view_get_offset_from_ptr(const lh_memory_view_t *self, const lh_ptr ptr,
                                   lh_soffset_t offset)
{
    if (lh_ptr_is_null(ptr))
    {
        return lh_memory_view_get_offset_from_begin(self,
                                                    lh_memory_view_get_ptr_by_offset(self, offset));
    }

    const lh_usize_t size = lh_memory_view_get_size(self);
    const lh_uoffset_t ptr_offset = lh_memory_view_get_offset_from_begin(self, ptr);

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
lh_memory_view_seek_ptr(const lh_memory_view_t *self, const lh_ptr ptr, lh_soffset_t offset)
{
    lh_runtime_try(e)
    {
        lh_uoffset_t cur = lh_memory_view_get_offset_from_ptr(self, ptr, offset);
        ptr = lh_memory_view_get_ptr_from_begin(self, cur);
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
lh_memory_view_next_ptr(const lh_memory_view_t *self, const lh_ptr ptr)
{
    return lh_memory_view_seek_ptr(self, ptr, 1);
}

const lh_ptr
lh_memory_view_prev_ptr(const lh_memory_view_t *self, const lh_ptr ptr)
{
    return lh_memory_view_seek_ptr(self, ptr, -1);
}

lh_byte_t
lh_memory_view_seek_value(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_ptr seek_ptr = lh_memory_view_seek_ptr(self, ptr, 0);
    lh_assert_runtime_ifn(lh_ptr_is_set(seek_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, seek_ptr));
}

lh_byte_t
lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_ptr next_ptr = lh_memory_view_next_ptr(self, ptr);
    lh_assert_runtime_ifn(lh_ptr_is_set(next_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, next_ptr));
}

lh_byte_t
lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_ptr prev_ptr = lh_memory_view_prev_ptr(self, ptr);
    lh_assert_runtime_ifn(lh_ptr_is_set(prev_ptr), lh_runtime_error_code_null_dereference);
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, prev_ptr));
}

lh_bool_t
lh_memory_view_overlaps_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;
    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_interval_ropen_overlaps_range(lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(self_end),
                                            lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_overlaps_of(self, begin, end);
}

lh_bool_t
lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;
    lh_memory_view_unpack(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_overlaps_of(self, begin, end);
}

lh_bool_t
lh_memory_view_overlaps_v(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_view_overlaps(self, other);
}

lh_bool_t
lh_memory_view_multiple_of(const lh_memory_view_t *self, lh_usize_t alignment)
{
    lh_assert_runtime_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    const lh_usize_t size = lh_memory_view_get_size(self);
    return lh_math_is_zero(lh_math_mod(size, alignment));
}

lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t multiple)
{
    return lh_memory_view_multiple_of(self, multiple);
}

lh_bool_t
lh_memory_view_aligned_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    const lh_ptr begin = lh_memory_view_get_begin_v(self);
    lh_assert_runtime_ifn(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    return lh_memory_view_aligned_is_begin_aligned(self, align);
}

lh_bool_t
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    const lh_bool_t is_begin_aligned = lh_memory_view_aligned_is_begin_aligned(self, align);
    const lh_ptr end = lh_memory_view_get_end_v(self);
    return is_begin_aligned && lh_ptr_is_aligned(end, align);
}

lh_bool_t
lh_memory_view_equals_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_slice_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_slice_equals(self, other);
}

const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;

    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_find(self_begin, self_end, begin, end);
}

const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;

    lh_memory_view_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_find_range(self, begin, end);
}

const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;

    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_rfind(self_begin, self_end, begin, end);
}

const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;

    lh_memory_view_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_rfind_range(self, begin, end);
}

const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;

    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_compare(self_begin, self_end, begin, end);
}

const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;

    lh_memory_view_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_compare_range(self, begin, end);
}

const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin;
    const lh_ptr self_end;

    lh_memory_view_unpack_v(self, lh_addr_of(self_begin), lh_addr_of(self_end));
    return lh_memory_raw_rcompare(self_begin, self_end, begin, end);
}

const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_void *begin, *end;

    lh_memory_view_unpack_v(other, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_rcompare_range(self, begin, end);
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_assert_runtime_ref(self);

    self->first = begin;
    self->second = end;
}

LH_ATTRIBUTE_STATIC
lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    const lh_void *begin, *end;
    lh_memory_view_unpack(other, lh_addr_of(begin), lh_addr_of(end));
    lh_memory_view_set(self, begin, end);
}

lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    lh_algorithm_swap(lh_memory_view_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

lh_void
lh_memory_view_swap_and_clear(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_memory_view_clear(self);

    if (lh_math_ne(self, other))
    {
        lh_assert_runtime_ref(other);
        lh_algorithm_swap(lh_memory_view_t, lh_ptr_deref(self), lh_ptr_deref(other));
    }
}

lh_void
lh_memory_view_clear(lh_memory_view_t *self)
{
    const lh_memory_view_t s = lh_memory_view_empty_initializer();
    lh_memory_view_assign(self, lh_addr_of(s));
}

lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_view_assign(self, other);
}

lh_void
lh_memory_view_set_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t view = lh_memory_view_initializer(begin, end);
    lh_memory_view_assign_v(self, lh_addr_of(view));
}

LH_ATTRIBUTE_STATIC
lh_memory_view_t
lh_memory_view_make(const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t view = lh_memory_view_initializer(begin, end);
    return view;
}

lh_memory_view_t
lh_memory_view_make_v(const lh_ptr begin, const lh_ptr end)
{
    const lh_memory_view_t view = lh_memory_view_make(begin, end);
    lh_assert_runtime_ifn(lh_memory_view_is_valid(lh_addr_of(view)),
                          lh_runtime_error_code_invalid_range);
    return view;
}

lh_memory_view_t
lh_memory_view_make_by_size(const lh_ptr begin, lh_usize_t size)
{
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    return lh_memory_view_make_v(begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, size));
}

lh_memory_view_t
lh_memory_view_make_empty(lh_void)
{
    const lh_memory_view_t view = lh_memory_view_empty_initializer();
    return view;
}

lh_void
lh_memory_view_set_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);

    const lh_ptr end = lh_ptr_add_by_offset_unsafe(const lh_void, begin, size);
    lh_memory_view_set(self, begin, end);
}

lh_void
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_set_v(self, begin, end);
}

lh_void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_set_by_size(self, begin, size);
}

lh_void
lh_memory_view_init_empty(lh_memory_view_t *self)
{
    lh_memory_view_clear(self);
}

lh_void
lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_memory_view_assign_v(self, other);
}

lh_memory_view_slice_t
lh_memory_view_make_slice(const lh_memory_view_t *self)
{
    if (lh_memory_view_is_uninitialized(self))
    {
        return lh_memory_view_slice_make_empty();
    }

    const lh_void *begin, *end;
    lh_memory_view_unpack_v(self, lh_addr_of(begin), lh_addr_of(end));
    return lh_memory_view_slice_make_v(begin, lh_ptr_sub_by_offset_unsafe(const lh_void, end, 1U));
}
