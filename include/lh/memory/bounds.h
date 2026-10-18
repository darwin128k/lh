/**
 * @file bounds.h
 * @brief Half-open mutable byte bounds (::lh_memory_bounds_t) and helpers.
 *
 * A memory bounds object stores a pointer @c begin to its first byte and its
 * length @c size in bytes, describing the right-open address interval
 * <tt>[begin, begin + size)</tt>. Its exclusive end is @c begin + @c size,
 * computed on the address, so any end that was set, backward or null
 * included, reads back unchanged.
 *
 * Bounds whose @c begin is ::lh_null are uninitialized. Valid bounds have a
 * non-null @c begin and a @c begin + @c size that does not wrap past the
 * address space, so the end is at or after @c begin. Functions with the
 * @c _v suffix and all size / containment / indexed access / raw byte
 * operation helpers require valid bounds and may raise runtime errors when
 * that precondition is not met.
 *
 * @see lh_memory_bounds_fields
 */

#ifndef LH_MEMORY_BOUNDS_H
#define LH_MEMORY_BOUNDS_H

#include <lh/memory/bounds/fields.h>
#include <lh/memory/view.h>

/**
 * @struct lh_memory_bounds
 * @brief Non-owning mutable half-open byte bounds: an ::lh_ptr and a byte size.
 *
 * @c begin and @c size describe <tt>[begin, begin + size)</tt> in address
 * space when @c begin is non-null and the sum does not wrap. Public typedef:
 * ::lh_memory_bounds_t.
 */
struct lh_memory_bounds
{
    /** Members: ::lh_memory_bounds_fields. */
    lh_memory_bounds_fields(lh_void);
};
/** @copybrief lh_memory_bounds */
typedef struct lh_memory_bounds lh_memory_bounds_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Return @c begin without validating the bounds range.
 * @param self Bounds to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_bounds_get_begin(const lh_memory_bounds_t *self);

/**
 * @brief Alias for ::lh_memory_bounds_get_begin.
 * @param self Bounds to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_bounds_get_data(const lh_memory_bounds_t *self);

/**
 * @brief Return the exclusive end @c begin + @c size without validating the bounds range.
 * @param self Bounds to read.
 * @return Stored exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_ptr
lh_memory_bounds_get_end(const lh_memory_bounds_t *self);

/**
 * @brief True if @c begin is ::lh_null.
 * @param self Bounds to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_is_uninitialized(const lh_memory_bounds_t *self);

/**
 * @brief True if @c begin is set and @c begin + @c size does not wrap past
 *        the address space.
 * @param self Bounds to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_is_valid(const lh_memory_bounds_t *self);

/**
 * @brief Return @c begin after validating @p self.
 * @param self Valid bounds to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_get_begin_v(const lh_memory_bounds_t *self);

/**
 * @brief Return the exclusive end @c begin + @c size after validating @p self.
 * @param self Valid bounds to read.
 * @return Stored exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_get_end_v(const lh_memory_bounds_t *self);

/**
 * @brief Return @c size without validating the bounds range.
 * @param self Bounds to read.
 * @return Stored size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_usize_t
lh_memory_bounds_get_size(const lh_memory_bounds_t *self);

/**
 * @brief Return @c size after validating @p self.
 * @param self Valid bounds to read.
 * @return Number of bytes covered by the half-open interval.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_usize_t
lh_memory_bounds_get_size_v(const lh_memory_bounds_t *self);

/**
 * @brief True if @p self is uninitialized or has zero size.
 *
 * A valid <tt>[x, x)</tt>, with @c size zero, is empty too.
 *
 * @param self Bounds to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_is_empty(const lh_memory_bounds_t *self);

/**
 * @brief True if @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_bounds_get_size(self))</tt>.
 *
 * @param self   Valid bounds to inspect.
 * @param offset Byte offset from @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_is_valid_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return byte offset of @p ptr from @c begin.
 *
 * @param self Valid bounds to inspect.
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
lh_memory_bounds_get_offset_from_begin(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from the last byte, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c begin + @c size - 1. The exclusive end
 * itself is not inside the bounds.
 *
 * @param self Valid bounds to inspect.
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
lh_memory_bounds_get_offset_from_end(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief True if @p ptr lies inside the half-open interval @p self.
 *
 * @param self Valid bounds to inspect.
 * @param ptr  Pointer to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_contains_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief True if half-open range <tt>[begin, end)</tt> lies inside @p self.
 *
 * @param self  Valid outer bounds.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_contains_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_bounds_contains_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_contains_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end);

/**
 * @brief True if @p other lies completely inside @p self.
 *
 * Both operands are interpreted as half-open bounds. @p other is not
 * validated.
 *
 * @param self  Valid outer bounds.
 * @param other Inner bounds to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_contains(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief True if valid @p other lies completely inside @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_bool_t
lh_memory_bounds_contains_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Return pointer at byte @p offset from @c begin.
 *
 * @param self   Valid bounds to index.
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
lh_ptr
lh_memory_bounds_get_ptr_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from the last byte, walking backward.
 *
 * Offset @c 0 returns @c begin + @c size - 1, offset @c 1 returns the previous byte,
 * and so on.
 *
 * @param self   Valid bounds to index.
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
lh_ptr
lh_memory_bounds_get_ptr_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer to the first byte.
 * @param self Valid bounds to index.
 * @return Pointer to @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_get_first_ptr(const lh_memory_bounds_t *self);

/**
 * @brief Return pointer to the last byte.
 * @param self Valid non-empty bounds to index.
 * @return Pointer to @c begin + @c size - 1.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
lh_ptr
lh_memory_bounds_get_last_ptr(const lh_memory_bounds_t *self);

/**
 * @brief Return pointer by signed offset.
 *
 * Non-negative offsets are measured from @c begin. Negative offsets are
 * measured from the last byte: @c -1 addresses @c begin + @c size - 1, @c -2 the
 * previous byte.
 *
 * @param self   Valid bounds to index.
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
lh_ptr
lh_memory_bounds_get_ptr(const lh_memory_bounds_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c begin.
 *
 * @param self   Valid bounds to index.
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
lh_memory_bounds_get_value_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from the last byte, walking backward.
 *
 * @param self   Valid bounds to index.
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
lh_memory_bounds_get_value_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte by signed offset.
 *
 * @param self   Valid bounds to index.
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
lh_memory_bounds_get_value(const lh_memory_bounds_t *self, lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 *
 * @param self Valid bounds to read.
 * @return Byte at @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_bounds_get_first_value(const lh_memory_bounds_t *self);

/**
 * @brief Read the last byte of @p self.
 *
 * @param self Valid bounds to read.
 * @return Byte at @c begin + @c size - 1.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_bounds_get_last_value(const lh_memory_bounds_t *self);

/**
 * @brief Return target byte offset after applying signed @p offset.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed offset
 * accepted by ::lh_memory_bounds_get_ptr. Otherwise @p offset is
 * applied relative to @p ptr.
 *
 * @param self   Valid bounds to seek in.
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
lh_memory_bounds_get_offset_from_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr,
                                     lh_soffset_t offset);

/**
 * @brief Return pointer reached by seeking @p offset bytes from @p ptr.
 *
 * Boundary underflow and overflow are converted to ::lh_null.
 *
 * @param self   Valid bounds to seek in.
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
lh_memory_bounds_seek_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr, lh_soffset_t offset);

/**
 * @brief Return the byte pointer after @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_bounds_next_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Return the byte pointer before @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_bounds_prev_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

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
lh_memory_bounds_seek_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

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
lh_memory_bounds_next_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

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
lh_memory_bounds_prev_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief True if half-open range <tt>[begin, end)</tt> overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_overlaps_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_bounds_overlaps_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_overlaps_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end);

/**
 * @brief True if @p other overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_overlaps(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief True if valid @p other overlaps @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_bool_t
lh_memory_bounds_overlaps_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief True if half-open bounds size is divisible by @p alignment.
 *
 * @param self      Valid bounds to inspect.
 * @param alignment Non-zero divisor for the bounds size.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_is_multiple_of(const lh_memory_bounds_t *self, lh_usize_t alignment);

/**
 * @brief True if @c begin is aligned to @p align.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align);

/**
 * @brief True if both endpoints are aligned to @p align.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_bounds_is_aligned(const lh_memory_bounds_t *self, lh_usize_t align);

/**
 * @brief True if @p self stores exactly @p begin and @p end.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_equals_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_bounds_equals_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_equals_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p self and @p other store the same endpoints.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_bool_t
lh_memory_bounds_equals(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Find the first occurrence of <tt>[data, data + size)</tt> inside @p self.
 *
 * The brick the <tt>[begin, end)</tt> and bounds-operand forms are built on.
 *
 * @param self Valid bounds.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_find_by_size(const lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Find the first occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid bounds used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
lh_ptr
lh_memory_bounds_find_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Find the first occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid bounds used as the haystack.
 * @param other Valid bounds used as the needle.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_find(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Find the last occurrence of <tt>[data, data + size)</tt> inside @p self.
 *
 * The brick the <tt>[begin, end)</tt> and bounds-operand forms are built on.
 *
 * @param self Valid bounds.
 * @param data First byte of the other range.
 * @param size Number of bytes in the other range.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_rfind_by_size(const lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Find the last occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid bounds used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
lh_ptr
lh_memory_bounds_rfind_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Find the last occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid bounds used as the haystack.
 * @param other Valid bounds used as the needle.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_rfind(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Compare @p self with <tt>[data, data + size)</tt> up to the smaller size.
 *
 * The brick the <tt>[begin, end)</tt> and bounds-operand forms are built on.
 *
 * @param self Valid bounds.
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
lh_ptr
lh_memory_bounds_compare_by_size(const lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Compare @p self with half-open range <tt>[begin, end)</tt>.
 *
 * Delegates to ::lh_memory_raw_compare and compares up to the smaller size.
 *
 * @param self  Valid left-hand bounds.
 * @param begin Right-hand range begin pointer.
 * @param end   Right-hand range exclusive end pointer.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when the
 *         compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
lh_ptr
lh_memory_bounds_compare_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                               const lh_ptr end);

/**
 * @brief Compare @p self with @p other.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid left-hand bounds.
 * @param other Valid right-hand bounds.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when the
 *         compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_compare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Compare suffixes of @p self and <tt>[data, data + size)</tt>.
 *
 * The brick the <tt>[begin, end)</tt> and bounds-operand forms are built on.
 *
 * @param self Valid bounds.
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
lh_ptr
lh_memory_bounds_rcompare_by_size(const lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Compare suffixes of @p self and <tt>[begin, end)</tt>.
 *
 * Delegates to ::lh_memory_raw_rcompare.
 *
 * @param self  Valid left-hand bounds.
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
lh_ptr
lh_memory_bounds_rcompare_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end);

/**
 * @brief Compare suffixes of @p self and @p other.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid left-hand bounds.
 * @param other Valid right-hand bounds.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_rcompare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Store @p begin in @p self, keeping @c size.
 *
 * @param self  Bounds to update.
 * @param begin New @c begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_bounds_set_begin(lh_memory_bounds_t *self, lh_ptr begin);

/**
 * @brief Store @p size in @p self, keeping @c begin.
 *
 * @param self Bounds to update.
 * @param size New @c size in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_bounds_set_size(lh_memory_bounds_t *self, lh_usize_t size);

/**
 * @brief Store @p begin and @p end in @p self without range validation.
 *
 * The caller is responsible for ensuring the resulting bounds is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param self  Bounds to update.
 * @param begin New @c begin.
 * @param end   New exclusive end; @c size becomes <tt>end - begin</tt>.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_bounds_set(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Copy endpoints from @p other to @p self.
 *
 * This function preserves the bounds state as-is. Use
 * ::lh_memory_bounds_assign_v when the source must be a valid half-open range.
 *
 * @param self  Bounds to update.
 * @param other Bounds to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_void
lh_memory_bounds_assign(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Reset @p self to the uninitialized empty bounds.
 *
 * @param self Bounds to clear.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_bounds_clear(lh_memory_bounds_t *self);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 *
 * @param self  Bounds to update.
 * @param other Valid bounds to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_bounds_assign_v(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Store @p begin and @p end in @p self after validation.
 *
 * @param self  Bounds to update.
 * @param begin New @c begin.
 * @param end   New exclusive end; @c size becomes <tt>end - begin</tt>.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
lh_void
lh_memory_bounds_set_v(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Swap @p self and @p other without range validation.
 *
 * The caller is responsible for ensuring both bounds are meaningful.
 * No range check is performed on either operand.
 *
 * @param self  Bounds to swap.
 * @param other Bounds to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_bounds_swap(lh_memory_bounds_t *self, lh_memory_bounds_t *other);

/**
 * @brief Swap two valid bounds.
 *
 * @param self  Valid bounds to swap.
 * @param other Valid bounds to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_void
lh_memory_bounds_swap_v(lh_memory_bounds_t *self, lh_memory_bounds_t *other);

/**
 * @brief Store a half-open range starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param self  Bounds to update.
 * @param begin New @c begin.
 * @param size  Number of bytes in the half-open bounds.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed bounds are not valid.
 */
lh_void
lh_memory_bounds_set_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Clear @p self, then swap it with @p other.
 *
 * When @p self and @p other are different objects, @p self receives the
 * previous endpoints of valid @p other, and @p other receives empty bounds.
 * When @p self and @p other are the same object, the bounds are only cleared.
 *
 * @param self  Bounds to clear and replace.
 * @param other Valid bounds to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_bounds_swap_and_clear(lh_memory_bounds_t *self, lh_memory_bounds_t *other);

/**
 * @brief Initialize @p self with valid @p begin and @p end endpoints.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
lh_void
lh_memory_bounds_init(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Initialize @p self as a half-open range starting at @p begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed bounds are not valid.
 */
lh_void
lh_memory_bounds_init_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with the empty bounds initializer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_bounds_init_empty(lh_memory_bounds_t *self);

/**
 * @brief Initialize @p self by copying endpoints from valid @p other.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_bounds_init_by_other(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Build bounds containing the first @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the returned
 * bounds is the empty <tt>[begin, begin)</tt>, still at @p begin.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the beginning.
 * @return Bounds <tt>[begin, begin + n)</tt>.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not valid bounds.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_bounds_take_first(lh_ptr begin, lh_ptr end, lh_usize_t n, lh_memory_bounds_t *out);

/**
 * @brief Build bounds containing the last @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the returned
 * bounds is the empty <tt>[end, end)</tt>, still at @p end.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the end.
 * @return Bounds <tt>[end - n, end)</tt>.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not valid bounds.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_bounds_take_last(lh_ptr begin, lh_ptr end, lh_usize_t n, lh_memory_bounds_t *out);

/**
 * @brief Build bounds containing the first @p size bytes of @p self.
 *
 * If @p size is zero, the returned bounds is empty, at the source begin.
 *
 * @param self Source bounds.
 * @param size Number of bytes to keep from the beginning.
 * @return Bounds taken from the beginning of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source bounds size.
 */
lh_void
lh_memory_bounds_from_begin(const lh_memory_bounds_t *self, lh_usize_t size,
                            lh_memory_bounds_t *out);

/**
 * @brief Build bounds containing the last @p size bytes of @p self.
 *
 * If @p size is zero, the returned bounds is empty, at the source end.
 *
 * @param self Source bounds.
 * @param size Number of bytes to keep from the end.
 * @return Bounds taken from the end of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source bounds size.
 */
lh_void
lh_memory_bounds_from_end(const lh_memory_bounds_t *self, lh_usize_t size, lh_memory_bounds_t *out);

/**
 * @brief Build sub-bounds between @p begin and @p end inside @p self.
 *
 * @param self  Source bounds that must contain the requested range.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 * @return Constructed valid inner bounds.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[begin, end)</tt> is not contained by @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not valid bounds.
 */
lh_void
lh_memory_bounds_between(const lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end,
                         lh_memory_bounds_t *out);

/**
 * @brief Build sub-bounds starting at @p offset with @p size bytes.
 *
 * If @p size is zero, the returned bounds is empty, at @p offset, which may then
 * equal the source size.
 *
 * @param self   Source bounds.
 * @param offset Offset from the source beginning.
 * @param size   Number of bytes in the returned bounds.
 * @return Bounds <tt>[begin + offset, begin + offset + size)</tt>.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[offset, offset + size)</tt> is outside @p self.
 */
lh_void
lh_memory_bounds_from_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset, lh_usize_t size,
                             lh_memory_bounds_t *out);

/**
 * @brief Build bounds with @p n bytes removed from the beginning.
 *
 * If @p n equals the source size, the returned bounds is empty, at the source end.
 *
 * @param self Source bounds.
 * @param n    Number of bytes to remove from the beginning.
 * @return Source bounds without the first @p n bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source bounds size.
 */
lh_void
lh_memory_bounds_drop_first(const lh_memory_bounds_t *self, lh_usize_t n, lh_memory_bounds_t *out);

/**
 * @brief Build bounds with @p n bytes removed from the end.
 *
 * If @p n equals the source size, the returned bounds is empty, at the source
 * begin.
 *
 * @param self Source bounds.
 * @param n    Number of bytes to remove from the end.
 * @return Source bounds without the last @p n bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source bounds size.
 */
lh_void
lh_memory_bounds_drop_last(const lh_memory_bounds_t *self, lh_usize_t n, lh_memory_bounds_t *out);

/**
 * @brief Build bounds with bytes removed from both sides.
 *
 * If @p left + @p right equals the source size, the returned bounds is empty,
 * @p left bytes past the source begin.
 *
 * @param self  Source bounds.
 * @param left  Number of bytes to remove from the beginning.
 * @param right Number of bytes to remove from the end.
 * @return Trimmed source bounds.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p left + @p right is greater than the source bounds size.
 */
lh_void
lh_memory_bounds_trim(const lh_memory_bounds_t *self, lh_usize_t left, lh_usize_t right,
                      lh_memory_bounds_t *out);

/**
 * @brief Copy bytes from <tt>[data, data + size)</tt> into @p self.
 *
 * Delegates to ::lh_memory_raw_copy, so it writes up to the smaller of the
 * destination and source sizes. The brick the <tt>[begin, end)</tt> and bounds-operand forms are
 * built on.
 *
 * @param self Valid destination bounds.
 * @param data First source byte.
 * @param size Number of source bytes.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_copy_by_size(lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Move bytes from <tt>[data, data + size)</tt> into @p self.
 *
 * Delegates to ::lh_memory_raw_move and supports overlapping storage. The brick the <tt>[begin, end)</tt> and bounds-operand forms are
 * built on.
 *
 * @param self Valid destination bounds.
 * @param data First source byte.
 * @param size Number of source bytes.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p data is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_move_by_size(lh_memory_bounds_t *self, const lh_ptr data, lh_usize_t size);

/**
 * @brief Fill @p self with @p value.
 *
 * @param self  Valid destination bounds.
 * @param value Byte value to write.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_fill(lh_memory_bounds_t *self, lh_byte_t value);

/**
 * @brief Fill @p self by repeating pattern range <tt>[begin, end)</tt>.
 *
 * @param self  Valid destination bounds.
 * @param begin Pattern range begin pointer.
 * @param end   Pattern range exclusive end pointer.
 * @return Pointer one past the last byte written, or ::lh_null when nothing is
 *         written by the raw operation.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_ptr
lh_memory_bounds_fill_pattern_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Write @p value at byte @p offset from @c begin.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the begin endpoint.
 * @param value  Byte value to write.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
lh_void
lh_memory_bounds_set_value(const lh_memory_bounds_t *self, lh_uoffset_t offset, lh_byte_t value);

/**
 * @brief A read-only view of the memory @p self describes.
 *
 * It only drops write access, so any function that reads through a
 * ::lh_memory_view_t can be handed a ::lh_memory_bounds_t. There is no way
 * back: a view never becomes writable bounds. Nothing is copied — the view
 * points at the same memory, and is valid as long as that memory is.
 *
 * Example usage:
 * @code{.c}
 * lh_byte_t buf[16];
 * lh_memory_bounds_t b;
 * lh_memory_view_t v;
 * lh_memory_bounds_init_by_size(lh_addr_of(b), buf, sizeof(buf));
 * lh_memory_bounds_as_view(lh_addr_of(b), lh_addr_of(v));
 * @endcode
 *
 * @param self Source.
 * @param out  Receives the view with the same @c begin and @c size as @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 */
lh_void
lh_memory_bounds_as_view(const lh_memory_bounds_t *self, lh_memory_view_t *out);

/**
 * @brief Copy bytes from @p other into @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid destination bounds.
 * @param other Valid source bounds.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_copy(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Copy bytes from half-open range <tt>[begin, end)</tt> into @p self.
 *
 * Delegates to ::lh_memory_raw_copy, so it writes up to the smaller of the
 * destination and source sizes.
 *
 * @param self  Valid destination bounds.
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
lh_ptr
lh_memory_bounds_copy_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Move bytes from @p other into @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid destination bounds.
 * @param other Valid source bounds.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_move(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Move bytes from half-open range <tt>[begin, end)</tt> into @p self.
 *
 * Delegates to ::lh_memory_raw_move and supports overlapping storage.
 *
 * @param self  Valid destination bounds.
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @return Pointer one past the last byte written.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or <tt>[begin, end)</tt> is not valid.
 */
lh_ptr
lh_memory_bounds_move_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Fill @p self by repeating bytes from @p other.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid destination bounds.
 * @param other Valid pattern bounds.
 * @return Pointer one past the last byte written, or ::lh_null when nothing is
 *         written by the raw operation.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_ptr
lh_memory_bounds_fill_pattern(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_BOUNDS_H */
