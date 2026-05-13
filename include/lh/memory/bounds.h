/**
 * @file bounds.h
 * @brief Half-open mutable byte bounds (::lh_memory_bounds_t) and helpers.
 *
 * A memory bounds object stores two endpoints @c first and @c second describing
 * a right-open address interval <tt>[first, second)</tt>. The first endpoint
 * points to the first byte in the range; the second endpoint points one byte
 * past the last byte. Consequently, size is @c second @c - @c first for
 * initialized forward bounds.
 *
 * This is intentionally different from ::lh_memory_bounds_slice_t helpers,
 * which treat the same two-field storage as a closed interval
 * <tt>[first, second]</tt>. The public typedef is layout-compatible with the
 * slice type, but every function in this header interprets @c second as an
 * exclusive endpoint.
 *
 * The bounds can also represent incomplete storage: each endpoint may be
 * ::lh_null. Endpoint presence is reported as
 * ::lh_memory_view_slice_flags_t, and initialized bounds are classified by
 * direction using ::lh_memory_view_slice_direction_t.
 *
 * Valid bounds are initialized and forward ordered for a right-open interval
 * (@c first &lt; @c second). Functions with the @c _v suffix and all size /
 * containment / indexed access / raw byte operation helpers require valid
 * bounds and may raise runtime errors when that precondition is not met.
 *
 * @see lh_memory_bounds_slice_t
 * @see lh_memory_view_slice_flags_t
 * @see lh_memory_view_slice_direction_t
 */

#ifndef LH_MEMORY_BOUNDS_H
#define LH_MEMORY_BOUNDS_H

#include <lh/memory/bounds/slice.h>

/**
 * @brief Non-owning mutable half-open byte bounds.
 *
 * Endpoints describe <tt>[first, second)</tt> in address space when both are
 * non-null and ordered. Public typedef: ::lh_memory_bounds_t.
 */
typedef lh_memory_bounds_slice_t lh_memory_bounds_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- unpack / getters ------------------------------------------------------ */

/**
 * @brief Read @c first / @c second from @p self into optional outputs.
 *
 * Pass ::lh_null for @p begin or @p end to skip that output.
 *
 * @param self  Bounds to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_unpack(const lh_memory_bounds_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Return @c first without validating the bounds range.
 * @param self Bounds to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_begin(const lh_memory_bounds_t *self);

/**
 * @brief Alias for ::lh_memory_bounds_get_begin.
 * @param self Bounds to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_data(const lh_memory_bounds_t *self);

/**
 * @brief Return @c second without validating the bounds range.
 * @param self Bounds to read.
 * @return Stored exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_end(const lh_memory_bounds_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief True iff neither endpoint is initialized.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_uninitialized(const lh_memory_bounds_t *self);

/**
 * @brief True iff both endpoints are initialized.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_initialized(const lh_memory_bounds_t *self);

/**
 * @brief Classify initialized endpoint order for a half-open interval.
 *
 * Returns ::lh_memory_view_slice_direction_unknown until both endpoints are
 * initialized. Otherwise returns forward for @c first &lt; @c second and
 * backward for equal or reversed bounds.
 *
 * @param self Bounds to inspect.
 * @return Direction value from ::lh_memory_view_slice_direction_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_slice_direction_t
lh_memory_bounds_get_direction(const lh_memory_bounds_t *self);

/**
 * @brief True iff @p self is initialized and ordered @c first &lt; @c second.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_forward(const lh_memory_bounds_t *self);

/**
 * @brief True iff @p self is initialized and not forward.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_backward(const lh_memory_bounds_t *self);

/**
 * @brief True iff @p self is initialized and forward ordered.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_valid(const lh_memory_bounds_t *self);

/**
 * @brief Logical negation of ::lh_memory_bounds_is_valid.
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_invalid(const lh_memory_bounds_t *self);

/* -- validated access, size, containment ---------------------------------- */

/**
 * @brief Like ::lh_memory_bounds_unpack but requires valid bounds.
 *
 * @param self  Valid bounds to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_unpack_v(const lh_memory_bounds_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Return @c first after validating @p self.
 * @param self Valid bounds to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_begin_v(const lh_memory_bounds_t *self);

/**
 * @brief Return @c second after validating @p self.
 * @param self Valid bounds to read.
 * @return Stored exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_end_v(const lh_memory_bounds_t *self);

/**
 * @brief Return half-open bounds size in bytes.
 *
 * For valid bounds this is @c second @c - @c first.
 *
 * @param self Valid bounds to read.
 * @return Number of bytes covered by the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_bounds_get_size(const lh_memory_bounds_t *self);

/**
 * @brief True iff @p self is uninitialized or has zero size.
 *
 * Because valid half-open bounds require @c first &lt; @c second, initialized
 * valid bounds have non-zero size. The uninitialized state is therefore the
 * practical empty value in this API.
 *
 * @param self Bounds to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is invalid and not uninitialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_empty(const lh_memory_bounds_t *self);

/**
 * @brief True iff @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_bounds_get_size(self))</tt>.
 *
 * @param self   Valid bounds to inspect.
 * @param offset Byte offset from @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_valid_offset(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return byte offset of @p ptr from @c first.
 *
 * @param self Valid bounds to inspect.
 * @param ptr  Pointer whose offset is requested.
 * @return Byte offset from @c first to @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_uoffset_t
lh_memory_bounds_get_offset_from_begin(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from the last byte, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c second - 1. The exclusive @c second
 * endpoint itself is not inside the bounds.
 *
 * @param self Valid bounds to inspect.
 * @param ptr  Pointer whose reverse offset is requested.
 * @return Byte offset from the last byte back to @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_uoffset_t
lh_memory_bounds_get_offset_from_end(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief True iff @p ptr lies inside the half-open interval @p self.
 *
 * @param self Valid bounds to inspect.
 * @param ptr  Pointer to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_contains_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief True iff half-open range <tt>[begin, end)</tt> lies inside @p self.
 *
 * @param self  Valid outer bounds.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_contains_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True iff @p other lies completely inside @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid outer bounds.
 * @param other Valid inner bounds.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        A bounds object is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_contains(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/* -- pointer and value access --------------------------------------------- */

/**
 * @brief Return pointer at byte @p offset from @c first.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the begin endpoint.
 * @return Pointer to the requested byte.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_ptr_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from the last byte, walking backward.
 *
 * Offset @c 0 returns @c second - 1, offset @c 1 returns the previous byte,
 * and so on.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the last byte.
 * @return Pointer to the requested byte.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_ptr_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer by signed offset.
 *
 * Non-negative offsets are measured from @c first. Negative offsets are
 * measured from the last byte: @c -1 addresses @c second - 1, @c -2 the
 * previous byte.
 *
 * @param self   Valid bounds to index.
 * @param offset Signed byte offset.
 * @return Pointer to the requested byte.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_get_ptr_by_offset(const lh_memory_bounds_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c first.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the begin endpoint.
 * @return Byte stored at the requested address.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_get_value_from_begin(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from the last byte, walking backward.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the last byte.
 * @return Byte stored at the requested address.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_get_value_from_end(const lh_memory_bounds_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte by signed offset.
 *
 * @param self   Valid bounds to index.
 * @param offset Signed byte offset.
 * @return Byte stored at the requested address.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_get_value_by_offset(const lh_memory_bounds_t *self, lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 *
 * @param self Valid bounds to read.
 * @return Byte at @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_get_first_value(const lh_memory_bounds_t *self);

/**
 * @brief Read the last byte of @p self.
 *
 * @param self Valid bounds to read.
 * @return Byte at @c second - 1.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_get_last_value(const lh_memory_bounds_t *self);

/**
 * @brief Write @p value at byte @p offset from @c first.
 *
 * @param self   Valid bounds to index.
 * @param offset Byte offset from the begin endpoint.
 * @param value  Byte value to write.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_set_value(const lh_memory_bounds_t *self, lh_uoffset_t offset, lh_byte_t value);

/**
 * @brief Return target byte offset after applying signed @p offset.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed offset
 * accepted by ::lh_memory_bounds_get_ptr_by_offset. Otherwise @p offset is
 * applied relative to @p ptr.
 *
 * @param self   Valid bounds to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target byte offset from @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self or the target offset is out of range.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_seek_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr, lh_soffset_t offset);

/**
 * @brief Return the byte pointer after @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_next_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Return the byte pointer before @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_prev_ptr(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Seek to @p ptr and read the byte there.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        The seek result is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_seek_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte after @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        There is no next byte inside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_next_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte before @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        There is no previous byte inside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_prev_value(const lh_memory_bounds_t *self, const lh_ptr ptr);

/* -- overlap, alignment, equality ----------------------------------------- */

/**
 * @brief True iff half-open range <tt>[begin, end)</tt> overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_overlaps_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_bounds_overlaps_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_overlaps_range(const lh_memory_bounds_t *self, const lh_ptr begin,
                                const lh_ptr end);

/**
 * @brief True iff @p other overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_overlaps(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief True iff valid @p other overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_overlaps_v(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief True iff half-open bounds size is divisible by @p alignment.
 *
 * @param self      Valid bounds to inspect.
 * @param alignment Non-zero divisor for the bounds size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_multiple_of(const lh_memory_bounds_t *self, lh_usize_t alignment);

/**
 * @brief Alias for ::lh_memory_bounds_multiple_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_multiple_of(const lh_memory_bounds_t *self, lh_usize_t multiple);

/**
 * @brief True iff @c first is aligned to @p align.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_aligned_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align);

/**
 * @brief Alias for ::lh_memory_bounds_aligned_is_begin_aligned.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_begin_aligned(const lh_memory_bounds_t *self, lh_usize_t align);

/**
 * @brief True iff both endpoints are aligned to @p align.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_is_aligned(const lh_memory_bounds_t *self, lh_usize_t align);

/**
 * @brief True iff @p self stores exactly @p begin and @p end.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_equals_of(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_bounds_equals_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_equals_range(const lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True iff @p self and @p other store the same endpoints.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_equals(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/* -- raw byte operations --------------------------------------------------- */

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_copy_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Copy bytes from @p other into @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid destination bounds.
 * @param other Valid source bounds.
 * @return Pointer one past the last byte written.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_copy(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_move_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Move bytes from @p other into @p self.
 *
 * Both operands are interpreted as half-open bounds.
 *
 * @param self  Valid destination bounds.
 * @param other Valid source bounds.
 * @return Pointer one past the last byte written.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_move(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Fill @p self with @p value.
 *
 * @param self  Valid destination bounds.
 * @param value Byte value to write.
 * @return Pointer one past the last byte written.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_fill_pattern_range(lh_memory_bounds_t *self, const lh_ptr begin, const lh_ptr end);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_fill_pattern(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Find the first occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid bounds used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_find(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Find the last occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid bounds used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range exclusive end pointer.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_rfind(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_compare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_rcompare(const lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/* -- mutation -------------------------------------------------------------- */

/**
 * @brief Reset @p self to the uninitialized empty bounds.
 *
 * @param self Bounds to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_clear(lh_memory_bounds_t *self);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 *
 * @param self  Bounds to update.
 * @param other Valid bounds to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_assign_v(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Store @p begin and @p end in @p self after validation.
 *
 * @param self  Bounds to update.
 * @param begin New @c first endpoint.
 * @param end   New exclusive @c second endpoint.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_set_v(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Build and validate bounds from @p begin and @p end.
 *
 * @param begin New @c first endpoint.
 * @param end   New exclusive @c second endpoint.
 * @return Constructed valid bounds value.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t
lh_memory_bounds_make_v(lh_ptr begin, lh_ptr end);

/**
 * @brief Build valid half-open bounds starting at @p begin with @p size bytes.
 *
 * The returned endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the half-open bounds.
 * @return Constructed valid bounds value.
 *
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t
lh_memory_bounds_make_by_size(lh_ptr begin, lh_usize_t size);

/**
 * @brief Return bounds with both endpoints null.
 *
 * @return Uninitialized empty bounds value.
 *
 * @see lh_memory_bounds_empty_initializer
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_t lh_memory_bounds_make_empty(lh_void);

/**
 * @brief Store a half-open range starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param self  Bounds to update.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the half-open bounds.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_set_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with valid @p begin and @p end endpoints.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_init(lh_memory_bounds_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Initialize @p self as a half-open range starting at @p begin.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed bounds are not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_init_by_size(lh_memory_bounds_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with the empty bounds initializer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_init_empty(lh_memory_bounds_t *self);

/**
 * @brief Initialize @p self by copying endpoints from valid @p other.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_init_by_other(lh_memory_bounds_t *self, const lh_memory_bounds_t *other);

/**
 * @brief Swap two valid bounds.
 *
 * @param self  Valid bounds to swap.
 * @param other Valid bounds to swap with.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_swap_v(lh_memory_bounds_t *self, lh_memory_bounds_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_swap_and_clear(lh_memory_bounds_t *self, lh_memory_bounds_t *other);

/* -- slice conversion ------------------------------------------------------ */

/**
 * @brief Build a closed slice copy from half-open @p self.
 *
 * Valid bounds <tt>[begin, end)</tt> become closed slice
 * <tt>[begin, end - 1]</tt>. Uninitialized empty bounds become an empty slice.
 *
 * @param self Bounds to copy as a closed slice.
 * @return Converted slice value.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is neither uninitialized nor valid half-open bounds.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_slice_t
lh_memory_bounds_make_slice(const lh_memory_bounds_t *self);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_BOUNDS_H
