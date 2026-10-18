#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/memory/std.h>
#include <lh/util/interval.h>
#include <lh/util/return.h>
#include <lh/util/swap.h>
#include <lh/offset/limits.h>
#include <lh/addr/limits.h>
#include <lh/assert.h>
#include <lh/math.h>

const lh_ptr
lh_memory_view_get_begin(const lh_memory_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->begin;
}

lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self)
{
    lh_assert_runtime_ref(self);
    return self->size;
}

lh_void
lh_memory_view_set_begin(lh_memory_view_t *self, const lh_ptr begin)
{
    lh_assert_runtime_ref(self);
    self->begin = begin;
}

lh_void
lh_memory_view_set_size(lh_memory_view_t *self, lh_usize_t size)
{
    lh_assert_runtime_ref(self);
    self->size = size;
}

lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    lh_assert_runtime_ref(self);
    lh_assert_runtime_ref(other);

    lh_swap(lh_memory_view_t, lh_ptr_deref(self), lh_ptr_deref(other));
}

const lh_ptr
lh_memory_view_get_data(const lh_memory_view_t *self)
{
    return lh_memory_view_get_begin(self);
}

const lh_ptr
lh_memory_view_get_end(const lh_memory_view_t *self)
{
    /* Address arithmetic, so begin + size gives back any end set, backward included. */
    return lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_get_begin(self),
                                       lh_memory_view_get_size(self));
}

lh_bool_t
lh_memory_view_is_uninitialized(const lh_memory_view_t *self)
{
    return lh_ptr_is_null(lh_memory_view_get_begin(self));
}

lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self)
{
    const lh_ptr begin = lh_memory_view_get_begin(self);
    /* begin + size within the address space: the end is at or after begin. */
    return lh_ptr_is_set(begin) && lh_math_le(lh_memory_view_get_size(self),
                                              lh_math_sub(LH_UADDR_T_MAX, lh_ptr_to_uaddr(begin)));
}

lh_void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_set_begin(self, begin);
    lh_memory_view_set_size(self, lh_ptr_udiff(end, begin));
}

const lh_ptr
lh_memory_view_get_begin_v(const lh_memory_view_t *self)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(self), lh_runtime_error_code_invalid_range);
    return lh_memory_view_get_begin(self);
}

const lh_ptr
lh_memory_view_get_end_v(const lh_memory_view_t *self)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(self), lh_runtime_error_code_invalid_range);
    return lh_memory_view_get_end(self);
}

lh_usize_t
lh_memory_view_get_size_v(const lh_memory_view_t *self)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(self), lh_runtime_error_code_invalid_range);
    return lh_memory_view_get_size(self);
}

lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self)
{
    return lh_memory_view_is_uninitialized(self) || lh_math_is_zero(lh_memory_view_get_size(self));
}

lh_bool_t
lh_memory_view_equals_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_ptr_eq(lh_memory_view_get_begin(self), begin) &&
           lh_ptr_eq(lh_memory_view_get_end(self), end);
}

lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_return_if(lh_math_eq(self, other));

    const lh_ptr begin = lh_memory_view_get_begin(other);
    lh_memory_view_set(self, begin, lh_memory_view_get_end(other));
}

lh_void
lh_memory_view_swap_v_other(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_view_swap(self, other);
}

lh_void
lh_memory_view_set_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_assert_runtime_ifn(lh_ptr_is_set(begin), lh_runtime_error_code_invalid_argument);
    lh_assert_runtime_if(lh_math_is_zero(size), lh_runtime_error_code_invalid_range);
    lh_memory_view_set(self, begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, size));
}

lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    const lh_usize_t size = lh_memory_view_get_size_v(self);
    return lh_interval_ropen_contains_value(LH_UOFFSET_T_MIN, size, offset);
}

lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_ptr begin = lh_memory_view_get_begin_v(self);
    return lh_interval_ropen_contains_value(lh_ptr_to_uaddr(begin),
                                            lh_ptr_to_uaddr(lh_memory_view_get_end(self)),
                                            lh_ptr_to_uaddr(ptr));
}

lh_bool_t
lh_memory_view_contains_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    return lh_interval_ropen_contains_range(
        lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(lh_memory_view_get_end(self)),
        lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_overlaps_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    return lh_interval_ropen_overlaps_range(
        lh_ptr_to_uaddr(self_begin), lh_ptr_to_uaddr(lh_memory_view_get_end(self)),
        lh_ptr_to_uaddr(begin), lh_ptr_to_uaddr(end));
}

lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t alignment)
{
    lh_assert_runtime_if(lh_math_is_zero(alignment), lh_runtime_error_code_division_by_zero);
    const lh_usize_t size = lh_memory_view_get_size_v(self);

    if (lh_math_is_power_of_two(alignment))
    {
        return lh_math_is_zero(size & (alignment - 1U));
    }
    return lh_math_is_zero(lh_math_mod(size, alignment));
}

lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    const lh_ptr begin = lh_memory_view_get_begin_v(self);
    lh_assert_runtime_ifn(lh_math_is_power_of_two(align), lh_runtime_error_code_not_power_of_two);
    return lh_ptr_is_aligned(begin, align);
}

lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    return lh_memory_view_equals_of(self, begin, end);
}

lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    return lh_memory_view_equals_of(self, lh_memory_view_get_begin(other),
                                    lh_memory_view_get_end(other));
}

const lh_ptr
lh_memory_view_find_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    return lh_memory_std_find(self_begin, lh_memory_view_get_size(self), data, size);
}

const lh_ptr
lh_memory_view_rfind_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    return lh_memory_std_rfind(self_begin, lh_memory_view_get_size(self), data, size);
}

const lh_ptr
lh_memory_view_compare_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    return lh_memory_std_compare(self_begin, data,
                                 lh_math_min(lh_memory_view_get_size(self), size));
}

const lh_ptr
lh_memory_view_rcompare_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size)
{
    const lh_ptr self_begin = lh_memory_view_get_begin_v(self);
    const lh_usize_t self_size = lh_memory_view_get_size(self);
    const lh_usize_t n = lh_math_min(self_size, size);

    return lh_memory_std_rcompare(
        lh_ptr_add_by_offset_unsafe(const lh_void, self_begin, lh_math_sub(self_size, n)),
        lh_ptr_add_by_offset_unsafe(const lh_void, data, lh_math_sub(size, n)), n);
}

lh_void
lh_memory_view_swap_v(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(self), lh_runtime_error_code_invalid_range);
    lh_memory_view_swap_v_other(self, other);
}

lh_void
lh_memory_view_clear(lh_memory_view_t *self)
{
    const lh_memory_view_t empty = lh_memory_view_empty_initializer();
    lh_memory_view_assign(self, lh_addr_of(empty));
}

lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    lh_memory_view_assign(self, other);
}

lh_void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size)
{
    lh_memory_view_set_by_size(self, begin, size);
}

lh_uoffset_t
lh_memory_view_get_offset_from_begin(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_uoffset_t offset = lh_ptr_udiff(ptr, lh_memory_view_get_begin(self));
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return offset;
}

/* Offset 0 is the last byte, one before the exclusive end. */
lh_uoffset_t
lh_memory_view_get_offset_from_end(const lh_memory_view_t *self, const lh_ptr ptr)
{
    const lh_uoffset_t offset = lh_math_sub_one(lh_ptr_udiff(lh_memory_view_get_end(self), ptr));
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return offset;
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

const lh_ptr
lh_memory_view_get_ptr_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_get_begin(self), offset);
}

/* Offset 0 is the last byte, one before the exclusive end. */
const lh_ptr
lh_memory_view_get_ptr_from_end(const lh_memory_view_t *self, lh_uoffset_t offset)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid_offset(self, offset),
                          lh_runtime_error_code_out_of_range);
    return lh_ptr_sub_by_offset_unsafe(const lh_void, lh_memory_view_get_end(self),
                                       lh_math_add(offset, 1U));
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
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align)
{
    /* The exclusive end, not the last byte. */
    const lh_bool_t is_begin_aligned = lh_memory_view_is_begin_aligned(self, align);
    return is_begin_aligned && lh_ptr_is_aligned(lh_memory_view_get_end_v(self), align);
}

const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_ptr other_begin = lh_memory_view_get_begin_v(other);
    return lh_memory_view_find_by_size(self, other_begin, lh_memory_view_get_size(other));
}

const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_ptr other_begin = lh_memory_view_get_begin_v(other);
    return lh_memory_view_rfind_by_size(self, other_begin, lh_memory_view_get_size(other));
}

const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_ptr other_begin = lh_memory_view_get_begin_v(other);
    return lh_memory_view_compare_by_size(self, other_begin, lh_memory_view_get_size(other));
}

const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    const lh_ptr other_begin = lh_memory_view_get_begin_v(other);
    return lh_memory_view_rcompare_by_size(self, other_begin, lh_memory_view_get_size(other));
}

lh_void
lh_memory_view_swap_and_clear(lh_memory_view_t *self, lh_memory_view_t *other)
{
    lh_memory_view_clear(self);

    if (lh_math_ne(self, other))
    {
        lh_memory_view_swap_v_other(self, other);
    }
}

lh_void
lh_memory_view_set_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t view;

    lh_memory_view_set(lh_addr_of(view), begin, end);
    lh_memory_view_assign_v(self, lh_addr_of(view));
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

lh_bool_t
lh_memory_view_contains_v(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_view_contains(self, other);
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
    if (lh_math_is_negative(offset))
    {
        /* -(offset + 1), not -offset - 1: -LH_SOFFSET_T_MIN overflows. */
        return lh_memory_view_get_ptr_from_end(
            self, lh_type_cast(lh_uoffset_t, lh_math_neg(lh_math_add(offset, 1))));
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

lh_bool_t
lh_memory_view_overlaps_v(const lh_memory_view_t *self, const lh_memory_view_t *other)
{
    lh_assert_runtime_ifn(lh_memory_view_is_valid(other), lh_runtime_error_code_invalid_range);
    return lh_memory_view_overlaps(self, other);
}

lh_void
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_set_v(self, begin, end);
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

lh_runtime_error_code_t
lh_memory_view_find_offset_from_ptr(const lh_memory_view_t *self, const lh_ptr ptr,
                                    lh_soffset_t offset, lh_uoffset_t *out)
{
    if (lh_ptr_is_null(ptr))
    {
        lh_ptr_deref(out) =
            lh_memory_view_get_offset_from_begin(self, lh_memory_view_get_ptr(self, offset));
        return lh_runtime_error_code_ok;
    }

    const lh_usize_t size = lh_memory_view_get_size_v(self);
    /* Offsets inside a view fit lh_soffset_t (an object is never larger than
       the signed pointer-difference range), so the check runs on signed operands
       with the signed offset as the addend: closed range [0, max_offset]. */
    const lh_soffset_t ptr_offset =
        lh_type_cast(lh_soffset_t, lh_memory_view_get_offset_from_begin(self, ptr));
    const lh_soffset_t max_offset = lh_type_cast(lh_soffset_t, lh_math_sub_one(size));

    if (lh_interval_closed_is_add_overflow(ptr_offset, offset, 0, max_offset))
    {
        return lh_math_is_negative(offset) ? lh_runtime_error_code_underflow
                                           : lh_runtime_error_code_overflow;
    }
    lh_ptr_deref(out) = lh_type_cast(lh_uoffset_t, lh_math_add(ptr_offset, offset));
    return lh_runtime_error_code_ok;
}

const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t other;
    lh_memory_view_init(lh_addr_of(other), begin, end);
    return lh_memory_view_find(self, lh_addr_of(other));
}

const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t other;
    lh_memory_view_init(lh_addr_of(other), begin, end);
    return lh_memory_view_rfind(self, lh_addr_of(other));
}

const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t other;
    lh_memory_view_init(lh_addr_of(other), begin, end);
    return lh_memory_view_compare(self, lh_addr_of(other));
}

const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end)
{
    lh_memory_view_t other;
    lh_memory_view_init(lh_addr_of(other), begin, end);
    return lh_memory_view_rcompare(self, lh_addr_of(other));
}

lh_void
lh_memory_view_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_memory_view_t base;
    lh_memory_view_init(lh_addr_of(base), begin, end);
    lh_assert_runtime_ifn(lh_math_le(n, lh_memory_view_get_size(lh_addr_of(base))),
                          lh_runtime_error_code_out_of_range);
    /* n == 0 is [begin, begin): empty, but still at begin. */
    lh_memory_view_init(out, begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, n));
}

lh_void
lh_memory_view_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_t *out)
{
    lh_assert_runtime_ref(out);
    lh_memory_view_t base;
    lh_memory_view_init(lh_addr_of(base), begin, end);
    lh_assert_runtime_ifn(lh_math_le(n, lh_memory_view_get_size(lh_addr_of(base))),
                          lh_runtime_error_code_out_of_range);
    /* n == 0 is [end, end): empty, but still at end. */
    lh_memory_view_init(out, lh_ptr_sub_by_offset_unsafe(const lh_void, end, n), end);
}

lh_void
lh_memory_view_between(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end,
                       lh_memory_view_t *out)
{
    lh_assert_runtime_ifn(lh_memory_view_contains_range(self, begin, end),
                          lh_runtime_error_code_out_of_range);
    lh_memory_view_init(out, begin, end);
}

lh_void
lh_memory_view_from_offset(const lh_memory_view_t *self, lh_uoffset_t offset, lh_usize_t size,
                           lh_memory_view_t *out)
{
    lh_assert_runtime_ref(out);
    const lh_usize_t total = lh_memory_view_get_size_v(self);
    lh_assert_runtime_ifn(lh_math_le(offset, total), lh_runtime_error_code_out_of_range);
    lh_assert_runtime_ifn(lh_math_le(size, lh_math_sub(total, offset)),
                          lh_runtime_error_code_out_of_range);

    /* offset may be total when size is 0: [end, end), past every byte but still inside. */
    const lh_ptr begin =
        lh_ptr_add_by_offset_unsafe(const lh_void, lh_memory_view_get_begin(self), offset);
    lh_memory_view_init(out, begin, lh_ptr_add_by_offset_unsafe(const lh_void, begin, size));
}

lh_uoffset_t
lh_memory_view_get_offset_from_ptr(const lh_memory_view_t *self, const lh_ptr ptr,
                                   lh_soffset_t offset)
{
    lh_uoffset_t result = 0U;
    lh_runtime_error_code_t code;

    code = lh_memory_view_find_offset_from_ptr(self, ptr, offset, lh_addr_of(result));
    lh_assert_runtime_if(lh_math_ne(code, lh_runtime_error_code_ok), code);
    return result;
}

const lh_ptr
lh_memory_view_seek_ptr(const lh_memory_view_t *self, const lh_ptr ptr, lh_soffset_t offset)
{
    lh_uoffset_t cur;

    if (lh_math_ne(lh_memory_view_find_offset_from_ptr(self, ptr, offset, lh_addr_of(cur)),
                   lh_runtime_error_code_ok))
    {
        return lh_null;
    }
    return lh_memory_view_get_ptr_from_begin(self, cur);
}

lh_void
lh_memory_view_from_begin(const lh_memory_view_t *self, lh_usize_t size, lh_memory_view_t *out)
{
    lh_memory_view_take_first(lh_memory_view_get_begin(self), lh_memory_view_get_end(self), size,
                              out);
}

lh_void
lh_memory_view_from_end(const lh_memory_view_t *self, lh_usize_t size, lh_memory_view_t *out)
{
    lh_memory_view_take_last(lh_memory_view_get_begin(self), lh_memory_view_get_end(self), size,
                             out);
}

lh_void
lh_memory_view_drop_first(const lh_memory_view_t *self, lh_usize_t n, lh_memory_view_t *out)
{
    const lh_usize_t total = lh_memory_view_get_size_v(self);
    lh_assert_runtime_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    lh_memory_view_from_offset(self, n, lh_math_sub(total, n), out);
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

lh_void
lh_memory_view_drop_last(const lh_memory_view_t *self, lh_usize_t n, lh_memory_view_t *out)
{
    const lh_usize_t total = lh_memory_view_get_size_v(self);
    lh_assert_runtime_ifn(lh_math_le(n, total), lh_runtime_error_code_out_of_range);
    lh_memory_view_from_begin(self, lh_math_sub(total, n), out);
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

lh_void
lh_memory_view_trim(const lh_memory_view_t *self, lh_usize_t left, lh_usize_t right,
                    lh_memory_view_t *out)
{
    lh_memory_view_t dropped;
    lh_memory_view_drop_first(self, left, lh_addr_of(dropped));
    lh_memory_view_drop_last(lh_addr_of(dropped), right, out);
}
