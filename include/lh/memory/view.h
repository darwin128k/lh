/**
 * @file view.h
 * @brief Non-owning read-only memory view type (::lh_memory_view_t) and helpers.
 *
 * A view stores two endpoints @c first and @c second (::lh_memory_view_fields)
 * denoting the half-open address interval <tt>[first, second)</tt>:
 * @c first points at the first byte, @c second is one past the last byte
 * (the usual C / C++ iterator / span convention).
 *
 * Size in bytes is @c second @c - @c first when both are non-null and ordered.
 *
 * Values are classified into a ::lh_memory_range_state_t word (null patterns and
 * address ordering). Helpers distinguish **valid** spans - empty (equal non-null
 * endpoints) or strictly ordered @c begin &lt; @c end - from reversed or
 * partially unspecified storage; see ::lh_memory_range_state_t.
 *
 * Mirrors the read-only subset of ::lh_memory_range_t.
 * Operations that write into the described memory
 * (copy, move, fill, indexed write) exist only on ::lh_memory_range_t.
 *
 * @see lh_memory_range_state_t
 * @see lh_memory_range_t
 */

#ifndef LH_MEMORY_VIEW_H
#define LH_MEMORY_VIEW_H

#include <lh/attribute/symbol.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/memory/range/state.h>
#include <lh/memory/view/fields.h>
#include <lh/offset.h>
#include <lh/size.h>
#include <lh/ptr.h>

/**
 * @struct lh_memory_view
 * @brief Non-owning read-only memory span: two @c const ::lh_ptr  endpoints.
 *
 * Endpoints describe <tt>[first, second)</tt> like ::lh_memory_range_t.
 * Public typedef: ::lh_memory_view_t.
 * Same layout as ::lh_memory_range_t but with const-qualified pointers.
 */
typedef struct lh_memory_view {
    lh_memory_view_fields(lh_void);
} lh_memory_view_t; /**< Typedef for struct ::lh_memory_view. */

LH_COMPILER_EXTERN_C_BEGIN

/* ── pack / unpack / init ─────────────────────────────────────────────────── */

/**
 * @brief Update @p self from optional inputs for @c first / @c second.
 *
 * Pass ::lh_null for @p begin
 * or @p end to leave that field unchanged.
 *
 * @param self  View to modify.
 * @param begin Input pointer whose value is stored in @c first, or ::lh_null to skip.
 * @param end   Input pointer whose value is stored in @c second, or ::lh_null to skip.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_pack(lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end);

/**
 * @brief Store @p begin as @c first and @p end as @c second (half-open <tt>[begin, end)</tt>).
 * @param self  View to modify.
 * @param begin Value for @c first (first byte).
 * @param end   Value for @c second (one past the last byte).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_set(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_set.
 * @param self  View to modify.
 * @param begin Value for @c first (first byte).
 * @param end   Value for @c second (one past the last byte).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_init(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Initialize @p self with @c first = @p begin and @c second = @p begin + @p size.
 *
 * Equivalent to half-open <tt>[begin, begin + size)</tt>.
 * @p begin must be non-null. @p size must be valid for byte offset arithmetic on @p begin.
 *
 * @param self  View to modify.
 * @param begin Start address.
 * @param size  Length in bytes.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_init_by_size(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief Read @c first / @c second from @p self into optional outputs.
 *
 * Pass ::lh_null for any pointer to skip that field.
 *
 * @param self  View to read.
 * @param begin Output for @c first, or ::lh_null.
 * @param end   Output for @c second, or ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_unpack(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end);

/**
 * @brief Copy bounds from @p other into @p self (no validity check on @p other).
 * @param self  Destination view.
 * @param other Source view (same layout).
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_init_by_other(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Initialize @p self with ::lh_memory_view_empty_initializer.
 * @param self Destination view.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_init_by_empty(lh_memory_view_t *self);

/**
 * @brief Return @c first.
 * @param self View to read.
 * @return Stored begin pointer.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_begin(const lh_memory_view_t *self);

/**
 * @brief Return @c second.
 * @param self View to read.
 * @return Stored end pointer.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_end(const lh_memory_view_t *self);

/**
 * @brief Copy @p other into @p self without requiring ::lh_memory_view_is_valid(@p other).
 * @param self  Destination.
 * @param other Source.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_assign(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Reset @p self to the "both null" pattern (::lh_memory_view_empty_initializer).
 * @param self View to clear.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_clear(lh_memory_view_t *self);

/**
 * @brief Swap the contents of @p self and @p other.
 * @param self  First view.
 * @param other Second view.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_swap(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Clear @p self, then swap with @p other (so @p other receives the cleared view).
 * @param self  View to replace.
 * @param other View to exchange with.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_exchange(lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Return a by-value copy of @p self without requiring validity.
 *
 * Copies raw stored endpoints (`first`, `second`) as-is.
 *
 * @param self Source view.
 * @return Cloned view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_clone(const lh_memory_view_t *self);

/**
 * @brief Copy @p self into @p other without requiring validity.
 *
 * @param self  Source view.
 * @param other Destination view.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_dup(const lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Copy @p self into @p other through validated assignment.
 *
 * @param self  Source view.
 * @param other Destination view.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_dup_v(const lh_memory_view_t *self, lh_memory_view_t *other);

/**
 * @brief Clone @p self and validate the produced value as in ::lh_memory_view_dup_v.
 *
 * @param self Source view.
 * @return Cloned valid view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_clone_v(const lh_memory_view_t *self);

/* ── classification ───────────────────────────────────────────────────────── */

/**
 * @brief Classify stored endpoints into a flag word (::lh_memory_range_state_t).
 * @param self View to inspect.
 * @return Bit pattern describing null endpoints and ordering when both are non-null.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_range_state_t
lh_memory_view_get_state(const lh_memory_view_t *self);

/**
 * @brief True iff state is ::lh_memory_view_state_uninitialized (both endpoints null).
 * @param self View to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_uninitialized(const lh_memory_view_t *self);

/**
 * @brief True iff both endpoints are non-null and @c first &lt; @c second.
 * @param self View to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_has_data(const lh_memory_view_t *self);

/**
 * @brief True iff both endpoints are non-null and equal (degenerate span).
 * @param self View to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_empty(const lh_memory_view_t *self);

/**
 * @brief True iff the view is ::lh_memory_view_is_empty or ::lh_memory_view_has_data.
 * @param self View to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_valid(const lh_memory_view_t *self);

/**
 * @brief Logical negation of ::lh_memory_view_is_valid.
 * @param self View to inspect.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_invalid(const lh_memory_view_t *self);

/* ── validated access, size, geometry ─────────────────────────────────────── */

/**
 * @brief Like ::lh_memory_view_unpack but requires ::lh_memory_view_is_valid(@p self).
 * @param self  Valid view to read.
 * @param begin Output for @c first.
 * @param end   Output for @c second.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_unpack_v(const lh_memory_view_t *self, const lh_ptr *begin, const lh_ptr *end);

/**
 * @brief Raw byte difference @c second @c - @c first (no validity check).
 * @param self View to read.
 * @return Signed address difference; may be negative for reversed bounds.
 */
LH_ATTRIBUTE_SYMBOL
lh_saddr_t
lh_memory_view_diff(const lh_memory_view_t *self);

/**
 * @brief Span length in bytes (::lh_memory_view_diff cast to ::lh_usize_t).
 * @param self Valid view to read.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_memory_view_get_size(const lh_memory_view_t *self);

/**
 * @brief True iff @c first is aligned to @p align.
 *
 * @p align must be a power of two.
 *
 * @param self  View to inspect.
 * @param align Required alignment in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_begin_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief True iff both @c first and @c second satisfy alignment @p align.
 *
 * @p align must be a power of two.
 *
 * @param self  View to inspect.
 * @param align Required alignment in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_aligned(const lh_memory_view_t *self, lh_usize_t align);

/**
 * @brief True iff ::lh_memory_view_get_size(@p self) is a multiple of @p multiple.
 * @param self     Valid view to inspect.
 * @param multiple Divisor to test (library convention for @p multiple applies).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_multiple_of(const lh_memory_view_t *self, lh_usize_t multiple);

/**
 * @brief Half-open containment: @p ptr satisfies @c first @c &lt;= @p ptr @c &lt; @c second.
 * @param self View to test (must be valid).
 * @param ptr  Address to test.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Half-open containment: <tt>[begin, end)</tt> lies inside @p self.
 * @param self  View to test (must be valid).
 * @param begin Inner half-open start (inclusive).
 * @param end   Inner half-open end (exclusive).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_contains_range after unpacking @p other.
 * @param self  View to test (must be valid).
 * @param other View whose endpoints are tested (unpacked without extra validity check).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_contains(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief True iff @p self has endpoints equal to @p begin / @p end.
 * @param self  View to test.
 * @param begin Expected begin pointer.
 * @param end   Expected end pointer.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_equals_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True iff @p self and @p other have identical stored endpoints.
 *
 * Compares the raw bounds (`first`, `second`) for equality.
 *
 * @param self  First view.
 * @param other Second view.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_equals(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Half-open overlap between @p self and <tt>[begin, end)</tt>.
 * @param self  View to test (must be valid).
 * @param begin Second half-open start (inclusive).
 * @param end   Second half-open end (exclusive).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_overlaps_range after unpacking @p other.
 * @param self  View to test (must be valid).
 * @param other Second view (unpacked without extra validity check on @p other).
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_overlaps(const lh_memory_view_t *self, const lh_memory_view_t *other);

/* ── indexing ─────────────────────────────────────────────────────────────── */

/**
 * @brief True iff @p offset &lt; ::lh_memory_view_get_size(@p self).
 * @param self   Valid view.
 * @param offset Zero-based byte offset from the start of the span.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_valid_offset(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Address @c first + @p offset (requires ::lh_memory_view_is_valid_offset).
 * @param self   Valid view.
 * @param offset Byte offset from @c first.
 * @return Pointer into the span.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_ptr_from_front(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Address of the element @p offset from the last byte (toward @c first).
 * @param self   Valid non-empty view.
 * @param offset Distance back from the last element (0 = last byte).
 * @return Pointer into the span.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_ptr_from_back(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Dispatch to ::lh_memory_view_get_ptr_from_front or ::lh_memory_view_get_ptr_from_back.
 * @param self      Valid view.
 * @param offset    Byte offset (interpretation depends on @p from_back).
 * @param from_back If true, count from the end of the span.
 * @return Pointer into the span.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_ptr(const lh_memory_view_t *self, lh_uoffset_t offset, lh_bool_t from_back);

/**
 * @brief Byte value at @p offset from the front (requires valid offset).
 * @param self   Valid view.
 * @param offset Byte offset from @c first.
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_value_from_front(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Byte value at @p offset from the back (requires valid offset).
 * @param self   Valid non-empty view.
 * @param offset Distance back from the last element (0 = last byte).
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_value_from_back(const lh_memory_view_t *self, lh_uoffset_t offset);

/**
 * @brief Dispatch to ::lh_memory_view_get_value_from_front or ::lh_memory_view_get_value_from_back.
 * @param self      Valid view.
 * @param offset    Byte offset (interpretation depends on @p from_back).
 * @param from_back If true, count from the end of the span.
 * @return Byte stored at the resolved address.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_value(const lh_memory_view_t *self, lh_uoffset_t offset, lh_bool_t from_back);

/**
 * @brief True iff @p self can produce a valid slice for (@p offset, @p size).
 *
 * This predicate mirrors the runtime precondition used by
 * ::lh_memory_view_slice.
 *
 * @param self   Source valid view.
 * @param offset Start byte offset from @c first.
 * @param size   Slice size in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_memory_view_is_sliceable(const lh_memory_view_t *self, lh_uoffset_t offset, lh_uoffset_t size);

/**
 * @brief Return a validated half-open subview <tt>[offset, offset + size)</tt> of @p self.
 *
 * Uses front-based indexing and fails the runtime check when the resulting
 * offsets are not sliceable for @p self (see ::lh_memory_view_is_sliceable).
 *
 * @param self   Source valid view.
 * @param offset Start byte offset from @c first.
 * @param size   Length of the subview in bytes.
 * @return Constructed valid subview.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_slice(const lh_memory_view_t *self, lh_uoffset_t offset, lh_uoffset_t size);

/**
 * @brief Like ::lh_memory_view_slice, but returns empty view on any failure.
 *
 * On invalid @p self or out-of-range/overflow slice request, returns
 * ::lh_memory_view_empty_initializer.
 *
 * @param self   Source view.
 * @param offset Start byte offset from @c first.
 * @param size   Length of the subview in bytes.
 * @return Constructed subview or empty view on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_slice_or_empty(const lh_memory_view_t *self, lh_uoffset_t offset, lh_uoffset_t size);

/**
 * @brief Same as ::lh_memory_view_get_ptr(@p self, 0, ::lh_bool_false).
 * @param self Valid view.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_front_ptr(const lh_memory_view_t *self);

/**
 * @brief Byte stored at ::lh_memory_view_get_front_ptr(@p self).
 * @param self Valid view.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_front_value(const lh_memory_view_t *self);

/**
 * @brief Same as ::lh_memory_view_get_ptr(@p self, 0, ::lh_bool_true).
 * @param self Valid non-empty view.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_get_back_ptr(const lh_memory_view_t *self);

/**
 * @brief Byte stored at ::lh_memory_view_get_back_ptr(@p self).
 * @param self Valid non-empty view.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_get_back_value(const lh_memory_view_t *self);

/**
 * @brief Return next pointer after @p ptr within @p self, or ::lh_null if no next element.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the last element (`second - 1`) of the half-open span.
 *
 * @param self View to iterate.
 * @param ptr  Current pointer.
 * @return Next pointer in view, or ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_next_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return previous pointer before @p ptr within @p self, or ::lh_null if no previous element.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the first element (`first`) of the half-open span.
 *
 * @param self View to iterate.
 * @param ptr  Current pointer.
 * @return Previous pointer in view, or ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_prev_ptr(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return value at next pointer after @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_memory_view_next_ptr. Fails runtime check
 * when next element does not exist.
 *
 * @param self View to iterate.
 * @param ptr  Current pointer.
 * @return Byte value at next position.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_next_value(const lh_memory_view_t *self, const lh_ptr ptr);

/**
 * @brief Return value at previous pointer before @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_memory_view_prev_ptr. Fails runtime check
 * when previous element does not exist.
 *
 * @param self View to iterate.
 * @param ptr  Current pointer.
 * @return Byte value at previous position.
 */
LH_ATTRIBUTE_SYMBOL
lh_byte_t
lh_memory_view_prev_value(const lh_memory_view_t *self, const lh_ptr ptr);

/* ── raw byte search / compare (read-only) ───────────────────────────────── */

/**
 * @brief Find the leftmost occurrence of <tt>[begin, end)</tt> inside @p self (see
 * ::lh_memory_raw_find).
 *
 * Requires ::lh_memory_view_is_valid(@p self).
 *
 * @param self  Valid view (haystack).
 * @param begin Needle start (inclusive).
 * @param end   Needle end (exclusive).
 *
 * @return Start of the first match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_find_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_find_range after unpacking @p other (requires valid @p other).
 * @param self  Haystack (must be valid).
 * @param other Needle view (must be valid).
 *
 * @return Start of the first match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_find(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Find the rightmost occurrence of <tt>[begin, end)</tt> in @p self (see
 * ::lh_memory_raw_rfind).
 *
 * Requires ::lh_memory_view_is_valid(@p self).
 *
 * @param self  Valid view (haystack).
 * @param begin Needle start (inclusive).
 * @param end   Needle end (exclusive).
 *
 * @return Start of the last match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rfind_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_rfind_range after unpacking @p other (requires valid @p other).
 * @param self  Haystack (must be valid).
 * @param other Needle view (must be valid).
 *
 * @return Start of the last match in @p self, or ::lh_null if none.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rfind(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Compare @p self to <tt>[begin, end)</tt> forward (see ::lh_memory_raw_compare).
 *
 * Requires ::lh_memory_view_is_valid(@p self).
 *
 * @param self  Valid view (left-hand side).
 * @param begin Right-hand span start (inclusive).
 * @param end   Right-hand span end (exclusive).
 *
 * @return Pointer to the first differing byte in @p self, or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_compare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_compare_range after unpacking @p other (requires valid @p
 * other).
 * @param self  Left-hand view (must be valid).
 * @param other Right-hand view (must be valid).
 *
 * @return Pointer to the first differing byte in @p self, or ::lh_null if all compared bytes match.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_compare(const lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Compare suffixes of @p self and <tt>[begin, end)</tt> from the ends inward (see
 * ::lh_memory_raw_rcompare).
 *
 * Requires ::lh_memory_view_is_valid(@p self).
 *
 * @param self  Valid view (left-hand side).
 * @param begin Right-hand span start (inclusive).
 * @param end   Right-hand span end (exclusive).
 *
 * @return Pointer into @p self at the differing byte in the compared suffix window, or ::lh_null if
 * equal.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rcompare_range(const lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Same as ::lh_memory_view_rcompare_range after unpacking @p other (requires valid @p
 * other).
 * @param self  Left-hand view (must be valid).
 * @param other Right-hand view (must be valid).
 *
 * @return Pointer into @p self at the differing byte in the compared suffix window, or ::lh_null if
 * equal.
 */
LH_ATTRIBUTE_SYMBOL
const lh_ptr
lh_memory_view_rcompare(const lh_memory_view_t *self, const lh_memory_view_t *other);

/* ── bounds validation / convenience ───────────────────────────────────────── */

/**
 * @brief Like ::lh_memory_view_assign but requires ::lh_memory_view_is_valid(@p other).
 * @param self  Destination.
 * @param other Source (must be valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_assign_v(lh_memory_view_t *self, const lh_memory_view_t *other);

/**
 * @brief Build a temporary view from @p begin / @p end and ::lh_memory_view_assign_v.
 *
 * Fails the runtime check if the pair is not a valid view.
 *
 * @param self  Destination.
 * @param begin New @c first.
 * @param end   New @c second.
 */
LH_ATTRIBUTE_SYMBOL
void
lh_memory_view_init_v(lh_memory_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief Like ::lh_memory_view_init_by_size, but on failure clears @p self.
 * @param self  Destination.
 * @param begin Start address.
 * @param size  Length in bytes.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_memory_view_init_by_size_or_clear(lh_memory_view_t *self, const lh_ptr begin, lh_usize_t size);

/**
 * @brief Build a view from @p begin / @p end and return it by value (no validation).
 *
 * @param begin New @c first.
 * @param end   New @c second.
 * @return Constructed view (may be invalid; see ::lh_memory_view_is_valid).
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Return ::lh_memory_view_empty_initializer by value.
 * @return Empty view value.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t lh_memory_view_make_by_empty(lh_void);

/**
 * @brief Build a view from @p begin and @p size and return it by value.
 *
 * Equivalent to constructing the half-open view <tt>[begin, begin + size)</tt>.
 * Uses the same preconditions as ::lh_memory_view_init_by_size:
 * @p begin must be non-null and @p size must be valid for pointer arithmetic.
 *
 * @param begin New @c first.
 * @param size  View length in bytes.
 * @return Constructed view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_by_size(const lh_ptr begin, lh_usize_t size);

/**
 * @brief Build and validate a view from @p begin / @p end, then return it by value.
 *
 * Fails the runtime check if the pair is not a valid view.
 *
 * @param begin New @c first.
 * @param end   New @c second.
 * @return Constructed valid view.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_v(const lh_ptr begin, const lh_ptr end);

/**
 * @brief Like ::lh_memory_view_make_v, but returns empty view on failure.
 *
 * @param begin New @c first.
 * @param end   New @c second.
 * @return Constructed valid view or empty view on failure.
 */
LH_ATTRIBUTE_SYMBOL
lh_memory_view_t
lh_memory_view_make_or_empty(const lh_ptr begin, const lh_ptr end);

LH_COMPILER_EXTERN_C_END

#endif // LH_MEMORY_VIEW_H
