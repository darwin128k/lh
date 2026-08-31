/**
 * @file view.h
 * @brief Half-open read-only byte view (::lh_memory_view_t) and helpers.
 *
 * A memory view object stores two @c const endpoints @c first and @c second
 * describing a right-open address interval <tt>[first, second)</tt>. The
 * first endpoint points to the first byte in the range; the second endpoint
 * points one byte past the last byte. Consequently, size is
 * @c second @c - @c first for initialized forward views.
 *
 * This is intentionally different from ::lh_memory_view_slice_t helpers,
 * which treat the same two-field storage as a closed interval
 * <tt>[first, second]</tt>. The public typedef is layout-compatible with the
 * slice type, but every function in this header interprets @c second as an
 * exclusive endpoint.
 *
 * The view can also represent incomplete storage: each endpoint may be
 * ::lh_null. Endpoint presence is reported as
 * ::lh_memory_view_slice_flags_t, and initialized views are classified by
 * direction using ::lh_memory_view_slice_direction_t.
 *
 * Valid views are initialized and forward ordered for a right-open interval
 * (@c first &lt; @c second). Functions with the @c _v suffix and all size /
 * containment / indexed access / raw byte operation helpers require valid
 * views and may raise runtime errors when that precondition is not met.
 *
 * @see lh_memory_view_slice_t
 * @see lh_memory_view_slice_flags_t
 * @see lh_memory_view_slice_direction_t
 */

#ifndef LH_MEMORY_VIEW_H
#define LH_MEMORY_VIEW_H

#include <lh/memory/view/slice.h>

/**
 * @brief Non-owning read-only half-open byte view.
 *
 * Endpoints describe <tt>[first, second)</tt> in address space when both are
 * non-null and ordered. Public typedef: ::lh_memory_view_t.
 */
typedef lh_memory_view_slice_t lh_memory_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- unpack / getters ------------------------------------------------------ */

/**
 * @brief Read @c first / @c second from @p self into optional outputs.
 *
 * Pass ::lh_null for @p begin or @p end to skip that output.
 *
 * @param self  View to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_unpack(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end);

/**
 * @brief Unpack @p self into @p other (alias for ::lh_memory_view_assign).
 * @param self  Source view (not null).
 * @param other Destination view (not null).
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_unpack_to_other(const lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Return @c first without validating the view range.
 * @param self View to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_begin(const lh_memory_view_t *self);

/**
 * @brief Alias for ::lh_memory_view_get_begin.
 * @param self View to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_data(const lh_memory_view_t *self);

/**
 * @brief Return @c second without validating the view range.
 * @param self View to read.
 * @return Stored exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_end(const lh_memory_view_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief Classify initialized endpoint order for a half-open interval.
 *
 * Returns ::lh_memory_view_slice_direction_unknown until both endpoints are
 * initialized. Otherwise returns forward for @c first &lt; @c second and
 * backward for equal or reversed views.
 *
 * @param self View to inspect.
 * @return Direction value from ::lh_memory_view_slice_direction_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_slice_direction_t
lh_memory_view_get_direction(const lh_memory_view_t *self);

/**
 * @brief Return endpoint-initialization flags for @p self.
 *
 * The begin flag is set when @c first is non-null. The end flag is set when
 * @c second is non-null.
 *
 * @param self View to inspect.
 * @return Bit pattern from ::lh_memory_view_slice_flags_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_slice_flags_t
lh_memory_view_get_flags(const lh_memory_view_t *self);

/**
 * @brief True if neither endpoint is initialized.
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_uninitialized(const lh_memory_view_t *self);

/**
 * @brief True if both endpoints are initialized.
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_initialized(const lh_memory_view_t *self);

/**
 * @brief True if @p self is initialized and ordered @c first &lt; @c second.
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_forward(const lh_memory_view_t *self);

/**
 * @brief True if @p self is initialized and not forward.
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_backward(const lh_memory_view_t *self);

/**
 * @brief True if @p self is initialized and forward ordered.
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self);

/* -- validated access, size, containment ---------------------------------- */

/**
 * @brief Like ::lh_memory_view_unpack but requires valid views.
 *
 * @param self  Valid view to read.
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
lh_memory_view_unpack_v(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end);

/**
 * @brief Return @c first after validating @p self.
 * @param self Valid view to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_begin_v(const lh_memory_view_t *self);

/**
 * @brief Return @c second after validating @p self.
 * @param self Valid view to read.
 * @return Stored exclusive end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_end_v(const lh_memory_view_t *self);

/**
 * @brief Return half-open view size in bytes.
 *
 * For valid views this is @c second @c - @c first.
 *
 * @param self Valid view to read.
 * @return Number of bytes covered by the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self);

/**
 * @brief True if @p self is uninitialized or has zero size.
 *
 * Because valid half-open views require @c first &lt; @c second, initialized
 * valid views have non-zero size. The uninitialized state is therefore the
 * practical empty value in this API.
 *
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is invalid and not uninitialized.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self);

/**
 * @brief True if @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_view_get_size(self))</tt>.
 *
 * @param self   Valid view to inspect.
 * @param offset Byte offset from @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return byte offset of @p ptr from @c first.
 *
 * @param self Valid view to inspect.
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
lh_memory_view_get_offset_from_begin(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from the last byte, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c second - 1. The exclusive @c second
 * endpoint itself is not inside the view.
 *
 * @param self Valid view to inspect.
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
lh_memory_view_get_offset_from_end(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief True if @p ptr lies inside the half-open interval @p self.
 *
 * @param self Valid view to inspect.
 * @param ptr  Pointer to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief True if half-open range <tt>[begin, end)</tt> lies inside @p self.
 *
 * @param self  Valid outer view.
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
lh_memory_view_contains_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_contains_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p other lies completely inside @p self.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid outer view.
 * @param other Valid inner view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        A view is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other);

/* -- pointer and value access --------------------------------------------- */

/**
 * @brief Return pointer at byte @p offset from @c first.
 *
 * @param self   Valid view to index.
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
const lh_ptr
lh_memory_view_get_ptr_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from the last byte, walking backward.
 *
 * Offset @c 0 returns @c second - 1, offset @c 1 returns the previous byte,
 * and so on.
 *
 * @param self   Valid view to index.
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
const lh_ptr
lh_memory_view_get_ptr_from_end(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer to the first byte.
 * @param self Valid view to index.
 * @return Pointer to @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_first_ptr(const lh_memory_view_t *self);

/**
 * @brief Return pointer to the last byte.
 * @param self Valid non-empty view to index.
 * @return Pointer to @c second - 1.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_last_ptr(const lh_memory_view_t *self);

/**
 * @brief Return pointer by signed offset.
 *
 * Non-negative offsets are measured from @c first. Negative offsets are
 * measured from the last byte: @c -1 addresses @c second - 1, @c -2 the
 * previous byte.
 *
 * @param self   Valid view to index.
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
const lh_ptr
lh_memory_view_get_ptr(const lh_memory_view_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c first.
 *
 * @param self   Valid view to index.
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
lh_memory_view_get_value_from_begin(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from the last byte, walking backward.
 *
 * @param self   Valid view to index.
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
lh_memory_view_get_value_from_end(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte by signed offset.
 *
 * @param self   Valid view to index.
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
lh_memory_view_get_value(const lh_memory_view_t *self, lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 *
 * @param self Valid view to read.
 * @return Byte at @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_first_value(const lh_memory_view_t *self);

/**
 * @brief Read the last byte of @p self.
 *
 * @param self Valid view to read.
 * @return Byte at @c second - 1.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_last_value(const lh_memory_view_t *self);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_seek_ptr(const lh_memory_view_t *self, const lh_ptr ptr, lh_soffset_t offset);

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
lh_memory_view_next_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

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
lh_memory_view_prev_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

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
lh_memory_view_seek_value(const lh_memory_view_t *self, const lh_ptr ptr);

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
lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr);

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
lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr);

/* -- overlap, alignment, equality ----------------------------------------- */

/**
 * @brief True if half-open range <tt>[begin, end)</tt> overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_overlaps_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p other overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief True if valid @p other overlaps @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps_v(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief True if half-open view size is divisible by @p alignment.
 *
 * @param self      Valid view to inspect.
 * @param alignment Non-zero divisor for the view size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t alignment);

/**
 * @brief True if @c first is aligned to @p align.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief True if both endpoints are aligned to @p align.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief True if @p self stores exactly @p begin and @p end.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_equals_of(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_equals_of.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True if @p self and @p other store the same endpoints.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other);

/* -- raw byte operations --------------------------------------------------- */

/**
 * @brief Find the first occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid view used as the haystack.
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
const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Find the first occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid view used as the haystack.
 * @param other Valid view used as the needle.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Find the last occurrence of <tt>[begin, end)</tt> inside @p self.
 *
 * @param self  Valid view used as the haystack.
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
const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Find the last occurrence of @p other inside @p self.
 *
 * Both operands are interpreted as half-open views.
 *
 * @param self  Valid view used as the haystack.
 * @param other Valid view used as the needle.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

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
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other);

/* -- mutation -------------------------------------------------------------- */

/**
 * @brief Reset @p self to the uninitialized empty view.
 *
 * @param self View to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_clear(lh_memory_view_t *self);

/**
 * @brief Copy endpoints from @p other to @p self without range validation.
 *
 * This function preserves the view state as-is. Use
 * ::lh_memory_view_assign_v when the source must be a valid half-open range.
 *
 * @param self  View to update.
 * @param other View to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 *
 * @param self  View to update.
 * @param other Valid view to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Exchange the contents of @p self and @p other.
 *
 * @param self  First view.
 * @param other Second view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Swap two valid views.
 *
 * @param self  Valid view to swap.
 * @param other Valid view to swap with.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_swap_v(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Clear @p self, then swap it with @p other.
 *
 * Discards the current contents of @p self, then moves the contents of
 * @p other into @p self, leaving @p other empty.
 *
 * @param self  View to clear and receive @p other's contents.
 * @param other View whose contents are moved into @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_swap_and_clear(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Store @p begin and @p end in @p self without range validation.
 *
 * The caller is responsible for ensuring the resulting view is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param self  View to update.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Store @p begin and @p end in @p self after validation.
 *
 * @param self  View to update.
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
lh_memory_view_set_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Build a view from @p begin and @p end without range validation.
 *
 * The caller is responsible for ensuring the resulting view is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 * @return Constructed view value (may be invalid or uninitialized).
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Build and validate a view from @p begin and @p end.
 *
 * @param begin New @c first endpoint.
 * @param end   New exclusive @c second endpoint.
 * @return Constructed valid view value.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid half-open range.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_v(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Build valid half-open view starting at @p begin with @p size bytes.
 *
 * The returned endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the half-open view.
 * @return Constructed valid view value.
 *
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_by_size(const lh_ptr begin, lh_usize_t size);

/**
 * @brief Return a view with both endpoints null.
 *
 * @return Uninitialized empty view value.
 *
 * @see lh_memory_view_empty_initializer
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t lh_memory_view_make_empty(lh_void);

/**
 * @brief Store a half-open range starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size)</tt>. @p size must be
 * non-zero.
 *
 * @param self  View to update.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the half-open view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_set_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

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
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Initialize @p self as a half-open range starting at @p begin.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with the empty view initializer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_init_empty(lh_memory_view_t *self);

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
lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other);

/* -- slice conversion ------------------------------------------------------ */

/**
 * @brief Build a closed slice copy from half-open @p self.
 *
 * Valid view <tt>[begin, end)</tt> becomes closed slice
 * <tt>[begin, end - 1]</tt>. Uninitialized empty view becomes an empty slice.
 *
 * @param self View to copy as a closed slice.
 * @return Converted slice value.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is neither uninitialized nor valid half-open view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_slice_t
lh_memory_view_make_slice(const lh_memory_view_t *self);

/* -- sub-range construction -------------------------------------------------- */

/**
 * @brief Build a view containing the first @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the returned
 * view is empty.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the beginning.
 * @return View <tt>[begin, begin + n)</tt>, or an empty view when @p n is zero.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n);

/**
 * @brief Build a view containing the last @p n bytes of a valid half-open range.
 *
 * The source range is <tt>[begin, end)</tt>. If @p n is zero, the returned
 * view is empty.
 *
 * @param begin Source range begin pointer.
 * @param end   Source range exclusive end pointer.
 * @param n     Number of bytes to keep from the end.
 * @return View <tt>[end - n, end)</tt>, or an empty view when @p n is zero.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n);

/**
 * @brief Build a view containing the first @p size bytes of @p self.
 *
 * If @p size is zero, the returned view is empty.
 *
 * @param self Source view.
 * @param size Number of bytes to keep from the beginning.
 * @return View taken from the beginning of @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source view size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_from_begin(const lh_memory_view_t *self, lh_usize_t size);

/**
 * @brief Build a view containing the last @p size bytes of @p self.
 *
 * If @p size is zero, the returned view is empty.
 *
 * @param self Source view.
 * @param size Number of bytes to keep from the end.
 * @return View taken from the end of @p self.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source view size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_from_end(const lh_memory_view_t *self, lh_usize_t size);

/**
 * @brief Build a sub-view between @p begin and @p end inside @p self.
 *
 * @param self  Source view that must contain the requested range.
 * @param begin Inner range begin pointer.
 * @param end   Inner range exclusive end pointer.
 * @return Constructed valid inner view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_out_of_range
 *        <tt>[begin, end)</tt> is not contained by @p self.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end)</tt> is not a valid view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_between(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Build a sub-view starting at @p offset with @p size bytes.
 *
 * If @p size is zero, the returned view is empty.
 *
 * @param self   Source view.
 * @param offset Offset from the source beginning.
 * @param size   Number of bytes in the returned view.
 * @return Constructed sub-view, or an empty view when @p size is zero.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        <tt>[offset, offset + size)</tt> is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_from_offset(const lh_memory_view_t *self, lh_uoffset_t offset,
                                lh_usize_t size);

/**
 * @brief Build a view with @p n bytes removed from the beginning.
 *
 * If @p n equals the source size, the returned view is empty.
 *
 * @param self Source view.
 * @param n    Number of bytes to remove from the beginning.
 * @return Source view without the first @p n bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source view size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_drop_first(const lh_memory_view_t *self, lh_usize_t n);

/**
 * @brief Build a view with @p n bytes removed from the end.
 *
 * If @p n equals the source size, the returned view is empty.
 *
 * @param self Source view.
 * @param n    Number of bytes to remove from the end.
 * @return Source view without the last @p n bytes.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source view size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_drop_last(const lh_memory_view_t *self, lh_usize_t n);

/**
 * @brief Build a view with bytes removed from both sides.
 *
 * If @p left + @p right equals the source size, the returned view is empty.
 *
 * @param self  Source view.
 * @param left  Number of bytes to remove from the beginning.
 * @param right Number of bytes to remove from the end.
 * @return Trimmed source view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p left + @p right is greater than the source view size.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_trim(const lh_memory_view_t *self, lh_usize_t left, lh_usize_t right);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_VIEW_H */
