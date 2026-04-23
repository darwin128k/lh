/**
 * @file range.h
 * @brief Mutable non-owning memory span type (::lh_memory_range_t) and helpers.
 *
 * A range stores two endpoints @c first and @c second (::lh_memory_range_fields)
 * denoting the half-open address interval <tt>[first, second)</tt>:
 * @c first points at the first byte, @c second is one past the last byte (the
 * usual C / C++ iterator / span convention). Size in bytes is
 * @c second @c - @c first when both are non-null and ordered.
 *
 * Values are classified into a ::lh_memory_range_state_t word (null patterns and
 * address ordering). Helpers distinguish **valid** spans — empty (equal non-null
 * endpoints) or strictly ordered @c begin &lt; @c end — from reversed or
 * partially unspecified storage; see ::lh_memory_range_state_t.
 *
 * For the const-qualified counterpart, see ::lh_memory_view_t.
 *
 * @see lh_memory_range_state_t
 * @see lh_memory_view_t
 */

#ifndef LH_MEMORY_RANGE_H
#define LH_MEMORY_RANGE_H

#include <lh/attribute.h>
#include <lh/bool.h>
#include <lh/compiler.h>
#include <lh/memory/range/fields.h>
#include <lh/memory/range/state.h>
#include <lh/offset.h>
#include <lh/ptr.h>
#include <lh/size.h>

/**
 * @struct lh_memory_range
 * @brief Non-owning mutable memory span: two ::lh_void * endpoints (@c first / @c second).
 *
 * Endpoints describe <tt>[first, second)</tt> in address space.
 * Public typedef: ::lh_memory_range_t.
 * Same layout as ::lh_memory_view_t without @c const on the pointer type.
 */
typedef struct lh_memory_range {
    lh_memory_range_fields(lh_void);
} lh_memory_range_t; /**< Typedef for struct ::lh_memory_range. */

LH_COMPILER(EXTERN_C_BEGIN)

/* ── pack / unpack / init ─────────────────────────────────────────────────── */

/**
 * @brief Update @p self from optional input pointers for @c first / @c second.
 *
 * Pass ::lh_null for @p begin or @p end to leave that field unchanged.
 *
 * @param self  Range to modify.
 * @param begin Input pointer whose value is stored in @c first, or ::lh_null to skip.
 * @param end   Input pointer whose value is stored in @c second, or ::lh_null to skip.
 *
 * @note This function does not perform range
 *       validity checks on the resulting endpoints.
 *
 *       It simply reads the pointed-to lh_ptr values
 *       and assigns them to the range fields (when non-null).
 *
 *       Use lh_memory_range_get_state() to verify
 *       the resulting span before performing further operations.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_pack(lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Store @p begin as @c first and @p end as @c second (half-open <tt>[begin, end)</tt>).
 * @param self  Range to modify.
 * @param begin Value for @c first (first byte).
 * @param end   Value for @c second (one past the last byte).
 *
 * @note This function does not validate the coherence
 *       of begin/end (e.g. that begin <= end).
 *
 *       Use lh_memory_range_get_state() to determine whether
 *       the resulting range is empty, has data, or is reversed.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_set(lh_memory_range_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Set @c first = @p begin and @c second = @p begin + @p size (byte offset).
 *
 * Equivalent to half-open <tt>[begin, begin + size)</tt>.
 * @p begin must be non-null. @p size must be valid for pointer arithmetic on @p begin.
 *
 * @param self  Range to modify.
 * @param begin Start address.
 * @param size  Length in bytes.
 *
 * @note This operation can fail if arguments are invalid.
 *
 *       In particular, if @p begin is NULL or @p size is not suitable for pointer arithmetic,
 *       a runtime error with code lh_runtime_error_code_invalid_argument may be raised.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_set_by_size(lh_memory_range_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Read @c first / @c second from @p self into optional outputs.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self  Range to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 *
 * @note This function does not validate the retrieved endpoints;
 *       to enforce validity before consuming, consider using
 *       lh_memory_range_unpack_v or lh_memory_range_get_state().
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_unpack(const lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Return @c first.
 * @param self Range to read.
 * @return Stored begin pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_begin(const lh_memory_range_t *self);

/**
 * @brief Return @c second.
 * @param self Range to read.
 * @return Stored end pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_end(const lh_memory_range_t *self);

/**
 * @brief Same as ::lh_memory_range_set for @p begin / @p end.
 * @param self  Range to initialize.
 * @param begin Value stored as @c first.
 * @param end   Value stored as @c second.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_init(lh_memory_range_t *self, lh_void *begin, lh_void *end);

/**
 * @brief Same as ::lh_memory_range_set_by_size.
 * @param self  Range to initialize.
 * @param begin Start address.
 * @param size  Length in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_init_by_size(lh_memory_range_t *self, lh_ptr begin, lh_usize_t size);

/**
 * @brief Copy bounds from @p other into @p self (no validity check on @p other).
 * @param self  Destination range.
 * @param other Source range (same layout).
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_init_by_other(lh_memory_range_t *self, const lh_memory_range_t *other);

/* ── classification ───────────────────────────────────────────────────────── */

/**
 * @brief Classify stored endpoints into a flag word (::lh_memory_range_state_t).
 * @param self Range to inspect.
 * @return Bit pattern describing null endpoints and ordering when both are non-null.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_state_t lh_memory_range_get_state(const lh_memory_range_t *self);

/**
 * @brief True iff state is ::lh_memory_view_state_uninitialized (both endpoints null).
 * @param self Range to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_uninitialized(const lh_memory_range_t *self);

/**
 * @brief True iff both endpoints are non-null and @c first &lt; @c second.
 * @param self Range to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_has_data(const lh_memory_range_t *self);

/**
 * @brief True iff both endpoints are non-null and equal (degenerate span).
 * @param self Range to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_empty(const lh_memory_range_t *self);

/**
 * @brief True iff the range is ::lh_memory_range_is_empty or ::lh_memory_range_has_data.
 * @param self Range to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_valid(const lh_memory_range_t *self);

/**
 * @brief Logical negation of ::lh_memory_range_is_valid.
 * @param self Range to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_invalid(const lh_memory_range_t *self);

/* ── validated access, size, geometry ─────────────────────────────────────── */

/**
 * @brief Like ::lh_memory_range_unpack but requires ::lh_memory_range_is_valid(@p self).
 * @param self  Valid range to read.
 * @param begin Output for @c first.
 * @param end   Output for @c second.
 */
LH_ATTRIBUTE(SYMBOL)
void lh_memory_range_unpack_v(const lh_memory_range_t *self, lh_ptr *begin, lh_ptr *end);

/**
 * @brief Raw byte difference @c second @c - @c first (no range validity check).
 * @param self Range to read.
 * @return Signed address difference; may be negative for reversed bounds.
 */
LH_ATTRIBUTE(SYMBOL)
lh_saddr_t lh_memory_range_diff(const lh_memory_range_t *self);

/**
 * @brief Span length in bytes (::lh_memory_range_diff cast to ::lh_usize_t).
 * @param self Valid range to read.
 */
LH_ATTRIBUTE(SYMBOL)
lh_usize_t lh_memory_range_get_size(const lh_memory_range_t *self);

/**
 * @brief True iff @c first is aligned to @p align.
 *
 * @p align must be a power of two.
 *
 * @param self  Range to inspect.
 * @param align Required alignment in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_begin_aligned(const lh_memory_range_t *self, lh_usize_t align);

/**
 * @brief True iff both @c first and @c second satisfy alignment @p align.
 *
 * @p align must be a power of two.
 *
 * @param self  Range to inspect.
 * @param align Required alignment in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_aligned(const lh_memory_range_t *self, lh_usize_t align);

/**
 * @brief True iff ::lh_memory_range_get_size(@p self) is a multiple of @p multiple.
 * @param self     Valid range to inspect.
 * @param multiple Divisor to test (library convention for @p multiple applies).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_multiple_of(const lh_memory_range_t *self, lh_usize_t multiple);

/**
 * @brief Half-open containment: @p ptr satisfies @c first @c &lt;= @p ptr @c &lt; @c second.
 * @param self Range to test (must be valid).
 * @param ptr  Address to test.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_contains_ptr(const lh_memory_range_t *self, const lh_ptr ptr);

/**
 * @brief Half-open containment: <tt>[begin, end)</tt> lies inside @p self.
 * @param self Range to test (must be valid).
 * @param begin Inner half-open start (inclusive).
 * @param end   Inner half-open end (exclusive).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_contains_range(const lh_memory_range_t *self, const lh_ptr begin,
                                         const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_contains_range after unpacking @p other.
 * @param self  Range to test (must be valid).
 * @param other Range whose endpoints are tested (unpacked without extra validity check).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_contains(const lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief True iff @p self has endpoints equal to @p begin / @p end.
 *
 * @param self  Range to test.
 * @param begin Expected begin pointer.
 * @param end   Expected end pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_equals_range(const lh_memory_range_t *self, const lh_ptr begin,
                                       const lh_ptr end);

/**
 * @brief True iff @p self and @p other have identical stored endpoints.
 *
 * Compares the raw bounds (`first`, `second`) for equality.
 *
 * @param self  First range.
 * @param other Second range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_equals(const lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Half-open overlap between @p self and <tt>[begin, end)</tt>.
 * @param self  Range to test (must be valid).
 * @param begin Second half-open start (inclusive).
 * @param end   Second half-open end (exclusive).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_overlaps_range(const lh_memory_range_t *self, const lh_ptr begin,
                                         const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_overlaps_range after unpacking @p other.
 * @param self  Range to test (must be valid).
 * @param other Second range (unpacked without extra validity check on @p other).
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_overlaps(const lh_memory_range_t *self, const lh_memory_range_t *other);

/* ── indexing ─────────────────────────────────────────────────────────────── */

/**
 * @brief True iff @p offset &lt; ::lh_memory_range_get_size(@p self).
 * @param self   Valid range.
 * @param offset Zero-based byte offset from the start of the span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_valid_offset(const lh_memory_range_t *self, lh_uoffset_t offset);

/**
 * @brief Address @c first + @p offset (requires ::lh_memory_range_is_valid_offset).
 * @param self   Valid range.
 * @param offset Byte offset from @c first.
 * @return Pointer into the span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_ptr_from_front(const lh_memory_range_t *self, lh_uoffset_t offset);

/**
 * @brief Address of the element @p offset from the last byte (toward @c first).
 * @param self   Valid non-empty range.
 * @param offset Distance back from the last element (0 = last byte).
 * @return Pointer into the span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_ptr_from_back(const lh_memory_range_t *self, lh_uoffset_t offset);

/**
 * @brief Dispatch to ::lh_memory_range_get_ptr_from_front or ::lh_memory_range_get_ptr_from_back.
 * @param self      Valid range.
 * @param offset  Byte offset (interpretation depends on @p is_back).
 * @param is_back If true, count from the end of the span.
 * @return Pointer into the span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_ptr(const lh_memory_range_t *self, lh_uoffset_t offset,
                               lh_bool_t is_back);

/**
 * @brief Byte value at @p offset from the front (requires valid offset).
 * @param self   Valid range.
 * @param offset Byte offset from @c first.
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_get_value_from_front(const lh_memory_range_t *self, lh_uoffset_t offset);

/**
 * @brief Byte value at @p offset from the back (requires valid offset).
 * @param self   Valid non-empty range.
 * @param offset Distance back from the last element (0 = last byte).
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_get_value_from_back(const lh_memory_range_t *self, lh_uoffset_t offset);

/**
 * @brief Dispatch to ::lh_memory_range_get_value_from_front or
 * ::lh_memory_range_get_value_from_back.
 * @param self      Valid range.
 * @param offset  Byte offset (interpretation depends on @p is_back).
 * @param is_back If true, count from the end of the span.
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_get_value(const lh_memory_range_t *self, lh_uoffset_t offset,
                                    lh_bool_t is_back);

/**
 * @brief True iff @p self can produce a valid slice for (@p offset, @p size).
 *
 * This predicate mirrors the runtime precondition used by
 * ::lh_memory_range_slice.
 *
 * @param self   Source valid range.
 * @param offset Start byte offset from @c first.
 * @param size   Slice size in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_memory_range_is_sliceable(const lh_memory_range_t *self, lh_uoffset_t offset,
                                       lh_uoffset_t size);

/**
 * @brief Return a validated half-open subrange <tt>[offset, offset + size)</tt> of @p self.
 *
 * Uses front-based indexing and fails the runtime check when the resulting
 * offsets are not sliceable for @p self (see ::lh_memory_range_is_sliceable).
 *
 * @param self   Source valid range.
 * @param offset Start byte offset from @c first.
 * @param size   Length of the subrange in bytes.
 * @return Constructed valid subrange.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t lh_memory_range_slice(const lh_memory_range_t *self, lh_uoffset_t offset,
                                        lh_uoffset_t size);

/**
 * @brief Write @p value at ::lh_memory_range_get_ptr(@p self, @p offset, @p is_back)
 * (inverse of ::lh_memory_range_get_value for the same @p offset / @p is_back).
 * @param self      Range to modify.
 * @param offset    Index as for ::lh_memory_range_get_ptr.
 * @param value     Byte to store.
 * @param is_back If true, index from the end.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_set_value(lh_memory_range_t *self, lh_uoffset_t offset, lh_byte_t value,
                                  lh_bool_t is_back);

/**
 * @brief Same as ::lh_memory_range_get_ptr(@p self, 0, ::lh_bool_false).
 * @param self Valid range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_front_ptr(const lh_memory_range_t *self);

/**
 * @brief Byte stored at ::lh_memory_range_get_front_ptr(@p self).
 * @param self Valid range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_get_front(const lh_memory_range_t *self);

/**
 * @brief Same as ::lh_memory_range_get_ptr(@p self, 0, ::lh_bool_true).
 * @param self Valid non-empty range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_get_back_ptr(const lh_memory_range_t *self);

/**
 * @brief Byte stored at ::lh_memory_range_get_back_ptr(@p self).
 * @param self Valid non-empty range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_get_back(const lh_memory_range_t *self);

/**
 * @brief Return next pointer after @p ptr within @p self, or ::lh_null if no next element.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the last element (`second - 1`) of the half-open span.
 *
 * @param self Range to iterate.
 * @param ptr  Current pointer.
 * @return Next pointer in range, or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_next_ptr(const lh_memory_range_t *self, lh_ptr ptr);

/**
 * @brief Return previous pointer before @p ptr within @p self, or ::lh_null if no previous element.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the first element (`first`) of the half-open span.
 *
 * @param self Range to iterate.
 * @param ptr  Current pointer.
 * @return Previous pointer in range, or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_prev_ptr(const lh_memory_range_t *self, lh_ptr ptr);

/**
 * @brief Return value at next pointer after @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_memory_range_next_ptr. Fails runtime check
 * when next element does not exist.
 *
 * @param self Range to iterate.
 * @param ptr  Current pointer.
 * @return Byte value at next position.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_next_value(const lh_memory_range_t *self, lh_ptr ptr);

/**
 * @brief Return value at previous pointer before @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_memory_range_prev_ptr. Fails runtime check
 * when previous element does not exist.
 *
 * @param self Range to iterate.
 * @param ptr  Current pointer.
 * @return Byte value at previous position.
 */
LH_ATTRIBUTE(SYMBOL)
lh_byte_t lh_memory_range_prev_value(const lh_memory_range_t *self, lh_ptr ptr);

/* ── raw byte ops (half-open spans) ───────────────────────────────────────── */

/**
 * @brief Copy bytes into @p self from <tt>[begin, end)</tt> (see ::lh_memory_raw_copy).
 *
 * Unpacks @p self as the destination span; @p begin / @p end are the source span.
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range used as @c dst / @c dst_end for ::lh_memory_raw_copy.
 * @param begin Source span start (inclusive).
 * @param end   Source span end (exclusive).
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_copy_range(lh_memory_range_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_copy_range after unpacking @p other (requires valid @p other).
 * @param self  Valid destination range.
 * @param other Valid source range.
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_copy(lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Move bytes into @p self from <tt>[begin, end)</tt> (see ::lh_memory_raw_move).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range used as destination.
 * @param begin Source span start (inclusive).
 * @param end   Source span end (exclusive).
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_move_range(lh_memory_range_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_move_range after unpacking @p other (requires valid @p other).
 * @param self  Valid destination range.
 * @param other Valid source range.
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_move(lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Fill @p self with a constant byte value (see ::lh_memory_raw_set).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Destination range (must be valid).
 * @param value Byte value to write into each element of @p self.
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_fill(lh_memory_range_t *self, lh_byte_t value);

/**
 * @brief Fill @p self by repeating bytes from <tt>[begin, end)</tt>
 * (see ::lh_memory_raw_set_pattern).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Destination range (must be valid).
 * @param begin Pattern span start (inclusive).
 * @param end   Pattern span end (exclusive).
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_fill_pattern_range(lh_memory_range_t *self, const lh_ptr begin,
                                          const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_fill_pattern_range after unpacking @p other
 * (requires valid @p other).
 * @param self  Destination range (must be valid).
 * @param other Pattern range (must be valid).
 *
 * @return Pointer one past the last byte written into @p self.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_fill_pattern(lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Find the leftmost occurrence of <tt>[begin, end)</tt> inside @p self (see
 * ::lh_memory_raw_find).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range (haystack).
 * @param begin Needle start (inclusive).
 * @param end   Needle end (exclusive).
 *
 * @return Start of the first match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_find_range(const lh_memory_range_t *self, const lh_ptr begin,
                                  const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_find_range after unpacking @p other (requires valid @p other).
 * @param self  Haystack (must be valid).
 * @param other Needle range (must be valid).
 *
 * @return Start of the first match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_find(const lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Find the rightmost occurrence of <tt>[begin, end)</tt> in @p self (see
 * ::lh_memory_raw_rfind).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range (haystack).
 * @param begin Needle start (inclusive).
 * @param end   Needle end (exclusive).
 *
 * @return Start of the last match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_rfind_range(const lh_memory_range_t *self, const lh_ptr begin,
                                   const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_rfind_range after unpacking @p other (requires valid @p other).
 * @param self  Haystack (must be valid).
 * @param other Needle range (must be valid).
 *
 * @return Start of the last match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_rfind(const lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Compare @p self to <tt>[begin, end)</tt> forward (see ::lh_memory_raw_compare).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range (left-hand side).
 * @param begin Right-hand span start (inclusive).
 * @param end   Right-hand span end (exclusive).
 *
 * @return Pointer to the first differing byte in @p self, or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_compare_range(const lh_memory_range_t *self, const lh_ptr begin,
                                     const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_compare_range after unpacking @p other (requires valid @p
 * other).
 * @param self  Left-hand range (must be valid).
 * @param other Right-hand range (must be valid).
 *
 * @return Pointer to the first differing byte in @p self, or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_compare(const lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Compare suffixes of @p self and <tt>[begin, end)</tt> from the ends inward (see
 * ::lh_memory_raw_rcompare).
 *
 * Requires ::lh_memory_range_is_valid(@p self).
 *
 * @param self  Valid range (left-hand side).
 * @param begin Right-hand span start (inclusive).
 * @param end   Right-hand span end (exclusive).
 *
 * @return Pointer into @p self at the differing byte in the compared suffix window, or ::lh_null if
 * equal.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_rcompare_range(const lh_memory_range_t *self, const lh_ptr begin,
                                      const lh_ptr end);

/**
 * @brief Same as ::lh_memory_range_rcompare_range after unpacking @p other (requires valid @p
 * other).
 * @param self  Left-hand range (must be valid).
 * @param other Right-hand range (must be valid).
 *
 * @return Pointer into @p self at the differing byte in the compared suffix window, or ::lh_null if
 * equal.
 */
LH_ATTRIBUTE(SYMBOL)
lh_ptr lh_memory_range_rcompare(const lh_memory_range_t *self, const lh_memory_range_t *other);

/* ── copy / clear / swap ──────────────────────────────────────────────────── */

/**
 * @brief Copy @p other into @p self without requiring ::lh_memory_range_is_valid(@p other).
 * @param self  Destination.
 * @param other Source.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_assign(lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Reset @p self to the “both null” pattern (::lh_memory_range_empty_initializer).
 * @param self Range to clear.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_clear(lh_memory_range_t *self);

/**
 * @brief Like ::lh_memory_range_assign but requires ::lh_memory_range_is_valid(@p other).
 * @param self  Destination.
 * @param other Source (must be valid).
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_assign_v(lh_memory_range_t *self, const lh_memory_range_t *other);

/**
 * @brief Build a temporary range from @p begin / @p end and ::lh_memory_range_assign_v.
 *
 * Fails the runtime check if the pair is not a valid range.
 *
 * @param self  Destination.
 * @param begin New @c first.
 * @param end   New @c second.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_set_v(lh_memory_range_t *self, lh_ptr begin, lh_ptr end);

/**
 * @brief Like ::lh_memory_range_set_by_size, but on failure clears @p self.
 * @param self  Destination.
 * @param begin Start address.
 * @param size  Length in bytes.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_set_by_size_or_clear(lh_memory_range_t *self, lh_ptr begin,
                                             lh_usize_t size);

/**
 * @brief Swap the contents of @p self and @p other.
 * @param self  First range.
 * @param other Second range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_swap(lh_memory_range_t *self, lh_memory_range_t *other);

/**
 * @brief Clear @p self, then swap with @p other (so @p other receives the cleared range).
 * @param self  Range to replace.
 * @param other Range to exchange with.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_memory_range_exchange(lh_memory_range_t *self, lh_memory_range_t *other);

/**
 * @brief Build a range from @p begin / @p end and return it by value (no validation).
 *
 * @param begin New @c first.
 * @param end   New @c second.
 * @return Constructed range (may be invalid; see ::lh_memory_range_is_valid).
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t lh_memory_range_make(lh_ptr begin, lh_ptr end);

/**
 * @brief Build and validate a range from @p begin and @p size.
 *
 * Equivalent to constructing the half-open range <tt>[begin, begin + size)</tt>.
 * Uses the same preconditions as ::lh_memory_range_set_by_size and validates
 * the resulting bounds as in ::lh_memory_range_make_v.
 *
 * Fails the runtime check if the resulting pair is not a valid range.
 *
 * @p begin must be non-null and @p size must be valid for pointer arithmetic.
 *
 * @param begin New @c first.
 * @param size  Range length in bytes.
 * @return Constructed valid range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t lh_memory_range_make_by_size(lh_ptr begin, lh_usize_t size);

/**
 * @brief Build and validate a range from @p begin / @p end, then return it by value.
 *
 * Fails the runtime check if the pair is not a valid range.
 *
 * @param begin New @c first.
 * @param end   New @c second.
 * @return Constructed valid range.
 */
LH_ATTRIBUTE(SYMBOL)
lh_memory_range_t lh_memory_range_make_v(lh_ptr begin, lh_ptr end);

LH_COMPILER(EXTERN_C_END)

#endif // LH_MEMORY_RANGE_H
