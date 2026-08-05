/**
 * @file view.h
 * @brief Non-owning read-only string view (::lh_str_view_t) and helpers.
 *
 * ::lh_str_view_t is a direct typedef of ::lh_memory_view_t with @c const
 * @c lh_char_t endpoints. The half-open interval semantics are identical:
 * <tt>[first, second)</tt> where @c second is one past the last character.
 *
 * Because the type is layout-compatible with ::lh_memory_view_t, all
 * ::lh_memory_view functions accept an ::lh_str_view_t pointer without a cast.
 * This header exposes string-specific construction, typed @c lh_char_t access,
 * and NUL-terminator helpers whose signatures differ meaningfully from the
 * base memory view API.
 *
 * @see lh_memory_view_t
 * @see lh_str_view_initializer
 */

#ifndef LH_STR_VIEW_H
#define LH_STR_VIEW_H

#include <lh/memory/view.h>
#include <lh/str/ptr.h>
#include <lh/char.h>
#include <lh/size.h>
#include <lh/offset.h>
#include <lh/bool.h>

#ifndef LH_STR_VIEW_INVALID
/**
 * @def LH_STR_VIEW_INVALID
 * @brief Sentinel value returned when a search inside a ::lh_str_view_t fails.
 */
#    define LH_STR_VIEW_INVALID LH_USIZE_T_MAX
#endif /* LH_STR_VIEW_INVALID */

/**
 * @brief Non-owning read-only half-open string view.
 *
 * A direct typedef of ::lh_memory_view_t.
 * Endpoints are expected to point into @c const @c lh_char_t storage.
 * Public typedef: ::lh_str_view_t.
 */
typedef lh_memory_view_t lh_str_view_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- init ------------------------------------------------------------------ */

/**
 * @brief Initialize @p self as a half-open string view starting at @p data.
 *
 * Stores <tt>[data, data + size)</tt>.
 *
 * @param self View to initialize.
 * @param data Pointer to the first character.
 * @param size Number of ::lh_char_t elements in the view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p data is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p size is zero or the computed view is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_init_by_size(lh_str_view_t *self, lh_str_cptr data, lh_usize_t size);

/**
 * @brief Initialize @p self from a NUL-terminated string @p data.
 *
 * The view length is determined by the NUL terminator.
 *
 * @param self View to initialize.
 * @param data NUL-terminated string to view.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_argument
 *        @p data is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_view_init(lh_str_view_t *self, lh_str_cptr data);

/**
 * @brief Initialize @p self with the empty view initializer.
 *
 * @param self View to initialize.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_view_init_empty(lh_str_view_t *self);

/**
 * @brief Initialize @p self as a copy of @p other.
 *
 * @param self  View to initialize.
 * @param other Source view to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_view_init_by_other(lh_str_view_t *self, const lh_str_view_t *other);

/* -- getters --------------------------------------------------------------- */

/**
 * @brief Return @c first as ::lh_str_cptr after validating @p self.
 *
 * @param self Valid view to read.
 * @return Begin pointer cast to ::lh_str_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_view_get_begin(const lh_str_view_t *self);

/**
 * @brief Return @c second as ::lh_str_cptr after validating @p self.
 *
 * @c second is the exclusive end: one past the last character.
 *
 * @param self Valid view to read.
 * @return End pointer cast to ::lh_str_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_view_get_end(const lh_str_view_t *self);

/**
 * @brief Return @c first as ::lh_str_cptr.
 * @param self View to read.
 * @return @c first cast to ::lh_str_cptr.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_view_get_data(const lh_str_view_t *self);

/**
 * @brief Return the number of characters in @p self.
 *
 * For valid views this is @c second @c - @c first.
 *
 * @param self Valid view to read.
 * @return Number of ::lh_char_t elements covered by the half-open interval.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_get_size(const lh_str_view_t *self);

/* -- state ----------------------------------------------------------------- */

/**
 * @brief Return ::lh_bool_true if @p self covers zero characters.
 *
 * @param self View to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_is_empty(const lh_str_view_t *self);

/* -- find ------------------------------------------------------------------ */

/**
 * @brief Find the first occurrence of @p ch in @p self.
 *
 * @param self Valid view to search.
 * @param ch   Character to find.
 * @return Zero-based offset of the first match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_find_char(const lh_str_view_t *self, lh_char_t ch);

/**
 * @brief Find the last occurrence of @p ch in @p self.
 *
 * @param self Valid view to search.
 * @param ch   Character to find.
 * @return Zero-based offset of the last match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_rfind_char(const lh_str_view_t *self, lh_char_t ch);

/**
 * @brief Return ::lh_bool_true if @p ch occurs anywhere in @p self.
 *
 * @param self Valid view to inspect.
 * @param ch   Character to look for.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_contains_char(const lh_str_view_t *self, lh_char_t ch);

/**
 * @brief Find the first occurrence of @p other in @p self.
 *
 * @param self        Haystack view.
 * @param other       Needle view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 * @return Zero-based offset of the first match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_find(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Find the last occurrence of @p other in @p self.
 *
 * @param self        Haystack view.
 * @param other       Needle view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 * @return Zero-based offset of the last match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_rfind(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Return ::lh_bool_true if @p other occurs anywhere in @p self.
 *
 * @param self        Haystack view.
 * @param other       Needle view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_contains(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Find the first character of @p self that belongs to @p chars.
 *
 * @param self  Valid view to search.
 * @param chars Valid view used as the character set.
 * @return Zero-based offset of the first match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p chars is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p chars is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_find_of(const lh_str_view_t *self, const lh_str_view_t *chars);

/**
 * @brief Find the last character of @p self that belongs to @p chars.
 *
 * @param self  Valid view to search.
 * @param chars Valid view used as the character set.
 * @return Zero-based offset of the last match, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p chars is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p chars is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_rfind_of(const lh_str_view_t *self, const lh_str_view_t *chars);

/**
 * @brief Find the first character of @p self that does not belong to @p chars.
 *
 * @param self  Valid view to search.
 * @param chars Valid view used as the character set.
 * @return Zero-based offset of the first non-matching character,
 *         or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p chars is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p chars is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_find_not_of(const lh_str_view_t *self, const lh_str_view_t *chars);

/**
 * @brief Find the last character of @p self that does not belong to @p chars.
 *
 * @param self  Valid view to search.
 * @param chars Valid view used as the character set.
 * @return Zero-based offset of the last non-matching character, or ::LH_STR_VIEW_INVALID if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p chars is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p chars is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_rfind_not_of(const lh_str_view_t *self, const lh_str_view_t *chars);

/* -- compare --------------------------------------------------------------- */

/**
 * @brief Compare the overlapping prefix of @p self and @p other (forward).
 *
 * @param self        First view.
 * @param other       Second view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 * @return Pointer to the first differing character in @p self,
 *         or ::lh_null if the compared prefixes match.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_view_compare(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Compare the overlapping suffix of @p self and @p other (from the ends).
 *
 * @param self        First view.
 * @param other       Second view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 * @return Pointer into @p self at the first suffix mismatch,
 *         or ::lh_null if the suffix window matches.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_str_cptr
lh_str_view_rcompare(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Return ::lh_bool_true if @p self and @p other have equal content.
 *
 * Sizes must match; then the full content is compared under the selected rules.
 *
 * @param self        First view.
 * @param other       Second view.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_equals(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/**
 * @brief Return ::lh_bool_true if @p self begins with @p other.
 *
 * Requires @p self to be at least as long as @p other;
 * then compares the leading @c size(@p other) characters.
 *
 * @param self        View whose prefix is inspected.
 * @param other       Expected prefix.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_starts_with(const lh_str_view_t *self, const lh_str_view_t *other,
                        lh_bool_t ignore_case);

/**
 * @brief Return ::lh_bool_true if @p self ends with @p other.
 *
 * Requires @p self to be at least as long as @p other;
 * then compares the trailing @c size(@p other) characters.
 *
 * @param self        View whose suffix is inspected.
 * @param other       Expected suffix.
 * @param ignore_case ::lh_bool_false for exact bytes, ::lh_bool_true for case-folded match.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self or @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_ends_with(const lh_str_view_t *self, const lh_str_view_t *other, lh_bool_t ignore_case);

/* -- null terminator ------------------------------------------------------- */

/**
 * @brief Find the offset of the first NUL character inside @p self.
 *
 * Returns the zero-based character offset of the first @c '\\0' found inside
 * the view. Returns ::LH_USIZE_T_MAX when no NUL terminator is present.
 *
 * @param self Valid view to scan.
 * @return Zero-based offset of the first NUL, or ::LH_USIZE_T_MAX if absent.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_str_view_find_null_terminator(const lh_str_view_t *self);

/**
 * @brief True if the view contains at least one NUL character.
 *
 * @param self Valid view to inspect.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_str_view_is_null_terminated(const lh_str_view_t *self);

/* -- character access ------------------------------------------------------ */

/**
 * @brief Return the first character of @p self.
 *
 * @param self Valid view to read.
 * @return Character at @c first.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_str_view_get_first_char(const lh_str_view_t *self);

/**
 * @brief Return the last character of @p self.
 *
 * @param self Valid view to read.
 * @return Character at @c second - 1.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_str_view_get_last_char(const lh_str_view_t *self);

/**
 * @brief Return the character at @p offset from @c first.
 *
 * @param self   Valid view to index.
 * @param offset Character offset from the begin endpoint.
 * @return Character at the requested position.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_str_view_get_char_from_begin(const lh_str_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return the character at @p offset from the last character, walking backward.
 *
 * @param self   Valid view to index.
 * @param offset Character offset from the last character.
 * @return Character at the requested position.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_str_view_get_char_from_end(const lh_str_view_t *self, lh_uoffset_t offset);

/**
 * @brief Return the character at signed @p offset.
 *
 * @param self   Valid view to index.
 * @param offset Signed character offset.
 * @return Character at the requested position.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p self is not valid.
 * @throw ::lh_runtime_error_code_out_of_range
 *        @p offset is outside @p self.
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t
lh_str_view_get_char_by_offset(const lh_str_view_t *self, lh_soffset_t offset);

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
lh_str_view_clear(lh_str_view_t *self);

/**
 * @brief Copy endpoints from @p other to @p self.
 *
 * @param self  View to update.
 * @param other Source view to copy from.
 *
 * @throw ::lh_runtime_error_code_null_pointer
 *        @p self or @p other is ::lh_null.
 * @throw ::lh_runtime_error_code_invalid_range
 *        @p other is not valid.
 */
LH_ATTRIBUTE_SYMBOL
lh_void
lh_str_view_assign(lh_str_view_t *self, const lh_str_view_t *other);

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
lh_str_view_swap(lh_str_view_t *self, lh_str_view_t *other);

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
lh_str_view_swap_and_clear(lh_str_view_t *self, lh_str_view_t *other);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_VIEW_H */
