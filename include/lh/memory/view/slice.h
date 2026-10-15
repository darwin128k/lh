/**
 * @file slice.h
 * @brief Read-only closed byte slice view (::lh_memory_view_slice_t) and helpers.
 *
 * A memory view slice stores two @c const endpoints @c first and @c second
 * describing a closed address interval <tt>[first, second]</tt>. Both
 * endpoints point to bytes that belong to the slice. Consequently, size is
 * @c second @c - @c first @c + @c 1 for initialized forward slices.
 *
 * The slice can also represent incomplete storage: each endpoint may be
 * ::lh_null. The endpoint presence is reported as
 * ::lh_memory_view_slice_flags_t, and initialized slices are classified by
 * direction using ::lh_memory_view_slice_direction_t.
 *
 * Valid slices are initialized and forward ordered (@c first &lt;= @c second).
 * Functions with the @c _v suffix and all size / containment / indexed access
 * helpers require a valid slice and may raise runtime errors when that
 * precondition is not met.
 *
 * @see lh_memory_view_slice_fields
 * @see lh_memory_view_slice_flags_t
 * @see lh_memory_view_slice_direction_t
 */

#ifndef LH_MEMORY_VIEW_SLICE_H
#define LH_MEMORY_VIEW_SLICE_H

#include <lh/void.h>
#include <lh/ptr.h>
#include <lh/bool.h>
#include <lh/byte.h>
#include <lh/size.h>
#include <lh/offset.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/view/slice/direction.h>
#include <lh/memory/view/slice/flags.h>
#include <lh/memory/view/slice/initializer.h>
#include <lh/memory/view/slice/fields.h>
#include <lh/util/ptr.h>
#include <lh/runtime/error/code.h>

/**
 * @struct lh_memory_view_slice
 * @brief Non-owning read-only closed byte slice: two <tt>const ::lh_ptr</tt> endpoints.
 *
 * Endpoints describe <tt>[first, second]</tt> in address space when both are
 * non-null and ordered. Public typedef: ::lh_memory_view_slice_t.
 */
struct lh_memory_view_slice
{
    /** Members: ::lh_memory_view_slice_fields. */
    lh_memory_view_slice_fields(lh_void);
};
/** @copybrief lh_memory_view_slice */
typedef struct lh_memory_view_slice lh_memory_view_slice_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- getters --------------------------------------------------------------- */

/**
 * @brief Return @c first without validating the slice range.
 * @param self Slice to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_slice_get_begin(const lh_memory_view_slice_t *self);

/**
 * @brief Alias for ::lh_memory_view_slice_get_begin.
 * @param self Slice to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_slice_get_data(const lh_memory_view_slice_t *self);

/**
 * @brief Return @c second without validating the slice range.
 * @param self Slice to read.
 * @return Stored end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
const lh_ptr
lh_memory_view_slice_get_end(const lh_memory_view_slice_t *self);

/* -- classification -------------------------------------------------------- */

/**
 * @brief Return endpoint-initialization flags for @p self.
 *
 * The begin flag is set when @c first is non-null. The end flag is set when
 * @c second is non-null.
 *
 * @param self Slice to inspect.
 * @return Bit pattern from ::lh_memory_view_slice_flags_t.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_memory_view_slice_flags_t
lh_memory_view_slice_get_flags(const lh_memory_view_slice_t *self);

/**
 * @brief True if neither endpoint is initialized.
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_uninitialized(const lh_memory_view_slice_t *self);

/**
 * @brief True if both endpoints are initialized.
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_initialized(const lh_memory_view_slice_t *self);

/**
 * @brief Classify initialized endpoint order.
 *
 * Returns ::lh_memory_view_slice_direction_unknown until both endpoints are
 * initialized. Otherwise returns forward for @c first &lt;= @c second and
 * backward for reversed bounds.
 *
 * @param self Slice to inspect.
 * @return Direction value from ::lh_memory_view_slice_direction_t.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_memory_view_slice_direction_t
lh_memory_view_slice_get_direction(const lh_memory_view_slice_t *self);

/**
 * @brief True if @p self is initialized and ordered @c first &lt;= @c second.
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_forward(const lh_memory_view_slice_t *self);

/**
 * @brief True if @p self is initialized and reversed.
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_backward(const lh_memory_view_slice_t *self);

/**
 * @brief True if @p self is initialized and forward ordered.
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_valid(const lh_memory_view_slice_t *self);

/* -- validated access, size, containment ---------------------------------- */

/**
 * @brief Return @c first after validating @p self.
 * @param self Valid slice to read.
 * @return Stored begin pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_get_begin_v(const lh_memory_view_slice_t *self);

/**
 * @brief Return @c second after validating @p self.
 * @param self Valid slice to read.
 * @return Stored end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_get_end_v(const lh_memory_view_slice_t *self);

/**
 * @brief Return closed slice size in bytes.
 *
 * For a valid slice this is @c second @c - @c first @c + @c 1.
 *
 * @param self Valid slice to read.
 * @return Number of bytes covered by the closed interval.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_usize_t
lh_memory_view_slice_get_size(const lh_memory_view_slice_t *self);

/**
 * @brief True if @p self is uninitialized or has zero size.
 *
 * For the closed interval representation, an initialized valid slice has at
 * least one byte. Therefore the uninitialized state is the practical empty
 * value.
 *
 * @param self Slice to inspect.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is invalid.
 */
lh_bool_t
lh_memory_view_slice_is_empty(const lh_memory_view_slice_t *self);

/**
 * @brief True if @p offset addresses a byte inside @p self from the begin side.
 *
 * Valid offsets are in the half-open numeric interval
 * <tt>[0, lh_memory_view_slice_get_size(self))</tt>.
 *
 * @param self   Valid slice to inspect.
 * @param offset Byte offset from @c first.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_is_valid_offset(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Return byte offset of @p ptr from @c first.
 *
 * This is the inverse of ::lh_memory_view_slice_get_ptr_from_begin for
 * pointers inside @p self.
 *
 * @param self Valid slice to inspect.
 * @param ptr  Pointer whose offset is requested.
 * @return Byte offset from @c first to @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_uoffset_t
lh_memory_view_slice_get_offset_from_begin(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief Return byte offset of @p ptr from @c second, walking backward.
 *
 * Offset @c 0 means @p ptr equals @c second. This is the inverse of
 * ::lh_memory_view_slice_get_ptr_from_end for pointers inside @p self.
 *
 * @param self Valid slice to inspect.
 * @param ptr  Pointer whose reverse offset is requested.
 * @return Byte offset from @c second back to @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_uoffset_t
lh_memory_view_slice_get_offset_from_end(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief True if @p ptr lies inside the closed interval @p self.
 * @param self Valid slice to inspect.
 * @param ptr  Pointer to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_contains_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief True if closed range <tt>[begin, end]</tt> lies inside @p self.
 * @param self  Valid outer slice.
 * @param begin Inner range begin pointer.
 * @param end   Inner range end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_contains_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_slice_contains_range.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_contains_of(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/**
 * @brief True if @p other lies completely inside @p self.
 *
 * @p other is not validated.
 *
 * @param self  Valid outer slice.
 * @param other Inner slice to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_contains(const lh_memory_view_slice_t *self,
                              const lh_memory_view_slice_t *other);

/**
 * @brief True if valid @p other lies completely inside @p self.
 * @param self  Valid outer slice.
 * @param other Valid inner slice.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        A slice is not valid.
 */
lh_bool_t
lh_memory_view_slice_contains_v(const lh_memory_view_slice_t *self,
                                const lh_memory_view_slice_t *other);

/**
 * @brief True if closed range <tt>[begin, end]</tt> overlaps @p self.
 * @param self  Valid slice.
 * @param begin Range begin pointer.
 * @param end   Range end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_overlaps_of(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_slice_overlaps_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_overlaps_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief True if @p other overlaps @p self.
 * @param self  Valid slice.
 * @param other Slice to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_bool_t
lh_memory_view_slice_overlaps(const lh_memory_view_slice_t *self,
                              const lh_memory_view_slice_t *other);

/**
 * @brief True if valid @p other overlaps @p self.
 * @param self  Valid slice.
 * @param other Valid slice to test.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        A slice is not valid.
 */
lh_bool_t
lh_memory_view_slice_overlaps_v(const lh_memory_view_slice_t *self,
                                const lh_memory_view_slice_t *other);

/**
 * @brief True if closed slice size is divisible by @p alignment.
 *
 * @param self      Valid slice to inspect.
 * @param alignment Non-zero divisor for the slice size.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_division_by_zero
 *        @p alignment is zero.
 */
lh_bool_t
lh_memory_view_slice_is_multiple_of(const lh_memory_view_slice_t *self, lh_usize_t alignment);

/**
 * @brief True if @c first is aligned to @p align.
 *
 * @param self  Valid slice to inspect.
 * @param align Power-of-two alignment.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_not_power_of_two
 *        @p align is not a power of two.
 */
lh_bool_t
lh_memory_view_slice_is_begin_aligned(const lh_memory_view_slice_t *self, lh_usize_t align);

/**
 * @brief True if both endpoints are aligned to @p align.
 *
 * @param self  Valid slice to inspect.
 * @param align Power-of-two alignment.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_not_power_of_two
 *        @p align is not a power of two.
 */
lh_bool_t
lh_memory_view_slice_is_aligned(const lh_memory_view_slice_t *self, lh_usize_t align);

/**
 * @brief True if @p self stores exactly @p begin and @p end.
 *
 * This compares the stored endpoints without requiring a valid range.
 *
 * @param self  Slice to inspect.
 * @param begin Expected begin pointer.
 * @param end   Expected end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_equals_of(const lh_memory_view_slice_t *self, const lh_ptr begin,
                               const lh_ptr end);

/**
 * @brief Alias for ::lh_memory_view_slice_equals_of.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_equals_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                  const lh_ptr end);

/**
 * @brief True if @p self and @p other store the same endpoints.
 *
 * This compares the stored endpoints without requiring valid ranges.
 *
 * @param self  Slice to inspect.
 * @param other Slice to compare against.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_equals(const lh_memory_view_slice_t *self,
                            const lh_memory_view_slice_t *other);

/* -- raw byte operations --------------------------------------------------- */

/**
 * @brief Find the first occurrence of closed range <tt>[begin, end]</tt>
 *        inside @p self.
 *
 * @param self  Valid slice used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range end pointer.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_find_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                const lh_ptr end);

/**
 * @brief Find the first occurrence of @p other inside @p self.
 *
 * @param self  Valid slice used as the haystack.
 * @param other Valid slice used as the needle.
 * @return Pointer to the first match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_slice_find(const lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other);

/**
 * @brief Find the last occurrence of closed range <tt>[begin, end]</tt>
 *        inside @p self.
 *
 * @param self  Valid slice used as the haystack.
 * @param begin Needle range begin pointer.
 * @param end   Needle range end pointer.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_rfind_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/**
 * @brief Find the last occurrence of @p other inside @p self.
 *
 * @param self  Valid slice used as the haystack.
 * @param other Valid slice used as the needle.
 * @return Pointer to the last match, or ::lh_null when no match exists.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_slice_rfind(const lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other);

/**
 * @brief Compare @p self against closed range <tt>[begin, end]</tt> byte by
 *        byte, from the front.
 *
 * @param self  Valid left-hand slice.
 * @param begin Right-hand range begin pointer.
 * @param end   Right-hand range end pointer.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when
 *         the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_compare_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end);

/**
 * @brief Compare @p self against @p other byte by byte, from the front.
 *
 * @param self  Valid left-hand slice.
 * @param other Valid right-hand slice.
 * @return Pointer to the first differing byte in @p self, or ::lh_null when
 *         the compared bytes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_slice_compare(const lh_memory_view_slice_t *self,
                             const lh_memory_view_slice_t *other);

/**
 * @brief Compare @p self against closed range <tt>[begin, end]</tt> byte by
 *        byte, from the back.
 *
 * @param self  Valid left-hand slice.
 * @param begin Right-hand range begin pointer.
 * @param end   Right-hand range end pointer.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_rcompare_range(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief Compare @p self against @p other byte by byte, from the back.
 *
 * @param self  Valid left-hand slice.
 * @param other Valid right-hand slice.
 * @return Pointer to the differing byte in @p self, or ::lh_null when the
 *         compared suffixes are equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
const lh_ptr
lh_memory_view_slice_rcompare(const lh_memory_view_slice_t *self,
                              const lh_memory_view_slice_t *other);

/* -- pointer and value access --------------------------------------------- */

/**
 * @brief Return pointer at byte @p offset from @c first.
 * @param self   Valid slice to index.
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
lh_memory_view_slice_get_ptr_from_begin(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer at byte @p offset from @c second, walking backward.
 *
 * Offset @c 0 returns @c second, offset @c 1 returns the previous byte, and so on.
 *
 * @param self   Valid slice to index.
 * @param offset Byte offset from the end endpoint.
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
lh_memory_view_slice_get_ptr_from_end(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Return pointer to the first byte.
 * @param self Valid slice to index.
 * @return Pointer to @c first.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
const lh_ptr
lh_memory_view_slice_get_first_ptr(const lh_memory_view_slice_t *self);

/**
 * @brief Return pointer to the last byte (@c second).
 * @param self Valid non-empty slice to index.
 * @return Pointer to @c second.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p self is empty.
 */
const lh_ptr
lh_memory_view_slice_get_last_ptr(const lh_memory_view_slice_t *self);

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
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
const lh_ptr
lh_memory_view_slice_get_ptr(const lh_memory_view_slice_t *self, lh_soffset_t offset);

/**
 * @brief Read byte at @p offset from @c first.
 * @param self   Valid slice to index.
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
lh_memory_view_slice_get_value_from_begin(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte at @p offset from @c second, walking backward.
 * @param self   Valid slice to index.
 * @param offset Byte offset from the end endpoint.
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
lh_memory_view_slice_get_value_from_end(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Read byte by signed offset.
 * @param self   Valid slice to index.
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
lh_memory_view_slice_get_value(const lh_memory_view_slice_t *self, lh_soffset_t offset);

/**
 * @brief Read the first byte of @p self.
 * @param self Valid slice to read.
 * @return Byte at @c first.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_view_slice_get_first_value(const lh_memory_view_slice_t *self);

/**
 * @brief Read the last byte of @p self.
 * @param self Valid slice to read.
 * @return Byte at @c second.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
lh_byte_t
lh_memory_view_slice_get_last_value(const lh_memory_view_slice_t *self);

/**
 * @brief Return target byte offset after applying signed @p offset.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed
 * offset accepted by ::lh_memory_view_slice_get_ptr.
 * Otherwise @p offset is applied relative to @p ptr.
 *
 * @param self   Valid slice to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target byte offset from @c first.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @fails ::lh_runtime_error_code_underflow
 *        Relative seek moves before @c first.
 * @fails ::lh_runtime_error_code_overflow
 *        Relative seek moves after @c second or overflows offset arithmetic.
 */
lh_uoffset_t
lh_memory_view_slice_get_offset_from_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr,
                                         lh_soffset_t offset);

/**
 * @brief Return pointer reached by seeking @p offset bytes from @p ptr.
 *
 * When @p ptr is ::lh_null, @p offset is treated as an absolute signed
 * offset accepted by ::lh_memory_view_slice_get_ptr.
 * Boundary underflow and overflow are converted to ::lh_null.
 *
 * @param self   Valid slice to seek in.
 * @param ptr    Base pointer inside @p self, or ::lh_null for absolute seek.
 * @param offset Signed byte offset.
 * @return Target pointer, or ::lh_null when the seek crosses slice bounds.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
const lh_ptr
lh_memory_view_slice_seek_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr,
                              lh_soffset_t offset);

/**
 * @brief Return the byte pointer after @p ptr.
 *
 * Equivalent to
 * ::lh_memory_view_slice_seek_ptr(@p self, @p ptr, 1).
 *
 * @param self Valid slice to seek in.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Next pointer, or ::lh_null when the seek crosses @c second.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
const lh_ptr
lh_memory_view_slice_next_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief Return the byte pointer before @p ptr.
 *
 * Equivalent to
 * ::lh_memory_view_slice_seek_ptr(@p self, @p ptr, -1).
 *
 * @param self Valid slice to seek in.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Previous pointer, or ::lh_null when the seek crosses @c first.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
const lh_ptr
lh_memory_view_slice_prev_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief Seek to @p ptr and read the byte there.
 *
 * Equivalent to dereferencing
 * ::lh_memory_view_slice_seek_ptr(@p self, @p ptr, 0).
 *
 * @param self Valid slice to read.
 * @param ptr  Pointer inside @p self, or ::lh_null to read @c first.
 * @return Byte at the resolved pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @fails ::lh_runtime_error_code_null_dereference
 *        The seek resolves to ::lh_null.
 */
lh_byte_t
lh_memory_view_slice_seek_value(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte after @p ptr.
 *
 * Equivalent to dereferencing
 * ::lh_memory_view_slice_next_ptr.
 *
 * @param self Valid slice to read.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Byte after @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @fails ::lh_runtime_error_code_null_dereference
 *        No next byte exists inside @p self.
 */
lh_byte_t
lh_memory_view_slice_next_value(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/**
 * @brief Read the byte before @p ptr.
 *
 * Equivalent to dereferencing
 * ::lh_memory_view_slice_prev_ptr.
 *
 * @param self Valid slice to read.
 * @param ptr  Base pointer inside @p self, or ::lh_null for absolute seek.
 * @return Byte before @p ptr.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 * @fails ::lh_runtime_error_code_null_dereference
 *        No previous byte exists inside @p self.
 */
lh_byte_t
lh_memory_view_slice_prev_value(const lh_memory_view_slice_t *self, const lh_ptr ptr);

/* -- mutation ------------------------------------------------------------- */

/**
 * @brief Reset @p self to the uninitialized empty slice.
 * @param self Slice to clear.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_view_slice_clear(lh_memory_view_slice_t *self);

/**
 * @brief Copy endpoints from @p other to @p self without range validation.
 *
 * The caller is responsible for ensuring @p other is a meaningful slice.
 * No range check is performed on the copied endpoints.
 *
 * @param self  Slice to update.
 * @param other Slice to copy from (may be invalid or uninitialized).
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_slice_assign(lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other);

/**
 * @brief Copy endpoints from valid @p other to @p self.
 * @param self  Slice to update.
 * @param other Valid slice to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_slice_assign_v(lh_memory_view_slice_t *self, const lh_memory_view_slice_t *other);

/**
 * @brief Store @p begin and @p end in @p self without range validation.
 *
 * The caller is responsible for ensuring the resulting slice is meaningful.
 * No range check is performed on @p begin or @p end.
 *
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_slice_set(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Store @p begin and @p end in @p self after validating the new slice.
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
lh_void
lh_memory_view_slice_set_v(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Swap @p self and @p other without range validation.
 *
 * The caller is responsible for ensuring both slices are meaningful.
 * No range check is performed on either operand.
 *
 * @param self  Slice to swap.
 * @param other Slice to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 *
 * @warning Unsafe — no range validation. Caller assumes full responsibility.
 */
lh_void
lh_memory_view_slice_swap(lh_memory_view_slice_t *self, lh_memory_view_slice_t *other);

/**
 * @brief Swap two valid slices.
 *
 * @param self  Valid slice to swap.
 * @param other Valid slice to swap with.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
lh_void
lh_memory_view_slice_swap_v(lh_memory_view_slice_t *self, lh_memory_view_slice_t *other);

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
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_slice_swap_and_clear(lh_memory_view_slice_t *self, lh_memory_view_slice_t *other);

/**
 * @brief Build a slice containing the first @p n bytes of a valid closed range.
 *
 * The source range is <tt>[begin, end]</tt>. If @p n is zero, the returned
 * slice is empty.
 *
 * @param begin Source range @c first endpoint.
 * @param end   Source range @c second endpoint.
 * @param n     Number of bytes to keep from the beginning.
 * @return Slice <tt>[begin, begin + n - 1]</tt>, or an empty slice when
 *         @p n is zero.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_view_slice_take_first(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Build a slice containing the last @p n bytes of a valid closed range.
 *
 * The source range is <tt>[begin, end]</tt>. If @p n is zero,
 * the returned slice is empty.
 *
 * @param begin Source range @c first endpoint.
 * @param end   Source range @c second endpoint.
 * @param n     Number of bytes to keep from the end.
 * @return Slice <tt>[end - n + 1, end]</tt>, or an empty slice when @p n is zero.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source range size.
 */
lh_void
lh_memory_view_slice_take_last(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Build a slice containing the first @p size bytes of @p self.
 *
 * If @p size is zero, the returned slice is empty.
 *
 * @param self Source slice.
 * @param size Number of bytes to keep from the beginning.
 * @return Slice taken from the beginning of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source slice size.
 */
lh_void
lh_memory_view_slice_from_begin(const lh_memory_view_slice_t *self, lh_usize_t size, lh_memory_view_slice_t *out);

/**
 * @brief Build a slice containing the last @p size bytes of @p self.
 *
 * If @p size is zero, the returned slice is empty.
 *
 * @param self Source slice.
 * @param size Number of bytes to keep from the end.
 * @return Slice taken from the end of @p self.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p size is greater than the source slice size.
 */
lh_void
lh_memory_view_slice_from_end(const lh_memory_view_slice_t *self, lh_usize_t size, lh_memory_view_slice_t *out);

/**
 * @brief Build a sub-slice between @p begin and @p end inside @p self.
 *
 * @param self  Source slice that must contain the requested range.
 * @param begin Inner range @c first endpoint.
 * @param end   Inner range @c second endpoint.
 * @return Constructed valid inner slice.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[begin, end]</tt> is not contained by @p self.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
lh_void
lh_memory_view_slice_between(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                  const lh_ptr end,
                                  lh_memory_view_slice_t *out);

/**
 * @brief Build a sub-slice starting at @p offset with @p size bytes.
 *
 * If @p size is zero, the returned slice is empty.
 *
 * @param self   Source slice.
 * @param offset Offset from the source beginning.
 * @param size   Number of bytes in the returned slice.
 * @return Constructed sub-slice, or an empty slice when @p size is zero.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[offset, offset + size)</tt> is outside @p self.
 */
lh_void
lh_memory_view_slice_from_offset(const lh_memory_view_slice_t *self, lh_uoffset_t offset,
                                      lh_usize_t size,
                                      lh_memory_view_slice_t *out);

/**
 * @brief Build a slice with @p n bytes removed from the beginning.
 *
 * If @p n equals the source size, the returned slice is empty.
 *
 * @param self Source slice.
 * @param n    Number of bytes to remove from the beginning.
 * @return Source slice without the first @p n bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source slice size.
 */
lh_void
lh_memory_view_slice_drop_first(const lh_memory_view_slice_t *self, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Build a slice with @p n bytes removed from the end.
 *
 * If @p n equals the source size, the returned slice is empty.
 *
 * @param self Source slice.
 * @param n    Number of bytes to remove from the end.
 * @return Source slice without the last @p n bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p n is greater than the source slice size.
 */
lh_void
lh_memory_view_slice_drop_last(const lh_memory_view_slice_t *self, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Build a slice with bytes removed from both sides.
 *
 * If @p left + @p right equals the source size, the returned slice is empty.
 *
 * @param self  Source slice.
 * @param left  Number of bytes to remove from the beginning.
 * @param right Number of bytes to remove from the end.
 * @return Trimmed source slice.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p left + @p right is greater than the source slice size.
 */
lh_void
lh_memory_view_slice_trim(const lh_memory_view_slice_t *self, lh_usize_t left, lh_usize_t right, lh_memory_view_slice_t *out);

/**
 * @brief Store a closed slice starting at @p begin with @p size bytes.
 *
 * The resulting endpoints are <tt>[begin, begin + size - 1]</tt>.
 *
 * @param self  Slice to update.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the closed slice; must be non-zero.
 *
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed slice is not valid.
 */
lh_void
lh_memory_view_slice_set_by_size(lh_memory_view_slice_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief Initialize @p self with valid @p begin and @p end endpoints.
 *
 * Same as ::lh_memory_view_slice_set_v.
 *
 * @param self  Slice to initialize.
 * @param begin New @c first endpoint.
 * @param end   New @c second endpoint.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
lh_void
lh_memory_view_slice_init(lh_memory_view_slice_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Initialize @p self as a closed slice starting at @p begin with @p size bytes.
 *
 * Same as ::lh_memory_view_slice_set_by_size.
 *
 * @param self  Slice to initialize.
 * @param begin New @c first endpoint.
 * @param size  Number of bytes in the closed slice; must be non-zero.
 *
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed slice is not valid.
 */
lh_void
lh_memory_view_slice_init_by_size(lh_memory_view_slice_t *self, const lh_ptr begin,
                                  lh_usize_t size);

/**
 * @brief Initialize @p self with the empty slice initializer.
 * @param self Slice to initialize.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_void
lh_memory_view_slice_init_empty(lh_memory_view_slice_t *self);

/**
 * @brief Initialize @p self by copying endpoints from valid @p other.
 *
 * Same as ::lh_memory_view_slice_assign_v.
 *
 * @param self  Slice to initialize.
 * @param other Valid slice to copy from.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p other is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
lh_void
lh_memory_view_slice_init_by_other(lh_memory_view_slice_t *self,
                                   const lh_memory_view_slice_t *other);

/**
 * @brief Return an uninitialized empty slice.
 * @return Slice with both endpoints ::lh_null.
 */
lh_void
lh_memory_view_slice_return_empty(lh_memory_view_slice_t *out);

/**
 * @brief Return a validated closed slice <tt>[begin, end]</tt>.
 *
 * @param begin First byte.
 * @param end   Last byte.
 * @return Validated slice value.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid slice.
 */
lh_void
lh_memory_view_slice_return_init_v(const lh_ptr begin, const lh_ptr end, lh_memory_view_slice_t *out);

/**
 * @brief Return a validated closed slice of @p size bytes at @p begin.
 *
 * @param begin First byte.
 * @param size  Number of bytes. Must be non-zero.
 * @return Slice <tt>[begin, begin + size - 1]</tt>.
 *
 * @fails ::lh_runtime_error_code_invalid_argument
 *        @p begin is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is zero.
 */
lh_void
lh_memory_view_slice_return_init_by_size(const lh_ptr begin, lh_usize_t size, lh_memory_view_slice_t *out);

/**
 * @brief Resolve @p ptr moved by @p offset to an offset from the slice begin.
 *
 * A ::lh_null @p ptr means "from the slice begin". On failure the error code
 * is returned and @p out is left unchanged, except the null-pointer path,
 * which reports range errors through the asserting getters.
 *
 * @param self   Slice to search.
 * @param ptr    Anchor inside the slice, or ::lh_null for the beginning.
 * @param offset Signed step from @p ptr.
 * @param out    Receives the resulting offset from the beginning.
 * @return ::lh_runtime_error_code_ok, or an underflow/overflow code.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self or @p out is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p ptr is outside @p self.
 */
lh_runtime_error_code_t
lh_memory_view_slice_find_offset_from_ptr(const lh_memory_view_slice_t *self, const lh_ptr ptr,
                                          lh_soffset_t offset, lh_uoffset_t *out);

/**
 * @brief True when @p self is a valid half-open empty range <tt>[x, x)</tt>.
 *
 * The same bits are a one-byte closed slice. This predicate is for callers
 * that interpret the storage as a half-open view.
 *
 * @param self View storage to test.
 * @return ::lh_bool_true when both endpoints are set, ordered, and equal.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_is_ropen_empty(const lh_memory_view_slice_t *self);

/**
 * @brief True when half-open <tt>[begin, end)</tt> has no closed form.
 *
 * That is a null endpoint or @p begin == @p end.
 *
 * @param begin Range begin.
 * @param end   Exclusive range end.
 * @return ::lh_bool_true when the pair cannot be stored as a closed slice.
 */
lh_bool_t
lh_memory_view_slice_is_unclosable_ropen(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Copy half-open @p self as a closed slice.
 *
 * Uninitialized storage becomes an empty slice. A valid non-empty view
 * <tt>[begin, end)</tt> becomes <tt>[begin, end - 1]</tt>.
 *
 * @param self View storage to convert.
 * @return Closed slice value.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not uninitialized and not a non-empty valid half-open view.
 */
lh_void
lh_memory_view_slice_from_view(const lh_memory_view_slice_t *self, lh_memory_view_slice_t *out);

/**
 * @brief Half-open containment of <tt>[begin, end)</tt> in @p self.
 *
 * Used when @p self or the argument range has no closed form.
 *
 * @param self  View storage.
 * @param begin Inner range begin.
 * @param end   Inner exclusive end.
 * @return ::lh_bool_true when the half-open inner range is contained.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid half-open view.
 */
lh_bool_t
lh_memory_view_slice_contains_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief Half-open overlap of @p self with <tt>[begin, end)</tt>.
 *
 * @param self  View storage.
 * @param begin Other range begin.
 * @param end   Other exclusive end.
 * @return ::lh_bool_true when the half-open ranges overlap.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid half-open view.
 */
lh_bool_t
lh_memory_view_slice_overlaps_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief Find half-open <tt>[begin, end)</tt> inside half-open @p self.
 * @return Match pointer, or ::lh_null.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid half-open view.
 */
const lh_ptr
lh_memory_view_slice_find_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                const lh_ptr end);

/** @copydoc lh_memory_view_slice_find_ropen */
const lh_ptr
lh_memory_view_slice_rfind_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/** @copydoc lh_memory_view_slice_find_ropen */
const lh_ptr
lh_memory_view_slice_compare_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                   const lh_ptr end);

/** @copydoc lh_memory_view_slice_find_ropen */
const lh_ptr
lh_memory_view_slice_rcompare_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                    const lh_ptr end);

/**
 * @brief Take the first @p n bytes of half-open <tt>[begin, end)</tt> as a closed slice.
 *
 * @p n of zero returns an uninitialized slice without validating the range.
 * A non-zero @p n on <tt>[x, x)</tt> fails with ::lh_runtime_error_code_out_of_range.
 *
 * @return Closed slice covering those bytes, or an empty slice when @p n is zero.
 */
lh_void
lh_memory_view_slice_take_first_ropen(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Take the last @p n bytes of half-open <tt>[begin, end)</tt> as a closed slice.
 * @return Closed slice covering those bytes, or an empty slice when @p n is zero.
 */
lh_void
lh_memory_view_slice_take_last_ropen(const lh_ptr begin, const lh_ptr end, lh_usize_t n, lh_memory_view_slice_t *out);

/**
 * @brief Sub-range of half-open @p self, returned as a closed slice.
 *
 * @p size of zero returns an uninitialized slice. A positive size is a closed
 * slice of that many bytes, which converts back to <tt>[ptr, ptr + size)</tt>.
 *
 * @fails ::lh_runtime_error_code_out_of_range
 *        The requested window is outside @p self.
 */
lh_void
lh_memory_view_slice_from_offset_ropen(const lh_memory_view_slice_t *self, lh_uoffset_t offset,
                                       lh_usize_t size,
                                       lh_memory_view_slice_t *out);

/**
 * @brief Inclusive last byte of half-open <tt>[begin, end)</tt>: @p end @c - @c 1.
 * @param end Exclusive end.
 * @return Pointer to the last byte of the half-open range.
 */
const lh_ptr
lh_memory_view_slice_get_ropen_last(const lh_ptr end);

/**
 * @brief Exclusive end one past a closed endpoint: @p end @c + @c 1.
 * @param end Inclusive end.
 * @return Pointer one past @p end.
 */
const lh_ptr
lh_memory_view_slice_get_ropen_past(const lh_ptr end);

/**
 * @brief Exclusive end of closed @p slice: stored @c second @c + @c 1.
 * @param slice Closed slice.
 * @return Half-open end pointer.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p slice is ::lh_null.
 */
const lh_ptr
lh_memory_view_slice_get_ropen_end(const lh_memory_view_slice_t *slice);

/**
 * @brief Byte length of half-open <tt>[begin, end)</tt>.
 *
 * @p end is required. @p begin may be ::lh_null; the length is then the
 * address distance from a null pointer to @p end.
 *
 * @param begin Range begin.
 * @param end   Exclusive end.
 * @return @p end @c - @p begin in bytes.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p end is ::lh_null.
 */
lh_usize_t
lh_memory_view_slice_get_ropen_size(const lh_ptr begin, const lh_ptr end);

/**
 * @brief True when half-open @p self or <tt>[begin, end)</tt> has no closed form.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
lh_bool_t
lh_memory_view_slice_needs_ropen(const lh_memory_view_slice_t *self, const lh_ptr begin,
                                 const lh_ptr end);

/**
 * @brief Closed slice of a valid non-empty half-open @p self.
 *
 * <tt>[begin, end)</tt> becomes <tt>[begin, end - 1]</tt>.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid non-empty half-open view.
 */
lh_void
lh_memory_view_slice_return_of_ropen_v(const lh_memory_view_slice_t *self, lh_memory_view_slice_t *out);

/**
 * @brief Half-open containment of <tt>[begin, end)</tt> in <tt>[self_begin, self_end)</tt>.
 */
lh_bool_t
lh_memory_view_slice_ropen_contains_range(const lh_ptr self_begin, const lh_ptr self_end,
                                          const lh_ptr begin, const lh_ptr end);

/**
 * @brief Half-open overlap of <tt>[self_begin, self_end)</tt> and <tt>[begin, end)</tt>.
 */
lh_bool_t
lh_memory_view_slice_ropen_overlaps_range(const lh_ptr self_begin, const lh_ptr self_end,
                                          const lh_ptr begin, const lh_ptr end);

/**
 * @brief Fail when half-open <tt>[begin, end)</tt> cannot yield a non-empty take.
 *
 * Validates the pair first, then rejects <tt>[x, x)</tt>.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        <tt>[begin, end]</tt> is not a valid closed slice.
 * @fails ::lh_runtime_error_code_out_of_range
 *        <tt>[begin, end)</tt> is empty.
 */
lh_void
lh_memory_view_slice_assert_ropen_not_empty(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Half-open size of @p self: zero for <tt>[x, x)</tt>, otherwise @c end - begin.
 *
 * @fails ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid half-open view.
 */
lh_usize_t
lh_memory_view_slice_get_ropen_view_size(const lh_memory_view_slice_t *self);

/**
 * @brief Require @p offset and @p size to sit inside a span of @p total bytes.
 *
 * @p offset is checked before @p size. A zero @p size is inside the span when
 * @p offset is.
 *
 * @fails ::lh_runtime_error_code_out_of_range
 *        The window does not fit.
 */
lh_void
lh_memory_view_slice_assert_sub_range(lh_usize_t total, lh_uoffset_t offset, lh_usize_t size);

/**
 * @brief Pointer @p offset bytes into the closed form of half-open @p self.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p self is not a valid non-empty half-open view.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside that closed slice.
 */
const lh_ptr
lh_memory_view_slice_get_ropen_ptr(const lh_memory_view_slice_t *self, lh_uoffset_t offset);

/**
 * @brief Closed slice of @p size bytes at @p offset in half-open @p self.
 *
 * @p size of zero returns an uninitialized slice without converting @p self.
 *
 * @fails ::lh_runtime_error_code_invalid_range
 *        @p size is non-zero and @p self is not a valid non-empty half-open view.
 * @fails ::lh_runtime_error_code_out_of_range
 *        @p offset is outside that closed slice.
 */
lh_void
lh_memory_view_slice_return_ropen_sub(const lh_memory_view_slice_t *self, lh_uoffset_t offset,
                                      lh_usize_t size,
                                      lh_memory_view_slice_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_MEMORY_VIEW_SLICE_H */
