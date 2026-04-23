/**
 * @file view.h
 * @brief String view as alias of ::lh_memory_view_t over character data.
 */

#ifndef LH_STR_VIEW_H
#define LH_STR_VIEW_H

#include <lh/memory/view.h>
#include <lh/str/ptr.h>

/**
 * @typedef lh_str_view_t
 * @brief Non-owning character range: same representation as ::lh_memory_view_t.
 *
 * Endpoints @c first and @c second are typically
 * @c const @c lh_schar_t pointers into existing storage.
 */
typedef lh_memory_view_t lh_str_view_t;

LH_COMPILER(EXTERN_C_BEGIN)

/* ── pack / unpack / init ─────────────────────────────────────────────────── */

/**
 * @brief Partially update @p self from optional begin/end pointers.
 *
 * Pass ::lh_null for @p begin or @p end to keep the corresponding endpoint unchanged.
 *
 * @param self  View to modify.
 * @param begin Optional input for the @c first endpoint (inclusive begin).
 * @param end   Optional input for the @c second endpoint (exclusive end).
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_pack(lh_str_view_t *self, const lh_str_ptr *begin, const lh_str_ptr *end);

/**
 * @brief Read begin/end endpoints from @p self into optional outputs.
 *
 * Requires a valid view and delegates to ::lh_memory_view_unpack_v.
 * Pass ::lh_null for any output pointer you want to skip.
 *
 * @param self  Source view.
 * @param begin Optional output for begin.
 * @param end   Optional output for end.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_unpack(const lh_str_view_t *self, const lh_str_ptr *begin,
                           const lh_str_ptr *end);

/**
 * @brief Store @p begin/@p end into @p self as half-open bounds `[begin, end)`.
 * @param self  View to modify.
 * @param begin Inclusive begin pointer.
 * @param end   Exclusive end pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_init(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end);

/**
 * @brief Initialize @p self from begin pointer and element count.
 *
 * Equivalent to `[begin, begin + size)`.
 *
 * @param self  View to modify.
 * @param begin Inclusive begin pointer.
 * @param size  Number of characters in the span.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_init_by_size(lh_str_view_t *self, const lh_str_ptr begin, lh_usize_t size);

/**
 * @brief Reset @p self to the uninitialized empty pattern (`null`, `null`).
 * @param self View to clear.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_init_by_empty(lh_str_view_t *self);

/**
 * @brief Copy bounds from @p other into @p self.
 * @param self  Destination view.
 * @param other Source view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_init_by_other(lh_str_view_t *self, const lh_str_view_t *other);

/* ── basic getters / modifiers ────────────────────────────────────────────── */

/**
 * @brief Return stored begin pointer.
 * @param self View to read.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_str_view_get_begin(const lh_str_view_t *self);

/**
 * @brief Return stored end pointer.
 * @param self View to read.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_ptr lh_str_view_get_end(const lh_str_view_t *self);

/**
 * @brief Assign @p other into @p self with validation of @p other.
 * @param self  Destination view.
 * @param other Source view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_assign(lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief Clear @p self to the uninitialized empty pattern.
 * @param self View to clear.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_clear(lh_str_view_t *self);

/**
 * @brief Swap two string views.
 * @param self  First view.
 * @param other Second view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_swap(lh_str_view_t *self, lh_str_view_t *other);

/**
 * @brief Clear @p self then swap with @p other.
 * @param self  View to replace.
 * @param other View to exchange with.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_exchange(lh_str_view_t *self, lh_str_view_t *other);

/**
 * @brief True iff @p self is an empty valid span (`begin == end` and both non-null).
 * @param self View to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_view_is_empty(const lh_str_view_t *self);

/* ── constructors / slicing / indexing ───────────────────────────────────── */

/**
 * @brief Build and validate a string view from explicit bounds.
 * @param begin Inclusive begin pointer.
 * @param end   Exclusive end pointer.
 * @return Constructed valid view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_str_view_t lh_str_make(const lh_str_ptr begin, const lh_str_ptr end);

/**
 * @brief Build a string view from begin pointer and size.
 * @param begin Inclusive begin pointer.
 * @param size  Number of characters in the view.
 * @return Constructed view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_str_view_t lh_str_make_by_size(const lh_str_ptr begin, lh_usize_t size);

/**
 * @brief Check whether subview `[offset, offset + size)` can be formed from @p self.
 * @param self   Source view.
 * @param offset Start offset from front.
 * @param size   Requested subview length.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_is_sliceable(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size);

/**
 * @brief Return validated subview `[offset, offset + size)`.
 * @param self   Source view.
 * @param offset Start offset from front.
 * @param size   Requested subview length.
 */
LH_ATTRIBUTE(SYMBOL)
lh_str_view_t lh_str_slice(const lh_str_view_t *self, lh_uoffset_t offset, lh_usize_t size);

/**
 * @brief Byte length of the view (half-open span), like @c std::span::size_bytes.
 *
 * Same as ::lh_memory_view_get_size: @c second @c - @c first in bytes.
 * Includes every byte in <tt>[begin, end)</tt>; no special case for @c '\\0'.
 *
 * @param self View to inspect.
 *
 * @see lh_str_get_size for the same span in @c lh_char_t units.
 */
LH_ATTRIBUTE(SYMBOL)
lh_usize_t lh_str_get_size_bytes(const lh_str_view_t *self);

/**
 * @brief Character count for @p self from stored bounds.
 *
 * Returns @c lh_str_get_size_bytes(@p self) / @c sizeof(::lh_char_t): the half-open span
 * <tt>[begin, end)</tt> measured in @c lh_char_t, analogous to @c std::string_view::size.
 * Does not scan for a terminating @c '\\0'.
 *
 * @note The divisor is @c sizeof(::lh_char_t), which is never zero. A byte length of zero
 *       yields zero characters (divide @c 0 by the element size, not the reverse).
 *
 * @note For the length of a C string (code units before the first @c '\\0'), use
 *       ::lh_str_raw_len on a suitable pointer when NUL termination is guaranteed; that value
 *       may differ from this function when the span is not NUL-terminated or contains
 *       embedded nulls.
 *
 * @param self View to inspect.
 */
LH_ATTRIBUTE(SYMBOL)
lh_usize_t lh_str_get_size(const lh_str_view_t *self);

/**
 * @brief Return pointer at @p offset from front or back.
 * @param self    Source view.
 * @param offset  Zero-based offset.
 * @param is_back If true, offset is interpreted from the back.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_get_ptr(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t is_back);

/**
 * @brief Return character value at @p offset from front or back.
 * @param self    Source view.
 * @param offset  Zero-based offset.
 * @param is_back If true, offset is interpreted from the back.
 */
LH_ATTRIBUTE(SYMBOL)
lh_char_t lh_str_get_value(const lh_str_view_t *self, lh_uoffset_t offset, lh_bool_t is_back);

/**
 * @brief Pointer to first character.
 * @param self Source view.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_get_front_ptr(const lh_str_view_t *self);

/**
 * @brief Pointer to last character.
 * @param self Source non-empty view.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_get_back_ptr(const lh_str_view_t *self);

/**
 * @brief First character.
 * @param self Source view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_char_t lh_str_get_front(const lh_str_view_t *self);

/**
 * @brief Last character.
 * @param self Source non-empty view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_char_t lh_str_get_back(const lh_str_view_t *self);

/**
 * @brief Return next character pointer after @p ptr within @p self.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the last character in the half-open span.
 *
 * @param self String view to iterate.
 * @param ptr  Current character pointer.
 * @return Next character pointer in view, or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_next_ptr(const lh_str_view_t *self, const lh_str_ptr ptr);

/**
 * @brief Return previous character pointer before @p ptr within @p self.
 *
 * Returns ::lh_null when @p ptr is outside @p self or when @p ptr already points
 * to the first character in the half-open span.
 *
 * @param self String view to iterate.
 * @param ptr  Current character pointer.
 * @return Previous character pointer in view, or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_prev_ptr(const lh_str_view_t *self, const lh_str_ptr ptr);

/**
 * @brief Return character at next pointer after @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_str_next_ptr. Fails runtime check when next
 * character does not exist.
 *
 * @param self String view to iterate.
 * @param ptr  Current character pointer.
 * @return Character at next position.
 */
LH_ATTRIBUTE(SYMBOL)
lh_char_t lh_str_next_value(const lh_str_view_t *self, const lh_str_ptr ptr);

/**
 * @brief Return character at previous pointer before @p ptr within @p self.
 *
 * Equivalent to dereferencing ::lh_str_prev_ptr. Fails runtime check when previous
 * character does not exist.
 *
 * @param self String view to iterate.
 * @param ptr  Current character pointer.
 * @return Character at previous position.
 */
LH_ATTRIBUTE(SYMBOL)
lh_char_t lh_str_prev_value(const lh_str_view_t *self, const lh_str_ptr ptr);

/* ── relations ─────────────────────────────────────────────────────────────── */

/**
 * @brief True iff @p ptr belongs to half-open span `[begin, end)`.
 * @param self View to test.
 * @param ptr  Pointer to check.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_contains_ptr(const lh_str_view_t *self, const lh_str_ptr ptr);

/**
 * @brief True iff range `[begin, end)` is contained in @p self.
 * @param self  Outer view.
 * @param begin Inner range begin.
 * @param end   Inner range end.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_contains_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                const lh_str_ptr end);

/**
 * @brief True iff @p other is fully contained in @p self.
 * @param self  Outer view.
 * @param other Inner view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_contains(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief True iff bounds of @p self equal explicit bounds.
 * @param self  View to compare.
 * @param begin Expected begin.
 * @param end   Expected end.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_equals_range(const lh_str_view_t *self, const lh_ptr begin, const lh_ptr end);

/**
 * @brief True iff bounds of @p self and @p other are equal.
 * @param self  First view.
 * @param other Second view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_equals(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief True iff @p self overlaps explicit range `[begin, end)`.
 * @param self  First view.
 * @param begin Second range begin.
 * @param end   Second range end.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_overlaps_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                const lh_str_ptr end);

/**
 * @brief True iff @p self and @p other overlap.
 * @param self  First view.
 * @param other Second view.
 */
LH_ATTRIBUTE(SYMBOL)
lh_bool_t lh_str_overlaps(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief Set explicit bounds for @p self.
 * @param self  View to modify.
 * @param begin Inclusive begin pointer.
 * @param end   Exclusive end pointer.
 */
LH_ATTRIBUTE(SYMBOL)
lh_void lh_str_view_set(lh_str_view_t *self, const lh_str_ptr begin, const lh_str_ptr end);

/* ── search / compare ─────────────────────────────────────────────────────── */

/**
 * @brief Find first occurrence of explicit range in @p self.
 * @param self  Haystack view.
 * @param begin Needle begin.
 * @param end   Needle end.
 * @return Pointer to first match or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_find_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                   const lh_str_ptr end);

/**
 * @brief Find first occurrence of @p other in @p self.
 * @param self  Haystack view.
 * @param other Needle view.
 * @return Pointer to first match or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_find(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief Find last occurrence of explicit range in @p self.
 * @param self  Haystack view.
 * @param begin Needle begin.
 * @param end   Needle end.
 * @return Pointer to last match or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_rfind_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                    const lh_str_ptr end);

/**
 * @brief Find last occurrence of @p other in @p self.
 * @param self  Haystack view.
 * @param other Needle view.
 * @return Pointer to last match or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_rfind(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief Forward-compare @p self against explicit range.
 * @param self  Left-hand view.
 * @param begin Right-hand range begin.
 * @param end   Right-hand range end.
 * @return Pointer to first differing byte in @p self or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_compare_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                      const lh_str_ptr end);

/**
 * @brief Forward-compare @p self against @p other.
 * @param self  Left-hand view.
 * @param other Right-hand view.
 * @return Pointer to first differing byte in @p self or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_compare(const lh_str_view_t *self, const lh_str_view_t *other);

/**
 * @brief Reverse-compare suffixes of @p self and explicit range.
 * @param self  Left-hand view.
 * @param begin Right-hand range begin.
 * @param end   Right-hand range end.
 * @return Pointer in @p self to differing suffix byte or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_rcompare_range(const lh_str_view_t *self, const lh_str_ptr begin,
                                       const lh_str_ptr end);

/**
 * @brief Reverse-compare suffixes of @p self and @p other.
 * @param self  Left-hand view.
 * @param other Right-hand view.
 * @return Pointer in @p self to differing suffix byte or ::lh_null.
 */
LH_ATTRIBUTE(SYMBOL)
const lh_str_ptr lh_str_rcompare(const lh_str_view_t *self, const lh_str_view_t *other);

LH_COMPILER(EXTERN_C_END)

#endif // LH_STR_VIEW_H