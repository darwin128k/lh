/**
 * @file ptr.h
 * @brief Narrow-string preprocessor helpers (::lh_str_ptr_lit, …)
 *        and low-level C-string scan helpers (::lh_str_ptr_find_of_char, …).
 *
 * Functions ::lh_str_ptr_* take ::lh_str_ptr views;
 * element counts are in ::lh_char_t code units.
 */

#ifndef LH_UTIL_STR_PTR_H
#define LH_UTIL_STR_PTR_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/byte/ptr.h>
#include <lh/cast/static.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/size/limits.h>
#include <lh/str/ptr.h>
#include <lh/util/array/ptr.h>

#ifndef LH_STR_PTR_INVALID
/**
 * @def LH_STR_PTR_INVALID
 * @brief Sentinel value for invalid string index.
 */
#    define LH_STR_PTR_INVALID LH_USIZE_T_MAX
#endif /* LH_STR_PTR_INVALID */

/**
 * @def lh_str_ptr_get_begin(x)
 * @brief Same as ::lh_array_ptr_get_begin(x) for a `char` array @p x.
 *
 * Example usage:
 * @code{.c}
 * char s[] = "ab";
 * char *p = lh_str_ptr_get_begin(s); // same as `s`, points at 'a'
 * @endcode
 */
#define lh_str_ptr_get_begin(x) lh_array_ptr_get_begin(x)

/**
 * @def lh_str_ptr_get_capacity(x)
 * @brief Total number of `char` slots in the array object @p x.
 *
 * Same as ::lh_array_ptr_get_size(x) for element type `char`.
 * For `char s[] = "ab"`, capacity is `3` (`'a'`, `'b'`, `'\0'`).
 * For `char buf[64]`, capacity is `64`.
 *
 * Example usage:
 * @code{.c}
 * char s[] = "ab";
 * // lh_str_ptr_get_capacity(s) == 3
 * @endcode
 */
#define lh_str_ptr_get_capacity(x) lh_array_ptr_get_size(x)

/**
 * @def lh_str_ptr_get_size(x)
 * @brief Payload slot count: `::lh_str_ptr_get_capacity(x) - 1`.
 *
 * Use when one `char` at the end is reserved for `'\0'` (`char buf[N]` gives size `N - 1`),
 * or for the literal body length of `char s[] = "text"` (excluding the stored terminator byte).
 *
 * If the array is larger than the stored string (e.g. `char buf[64] = "hi"`),
 * the result is still `63`, not `strlen(buf)`.
 *
 * @param x `char` array object (not `char *`).
 *
 * @note No guard for `capacity == 0`: the subtraction uses unsigned arithmetic and can wrap;
 *       raw helpers assume the caller uses a suitable array.
 *
 * Example usage:
 * @code{.c}
 * char s[] = "ab";
 * // lh_str_ptr_get_size(s) == 2  // body length; capacity still includes '\0'
 * @endcode
 */
#define lh_str_ptr_get_size(x) ((lh_str_ptr_get_capacity(x)) - 1)

/**
 * @def lh_str_ptr_get_end(x)
 * @brief End of the payload span: `lh_str_ptr_get_begin(x) + lh_str_ptr_get_size(x)`.
 *
 * Points at the slot where the terminating `'\0'`
 * is stored (first byte after the last payload character).
 * The range `[begin, end)` has length ::lh_str_ptr_get_size(x)
 * and does not include the terminator in the half-open interval.
 *
 * This is **not** one past the whole array;
 * for that (e.g. to walk every byte including `'\0'`) use ::lh_array_ptr_get_end(x).
 *
 * Example usage:
 * @code{.c}
 * char s[] = "ab";
 * for (char *p = lh_str_ptr_get_begin(s); p != lh_str_ptr_get_end(s); ++p) { } // 'a', 'b' only
 * // *lh_str_ptr_get_end(s) == '\0'
 * @endcode
 */
#define lh_str_ptr_get_end(x) ((lh_str_ptr_get_begin(x)) + lh_str_ptr_get_size(x))

/**
 * @brief Stringify a token.
 *
 * Converts the given token @p c into
 * a narrow string literal using the `#` operator.
 *
 * @param c Token to stringify.
 * @return Narrow string literal representation of the token.
 */
#define lh_str_ptr_lit(c) #c

/**
 * @brief Stringify variadic tokens.
 *
 * Converts @p __VA_ARGS__ into
 * a narrow string literal.
 *
 * @param ... Tokens to stringify.
 * @return Narrow string literal representation of the tokens.
 */
#define lh_str_ptr_lit_va(...) lh_str_ptr_lit(__VA_ARGS__)

/**
 * @brief Check whether @p x is ::lh_str_ptr or const lh_str_ptr.
 *
 * @param x Expression to inspect.
 * @return `1` if @p x has type ::lh_str_ptr or const lh_str_ptr, `0` otherwise.
 */
#define lh_str_ptr_is(x) _Generic((x), const lh_str_ptr: 1, lh_str_ptr: 1, default: 0)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Write @p ch at @p str[@p pos], unchecked.
 * @return @p pos + 1, where the next character goes.
 */
lh_usize_t
lh_str_ptr_put_char(lh_str_ptr str, lh_usize_t pos, lh_char_t ch);

/**
 * @brief Write the @p n characters of @p chars at @p str from @p pos, unchecked.
 * @return @p pos + @p n.
 */
lh_usize_t
lh_str_ptr_put_chars(lh_str_ptr str, lh_usize_t pos, const lh_str_ptr chars, lh_usize_t n);

/**
 * @brief Write @p n copies of @p ch at @p str from @p pos, unchecked.
 * @return @p pos + @p n.
 */
lh_usize_t
lh_str_ptr_put_run(lh_str_ptr str, lh_usize_t pos, lh_char_t ch, lh_usize_t n);

#ifndef LH_STR_PTR_CHAR_SET_BYTES
/**
 * @def LH_STR_PTR_CHAR_SET_BYTES
 * @brief Size of the bitset filled by ::lh_str_ptr_fill_char_set (256 bits).
 */
#    define LH_STR_PTR_CHAR_SET_BYTES 32U
#endif /* LH_STR_PTR_CHAR_SET_BYTES */

/**
 * @brief Fill a 256-bit membership bitset @p set (32 bytes): bit @c b set iff
 *        byte value @c b appears in @p chars.
 *
 * What find_of_chars / find_not_of_chars (and the reverse ones) test against:
 * membership is one bit test per haystack byte instead of a scan of @p chars.
 *
 * @param set        ::LH_STR_PTR_CHAR_SET_BYTES bytes to fill.
 * @param chars      The characters of the set.
 * @param chars_size Number of characters in @p chars.
 */
lh_void
lh_str_ptr_fill_char_set(lh_byte_t *set, const lh_str_ptr chars, lh_usize_t chars_size);

/**
 * @brief True if byte value of @p ch is set in a map from ::lh_str_ptr_fill_char_set.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_char_set_has(const lh_byte_t *set, lh_char_t ch)
{
    const lh_byte_t b = lh_cast_static(lh_byte_t, ch);
    return lh_cast_static(lh_bool_t,
                          (set[b >> 3] & lh_cast_static(lh_byte_t, 1U << (b & 7U))) != 0);
}

/**
 * @brief Find the first ::lh_char_t equal to @p ch
 *        in the first @p size elements of @p str.
 *
 * @param str Non-null sequence to scan (::lh_runtime_check_ref).
 * @param size Maximum number of characters to inspect.
 * @param ch Code unit to search for.
 *
 * @return Pointer to the first match,
 *         or ::lh_null if @p ch does not occur in that prefix.
 */
const lh_str_ptr
lh_str_ptr_find_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch);

/**
 * @brief Find the last ::lh_char_t equal to @p ch
 *        in the first @p size elements of @p str.
 *
 * @param str  Non-null sequence to scan (::lh_runtime_check_ref).
 * @param size Maximum number of characters to inspect.
 * @param ch   Code unit to search for.
 *
 * @return Pointer to the last match,
 *         or ::lh_null if @p ch does not occur in that prefix.
 *
 * @see lh_str_ptr_find_of_char
 */
const lh_str_ptr
lh_str_ptr_rfind_of_char(const lh_str_ptr str, lh_usize_t size, lh_char_t ch);

/**
 * @brief Find the first null terminator (`'\\0'`) in a NUL-terminated @p str.
 *
 * Page-safe aligned word walk (same rule as ::lh_memory_scan): an aligned
 * machine word never crosses a page boundary. @p str must be NUL-terminated;
 * without a terminator the scan does not return.
 *
 * @param str NUL-terminated string (non-null under runtime checks).
 *
 * @return Pointer to the terminating NUL byte.
 *
 * @see lh_str_ptr_find_of_null_terminator_by_size
 * @see lh_str_ptr_len
 */
const lh_str_ptr
lh_str_ptr_find_of_null_terminator(const lh_str_ptr str);

/**
 * @brief Copy up to @c min(str_size, src_size) characters
 *        from @p src to @p str forward.
 *
 * Sizes are in ::lh_char_t code units (bytes for plain `char`).
 *
 * @param str      Destination buffer.
 * @param str_size Capacity of @p str in characters
 *                 (upper bound on the transfer length).
 * @param src      Source buffer.
 * @param src_size Available length of @p src in characters
 *                 (upper bound on the transfer length).
 *
 * @return Pointer one past the last character written (i.e. @p str + @c n).
 *
 * @see lh_memory_copy
 */
lh_str_ptr
lh_str_ptr_copy(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size);

/**
 * @brief Move up to @c min(str_size, src_size)
 *        characters from @p src to @p str.
 *
 * @see lh_str_ptr_copy
 * @see lh_memory_move
 */
lh_str_ptr
lh_str_ptr_move(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src, lh_usize_t src_size);

/**
 * @brief Fill @p str with a repeating pattern from @p src.
 *
 * @param str      Destination buffer.
 * @param str_size Total number of characters to write in @p str.
 * @param src      Pattern buffer.
 * @param src_size Length of the pattern in characters.
 *
 * @return Pointer one past the last character written
 *         in the destination range.
 *
 * @see lh_memory_set_pattern
 */
lh_str_ptr
lh_str_ptr_set_pattern(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                       lh_usize_t src_size);

/**
 * @brief Set every character in the first @p size elements of @p str to @p ch.
 *
 * @note Implemented with ::lh_str_ptr_set_pattern:
 *       pattern length is 1 character,
 *       repeated to cover @p size code units.
 *
 * @see lh_str_ptr_set_pattern
 * @see lh_memory_set
 */
lh_str_ptr
lh_str_ptr_set(lh_str_ptr str, lh_usize_t size, lh_char_t ch);

/**
 * @brief Compare the first min(@p str_size, @p src_size) characters
 *        of @p str and @p src using ::lh_char_fold_case per code unit.
 *
 * Same idea as ::lh_str_ptr_compare with @c ignore_case ::lh_bool_true,
 * but always uses the folding path. Not full Unicode string case fold for UTF-8.
 *
 * @param str      First buffer (::lh_runtime_check_ref).
 * @param str_size Length of @p str in characters.
 * @param src      Second buffer (::lh_runtime_check_ref).
 * @param src_size Length of @p src in characters.
 *
 * @return Pointer to the first differing character in @p str under folding,
 *         or ::lh_null if all compared positions match.
 *
 * @see lh_str_ptr_compare
 * @see lh_char_fold_case
 */
const lh_str_ptr
lh_str_ptr_compare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                  lh_usize_t src_size);

/**
 * @brief Compare overlapping suffixes
 *        of length min(@p str_size, @p src_size) using ::lh_char_fold_case.
 *
 * Same idea as ::lh_str_ptr_rcompare with @c ignore_case ::lh_bool_true.
 *
 * @param str      First buffer (::lh_runtime_check_ref).
 * @param str_size Length of @p str in characters.
 * @param src      Second buffer (::lh_runtime_check_ref).
 * @param src_size Length of @p src in characters.
 *
 * @return Pointer into @p str at the first suffix mismatch under folding,
 *         or ::lh_null if the window matches.
 *
 * @see lh_str_ptr_rcompare
 * @see lh_char_fold_case
 */
const lh_str_ptr
lh_str_ptr_rcompare_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                   lh_usize_t src_size);

/**
 * @brief Write the terminator at @p index, cutting the C string there.
 *
 * @param str   Target buffer.
 * @param index Offset of the terminator in @p str.
 *
 * @return @p str.
 *
 * @see lh_str_ptr_clear
 */
lh_str_ptr
lh_str_ptr_erase(lh_str_ptr str, lh_usize_t index);

/**
 * @brief Find the first character in @p str that belongs to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 * @return Pointer to the first matching character, or ::lh_null if none found.
 */
const lh_str_ptr
lh_str_ptr_find_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                         lh_usize_t chars_size);

/**
 * @brief Find the last character in @p str that belongs to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 * @return Pointer to the last matching character, or ::lh_null if none found.
 */
const lh_str_ptr
lh_str_ptr_rfind_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                          lh_usize_t chars_size);

/**
 * @brief Find the first character in @p str that does not belong to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 * @return Pointer to the first non-matching character, or ::lh_null if none found.
 */
const lh_str_ptr
lh_str_ptr_find_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                             lh_usize_t chars_size);

/**
 * @brief Find the last character in @p str that does not belong to @p chars.
 *
 * @param str        Haystack buffer (::lh_runtime_check_ref).
 * @param str_size   Length of @p str in characters.
 * @param chars      Character set buffer (::lh_runtime_check_ref).
 * @param chars_size Number of elements in @p chars.
 * @return Pointer to the last non-matching character, or ::lh_null if none found.
 */
const lh_str_ptr
lh_str_ptr_rfind_not_of_chars(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr chars,
                              lh_usize_t chars_size);

/**
 * @brief Find the first null terminator (`'\\0'`)
 *        in the first @p size elements of @p str.
 *
 * @param str String to scan.
 * @param size Maximum number of characters to inspect.
 *
 * @return Pointer to the terminating NUL byte,
 *         or ::lh_null if none was found in that prefix.
 *
 * @see lh_str_ptr_find_of_char
 */
const lh_str_ptr
lh_str_ptr_find_of_null_terminator_by_size(const lh_str_ptr str, lh_usize_t size);

/**
 * @brief Length of @p str in characters,
 *        not counting the terminating NUL.
 *
 * Equivalent to the distance from @p str
 * to the first `\\0` found by ::lh_str_ptr_find_of_null_terminator.
 *
 * @param str NUL-terminated string
 *            within the bounded scan used by ::lh_str_ptr_find_of_null_terminator.
 *
 * @return Number of characters before the terminator.
 *
 * @note If no NUL is found within the scan bound used by ::lh_str_ptr_find_of_null_terminator,
 *       a failed check terminates (::lh_runtime_terminate) with
 *       ::lh_runtime_error_code_no_null_terminator. That and the null check on @p str are
 *       contract checks (::LH_RUNTIME_ASSERT_ENABLED); without them, in Release by default,
 *       a null or unterminated @p str is undefined behaviour.
 *
 * @see lh_str_ptr_find_of_null_terminator
 * @see lh_memory_scan
 * @see lh_runtime_error_code_no_null_terminator
 */
lh_usize_t
lh_str_ptr_len(const lh_str_ptr str);

/**
 * @brief Leftmost substring search in the first @p str_size characters of @p str
 *        (folded byte-wise match).
 *
 * Same idea as ::lh_str_ptr_find with @c ignore_case ::lh_bool_true.
 *
 * @param str      Haystack (::lh_runtime_check_ref).
 * @param str_size Haystack length in characters.
 * @param src      Needle (::lh_runtime_check_ref).
 * @param src_size Needle length in characters.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none.
 *
 * @see lh_str_ptr_find
 * @see lh_char_fold_case
 */
const lh_str_ptr
lh_str_ptr_find_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size);

/**
 * @brief Rightmost substring search with the same folded equality
 *        as ::lh_str_ptr_find_by_ignore_case.
 *
 * Same idea as ::lh_str_ptr_rfind with @c ignore_case ::lh_bool_true.
 *
 * @param str      Haystack (::lh_runtime_check_ref).
 * @param str_size Haystack length in characters.
 * @param src      Needle (::lh_runtime_check_ref).
 * @param src_size Needle length in characters.
 *
 * @return Pointer to the start of the last match, or ::lh_null if none.
 *
 * @see lh_str_ptr_rfind
 * @see lh_char_fold_case
 */
const lh_str_ptr
lh_str_ptr_rfind_by_ignore_case(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                                lh_usize_t src_size);

/**
 * @brief Compare the overlapping prefix of @p str and @p src (forward).
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_compare.
 *                     ::lh_bool_true → ::lh_str_ptr_compare_by_ignore_case.
 *
 * @return Pointer to the first differing character in @p str,
 *         or ::lh_null if the compared prefixes match.
 *
 * @see lh_memory_compare
 * @see lh_str_ptr_compare_by_ignore_case
 */
const lh_str_ptr
lh_str_ptr_compare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                   lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Compare the overlapping suffix of @p str and @p src (from the ends).
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_rcompare.
 *                     ::lh_bool_true → ::lh_str_ptr_rcompare_by_ignore_case.
 *
 * @return Pointer to the first differing character in the suffix window in @p str,
 *         or ::lh_null if that window matches.
 *
 * @see lh_memory_rcompare
 * @see lh_str_ptr_rcompare_by_ignore_case
 */
const lh_str_ptr
lh_str_ptr_rcompare(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                    lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Check whether @p ch exists in the first @p chars_size elements of @p chars.
 *
 * @param chars       Character set buffer.
 * @param chars_size  Number of elements available in @p chars.
 * @param ch          Character to search for.
 *
 * @return ::lh_true if found, ::lh_false otherwise.
 *
 * @see lh_str_ptr_find_of_char
 */
lh_bool_t
lh_str_ptr_contains_char(const lh_str_ptr chars, lh_usize_t chars_size, lh_char_t ch);

/**
 * @brief Make @p str an empty C string (::lh_str_ptr_erase at index 0).
 *
 * @param str NUL-terminated target buffer.
 *
 * @return @p str.
 */
lh_str_ptr
lh_str_ptr_clear(lh_str_ptr str);

/**
 * @brief Find the leftmost position in @p str where a substring matches @p src.
 *
 * @param str          Haystack.
 * @param str_size     Length of @p str in characters.
 * @param src          Needle.
 * @param src_size     Length of @p src in characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_compare.
 *                     ::lh_bool_true → ::lh_str_ptr_find_by_ignore_case.
 *
 * @return Pointer to the start of the first match, or ::lh_null if none.
 *
 * @see lh_memory_find
 * @see lh_str_ptr_find_by_ignore_case
 */
const lh_str_ptr
lh_str_ptr_find(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Find the rightmost occurrence of @p src in @p str.
 *
 * @param str          Haystack.
 * @param str_size     Length of @p str in characters.
 * @param src          Needle.
 * @param src_size     Length of @p src in characters.
 * @param ignore_case  ::lh_bool_false → ::lh_memory_rfind.
 *                     ::lh_bool_true → ::lh_str_ptr_rfind_by_ignore_case.
 *
 * @return Pointer to the start of the last match, or ::lh_null if none.
 *
 * @see lh_str_ptr_find
 * @see lh_memory_rfind
 * @see lh_str_ptr_rfind_by_ignore_case
 */
const lh_str_ptr
lh_str_ptr_rfind(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                 lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Size-bounded prefix compare using ::lh_str_ptr_compare.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in characters.
 * @param src          Prefix buffer.
 * @param src_size     Prefix size in characters.
 * @param ignore_case  ::lh_bool_false → raw bytes (::lh_memory_compare).
 *                     ::lh_bool_true → ::lh_char_fold_case per code unit.
 *
 * @return ::lh_true if the leading @p src_size characters of @p str match @p src,
 *         ::lh_false otherwise.
 *
 * @see lh_str_ptr_compare
 * @see lh_str_ptr_starts_with
 */
lh_bool_t
lh_str_ptr_starts_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                               lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Size-bounded suffix compare using ::lh_str_ptr_rcompare.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in characters.
 * @param src          Suffix buffer.
 * @param src_size     Suffix size in characters.
 * @param ignore_case  ::lh_bool_false → raw bytes (::lh_memory_rcompare).
 *                     ::lh_bool_true → ::lh_str_ptr_rcompare_by_ignore_case.
 *
 * @return ::lh_true if the trailing @p src_size characters of @p str match @p src,
 *         ::lh_false otherwise.
 *
 * @see lh_str_ptr_rcompare
 * @see lh_str_ptr_ends_with
 */
lh_bool_t
lh_str_ptr_ends_with_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                             lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Size-bounded string equality.
 *
 * @param str          First buffer.
 * @param str_size     Length of @p str in characters.
 * @param src          Second buffer.
 * @param src_size     Length of @p src in characters.
 * @param ignore_case  ::lh_bool_false → raw ::lh_char_t equality (::lh_memory_compare path).
 *                     ::lh_bool_true → ::lh_char_fold_case per code unit
 *                     (::lh_str_ptr_compare_by_ignore_case).
 *
 * @return ::lh_true if @p str_size equals @p src_size and
 *         ::lh_str_ptr_compare finds no difference, ::lh_false otherwise.
 *
 * @see lh_str_ptr_equals
 * @see lh_str_ptr_compare
 */
lh_bool_t
lh_str_ptr_equals_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                          lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Copy characters from @p src to @p str, skipping or replacing any
 *        character that occurs in @p except.
 *
 * Stops when @p src is exhausted or @p str is full. Unlike
 * ::lh_str_ptr_copy, the number written can be smaller than both sizes
 * because skipped characters do not occupy destination slots. No NUL is
 * appended.
 *
 * If @p replace is ::lh_null, excepted characters are dropped. If it is
 * non-null, each excepted character is written as @c *replace instead.
 *
 * @param str         Destination buffer.
 * @param str_size    Capacity of @p str in characters.
 * @param src         Source buffer.
 * @param src_size    Available length of @p src in characters.
 * @param except      Character set to skip or replace.
 * @param except_size Number of characters in @p except.
 * @param replace     Optional replacement character, or ::lh_null to drop.
 *
 * @return Pointer one past the last character written.
 *
 * @see lh_str_ptr_copy
 * @see lh_str_ptr_contains_char
 */
lh_str_ptr
lh_str_ptr_copy_except(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                       lh_usize_t src_size, const lh_str_ptr except, lh_usize_t except_size,
                       const lh_char_t *replace);

/**
 * @brief Right trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_rtrim_span
 */
lh_str_ptr
lh_str_ptr_rtrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size);

/**
 * @brief Left trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_ltrim_span
 */
lh_str_ptr
lh_str_ptr_ltrim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
                        lh_usize_t whitespace_size);

/**
 * @brief Size-bounded leftmost substring index.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in characters.
 * @param src          Needle buffer.
 * @param src_size     Needle size in characters.
 * @param ignore_case  Same matching mode as ::lh_str_ptr_find.
 *
 * @return Zero-based index of the first match,
 *         or ::LH_STR_PTR_INVALID if not found.
 */
lh_usize_t
lh_str_ptr_index_of_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief Size-bounded substring search.
 *
 * @param str          Haystack buffer.
 * @param str_size     Haystack size in characters.
 * @param src          Needle buffer.
 * @param src_size     Needle size in characters.
 * @param ignore_case  Same matching mode as ::lh_str_ptr_find.
 *
 * @return ::lh_true if @p src occurs in @p str under the selected rules, ::lh_false otherwise.
 *
 * @see lh_str_ptr_find
 * @see lh_str_ptr_contains
 */
lh_bool_t
lh_str_ptr_contains_by_size(const lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr src,
                            lh_usize_t src_size, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated prefix compare.
 *
 * Walks only the prefix (no strlen). With @p ignore_case ::lh_bool_true, folds
 * each pair via ::lh_char_fold_case.
 *
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Prefix.
 * @param ignore_case  ::lh_bool_false → raw bytes.
 *                     ::lh_bool_true → ::lh_char_fold_case per code unit.
 *
 * @return ::lh_true if the compared prefixes match, ::lh_false otherwise.
 *
 * @see lh_str_ptr_compare
 * @see lh_char_fold_case
 */
lh_bool_t
lh_str_ptr_starts_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated suffix compare using ::lh_str_ptr_rcompare on lengths from
 *        ::lh_str_ptr_len.
 *
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Suffix.
 * @param ignore_case  ::lh_bool_false → raw bytes (::lh_memory_rcompare).
 *                     ::lh_bool_true → ::lh_str_ptr_rcompare_by_ignore_case.
 *
 * @return ::lh_true if the suffix window matches under the selected rules, ::lh_false otherwise.
 *
 * @see lh_str_ptr_rcompare
 * @see lh_str_ptr_rcompare_by_ignore_case
 * @see lh_char_fold_case
 */
lh_bool_t
lh_str_ptr_ends_with(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case);

/**
 * @brief NUL-terminated string equality.
 *
 * With @p ignore_case ::lh_bool_false, ::lh_memory_bytes_eq_cstr (aligned SWAR).
 * With ::lh_bool_true, a paired ::lh_char_fold_case walk (no double strlen).
 * Both arguments must be NUL-terminated.
 *
 * @param str          First string.
 * @param src          Second string.
 * @param ignore_case  ::lh_bool_false → raw ::lh_char_t equality (::lh_memory_compare path).
 *                     ::lh_bool_true → ::lh_char_fold_case per code unit
 *                     (::lh_str_ptr_compare_by_ignore_case).
 *
 * @return ::lh_true if equal under the selected rules, ::lh_false otherwise.
 *
 * @see lh_str_ptr_compare
 * @see lh_str_ptr_compare_by_ignore_case
 * @see lh_char_fold_case
 */
lh_bool_t
lh_str_ptr_equals(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case);

/**
 * @brief Right trim with explicit trim-set size; string size is auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_rtrim_set
 */
lh_str_ptr
lh_str_ptr_rtrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size);

/**
 * @brief Two-sided trim with explicit string and trim-set sizes (custom mode).
 *
 * @param str               Target string buffer.
 * @param str_size          Number of characters in @p str to inspect.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_trim_span
 */
lh_str_ptr
lh_str_ptr_trim_custom(lh_str_ptr str, lh_usize_t str_size, const lh_str_ptr whitespace_chars,
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
 * @see lh_str_ptr_ltrim_set
 */
lh_str_ptr
lh_str_ptr_ltrim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size);

/**
 * @brief NUL-terminated leftmost substring index.
 *
 * With a single-byte needle and @p ignore_case ::lh_bool_false, walks until the
 * match or NUL (no full haystack strlen). Otherwise lengths from ::lh_str_ptr_len
 * then ::lh_str_ptr_find.
 *
 * @param str          Haystack (NUL-terminated).
 * @param src          Needle (NUL-terminated).
 * @param ignore_case  Same matching mode as ::lh_str_ptr_find.
 *
 * @return Zero-based index of the first match,
 *         or ::LH_STR_PTR_INVALID if not found.
 */
lh_usize_t
lh_str_ptr_index_of(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case);

/**
 * @brief Right trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_rtrim_custom
 */
lh_str_ptr
lh_str_ptr_rtrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars);

/**
 * @brief Two-sided trim with explicit trim-set size; string size is auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  Trim character set.
 * @param whitespace_size   Number of elements in @p whitespace_chars.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_trim_set
 */
lh_str_ptr
lh_str_ptr_trim_set(lh_str_ptr str, const lh_str_ptr whitespace_chars, lh_usize_t whitespace_size);

/**
 * @brief Left trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_ltrim_custom
 */
lh_str_ptr
lh_str_ptr_ltrim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars);

/**
 * @brief Left trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_ltrim_custom
 */
lh_str_ptr
lh_str_ptr_ltrim(lh_str_ptr str);

/**
 * @brief Right trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_rtrim_custom
 */
lh_str_ptr
lh_str_ptr_rtrim(lh_str_ptr str);

/**
 * @brief NUL-terminated substring search.
 *
 * Delegates to ::lh_str_ptr_index_of (single-byte needles avoid a full haystack
 * strlen when @p ignore_case is ::lh_bool_false).
 * Both arguments must be NUL-terminated.
 *
 * @param str          Haystack.
 * @param src          Needle.
 * @param ignore_case  ::lh_bool_false → exact bytes (::lh_memory_find).
 *                     ::lh_bool_true → ::lh_str_ptr_find_by_ignore_case.
 *
 * @return ::lh_true if @p src occurs in @p str under the selected rules, ::lh_false otherwise.
 *
 * @see lh_str_ptr_find
 * @see lh_str_ptr_find_by_ignore_case
 * @see lh_char_fold_case
 */
lh_bool_t
lh_str_ptr_contains(const lh_str_ptr str, const lh_str_ptr src, lh_bool_t ignore_case);

/**
 * @brief Two-sided trim with NUL-terminated trim set; both sizes are auto-detected.
 *
 * @param str               NUL-terminated target string.
 * @param whitespace_chars  NUL-terminated trim character set.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_trim_custom
 */
lh_str_ptr
lh_str_ptr_trim_auto(lh_str_ptr str, const lh_str_ptr whitespace_chars);

/**
 * @brief Two-sided trim using the built-in default whitespace set.
 *
 * @param str NUL-terminated target string.
 *
 * @return Pointer to the resulting trimmed string (same address as @p str).
 *
 * @see lh_str_ptr_trim_custom
 */
lh_str_ptr
lh_str_ptr_trim(lh_str_ptr str);

/**
 * @brief Replace each of the first @p size ::lh_char_t elements of @p str
 *        using Unicode simple lowercase mapping for single-byte scalars (0..255),
 *        same subset as ::lh_wstr_ptr_to_lower for Latin-1 style buffers.
 *
 * @note Interprets each code unit as an unsigned byte; not UTF-8 scalar mapping.
 *
 * @param str  Buffer to transform (::lh_runtime_check_ref).
 * @param size Number of ::lh_char_t elements to map.
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 */
lh_str_ptr
lh_str_ptr_to_lower(lh_str_ptr str, lh_usize_t size);

/**
 * @brief Replace each of the first @p size ::lh_char_t elements of @p str
 *        using Unicode simple uppercase mapping for single-byte scalars (0..255).
 *
 * @see lh_str_ptr_to_lower
 */
lh_str_ptr
lh_str_ptr_to_upper(lh_str_ptr str, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_STR_PTR_H */
