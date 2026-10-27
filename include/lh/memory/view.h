/**
 * @file view.h
 * @brief Half-open read-only byte view (::lh_memory_view_t) and helpers.
 *
 * A memory view object stores a @c const pointer @c begin to its first byte
 * and its length @c size in bytes, describing the right-open address interval
 * <tt>[begin, begin + size)</tt>. Its exclusive end is @c begin + @c size,
 * computed on the address, so any end that was set, backward or null
 * included, reads back unchanged.
 *
 * A view whose @c begin is ::lh_null is uninitialized. Valid views have a
 * non-null @c begin and a @c begin + @c size that does not wrap past the
 * address space, so the end is at or after @c begin. Functions with the
 * @c _v suffix and all size / containment / indexed access / raw byte
 * operation helpers require valid views and may raise runtime errors when
 * that precondition is not met.
 *
 * @see lh_memory_view_fields
 */

#ifndef LH_MEMORY_VIEW_H
#define LH_MEMORY_VIEW_H

#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/size.h>
#include <lh/offset.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/view/fields.h>
#include <lh/runtime/error/code.h>
#include <lh/util/ptr.h>

/**
 * @struct lh_memory_view
 * @brief Non-owning read-only half-open byte view: a <tt>const ::lh_ptr</tt> and a byte size.
 *
 * @c begin and @c size describe <tt>[begin, begin + size)</tt> in address
 * space when @c begin is non-null and the sum does not wrap. Public typedef:
 * ::lh_memory_view_t.
 */
struct lh_memory_view
{
    /** Members: ::lh_memory_view_fields. */
    lh_memory_view_fields(lh_void);
};
/** @copybrief lh_memory_view */
typedef struct lh_memory_view lh_memory_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return @c begin without validating the view range.
 * @param self View to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_get_begin(const lh_memory_view_t *self);

/**
 * @brief Return @c size without validating the view range.
 * @param self View to read.
 * @return Stored size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self);

/**
 * @brief Store @p begin in @p self, keeping @c size.
 *
 * @param self  View to update.
 * @param begin New @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_set_begin(lh_memory_view_t *self, const lh_ptr begin);

/**
 * @brief Store @p size in @p self, keeping @c begin.
 *
 * @param self View to update.
 * @param size New @c size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_set_size(lh_memory_view_t *self, lh_usize_t size);

/**
 * @brief Exchange the contents of @p self and @p other.
 *
 * @param self  First view.
 * @param other Second view.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Alias for ::lh_memory_view_get_begin.
 * @param self View to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_get_data(const lh_memory_view_t *self);

/**
 * @brief Return the exclusive end @c begin + @c size without validating the view range.
 * @param self View to read.
 * @return Exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_get_end(const lh_memory_view_t *self);

/**
 * @brief True if @c begin is ::lh_null.
 * @param self View to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_is_uninitialized(const lh_memory_view_t *self);

/**
 * @brief True if @c begin is set and @c begin + @c size does not wrap past
 *        the address space.
 * @param self View to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self);

/**
 * @brief Store @p begin and @p end in @p self without range validation.
 *
 * The caller is responsible for ensuring the resulting view is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param self  View to update.
 * @param begin New @c begin.
 * @param end   New exclusive end; @c size becomes <tt>end - begin</tt>.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Return @c begin after validating @p self.
 * @param self Valid view to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_get_begin_v(const lh_memory_view_t *self);

/**
 * @brief Return the exclusive end @c begin + @c size after validating @p self.
 * @param self Valid view to read.
 * @return Stored exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_get_end_v(const lh_memory_view_t *self);

/**
 * @brief Return @c size after validating @p self.
 * @param self Valid view to read.
 * @return Number of bytes covered by the half-open interval.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid, including an uninitialized view.
 */
lh_usize_t
lh_memory_view_get_size_v(const lh_memory_view_t *self);

/**
 * @brief True if @p self is uninitialized or has zero size.
 *
 * A valid <tt>[x, x)</tt>, with @c size zero, is empty too.
 *
 * @param self View to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self);

/**
 * @brief True if @p self stores exactly @p begin and @p end.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_equals_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Copy endpoints from @p other to @p self without range validation.
 *
 * This function preserves the view state as-is. Use
 * ::lh_memory_view_assign_v when the source must be a valid half-open range.
 *
 * @param self  View to update.
 * @param other View to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Swap @p self with valid @p other; @p self is not checked.
 *
 * The brick ::lh_memory_view_swap_v and ::lh_memory_view_swap_and_clear share.
 *
 * @param self  View to swap.
 * @param other Valid view to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_swap_v_other(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Store a half-open range starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param self  View to update.
 * @param begin New @c begin.
 * @param size  Number of bytes in the half-open view.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
lh_void
lh_memory_view_set_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief True if @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_view_get_size(self))</tt>.
 *
 * @param self   Valid view to inspect.
 * @param offset Byte offset from @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief True if @p ptr lies inside the half-open interval @p self.
 *
 * @param self Valid view to inspect.
 * @param ptr  Pointer to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief True if half-open range <tt>[begin, end)</tt> lies inside @p self.
 *
 * @param self  Valid outer view.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_contains_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if half-open range <tt>[begin, end)</tt> overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_overlaps_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if half-open view size is divisible by @p alignment.
 *
 * @param self      Valid view to inspect.
 * @param alignment Non-zero divisor for the view size.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        @p alignment is zero.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid, including an uninitialized view.
 */
lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t alignment);

/**
 * @brief True if @c begin is aligned to @p align.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief Alias for ::lh_memory_view_equals_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p self and @p other store the same endpoints.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Find the first occurrence of <tt>[data, data + size)</tt> inside @p self.
 *
 * The brick the <tt>[begin, end)</tt> and view-operand forms are built on.
 *
 * @param self Valid view.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_find_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Find the last occurrence of <tt>[data, data + size)</tt> inside @p self.
 *
 * The brick the <tt>[begin, end)</tt> and view-operand forms are built on.
 *
 * @param self Valid view.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_rfind_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Compare @p self with <tt>[data, data + size)</tt> up to the smaller size.
 *
 * The brick the <tt>[begin, end)</tt> and view-operand forms are built on.
 *
 * @param self Valid view.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when
 *         the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_compare_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Compare suffixes of @p self and <tt>[data, data + size)</tt>.
 *
 * The brick the <tt>[begin, end)</tt> and view-operand forms are built on.
 *
 * @param self Valid view.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_rcompare_by_size(const lh_memory_view_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Swap two valid views.
 *
 * @param self  Valid view to swap.
 * @param other Valid view to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_void
lh_memory_view_swap_v(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Reset @p self to the uninitialized empty view.
 *
 * @param self View to clear.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_view_clear(lh_memory_view_t *self);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 *
 * @param self  View to update.
 * @param other Valid view to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Initialize @p self as a half-open range starting at @p begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
lh_void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief Return byte offset of @p ptr from @c begin.
 *
 * @param self Valid view to inspect.
 * @param ptr  Pointer whose offset is requested.
 * @return Byte offset from @c begin to @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_uoffset_t
lh_memory_view_get_offset_from_begin(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from the last byte, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c begin + @c size - 1. The exclusive end
 * itself is not inside the view.
 *
 * @param self Valid view to inspect.
 * @param ptr  Pointer whose reverse offset is requested.
 * @return Byte offset from the last byte back to @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_uoffset_t
lh_memory_view_get_offset_from_end(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Alias for ::lh_memory_view_contains_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_contains_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p other lies completely inside @p self.
 *
 * Both operands are interpreted as half-open views. @p other is not
 * validated.
 *
 * @param self  Valid outer view.
 * @param other Inner view to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Return pointer at byte @p offset from @c begin.
 *
 * @param self   Valid view to index.
 * @param offset Byte offset from the begin endpoint.
 * @return Pointer to the requested byte.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
const lh_ptr
lh_memory_view_get_ptr_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from the last byte, walking backward.
 *
 * Offset @c 0 returns @c begin + @c size - 1, offset @c 1 returns the previous byte,
 * and so on.
 *
 * @param self   Valid view to index.
 * @param offset Byte offset from the last byte.
 * @return Pointer to the requested byte.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
const lh_ptr
lh_memory_view_get_ptr_from_end(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Alias for ::lh_memory_view_overlaps_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p other overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief True if both endpoints are aligned to @p align.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief Find the first occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid view used as the haystack.
 * @param other Valid view used as the needle.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Find the last occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid view used as the haystack.
 * @param other Valid view used as the needle.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Compare @p self with @p other.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid left-hand view.
 * @param other Valid right-hand view.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when
 *         the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Compare suffixes of @p self and @p other.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid left-hand view.
 * @param other Valid right-hand view.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Clear @p self, then swap it with @p other.
 *
 * Discards the current contents of @p self, then moves the contents of
 * @p other into @p self, leaving @p other empty.
 *
 * @param self  View to clear and receive @p other's contents.
 * @param other View whose contents are moved into @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_memory_view_swap_and_clear(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Store @p begin and @p end in @p self after validation.
 *
 * @param self  View to update.
 * @param begin New @c begin.
 * @param end   New exclusive end; @c size becomes <tt>end - begin</tt>.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
lh_void
lh_memory_view_set_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Initialize @p self with the empty view initializer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_view_init_empty(lh_memory_view_t *self);

/**
 * @brief Initialize @p self by copying endpoints from valid @p other.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief True if valid @p other lies completely inside @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_bool_t
lh_memory_view_contains_v(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Return pointer to the first byte.
 * @param self Valid view to index.
 * @return Pointer to @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_get_first_ptr(const lh_memory_view_t *self);

/**
 * @brief Return pointer to the last byte.
 * @param self Valid non-empty view to index.
 * @return Pointer to @c begin + @c size - 1.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
const lh_ptr
lh_memory_view_get_last_ptr(const lh_memory_view_t *self);

/**
 * @brief Return pointer by signed offset.
 *
 * Non-negative offsets are measured from @c begin. Negative offsets are
 * measured from the last byte: @c -1 addresses @c begin + @c size - 1, @c -2 the
 * previous byte.
 *
 * @param self   Valid view to index.
 * @param offset Signed byte offset.
 * @return Pointer to the requested byte.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
const lh_ptr
lh_memory_view_get_ptr(const lh_memory_view_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c begin.
 *
 * @param self   Valid view to index.
 * @param offset Byte offset from the begin endpoint.
 * @return Byte stored at the requested address.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
lh_byte_t
lh_memory_view_get_value_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from the last byte, walking backward.
 *
 * @param self   Valid view to index.
 * @param offset Byte offset from the last byte.
 * @return Byte stored at the requested address.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
lh_byte_t
lh_memory_view_get_value_from_end(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief True if valid @p other overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_bool_t
lh_memory_view_overlaps_v(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Initialize @p self with valid @p begin and @p end endpoints.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
lh_void
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Read byte by signed offset.
 *
 * @param self   Valid view to index.
 * @param offset Signed byte offset.
 * @return Byte stored at the requested address.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
lh_byte_t
lh_memory_view_get_value(const lh_memory_view_t *self, lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 *
 * @param self Valid view to read.
 * @return Byte at @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_view_get_first_value(const lh_memory_view_t *self);

/**
 * @brief Read the last byte of @p self.
 *
 * @param self Valid view to read.
 * @return Byte at @c begin + @c size - 1.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_view_get_last_value(const lh_memory_view_t *self);

/**
 * @brief Resolve @p ptr moved by @p offset to an offset from @c begin, or say
 *        why it would leave the view.
 *
 * The brick ::lh_memory_view_get_offset_from_ptr (which fails on the code) and
 * ::lh_memory_view_seek_ptr (which turns it into ::lh_null) share. A ::lh_null
 * @p ptr seeks from the ends, as ::lh_memory_view_get_ptr does, and fails
 * through it instead of returning a code.
 *
 * @param self   Valid view to seek in.
 * @param ptr    Anchor inside @p self, or ::lh_null.
 * @param offset Signed step from @p ptr.
 * @param out    Receives the offset from @c begin; left unchanged on failure.
 * @return ::lh_runtime_error_code_ok, ::lh_runtime_error_code_underflow or
 *         ::lh_runtime_error_code_overflow.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_runtime_error_code_t
lh_memory_view_find_offset_from_ptr(const lh_memory_view_t *self, const lh_ptr ptr,
                                    lh_soffset_t offset, lh_uoffset_t *out);

/**
 * @brief Find the first occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid view used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Find the last occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid view used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Compare @p self with half-open range <tt>[begin, end)</tt>.
 *
 * Delegates to ::lh_memory_raw_compare and compares up to the smaller size.
 *
 * @param self  Valid left-hand view.
 * @param begin Right-hand range begin pointer.
 * @param end   Right-hand range exclusive end pointer.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when
 *         the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Compare suffixes of @p self and <tt>[begin, end)</tt>.
 *
 * Delegates to ::lh_memory_raw_rcompare.
 *
 * @param self  Valid left-hand view.
 * @param begin Right-hand range begin pointer.
 * @param end   Right-hand range exclusive end pointer.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Build a view containing the first @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the view written
 * to @p out is the empty <tt>[begin, begin)</tt>, still at @p begin.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the beginning.
 * @param out   Receives the view <tt>[begin, begin + n)</tt>; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_view_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n,
                          lh_memory_view_t *out);

/**
 * @brief Build a view containing the last @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the view written
 * to @p out is the empty <tt>[end, end)</tt>, still at @p end.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the end.
 * @param out   Receives the view <tt>[end - n, end)</tt>; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_view_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_t *out);

/**
 * @brief Build a sub-view between @p begin and @p end inside @p self.
 *
 * @param self  Source view that must contain the requested range.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 * @param out   Receives the inner view <tt>[begin, end)</tt>; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[begin, end)</tt> is not contained by @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 */
lh_void
lh_memory_view_between(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end,
                       lh_memory_view_t *out);

/**
 * @brief Build a sub-view starting at @p offset with @p size bytes.
 *
 * If @p size is zero, the view written to @p out is empty, at @p offset, which
 * may then equal the source size.
 *
 * @param self   Source view.
 * @param offset Offset from the source beginning.
 * @param size   Number of bytes in the view written to @p out.
 * @param out    Receives the view <tt>[begin + offset, begin + offset + size)</tt>;
 *               must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[offset, offset + size)</tt> is outside @p self.
 */
lh_void
lh_memory_view_from_offset(const lh_memory_view_t *self, lh_uoffset_t offset, lh_usize_t size,
                           lh_memory_view_t *out);

/**
 * @brief Return target byte offset after applying signed @p offset.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed offset
 * accepted by ::lh_memory_view_get_ptr. Otherwise @p offset is
 * applied relative to @p ptr.
 *
 * @param self   Valid view to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target byte offset from @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self or the target offset is out of range.
 */
lh_uoffset_t
lh_memory_view_get_offset_from_ptr(const lh_memory_view_t *self, const lh_ptr ptr,
                                   lh_soffset_t offset);

/**
 * @brief Return pointer reached by seeking @p offset bytes from @p ptr.
 *
 * Boundary underflow and overflow are converted to ::lh_null.
 *
 * @param self   Valid view to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target pointer, or ::lh_null when the seek crosses bounds.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_seek_ptr(const lh_memory_view_t *self, const lh_ptr ptr, lh_soffset_t offset);

/**
 * @brief Build a view containing the first @p size bytes of @p self.
 *
 * If @p size is zero, the view written to @p out is empty, at the source begin.
 *
 * @param self Source view.
 * @param size Number of bytes to keep from the beginning.
 * @param out  Receives the view taken from the beginning of @p self; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source view size.
 */
lh_void
lh_memory_view_from_begin(const lh_memory_view_t *self, lh_usize_t size, lh_memory_view_t *out);

/**
 * @brief Build a view containing the last @p size bytes of @p self.
 *
 * If @p size is zero, the view written to @p out is empty, at the source end.
 *
 * @param self Source view.
 * @param size Number of bytes to keep from the end.
 * @param out  Receives the view taken from the end of @p self; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source view size.
 */
lh_void
lh_memory_view_from_end(const lh_memory_view_t *self, lh_usize_t size, lh_memory_view_t *out);

/**
 * @brief Build a view with @p n bytes removed from the beginning.
 *
 * If @p n equals the source size, the view written to @p out is empty, at the
 * source end.
 *
 * @param self Source view.
 * @param n    Number of bytes to remove from the beginning.
 * @param out  Receives the source view without the first @p n bytes; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source view size.
 */
lh_void
lh_memory_view_drop_first(const lh_memory_view_t *self, lh_usize_t n, lh_memory_view_t *out);

/**
 * @brief Return the byte pointer after @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_next_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return the byte pointer before @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_prev_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Seek to @p ptr and read the byte there.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        The seek result is outside @p self.
 */
lh_byte_t
lh_memory_view_seek_value(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Build a view with @p n bytes removed from the end.
 *
 * If @p n equals the source size, the view written to @p out is empty, at the
 * source begin.
 *
 * @param self Source view.
 * @param n    Number of bytes to remove from the end.
 * @param out  Receives the source view without the last @p n bytes; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source view size.
 */
lh_void
lh_memory_view_drop_last(const lh_memory_view_t *self, lh_usize_t n, lh_memory_view_t *out);

/**
 * @brief Read the byte after @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        There is no next byte inside @p self.
 */
lh_byte_t
lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte before @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        There is no previous byte inside @p self.
 */
lh_byte_t
lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Build a view with bytes removed from both sides.
 *
 * If @p left + @p right equals the source size, the view written to @p out is
 * empty, @p left bytes past the source begin.
 *
 * @param self  Source view.
 * @param left  Number of bytes to remove from the beginning.
 * @param right Number of bytes to remove from the end.
 * @param out   Receives the trimmed source view; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p left + @p right is greater than the source view size.
 */
lh_void
lh_memory_view_trim(const lh_memory_view_t *self, lh_usize_t left, lh_usize_t right,
                    lh_memory_view_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_VIEW_H */
