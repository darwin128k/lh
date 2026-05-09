/**
 * @file slice.h
 * @brief Closed byte slice bounds (::lh_memory_bounds_slice_t) and helpers.
 *
 * A memory bounds slice stores two endpoints @c first and @c second describing
 * a closed address interval <tt>[first, second]</tt>. Both endpoints point to
 * bytes that belong to the slice. Consequently, size is
 * @c second @c - @c first @c + @c 1 for initialized forward slices.
 *
 * The slice can also represent incomplete storage: each endpoint may be
 * ::lh_null. The endpoint presence is reported as
 * ::lh_memory_bounds_slice_flags_t, and initialized slices are classified by
 * direction using ::lh_memory_bounds_slice_direction_t.
 *
 * Valid slices are initialized and forward ordered (@c first &lt;= @c second).
 * Functions with the @c _v suffix and all size / containment / indexed access
 * helpers require a valid slice and may raise runtime errors when that
 * precondition is not met.
 *
 * @see lh_memory_bounds_slice_fields
 * @see lh_memory_bounds_slice_flags_t
 * @see lh_memory_bounds_slice_direction_t
 */

#ifndef LH_MEMORY_BOUNDS_SLICE_H
#define LH_MEMORY_BOUNDS_SLICE_H

#include <lh/ptr.h>
#include <lh/bool.h>
#include <lh/size.h>
#include <lh/offset.h>
#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/bounds/slice/flags.h>
#include <lh/memory/bounds/slice/fields.h>
#include <lh/memory/bounds/slice/direction.h>

/**
 * @struct lh_memory_bounds_slice
 * @brief Non-owning mutable closed byte slice: two ::lh_void * endpoints.
 *
 * Endpoints describe <tt>[first, second]</tt> in address space when both are
 * non-null and ordered. Public typedef: ::lh_memory_bounds_slice_t.
 */
typedef struct lh_memory_bounds_slice
{
    lh_memory_bounds_slice_fields(lh_void);
} lh_memory_bounds_slice_t; /**< Typedef for struct ::lh_memory_bounds_slice. */

LH_COMPILER_EXTERN_C_BEGIN

/* -- unpack / getters ------------------------------------------------------ */

/**
 * @brief Read @c first / @c second from @p self into optional outputs.
 *
 * Pass ::lh_null for @p begin or @p end to skip that output.
 *
 * @param self  Slice to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_unpack(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Return @c first without validating the slice range.
 * @param self Slice to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_slice_get_begin(const lh_memory_bounds_slice_t *self);

/**
 * @brief Return @c second without validating the slice range.
 * @param self Slice to read.
 * @return Stored end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_slice_get_end(const lh_memory_bounds_slice_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief Return endpoint-initialization flags for @p self.
 *
 * The begin flag is set when @c first is non-null. The end flag is set when
 * @c second is non-null.
 *
 * @param self Slice to inspect.
 * @return Bit pattern from ::lh_memory_bounds_slice_flags_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_slice_flags_t
lh_memory_bounds_slice_get_flags(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff neither endpoint is initialized.
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_uninitialized(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff both endpoints are initialized.
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_initialized(const lh_memory_bounds_slice_t *self);

/**
 * @brief Classify initialized endpoint order.
 *
 * Returns ::lh_memory_bounds_slice_direction_unknown until both endpoints are
 * initialized. Otherwise returns forward for @c first &lt;= @c second and
 * backward for reversed bounds.
 *
 * @param self Slice to inspect.
 * @return Direction value from ::lh_memory_bounds_slice_direction_t.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_bounds_slice_direction_t
lh_memory_bounds_slice_get_direction(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff @p self is initialized and ordered @c first &lt;= @c second.
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_forward(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff @p self is initialized and reversed.
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_backward(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff @p self is initialized and forward ordered.
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_valid(const lh_memory_bounds_slice_t *self);

/* -- validated access, size, containment ---------------------------------- */

/**
 * @brief Like ::lh_memory_bounds_slice_unpack but requires a valid slice.
 *
 * @param self  Valid slice to read.
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
lh_memory_bounds_slice_unpack_v(const lh_memory_bounds_slice_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Return @c first after validating @p self.
 * @param self Valid slice to read.
 * @return Stored begin pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_slice_get_begin_v(const lh_memory_bounds_slice_t *self);

/**
 * @brief Return @c second after validating @p self.
 * @param self Valid slice to read.
 * @return Stored end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_ptr
lh_memory_bounds_slice_get_end_v(const lh_memory_bounds_slice_t *self);

/**
 * @brief Return closed slice size in bytes.
 *
 * For a valid slice this is @c second @c - @c first @c + @c 1.
 *
 * @param self Valid slice to read.
 * @return Number of bytes covered by the closed interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_bounds_slice_get_size(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff closed slice size is divisible by @p alignment.
 *
 * @param self      Valid slice to inspect.
 * @param alignment Non-zero divisor for the slice size.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_division_by_zero
 *        @p alignment is zero.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_multiple_of(const lh_memory_bounds_slice_t *self, lh_usize_t alignment);

/**
 * @brief True iff @c first is aligned to @p align.
 *
 * @param self  Valid slice to inspect.
 * @param align Power-of-two alignment.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_not_power_of_two
 *        @p align is not a power of two.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_aligned_is_begin_aligned(const lh_memory_bounds_slice_t *self,
                                                lh_usize_t align);

/**
 * @brief True iff both endpoints are aligned to @p align.
 *
 * @param self  Valid slice to inspect.
 * @param align Power-of-two alignment.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_not_power_of_two
 *        @p align is not a power of two.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_aligned(const lh_memory_bounds_slice_t *self, lh_usize_t align);

/**
 * @brief True iff @p self is uninitialized or has zero size.
 *
 * For the closed interval representation, an initialized valid slice has at
 * least one byte. Therefore the uninitialized state is the practical empty
 * value.
 *
 * @param self Slice to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is invalid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_empty(const lh_memory_bounds_slice_t *self);

/**
 * @brief True iff @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_bounds_slice_get_size(self))</tt>.
 *
 * @param self   Valid slice to inspect.
 * @param offset Byte offset from @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_is_valid_offset(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Return byte offset of @p ptr from @c first.
 *
 * This is the inverse of ::lh_memory_bounds_slice_get_ptr_from_begin for
 * pointers inside @p self.
 *
 * @param self Valid slice to inspect.
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
lh_memory_bounds_slice_get_offset_from_begin(const lh_memory_bounds_slice_t *self,
                                             const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from @c second, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c second. This is the inverse of
 * ::lh_memory_bounds_slice_get_ptr_from_end for pointers inside @p self.
 *
 * @param self Valid slice to inspect.
 * @param ptr  Pointer whose reverse offset is requested.
 * @return Byte offset from @c second back to @p ptr.
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
lh_memory_bounds_slice_get_offset_from_end(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief True iff @p ptr lies inside the closed interval @p self.
 * @param self Valid slice to inspect.
 * @param ptr  Pointer to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_contains_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief True iff closed range <tt>[begin, end]</tt> lies inside @p self.
 * @param self  Valid outer slice.
 * @param begin Inner range begin pointer.
 * @param end   Inner range end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_contains_range(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                      const lh_ptr end);

/**
 * @brief True iff @p other lies completely inside @p self.
 * @param self  Valid outer slice.
 * @param other Valid inner slice.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        A slice is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_contains(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other);

/**
 * @brief True iff closed range <tt>[begin, end]</tt> overlaps @p self.
 * @param self  Valid slice.
 * @param begin Range begin pointer.
 * @param end   Range end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_overlaps_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end);

/**
 * @brief True iff @p other overlaps @p self.
 * @param self  Valid slice.
 * @param other Valid slice to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        A slice is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_overlaps(const lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other);

/**
 * @brief True iff valid @p other overlaps @p self.
 * @param self  Valid slice.
 * @param other Valid slice to test.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        A slice is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_overlaps_v(const lh_memory_bounds_slice_t *self,
                                  const lh_memory_bounds_slice_t *other);

/**
 * @brief True iff @p self stores exactly @p begin and @p end.
 *
 * This compares the stored endpoints without requiring a valid range.
 *
 * @param self  Slice to inspect.
 * @param begin Expected begin pointer.
 * @param end   Expected end pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_equals_of(const lh_memory_bounds_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/**
 * @brief True iff @p self and @p other store the same endpoints.
 *
 * This compares the stored endpoints without requiring valid ranges.
 *
 * @param self  Slice to inspect.
 * @param other Slice to compare against.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_bounds_slice_equals(const lh_memory_bounds_slice_t *self,
                              const lh_memory_bounds_slice_t *other);

/* -- pointer and value access --------------------------------------------- */

/**
 * @brief Return pointer at byte @p offset from @c first.
 * @param self   Valid slice to index.
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
lh_memory_bounds_slice_get_ptr_from_begin(const lh_memory_bounds_slice_t *self,
                                          lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from @c second, walking backward.
 *
 * Offset @c 0 returns @c second, offset @c 1 returns the previous byte, and so on.
 *
 * @param self   Valid slice to index.
 * @param offset Byte offset from the end endpoint.
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
lh_memory_bounds_slice_get_ptr_from_end(const lh_memory_bounds_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer by signed offset.
 *
 * Non-negative offsets are measured from @c first. Negative offsets are
 * measured from @c second: @c -1 addresses @c second, @c -2 the previous byte.
 *
 * @param self   Valid slice to index.
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
lh_memory_bounds_slice_get_ptr_by_offset(const lh_memory_bounds_slice_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c first.
 * @param self   Valid slice to index.
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
lh_memory_bounds_slice_get_value_from_begin(const lh_memory_bounds_slice_t *self,
                                            lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from @c second, walking backward.
 * @param self   Valid slice to index.
 * @param offset Byte offset from the end endpoint.
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
lh_memory_bounds_slice_get_value_from_end(const lh_memory_bounds_slice_t *self,
                                          lh_uoffset_t offset);

/**
 * @brief Read byte by signed offset.
 * @param self   Valid slice to index.
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
lh_memory_bounds_slice_get_value_by_offset(const lh_memory_bounds_slice_t *self,
                                           lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 * @param self Valid slice to read.
 * @return Byte at @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_slice_get_begin_value(const lh_memory_bounds_slice_t *self);

/**
 * @brief Read the last byte of @p self.
 * @param self Valid slice to read.
 * @return Byte at @c second.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_slice_get_end_value(const lh_memory_bounds_slice_t *self);

/**
 * @brief Return target byte offset after applying signed @p offset.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed
 * offset accepted by ::lh_memory_bounds_slice_get_ptr_by_offset.
 * Otherwise @p offset is applied relative to @p ptr.
 *
 * @param self   Valid slice to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target byte offset from @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @throw ::lh_runtime_error_code_underflow
 *        Relative seek moves before @c first.
 * @throw ::lh_runtime_error_code_overflow
 *        Relative seek moves after @c second or overflows offset arithmetic.
 */
LH_ATTRIBUTE_SYMBOL
lh_uoffset_t
lh_memory_bounds_slice_get_offset_from_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                           lh_soffset_t offset);

/**
 * @brief Return pointer reached by seeking @p offset bytes from @p ptr.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed
 * offset accepted by ::lh_memory_bounds_slice_get_ptr_by_offset.
 * Boundary underflow and overflow are converted to ::lh_null.
 *
 * @param self   Valid slice to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target pointer, or ::lh_null when the seek crosses slice bounds.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_slice_seek_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr,
                                lh_soffset_t offset);

/**
 * @brief Return the byte pointer after @p ptr.
 *
 * Equivalent to
 * ::lh_memory_bounds_slice_seek_ptr(@p self, @p ptr, 1).
 *
 * @param self Valid slice to seek in.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Next pointer, or ::lh_null when the seek crosses @c second.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_slice_next_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief Return the byte pointer before @p ptr.
 *
 * Equivalent to
 * ::lh_memory_bounds_slice_seek_ptr(@p self, @p ptr, -1).
 *
 * @param self Valid slice to seek in.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Previous pointer, or ::lh_null when the seek crosses @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_bounds_slice_prev_ptr(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief Seek to @p ptr and read the byte there.
 *
 * Equivalent to dereferencing
 * ::lh_memory_bounds_slice_seek_ptr(@p self, @p ptr, 0).
 *
 * @param self Valid slice to read.
 * @param ptr  Pointer inside @p self, or ::lh_null to read @c first.
 * @return Byte at the resolved pointer.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @throw ::lh_runtime_error_code_null_dereference
 *        The seek resolves to ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_slice_seek_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte after @p ptr.
 *
 * Equivalent to dereferencing
 * ::lh_memory_bounds_slice_next_ptr.
 *
 * @param self Valid slice to read.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Byte after @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @throw ::lh_runtime_error_code_null_dereference
 *        No next byte exists inside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_slice_next_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte before @p ptr.
 *
 * Equivalent to dereferencing
 * ::lh_memory_bounds_slice_prev_ptr.
 *
 * @param self Valid slice to read.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Byte before @p ptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @throw ::lh_runtime_error_code_null_dereference
 *        No previous byte exists inside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_bounds_slice_prev_value(const lh_memory_bounds_slice_t *self, const lh_ptr ptr);

/* -- mutation ------------------------------------------------------------- */

/**
 * @brief Store @p begin and @p end in @p self without validating the range.
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_set(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Copy stored endpoints from @p other to @p self.
 * @param self  Slice to update.
 * @param other Slice to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_assign(lh_memory_bounds_slice_t *self,
                              const lh_memory_bounds_slice_t *other);

/**
 * @brief Reset @p self to the uninitialized empty slice.
 * @param self Slice to clear.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_clear(lh_memory_bounds_slice_t *self);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 * @param self  Slice to update.
 * @param other Valid slice to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_assign_v(lh_memory_bounds_slice_t *self,
                                const lh_memory_bounds_slice_t *other);

/**
 * @brief Store @p begin and @p end in @p self after validating the new slice.
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_set_v(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Store a closed slice starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size - 1]</tt>.
 *
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the closed slice; must be non-zero.
 *
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed slice is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_set_by_size(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with valid @p begin and @p end endpoints.
 *
 * Same as ::lh_memory_bounds_slice_set_v.
 *
 * @param self  Slice to initialize.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_init(lh_memory_bounds_slice_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Initialize @p self as a closed slice starting at @p begin with @p size bytes.
 *
 * Same as ::lh_memory_bounds_slice_set_by_size.
 *
 * @param self  Slice to initialize.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the closed slice; must be non-zero.
 *
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed slice is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_init_by_size(lh_memory_bounds_slice_t *self, lh_ptr begin,
                                    lh_usize_t size);

/**
 * @brief Initialize @p self with the empty slice initializer.
 * @param self Slice to initialize.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_init_empty(lh_memory_bounds_slice_t *self);

/**
 * @brief Initialize @p self by copying endpoints from valid @p other.
 *
 * Same as ::lh_memory_bounds_slice_assign_v.
 *
 * @param self  Slice to initialize.
 * @param other Valid slice to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_init_by_other(lh_memory_bounds_slice_t *self,
                                     const lh_memory_bounds_slice_t *other);

/**
 * @brief Swap stored endpoints between @p self and @p other.
 * @param self  First slice.
 * @param other Second slice.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_swap(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other);

/**
 * @brief Swap @p self with valid @p other.
 * @param self  Slice to swap.
 * @param other Valid slice to swap with.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_swap_v_other(lh_memory_bounds_slice_t *self,
                                    lh_memory_bounds_slice_t *other);

/**
 * @brief Clear @p self, then move valid @p other into it by swapping.
 *
 * When @p self and @p other are different objects, @p other receives the empty
 * slice left by ::lh_memory_bounds_slice_clear.
 *
 * @param self  Slice to replace.
 * @param other Valid slice to move from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_swap_v(lh_memory_bounds_slice_t *self, lh_memory_bounds_slice_t *other);

/**
 * @brief Clear @p self, then swap it with @p other.
 *
 * When @p self and @p other are different objects, @p self receives the previous
 * endpoints of valid @p other, and @p other receives the empty slice.
 *
 * When @p self and @p other are the same object, the slice is only cleared.
 *
 * @param self  Slice to clear and replace.
 * @param other Valid slice to swap with.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_bounds_slice_swap_and_clear(lh_memory_bounds_slice_t *self,
                                      lh_memory_bounds_slice_t *other);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_BOUNDS_SLICE_H
