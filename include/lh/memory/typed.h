/**
 * @file typed.h
 * @brief Raw memory bounds read as an array of equally sized elements
 *        (::lh_memory_typed_t) and helpers.
 *
 * A typed memory object stores the fields of ::lh_memory_bounds_t plus the
 * size @c type_size of one element in bytes, so that the bytes it covers are
 * read as an array of equally sized elements. The element count is not
 * stored: it is the byte size divided by @c type_size.
 *
 * Typed memory whose bounds are uninitialized holds no elements. Valid typed
 * memory has valid bounds and a byte size that is a multiple of
 * @c type_size. Functions with the @c _v suffix and all indexed access
 * helpers require valid typed memory and may raise runtime errors when that
 * precondition is not met.
 *
 * @see lh_memory_typed_fields
 */

#ifndef LH_MEMORY_TYPED_H
#define LH_MEMORY_TYPED_H

#include <lh/memory/typed/fields.h>
#include <lh/memory/bounds.h>
#include <lh/index.h>

/**
 * @struct lh_memory_typed
 * @brief Raw memory bounds read as an array of equally sized elements: the
 *        fields of ::lh_memory_bounds_t and the size of one element.
 */
struct lh_memory_typed
{
    /** Members: ::lh_memory_typed_fields. */
    lh_memory_typed_fields(lh_void);
};
/** @copybrief lh_memory_typed */
typedef struct lh_memory_typed lh_memory_typed_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return the bounds of @p self as mutable bounds.
 *
 * A typed object starts with the fields of ::lh_memory_bounds_t, so the bounds
 * are the object itself and not a copy: writing through the returned pointer
 * changes the endpoints of @p self.
 *
 * @param self Typed memory to read.
 * @return The bounds of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_memory_bounds_t *
lh_memory_typed_get_bounds_mut(lh_memory_typed_t *self);

/**
 * @brief Return the bounds of @p self as read-only bounds.
 *
 * The read-only twin of ::lh_memory_typed_get_bounds_mut: the returned pointer
 * must not be used to change the endpoints of @p self.
 *
 * @param self Typed memory to read.
 * @return The bounds of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_memory_bounds_t *
lh_memory_typed_get_bounds(const lh_memory_typed_t *self);

/**
 * @brief Return the size of one element in bytes.
 *
 * @param self Typed memory to read.
 * @return Stored element size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_memory_typed_get_type_size(const lh_memory_typed_t *self);

/**
 * @brief Store the size of one element in @p self.
 *
 * The endpoints are left alone, so the byte size may stop being a multiple of
 * the new element size.
 *
 * @param self      Typed memory to update.
 * @param type_size New element size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_typed_set_type_size(lh_memory_typed_t *self, lh_usize_t type_size);

/**
 * @brief Exchange the contents of @p self and @p other.
 *
 * Both the endpoints and the element size are exchanged. Nothing is validated,
 * and swapping an object with itself does nothing.
 *
 * @param self  Typed memory to swap.
 * @param other Typed memory to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_typed_swap(lh_memory_typed_t *self, lh_memory_typed_t *other);

/**
 * @brief Return @c begin without validating the bounds range.
 * @param self Typed memory to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_typed_get_begin(const lh_memory_typed_t *self);

/**
 * @brief Return the exclusive end @c begin + @c size without validating the bounds range.
 * @param self Typed memory to read.
 * @return Stored exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_typed_get_end(const lh_memory_typed_t *self);

/**
 * @brief Return the size of @p self in bytes without validating the bounds range.
 * @param self Typed memory to read.
 * @return Stored size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_memory_typed_get_size_of_bytes(const lh_memory_typed_t *self);

/**
 * @brief True if @p self is uninitialized or has zero size.
 *
 * A valid <tt>[x, x)</tt>, with @c size zero, is empty too.
 *
 * @param self Typed memory to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_typed_is_empty(const lh_memory_typed_t *self);

/**
 * @brief True if the byte size of @p self is a multiple of the element size.
 *
 * @param self Typed memory to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        The element size is zero.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid.
 */
lh_bool_t
lh_memory_typed_is_multiple_of(const lh_memory_typed_t *self);

/**
 * @brief Reset the bounds of @p self to the uninitialized empty bounds.
 *
 * The element size is kept.
 *
 * @param self Typed memory to clear.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_typed_clear(lh_memory_typed_t *self);

/**
 * @brief Store @p begin, @p end and @p type_size in @p self without range validation.
 *
 * The caller is responsible for ensuring the resulting range is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param self      Typed memory to update.
 * @param begin     New @c begin.
 * @param end       New exclusive end; @c size becomes <tt>end - begin</tt>.
 * @param type_size New element size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_typed_set(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Copy the endpoints and the element size from @p other to @p self.
 *
 * This function preserves the typed state as-is. Use ::lh_memory_typed_assign_v
 * when the source must be a valid typed range.
 *
 * @param self  Typed memory to update.
 * @param other Typed memory to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_memory_typed_assign(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/**
 * @brief Alias for ::lh_memory_typed_get_begin.
 * @param self Typed memory to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_typed_get_data(const lh_memory_typed_t *self);

/**
 * @brief Return the number of elements in @p self.
 *
 * The element count is not stored: it is the byte size divided by the element
 * size. A range of zero bytes has no elements, whatever the element size is.
 * The element size must be non-zero for a non-empty range.
 *
 * @param self Typed memory to read.
 * @return Number of elements in @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_memory_typed_get_size(const lh_memory_typed_t *self);

/**
 * @brief True if @p self is a valid typed range.
 *
 * The bounds must be valid and the byte size must be a multiple of the
 * element size.
 *
 * @param self Typed memory to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        The element size is zero and the bounds of @p self are valid.
 */
lh_bool_t
lh_memory_typed_is_valid(const lh_memory_typed_t *self);

/**
 * @brief Initialize @p self as an empty typed range of the given element size.
 *
 * The bounds are reset to the uninitialized empty bounds, so @p self holds no
 * elements.
 *
 * @param self      Typed memory to initialize.
 * @param type_size Size of one element in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_typed_init_empty(lh_memory_typed_t *self, lh_usize_t type_size);

/**
 * @brief True if @p index addresses an element of @p self.
 *
 * Valid indices are in the half-open numeric interval
 * <tt>[0, lh_memory_typed_get_size(self))</tt>. An index outside it is
 * reported here, not raised.
 *
 * @param self  Typed memory to inspect.
 * @param index Element index to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_typed_is_valid_index(const lh_memory_typed_t *self, lh_uindex_t index);

/**
 * @brief Copy the endpoints and the element size from valid @p other to @p self.
 *
 * @param self  Typed memory to update.
 * @param other Valid typed memory to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        The element size of @p other is zero and its bounds are valid.
 */
lh_void
lh_memory_typed_assign_v(lh_memory_typed_t *self, const lh_memory_typed_t *other);

/**
 * @brief Swap two valid typed ranges.
 *
 * @param self  Valid typed memory to swap.
 * @param other Valid typed memory to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        The element size of @p self or @p other is zero and its bounds are valid.
 */
lh_void
lh_memory_typed_swap_v(lh_memory_typed_t *self, lh_memory_typed_t *other);

/**
 * @brief Return the byte offset of the element at @p index from @c begin.
 *
 * @param self  Typed memory to inspect.
 * @param index Element index to convert.
 * @return Byte offset of the element from @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 */
lh_uoffset_t
lh_memory_typed_get_offset_from_index(const lh_memory_typed_t *self, lh_uindex_t index);

/**
 * @brief Return the index of the last element of @p self.
 *
 * @param self Typed memory to inspect.
 * @return Index of the last element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self holds no elements.
 */
lh_uindex_t
lh_memory_typed_get_last_index(const lh_memory_typed_t *self);

/**
 * @brief Return the index of the element at byte @p offset from @c begin.
 *
 * The element size must be non-zero.
 *
 * @param self   Typed memory to inspect.
 * @param offset Byte offset from the begin endpoint.
 * @return Index of the element at @p offset.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_size_not_multiple_of_type_size
 *        @p offset is not a multiple of the element size.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
lh_uindex_t
lh_memory_typed_get_index_from_offset(const lh_memory_typed_t *self, lh_uoffset_t offset);

/**
 * @brief Store @p begin, @p end and @p type_size in @p self after validation.
 *
 * The resulting range must be valid and its byte size must be a multiple of
 * the element size.
 *
 * @param self      Typed memory to update.
 * @param begin     New @c begin.
 * @param end       New exclusive end; @c size becomes <tt>end - begin</tt>.
 * @param type_size New element size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The resulting range is not a valid typed range.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        @p type_size is zero and the resulting bounds are valid.
 */
lh_void
lh_memory_typed_set_v(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Return the pointer to the element at @p index, counting from the beginning.
 *
 * @param self  Typed memory to index.
 * @param index Element index to address.
 * @return Pointer to the first byte of the element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid.
 */
lh_ptr
lh_memory_typed_get_ptr_from_begin(const lh_memory_typed_t *self, lh_uindex_t index);

/**
 * @brief Return the index @p index elements before the last element of @p self.
 *
 * Index @c 0 is the last element, index @c 1 the one before it, and so on.
 *
 * @param self  Typed memory to inspect.
 * @param index Number of elements to walk back from the last one.
 * @return Index of the element @p index steps before the last one.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 */
lh_uindex_t
lh_memory_typed_get_index_from_end(const lh_memory_typed_t *self, lh_uindex_t index);

/**
 * @brief Return the index of the element at @p ptr.
 *
 * @param self Typed memory to inspect.
 * @param ptr  Pointer to an element of @p self.
 * @return Index of the element at @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @fails ::lh_runtime_error_code_size_not_multiple_of_type_size
 *        @p ptr does not address the first byte of an element.
 */
lh_uindex_t
lh_memory_typed_get_index_from_ptr(const lh_memory_typed_t *self, const lh_ptr ptr);

/**
 * @brief Initialize @p self as a valid typed range.
 *
 * @param self      Typed memory to initialize.
 * @param begin     New @c begin.
 * @param end       New exclusive end; @c size becomes <tt>end - begin</tt>.
 * @param type_size New element size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The resulting range is not a valid typed range.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        @p type_size is zero and the resulting bounds are valid.
 */
lh_void
lh_memory_typed_init(lh_memory_typed_t *self, lh_ptr begin, lh_ptr end, lh_usize_t type_size);

/**
 * @brief Return the pointer to the element @p index steps before the last one.
 *
 * Index @c 0 is the last element, index @c 1 the one before it, and so on.
 *
 * @param self  Typed memory to index.
 * @param index Number of elements to walk back from the last one.
 * @return Pointer to the first byte of the element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 */
lh_ptr
lh_memory_typed_get_ptr_from_end(const lh_memory_typed_t *self, lh_uindex_t index);

/**
 * @brief Return the pointer to the first element of @p self.
 *
 * @param self Typed memory to index.
 * @return Pointer to the first byte of the first element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self holds no elements.
 */
lh_ptr
lh_memory_typed_get_first_ptr(const lh_memory_typed_t *self);

/**
 * @brief Build the bounds of the element at @p index.
 *
 * @param self  Typed memory to index.
 * @param index Element index to address.
 * @param out   Receives the bounds of the element; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the element size is zero.
 */
lh_void
lh_memory_typed_get_value_bounds(const lh_memory_typed_t *self, lh_uindex_t index,
                                 lh_memory_bounds_t *out);

/**
 * @brief Build the bounds of @p count elements starting at @p index.
 *
 * @param self  Typed memory to index.
 * @param index Index of the first element of the range.
 * @param count Number of elements in the range.
 * @param out   Receives the bounds of the range; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        The range is not inside @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the range is empty.
 */
lh_void
lh_memory_typed_get_range_bounds(const lh_memory_typed_t *self, lh_uindex_t index, lh_usize_t count,
                                 lh_memory_bounds_t *out);

/**
 * @brief Copy the element at @p index into @p dst.
 *
 * Exactly one element is copied: the element size in bytes.
 *
 * @param self  Typed memory to read.
 * @param index Element index to read.
 * @param dst   Receives the element; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p dst is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the element size is zero.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 */
lh_void
lh_memory_typed_get_value_into(const lh_memory_typed_t *self, lh_uindex_t index, lh_ptr dst);

/**
 * @brief Return the pointer to the last element of @p self.
 *
 * @param self Typed memory to index.
 * @return Pointer to the first byte of the last element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self holds no elements.
 */
lh_ptr
lh_memory_typed_get_last_ptr(const lh_memory_typed_t *self);

/**
 * @brief Return the pointer to the element at @p index.
 *
 * Non-negative indices are counted from the beginning. Negative indices are
 * counted from the last element: @c -1 addresses the last element, @c -2 the
 * one before it.
 *
 * @param self  Typed memory to index.
 * @param index Signed element index.
 * @return Pointer to the first byte of the element.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index does not address an element of @p self.
 */
lh_ptr
lh_memory_typed_get_ptr(const lh_memory_typed_t *self, lh_sindex_t index);

/**
 * @brief Copy @p value into the element at @p index.
 *
 * Exactly one element is written: the element size in bytes.
 *
 * @param self  Typed memory to update.
 * @param index Element index to write.
 * @param value Source of the element bytes; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p value is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the element size is zero.
 */
lh_void
lh_memory_typed_set_value(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr value);

/**
 * @brief True if the element at @p index equals @p other.
 *
 * The element size in bytes is compared, so the bytes may be of any type.
 *
 * @param self  Typed memory to read.
 * @param index Element index to read.
 * @param other Bytes to compare against; must not be null.
 * @return ::lh_bool_true when the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p index is not below the element count of @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the element size is zero.
 */
lh_bool_t
lh_memory_typed_value_equals(const lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr other);

/**
 * @brief Copy @p count elements from @p values into @p self, starting at @p index.
 *
 * A count of zero copies nothing and does nothing.
 *
 * @param self   Typed memory to update.
 * @param index  Index of the first element to write.
 * @param values Source of the element bytes; must not be null.
 * @param count  Number of elements to copy.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p values is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        The range is not inside @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the range is empty.
 */
lh_void
lh_memory_typed_set_values(lh_memory_typed_t *self, lh_uindex_t index, const lh_ptr values,
                           lh_usize_t count);

/**
 * @brief Move @p count elements inside @p self, from @p src_index to @p dst_index.
 *
 * The move supports overlapping source and destination ranges. A count of zero
 * moves nothing and does nothing.
 *
 * @param self      Typed memory to update.
 * @param dst_index Index of the first element to write.
 * @param src_index Index of the first element to read.
 * @param count     Number of elements to move.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        A range is not inside @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or a range is empty.
 */
lh_void
lh_memory_typed_move_within(lh_memory_typed_t *self, lh_uindex_t dst_index, lh_uindex_t src_index,
                            lh_usize_t count);

/**
 * @brief Exchange the elements at @p i and @p j.
 *
 * @p scratch is used as temporary storage: it must have room for one element,
 * the element size in bytes.
 *
 * @param self    Typed memory to update.
 * @param i       Index of the first element.
 * @param j       Index of the second element.
 * @param scratch Temporary buffer of one element; must not be null.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p scratch is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p i or @p j is not below the element count of @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        The bounds of @p self are not valid, or the element size is zero.
 */
lh_void
lh_memory_typed_swap_values(lh_memory_typed_t *self, lh_uindex_t i, lh_uindex_t j, lh_ptr scratch);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_TYPED_H */
