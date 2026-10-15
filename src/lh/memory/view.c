#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/initializer.h>
#include <lh/assert.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>
#include <lh/math.h>

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
    return lh_memory_view_slice_get_direction(self);
}

lh_memory_view_slice_flags_t
lh_memory_view_get_flags(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_flags(self);
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
    return lh_memory_view_slice_is_forward(self);
}

lh_bool_t
lh_memory_view_is_backward(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_is_backward(self);
}

lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_is_valid(self);
}

const lh_ptr
lh_memory_view_get_begin_v(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_begin_v(self);
}

const lh_ptr
lh_memory_view_get_end_v(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_end_v(self);
}

lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_get_ropen_view_size(self);
}

lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self)
{
    return lh_memory_view_is_uninitialized(self) || lh_math_is_zero(lh_memory_view_get_size(self));
}

lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        return lh_bool_false;
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_is_valid_offset(&slice, offset);
}

lh_uoffset_t
lh_memory_view_get_offset_from_begin(const lh_memory_view_t *self, const lh_ptr ptr)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_offset_from_begin(&slice, ptr);
}

lh_uoffset_t
lh_memory_view_get_offset_from_end(const lh_memory_view_t *self, const lh_ptr ptr)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_offset_from_end(&slice, ptr);
}

lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        return lh_bool_false;
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_contains_ptr(&slice, ptr);
}

lh_bool_t
lh_memory_view_contains_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_contains_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_contains_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

lh_bool_t
lh_memory_view_contains_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_contains_of(self, begin, end);
}

lh_bool_t
lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_contains_of(self, lh_memory_view_get_begin(other),
                                      lh_memory_view_get_end(other));
}

lh_bool_t
lh_memory_view_contains_v(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_view_contains(self, other);
}

const lh_ptr
lh_memory_view_get_ptr_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_ptr_from_begin(&slice, offset);
}

/* slice end is inclusive, so offset 0 there is view's exclusive end minus one. */
const lh_ptr
lh_memory_view_get_ptr_from_end(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_ptr_from_end(&slice, offset);
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
lh_memory_view_get_ptr(const lh_memory_view_t *self, lh_soffset_t offset)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_ptr(&slice, offset);
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
lh_memory_view_get_value(const lh_memory_view_t *self, lh_soffset_t offset)
{
    return lh_ptr_deref(lh_ptr_cast(const lh_byte_t, lh_memory_view_get_ptr(self, offset)));
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
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_get_offset_from_ptr(&slice, ptr, offset);
}

const lh_ptr
lh_memory_view_seek_ptr(const lh_memory_view_t *self, const lh_ptr ptr, lh_soffset_t offset)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_seek_ptr(&slice, ptr, offset);
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
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_seek_value(&slice, ptr);
}

lh_byte_t
lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_next_value(&slice, ptr);
}

lh_byte_t
lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr)
{
    if (lh_memory_view_slice_is_ropen_empty(self))
        lh_assert_runtime_if(lh_bool_true, lh_runtime_error_code_out_of_range);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_prev_value(&slice, ptr);
}

lh_bool_t
lh_memory_view_overlaps_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_overlaps_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_overlaps_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

lh_bool_t
lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_overlaps_of(self, begin, end);
}

lh_bool_t
lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_overlaps_of(self, lh_memory_view_get_begin(other),
                                      lh_memory_view_get_end(other));
}

lh_bool_t
lh_memory_view_overlaps_v(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_view_overlaps(self, other);
}

lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t alignment)
{
    lh_assert_runtime_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    if (lh_memory_view_slice_is_ropen_empty(self)) return lh_bool_true;
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_is_multiple_of(&slice, alignment);
}

lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    return lh_memory_view_slice_is_begin_aligned(self, align);
}

lh_bool_t
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    /* Exclusive end, not the closed slice's last byte. */
    const lh_bool_t is_begin_aligned = lh_memory_view_is_begin_aligned(self, align);
    return is_begin_aligned && lh_ptr_is_aligned(lh_memory_view_get_end_v(self), align);
}

lh_bool_t
lh_memory_view_equals_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_slice_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_slice_equals_range(self, begin, end);
}

lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_slice_equals(self, other);
}

const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_find_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_find_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_find_range(self, lh_memory_view_get_begin_v(other),
                                     lh_memory_view_get_end_v(other));
}

const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_rfind_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_rfind_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_rfind_range(self, lh_memory_view_get_begin_v(other),
                                      lh_memory_view_get_end_v(other));
}

const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_compare_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_compare_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_compare_range(self, lh_memory_view_get_begin_v(other),
                                        lh_memory_view_get_end_v(other));
}

const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    if (lh_memory_view_slice_needs_ropen(self, begin, end))
        return lh_memory_view_slice_rcompare_ropen(self, begin, end);
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_view(self);
    return lh_memory_view_slice_rcompare_range(&slice, begin, lh_memory_view_slice_get_ropen_last(end));
}

const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_rcompare_range(self, lh_memory_view_get_begin_v(other),
                                         lh_memory_view_get_end_v(other));
}

lh_void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_slice_set(self, begin, end);
}

lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_memory_view_slice_assign(self, other);
}

lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_memory_view_slice_swap(self, other);
}

lh_void
lh_memory_view_swap_v(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_memory_view_slice_swap_v(self, other);
}

lh_void
lh_memory_view_swap_and_clear(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_memory_view_slice_swap_and_clear(self, other);
}

lh_void
lh_memory_view_clear(lh_memory_view_t *self)
{
    lh_memory_view_slice_clear(self);
}

lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_memory_view_slice_assign_v(self, other);
}

lh_void
lh_memory_view_set_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_slice_set_v(self, begin, end);
}

lh_void
lh_memory_view_set_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    /* Exclusive end is begin + size. slice set_by_size would store begin + size - 1. */
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);
    lh_memory_view_set(self, begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, size));
}

lh_void
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_slice_init(self, begin, end);
}

lh_void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_set_by_size(self, begin, size);
}

lh_void
lh_memory_view_init_empty(lh_memory_view_t *self)
{
    lh_memory_view_slice_init_empty(self);
}

lh_void
lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_memory_view_slice_init_by_other(self, other);
}

lh_memory_view_slice_t
lh_memory_view_to_slice(const lh_memory_view_t *self)
{
    return lh_memory_view_slice_from_view(self);
}

lh_memory_view_t
lh_memory_view_return_of_slice(const lh_memory_view_slice_t *slice)
{
    if (lh_memory_view_slice_is_uninitialized(slice))
        return lh_memory_view_return_empty();
    return lh_memory_view_return_init_of_slice(slice);
}

lh_memory_view_t
lh_memory_view_return_init_of_slice(const lh_memory_view_slice_t *slice)
{
    lh_assert_runtime_ifn(lh_memory_view_slice_is_valid(slice), lh_runtime_error_code_invalid_range);
    return lh_initializer_of(lh_memory_view_t, lh_memory_view_initializer,
                             lh_memory_view_slice_get_begin(slice),
                             lh_memory_view_slice_get_ropen_end(slice));
}

lh_memory_view_t
lh_memory_view_return_empty(void)
{
    lh_memory_view_t view;

    lh_memory_view_init_empty(&view);
    return view;
}

lh_memory_view_t
lh_memory_view_return_init_v(const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t view;

    lh_memory_view_init(&view, begin, end);
    return view;
}

lh_memory_view_t
lh_memory_view_return_init_by_size(const lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_t view;

    lh_memory_view_init_by_size(&view, begin, size);
    return view;
}

lh_memory_view_t
lh_memory_view_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_take_first_ropen(begin, end, n);
    return lh_memory_view_return_of_slice(&slice);
}

lh_memory_view_t
lh_memory_view_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_take_last_ropen(begin, end, n);
    return lh_memory_view_return_of_slice(&slice);
}

lh_memory_view_t
lh_memory_view_from_begin(const lh_memory_view_t *self, lh_usize_t size)
{
    return lh_memory_view_take_first(lh_memory_view_get_begin(self), lh_memory_view_get_end(self),
                                     size);
}

lh_memory_view_t
lh_memory_view_from_end(const lh_memory_view_t *self, lh_usize_t size)
{
    return lh_memory_view_take_last(lh_memory_view_get_begin(self), lh_memory_view_get_end(self),
                                    size);
}

lh_memory_view_t
lh_memory_view_between(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    /* [p, p) is a valid empty view and has no closed slice. */
    lh_assert_runtime_ifn(lh_memory_view_contains_range(self, begin, end),
                          lh_runtime_error_code_out_of_range);
    return lh_memory_view_return_init_v(begin, end);
}

lh_memory_view_t
lh_memory_view_from_offset(const lh_memory_view_t *self, lh_uoffset_t offset, lh_usize_t size)
{
    const lh_memory_view_slice_t slice = lh_memory_view_slice_from_offset_ropen(self, offset, size);
    return lh_memory_view_return_of_slice(&slice);
}

lh_memory_view_t
lh_memory_view_drop_first(const lh_memory_view_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_view_get_size(self);
    lh_assert_runtime_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_view_from_offset(self, n, lh_math_sub(total, n));
}

lh_memory_view_t
lh_memory_view_drop_last(const lh_memory_view_t *self, lh_usize_t n)
{
    const lh_usize_t total = lh_memory_view_get_size(self);
    lh_assert_runtime_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    return lh_memory_view_from_begin(self, lh_math_sub(total, n));
}

lh_memory_view_t
lh_memory_view_trim(const lh_memory_view_t *self, lh_usize_t left, lh_usize_t right)
{
    lh_memory_view_t dropped = lh_memory_view_drop_first(self, left);
    return lh_memory_view_drop_last(&dropped, right);
}
