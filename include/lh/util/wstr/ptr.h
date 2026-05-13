/**
 * @file ptr.h
 * @brief Wide-string preprocessor helpers and low-level wide-character buffer routines.
 *
 * Macros ::lh_wstr_ptr_cat / ::lh_wstr_ptr_lit build wide literals at compile time.
 *
 * Functions ::lh_wstr_ptr_* operate on ::lh_wstr_ptr buffers; sizes are in ::lh_wchar_t elements
 * (see ::LH_WCHAR_T_SIZE when interacting with @c <lh/memory.h>).
 */

#ifndef LH_UTIL_WSTR_PTR_H
#define LH_UTIL_WSTR_PTR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/bool.h>
#include <lh/wstr/ptr.h>
#include <lh/util/array/ptr.h>

#ifndef LH_WSTR_PTR_INVALID
/**
 * @def LH_WSTR_PTR_INVALID
 * @brief Sentinel value for invalid string index.
 */
#    define LH_WSTR_PTR_INVALID LH_USIZE_T_MAX
#endif // LH_WSTR_PTR_INVALID

/**
 * @def lh_wstr_ptr_get_begin(x)
 * @brief Same as ::lh_array_ptr_get_begin(x) for a `wchar_t` array @p x.
 *
 * Example usage:
 * @code{.c}
 * wchar_t s[] = L"ab";
 * wchar_t *p = lh_wstr_ptr_get_begin(s); // same as `s`, points at L'a'
 * @endcode
 */
#define lh_wstr_ptr_get_begin(x) lh_array_ptr_get_begin(x)

/**
 * @def lh_wstr_ptr_get_capacity(x)
 * @brief Total number of `wchar_t` slots in the array object @p x.
 *
 * Same as ::lh_array_ptr_get_size(x) for element type `wchar_t`.
 * For `wchar_t s[] = L"ab"`, capacity is `3` (`L'a'`, `L'b'`, `L'\\0'`).
 * For `wchar_t buf[64]`, capacity is `64`.
 *
 * Example usage:
 * @code{.c}
 * wchar_t s[] = L"ab";
 * // lh_wstr_ptr_get_capacity(s) == 3
 * @endcode
 */
#define lh_wstr_ptr_get_capacity(x) lh_array_ptr_get_size(x)

/**
 * @def lh_wstr_ptr_get_size(x)
 * @brief Payload slot count: `::lh_wstr_ptr_get_capacity(x) - 1`.
 *
 * Use when one `wchar_t` at the end is reserved for `L'\\0'`
 * (`wchar_t buf[N]` gives size `N - 1`), or for the literal body length of
 * `wchar_t s[] = L"text"` (excluding the stored terminator code unit).
 *
 * If the array is larger than the stored string (e.g. `wchar_t buf[64] = L"hi"`),
 * the result is still `63`, not ::lh_wstr_ptr_len(buf).
 *
 * @param x `wchar_t` array object (not `wchar_t *`).
 *
 * @note No guard for `capacity == 0`: the subtraction uses unsigned arithmetic and can wrap;
 *       raw helpers assume the caller uses a suitable array.
 *
 * Example usage:
 * @code{.c}
 * wchar_t s[] = L"ab";
 * // lh_wstr_ptr_get_size(s) == 2  // body length; capacity still includes L'\0'
 * @endcode
 */
#define lh_wstr_ptr_get_size(x) ((lh_wstr_ptr_get_capacity(x)) - 1)

/**
 * @def lh_wstr_ptr_get_end(x)
 * @brief End of the payload span: `lh_wstr_ptr_get_begin(x) + lh_wstr_ptr_get_size(x)`.
 *
 * Points at the slot where the terminating `L'\\0'` is stored
 * (first code unit after the last payload character).
 * The range `[begin, end)` has length ::lh_wstr_ptr_get_size(x)
 * and does not include the terminator in the half-open interval.
 *
 * This is **not** one past the whole array;
 * for that (e.g. to walk every code unit including `L'\\0'`) use ::lh_array_ptr_get_end(x).
 *
 * Example usage:
 * @code{.c}
 * wchar_t s[] = L"ab";
 * for (wchar_t *p = lh_wstr_ptr_get_begin(s);
 *     p != lh_wstr_ptr_get_end(s); ++p) { } // L'a', L'b' only
 * // *lh_wstr_ptr_get_end(s) == L'\0'
 * @endcode
 */
#define lh_wstr_ptr_get_end(x) ((lh_wstr_ptr_get_begin(x)) + lh_wstr_ptr_get_size(x))

/**
 * @brief Preprocessor paste: @c L##c
 *        (wide literal / token per compiler token rules).
 *
 * @param c Token pasted immediately after @c L.
 */
#define lh_wstr_ptr_cat(c) L##c

/**
 * @brief Like ::lh_wstr_ptr_cat for several tokens
 *        (forwards to ::lh_wstr_ptr_cat).
 *
 * @param ... Tokens passed to ::lh_wstr_ptr_cat.
 */
#define lh_wstr_ptr_cat_va(...) lh_wstr_ptr_cat(__VA_ARGS__)

/**
 * @brief Turn a single character token
 *        into a one-element wide string literal (@c L#c).
 *
 * @param c Character token (e.g. @c a → @c L"a").
 */
#define lh_wstr_ptr_lit(c) L#c

/**
 * @brief Like ::lh_wstr_ptr_lit for several tokens
 *        (forwards to ::lh_wstr_ptr_lit).
 *
 * @param ... Tokens passed to ::lh_wstr_ptr_lit.
 */
#define lh_wstr_ptr_lit_va(...) lh_wstr_ptr_lit(__VA_ARGS__)

/**
 * @brief Check whether @p x is ::lh_wstr_ptr or ::lh_wstr_cptr.
 *
 * @param x Expression to inspect.
 * @return `1` if @p x has type ::lh_wstr_ptr or ::lh_wstr_cptr, `0` otherwise.
 */
#define lh_wstr_ptr_is(x) _Generic((x), lh_wstr_cptr: 1, lh_wstr_ptr: 1, default: 0)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Find the first ::lh_wchar_t equal to @p val
 *        in the first @p size elements of @p str.
 *
 * @param str  Non-null sequence to scan (::lh_runtime_check_ref).
 * @param size Maximum number of wide characters to inspect.
 * @param val  Code unit to search for.
 *
 * @return Pointer to the first matching element (::lh_wchar_t-aligned relative to @p str),
 *         or ::lh_null if @p val does not occur in that prefix.
 *
 * @note Uses ::lh_memory_find_step with step ::LH_WCHAR_T_SIZE (aligned grid).
 *       A byte-step search such as ::lh_memory_find (@c step 1)
 *       can match inside a code unit (e.g. UTF-16 LE ASCII high byte).
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_char(const lh_wstr_ptr str, lh_usize_t size, lh_wchar_t val);

/**
 * @brief Find the last ::lh_wchar_t equal to @p val
 *        in the first @p size elements of @p str.
 *
 * @param str  Non-null sequence to scan (::lh_runtime_check_ref).
 * @param size Maximum number of wide characters to inspect.
 * @param val  Code unit to search for.
 *
 * @return Pointer to the last matching element (::lh_wchar_t-aligned relative to @p str),
 *         or ::lh_null if @p val does not occur in that prefix.
 *
 * @see lh_wstr_ptr_find_char
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rfind_char(const lh_wstr_ptr str, lh_usize_t size, lh_wchar_t val);

/**
 * @brief Find the first wide character in @p str that belongs to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in wide characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 *
 * @return Pointer to the first matching wide character, or ::lh_null if none found.
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_of_chars(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr chars,
                          lh_usize_t chars_size);

/**
 * @brief Find the last wide character in @p str that belongs to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in wide characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 *
 * @return Pointer to the last matching wide character, or ::lh_null if none found.
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rfind_of_chars(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr chars,
                           lh_usize_t chars_size);

/**
 * @brief Find the first wide character in @p str that does not belong to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in wide characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 *
 * @return Pointer to the first non-matching wide character, or ::lh_null if none found.
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_not_of_chars(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr chars,
                              lh_usize_t chars_size);

/**
 * @brief Find the last wide character in @p str that does not belong to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in wide characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 *
 * @return Pointer to the last non-matching wide character, or ::lh_null if none found.
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rfind_not_of_chars(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr chars,
                               lh_usize_t chars_size);

/**
 * @brief Find the first wide NUL (@c L'\\0')
 *        in the first @p size elements of @p str.
 *
 * @param str  String to scan.
 * @param size Maximum number of wide characters to inspect.
 *
 * @return Pointer to the terminating wide NUL,
 *         or ::lh_null if none in that prefix.
 *
 * @see lh_wstr_ptr_find_char
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_of_null_terminator_by_size(const lh_wstr_ptr str, lh_usize_t size);

/**
 * @brief Find the first wide NUL in @p str within a bounded scan range.
 *
 * At most @c (::LH_USIZE_T_MAX - ::lh_ptr_to_uaddr(str)) wide characters are read (not bytes).
 * Returns ::lh_null if no NUL appears in that prefix.
 *
 * @param str String to scan (non-null for defined behaviour).
 *
 * @return Pointer to the terminating wide NUL,
 *         or ::lh_null if none in range.
 *
 * @see lh_wstr_ptr_find_of_null_terminator_by_size
 * @see lh_wstr_ptr_find_char
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_of_null_terminator(const lh_wstr_ptr str);

/**
 * @brief Length of @p str in wide characters,
 *        excluding the terminating NUL.
 *
 * Distance in ::lh_wchar_t from @p str to the first
 * wide NUL found by ::lh_wstr_ptr_find_of_null_terminator.
 *
 * @param str NUL-terminated wide string within the bounded scan
 *            used by ::lh_wstr_ptr_find_of_null_terminator.
 *
 * @return Number of ::lh_wchar_t values before the terminator.
 *
 * @note If no wide NUL is found within the scan bound used by
 *       ::lh_wstr_ptr_find_of_null_terminator, ::lh_runtime_check_if_not calls
 *       ::lh_runtime_throw_with_code with ::lh_runtime_error_code_no_null_terminator.
 *
 * @see lh_wstr_ptr_find_of_null_terminator
 * @see lh_runtime_error_code_no_null_terminator
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_ptr_len(const lh_wstr_ptr str);

/**
 * @brief Copy up to min(@p str_size, @p src_size) wide characters from @p src to @p str.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in wide characters (transfer upper bound).
 * @param src      Source buffer.
 * @param src_size Available length of @p src in wide characters (transfer upper bound).
 *
 * @return Pointer one past the last byte written
 *         (same as one past the last ::lh_wchar_t
 *         when the transfer length is a whole number of code units).
 *
 * @see lh_memory_copy
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_copy(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src, lh_usize_t src_size);

/**
 * @brief Move up to min(@p str_size, @p src_size) wide characters from @p src to @p str.
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in wide characters.
 * @param src      Source buffer.
 * @param src_size Available length of @p src in wide characters.
 *
 * @return Pointer one past the last byte written
 *         in the destination (see ::lh_wstr_ptr_copy).
 *
 * @see lh_memory_move
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_move(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src, lh_usize_t src_size);

/**
 * @brief Tile @p str with repeats of the pattern @p src / @p src_size.
 *
 * @param str      Destination buffer.
 * @param str_size Number of wide characters to write in @p str.
 * @param src      Pattern buffer.
 * @param src_size Pattern length in wide characters.
 *
 * @return Pointer one past the last byte written in the destination.
 *
 * @see lh_memory_set_pattern
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_set_pattern(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                        lh_usize_t src_size);

/**
 * @brief Set each of the first @p str_size wide characters of @p str to @p ch.
 *
 * @param str      Destination buffer.
 * @param str_size Number of wide characters to write.
 * @param ch       Value for every @c str[i] (full ::lh_wchar_t, not a byte fill).
 *
 * @return Pointer one past the last byte written
 *         (one past the last ::lh_wchar_t written).
 *
 * @note Implemented via ::lh_wstr_ptr_set_pattern
 *       with a one-code-unit pattern repeated @p str_size times.
 *
 * @see lh_wstr_ptr_set_pattern
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_set(lh_wstr_ptr str, lh_usize_t str_size, lh_wchar_t ch);

/**
 * @brief Replace each of the first @p size wide characters
 *        of @p str with ::lh_wchar_to_lower.
 *
 * In-place; uses Unicode simple lowercase mapping (see ::lh_wchar_to_lower),
 * not ::lh_wchar_fold_case.
 *
 * @param str  Buffer to transform.
 * @param size Number of ::lh_wchar_t elements to map (not bytes).
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 *
 * @see lh_wchar_to_lower
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_to_lower(lh_wstr_ptr str, lh_usize_t size);

/**
 * @brief Replace each of the first @p size wide characters
 *        of @p str with ::lh_wchar_to_upper.
 *
 * @param str  Buffer to transform.
 * @param size Number of ::lh_wchar_t elements to map (not bytes).
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 *
 * @see lh_wchar_to_upper
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_to_upper(lh_wstr_ptr str, lh_usize_t size);

/**
 * @brief Compare the first min(@p str_size, @p src_size) wide characters
 *        of @p str and @p src.
 *
 * Each position is compared after ::lh_wchar_fold_case.
 * Same idea as ::lh_wstr_ptr_compare with @c ignore_case ::lh_bool_true,
 * but always uses the folding path.
 *
 * This is not full Unicode string case folding
 * (multi-code-unit expansions such as ß→ss are omitted);
 *
 * @param str      First buffer (::lh_runtime_check_ref).
 * @param str_size Length of @p str in ::lh_wchar_t elements (not bytes).
 * @param src      Second buffer (::lh_runtime_check_ref).
 * @param src_size Length of @p src in ::lh_wchar_t elements.
 *
 * @return Pointer to the first code unit in @p str where folded values differ,
 *         or ::lh_null if all compared positions match.
 *
 * @see lh_wstr_ptr_compare
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_compare_by_ignore_case(const lh_wstr_ptr str, lh_usize_t str_size,
                                   const lh_wstr_ptr src, lh_usize_t src_size);

/**
 * @brief Compare overlapping suffixes
 *        of length min(@p str_size, @p src_size) using folding.
 *
 * Compares code unit by code unit from the end of that window
 * (same shape as ::lh_memory_rcompare on the byte spans).
 *
 * Same idea as ::lh_wstr_ptr_rcompare
 * with @c ignore_case ::lh_bool_true.
 *
 * @param str      First buffer (::lh_runtime_check_ref).
 * @param str_size Length of @p str in wide characters.
 * @param src      Second buffer (::lh_runtime_check_ref).
 * @param src_size Length of @p src in wide characters.
 *
 * @return Pointer into @p str at the first suffix mismatch under folding,
 *         or ::lh_null if the window matches.
 *
 * @see lh_wstr_ptr_rcompare
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rcompare_by_ignore_case(const lh_wstr_ptr str, lh_usize_t str_size,
                                    const lh_wstr_ptr src, lh_usize_t src_size);

/**
 * @brief Leftmost substring search
 *        in the first @p str_size wide characters of @p str (folded match).
 *
 * Each candidate start is ::lh_wchar_t-aligned relative to @p str (::lh_memory_find_step grid).
 * Same idea as ::lh_wstr_ptr_find with @c ignore_case ::lh_bool_true.
 *
 * @param str      Haystack (::lh_runtime_check_ref).
 * @param str_size Haystack length in wide characters.
 * @param src      Needle (::lh_runtime_check_ref).
 * @param src_size Needle length in wide characters.
 *
 * @return Pointer to the first code unit of the match, or ::lh_null if there is no match (including
 *         when @p str_size is less than @p src_size or either size is zero).
 *
 * @see lh_wstr_ptr_find
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find_by_ignore_case(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                                lh_usize_t src_size);

/**
 * @brief Rightmost substring search
 *        with the same folded equality as ::lh_wstr_ptr_find_by_ignore_case.
 *
 * Same idea as ::lh_wstr_ptr_rfind with @c ignore_case ::lh_bool_true.
 *
 * @param str      Haystack (::lh_runtime_check_ref).
 * @param str_size Haystack length in wide characters.
 * @param src      Needle (::lh_runtime_check_ref).
 * @param src_size Needle length in wide characters.
 *
 * @return Pointer to the first code unit of the last match,
 *         or ::lh_null if there is no match.
 *
 * @see lh_wstr_ptr_rfind
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rfind_by_ignore_case(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                                 lh_usize_t src_size);

/**
 * @brief Find the leftmost occurrence of @p src in @p str (wide-character grid).
 *
 * Sizes are in wide characters;
 * byte lengths are @c str_size ::LH_WCHAR_T_SIZE and @c src_size ::LH_WCHAR_T_SIZE.
 * Starts are stepped by ::LH_WCHAR_T_SIZE (::lh_memory_find_step),
 * not single-byte steps like ::lh_memory_find.
 *
 * @param str          Haystack.
 * @param str_size     Length of @p str in wide characters.
 * @param src          Needle.
 * @param src_size     Length of @p src in wide characters.
 * @param ignore_case  ::lh_bool_false → case-sensitive (::lh_memory_find_step).
 *                     ::lh_bool_true → ::lh_wstr_ptr_find_by_ignore_case.
 *
 * @return Pointer to the first code unit of the match (aligned to @p str),
 *         or ::lh_null if none.
 *
 * @see lh_memory_find_step
 * @see lh_wstr_ptr_find_by_ignore_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_find(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                 lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Find the rightmost occurrence of @p src in @p str
 *        on the same ::lh_wchar_t grid as ::lh_wstr_ptr_find.
 *
 * Implemented with ::lh_memory_rfind_step
 * and step ::LH_WCHAR_T_SIZE.
 *
 * @param str          Haystack.
 * @param str_size     Length of @p str in wide characters.
 * @param src          Needle.
 * @param src_size     Length of @p src in wide characters.
 * @param ignore_case  ::lh_bool_false → case-sensitive step search.
 *                     ::lh_bool_true → ::lh_wstr_ptr_rfind_by_ignore_case.
 *
 * @return Pointer to the first code unit of the last match,
 *         or ::lh_null if none.
 *
 * @see lh_memory_rfind_step
 * @see lh_wstr_ptr_rfind_by_ignore_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rfind(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                  lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Compare prefixes of @p str
 *        and @p src via ::lh_memory_compare on wide byte spans.
 *
 * Byte lengths are @c str_size ::LH_WCHAR_T_SIZE and @c src_size ::LH_WCHAR_T_SIZE
 * (i.e. the first min(@p str_size, @p src_size) code units
 * when both use the same ::lh_wchar_t width).
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in wide characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in wide characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_compare.
 *                     ::lh_bool_true → ::lh_wstr_ptr_compare_by_ignore_case.
 *
 * @return Pointer to the first differing byte in @p str (::lh_memory_compare rules),
 *         or ::lh_null if the compared prefixes match.
 *
 * @see lh_memory_compare
 * @see lh_wstr_ptr_compare_by_ignore_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_compare(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                    lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Compare overlapping suffixes of @p str
 *        and @p src (::lh_memory_rcompare on wide byte spans).
 *
 * Byte lengths are @c str_size ::LH_WCHAR_T_SIZE
 * and @c src_size ::LH_WCHAR_T_SIZE.
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in wide characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in wide characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_rcompare.
 *                     ::lh_bool_true → ::lh_wstr_ptr_rcompare_by_ignore_case.
 *
 * @return Pointer into @p str at the first differing byte in the suffix window,
 *         or ::lh_null if that window matches.
 *
 * @see lh_memory_rcompare
 * @see lh_wstr_ptr_rcompare_by_ignore_case
 */
LH_ATTRIBUTE_SYMBOL
const lh_wstr_ptr
lh_wstr_ptr_rcompare(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                     lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Size-bounded leftmost substring index for wide strings.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in wide characters.
 * @param src          Needle buffer.
 * @param src_size     Needle size in wide characters.
 * @param ignore_case  Same matching mode as ::lh_wstr_ptr_find.
 *
 * @return Zero-based index of the first match,
 *         or ::LH_WSTR_PTR_INVALID if not found.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_ptr_index_of_by_size(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                             lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated leftmost substring index for wide strings.
 *
 * Uses ::lh_wstr_ptr_len on both strings, then ::lh_wstr_ptr_find.
 *
 * @param str          Haystack (NUL-terminated).
 * @param src          Needle (NUL-terminated).
 * @param ignore_case  Same matching mode as ::lh_wstr_ptr_find.
 *
 * @return Zero-based index of the first match,
 *         or ::LH_WSTR_PTR_INVALID if not found.
 */
LH_ATTRIBUTE_SYMBOL
lh_usize_t
lh_wstr_ptr_index_of(const lh_wstr_ptr str, const lh_wstr_ptr src, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated substring search.
 *
 * Uses ::lh_wstr_ptr_len on both arguments,
 * then ::lh_wstr_ptr_find on those lengths.
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Needle.
 * @param ignore_case  ::lh_bool_false → case-sensitive (::lh_memory_find_step on wide spans).
 *                     ::lh_bool_true → folded code units (::lh_wstr_ptr_find_by_ignore_case).
 *
 * @return ::lh_true if @p src occurs in @p str under the selected rules, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_find
 * @see lh_wstr_ptr_find_by_ignore_case
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_contains(const lh_wstr_ptr str, const lh_wstr_ptr src, lh_bool_t ignore_case);

/**
 * @brief Size-bounded substring search.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in wide characters.
 * @param src          Needle buffer.
 * @param src_size     Needle size in wide characters.
 * @param ignore_case  Same matching mode as ::lh_wstr_ptr_find.
 *
 * @return ::lh_true if @p src occurs in @p str under the selected rules, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_find
 * @see lh_wstr_ptr_contains
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_contains_by_size(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                             lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated string equality.
 *
 * Lengths from ::lh_wstr_ptr_len must match.
 * Then ::lh_wstr_ptr_compare is used on the full length.
 * Both arguments must be NUL-terminated.
 *
 * @param str          First string.
 * @param src          Second string.
 * @param ignore_case  ::lh_bool_false → exact ::lh_wchar_t equality (::lh_memory_compare path).
 *                     ::lh_bool_true → pair-wise ::lh_wchar_fold_case
 *                     (::lh_wstr_ptr_compare_by_ignore_case).
 *
 * @return ::lh_true if equal under the selected rules,
 *         ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_compare
 * @see lh_wstr_ptr_compare_by_ignore_case
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_equals(const lh_wstr_ptr str, const lh_wstr_ptr src, lh_bool_t ignore_case);

/**
 * @brief Size-bounded string equality.
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in wide characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in wide characters.
 * @param ignore_case  ::lh_bool_false → exact ::lh_wchar_t equality (::lh_memory_compare path).
 *                     ::lh_bool_true → pair-wise ::lh_wchar_fold_case
 *                     (::lh_wstr_ptr_compare_by_ignore_case).
 *
 * @return ::lh_true if @p str_size equals @p src_size and
 *         ::lh_wstr_ptr_compare finds no difference, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_equals
 * @see lh_wstr_ptr_compare
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_equals_by_size(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                           lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated prefix compare
 *        using ::lh_wstr_ptr_compare on both lengths from ::lh_wstr_ptr_len.
 *
 * Compares the first min(len(@p str),
 * len(@p src)) wide characters (same rule as ::lh_wstr_ptr_compare).
 *
 * When len(@p str) >= len(@p src), this is ::lh_true exactly when @p str starts with @p src.
 * When @p src is longer, ::lh_true means all of @p str matches the start of @p src
 * (not that the full @p src appears at the start of @p str).
 *
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Prefix.
 * @param ignore_case  ::lh_bool_false → raw code units.
 *                     ::lh_bool_true → ::lh_wchar_fold_case per code unit.
 *
 * @return ::lh_true if the compared prefixes match, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_compare
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_starts_with(const lh_wstr_ptr str, const lh_wstr_ptr src, lh_bool_t ignore_case);

/**
 * @brief Size-bounded prefix compare using ::lh_wstr_ptr_compare.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in wide characters.
 * @param src          Prefix buffer.
 * @param src_size     Prefix size in wide characters.
 * @param ignore_case  ::lh_bool_false → raw code units (::lh_memory_compare on wide spans).
 *                     ::lh_bool_true → ::lh_wchar_fold_case per code unit.
 *
 * @return ::lh_true if the leading @p src_size wide characters of @p str match @p src,
 *         ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_compare
 * @see lh_wstr_ptr_starts_with
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_starts_with_by_size(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                                lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated suffix compare using ::lh_wstr_ptr_rcompare on lengths from
 * ::lh_wstr_ptr_len.
 *
 * The overlapping suffix window of length min(len(@p str),
 * len(@p src)) must match (::lh_memory_rcompare shape on wide spans).
 *
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Suffix.
 * @param ignore_case  ::lh_bool_false → raw code units (::lh_memory_rcompare).
 *                     ::lh_bool_true → ::lh_wstr_ptr_rcompare_by_ignore_case.
 *
 * @return ::lh_true if the suffix window matches
 *         under the selected rules, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_rcompare
 * @see lh_wstr_ptr_rcompare_by_ignore_case
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_ends_with(const lh_wstr_ptr str, const lh_wstr_ptr src, lh_bool_t ignore_case);

/**
 * @brief Size-bounded suffix compare using ::lh_wstr_ptr_rcompare.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in wide characters.
 * @param src          Suffix buffer.
 * @param src_size     Suffix size in wide characters.
 * @param ignore_case  ::lh_bool_false → raw code units (::lh_memory_rcompare on wide spans).
 *                     ::lh_bool_true → ::lh_wstr_ptr_rcompare_by_ignore_case.
 *
 * @return ::lh_true if the trailing @p src_size wide characters of @p str match @p src,
 *         ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_rcompare
 * @see lh_wstr_ptr_ends_with
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_ends_with_by_size(const lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr src,
                              lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Check whether @p ch exists in the first @p chars_size elements of @p chars.
 *
 * @param chars       Character set buffer.
 * @param chars_size  Number of elements available in @p chars.
 * @param ch          Wide character to search for.
 *
 * @return ::lh_true if found, ::lh_false otherwise.
 *
 * @see lh_wstr_ptr_find_char
 */
LH_ATTRIBUTE_SYMBOL
lh_bool_t
lh_wstr_ptr_contains_char(const lh_wstr_ptr chars, lh_usize_t chars_size, lh_wchar_t ch);

/**
 * @brief Left trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_ltrim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_ltrim(lh_wstr_ptr str);

/**
 * @brief Right trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_rtrim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_rtrim(lh_wstr_ptr str);

/**
 * @brief Two-sided trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_trim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_trim(lh_wstr_ptr str);

/**
 * @brief Left trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of wide characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_ltrim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_ltrim_custom(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr whitespace_chars,
                         lh_usize_t whitespace_size);

/**
 * @brief Right trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of wide characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_rtrim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_rtrim_custom(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr whitespace_chars,
                         lh_usize_t whitespace_size);

/**
 * @brief Two-sided trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of wide characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_trim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_trim_custom(lh_wstr_ptr str, lh_usize_t str_size, const lh_wstr_ptr whitespace_chars,
                        lh_usize_t whitespace_size);

/**
 * @brief Left trim with explicit trim-set size; string size is auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_ltrim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_ltrim_set(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars,
                      lh_usize_t whitespace_size);

/**
 * @brief Right trim with explicit trim-set size; string size is auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_rtrim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_rtrim_set(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars,
                      lh_usize_t whitespace_size);

/**
 * @brief Two-sided trim with explicit trim-set size; string size is auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_trim_set
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_trim_set(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars,
                     lh_usize_t whitespace_size);

/**
 * @brief Left trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_ltrim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_ltrim_auto(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars);

/**
 * @brief Right trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_rtrim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_rtrim_auto(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars);

/**
 * @brief Two-sided trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_wstr_ptr_trim_custom
 */
LH_ATTRIBUTE_SYMBOL
lh_wstr_ptr
lh_wstr_ptr_trim_auto(lh_wstr_ptr str, const lh_wstr_ptr whitespace_chars);

LH_COMPILER_EXTERN_C_END

#endif // LH_UTIL_WSTR_PTR_H
