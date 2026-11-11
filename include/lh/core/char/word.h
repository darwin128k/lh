/**
 * @file word.h
 * @brief ASCII word character test (`A`-`Z`, `a`-`z`, `0`-`9`, `_`).
 *
 * Locale-independent, the same set as `\w` in a regular expression: what an
 * identifier, a placeholder name or a file-safe tag is made of.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_WORD_H
#define LH_CHAR_WORD_H

#include <lh/core/char/digit.h>
#include <lh/core/char/letter.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is a word character: an ASCII letter, a digit or `_`.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is a word character, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_word(lh_char_t ch)
{
    return (lh_char_is_letter(ch) || lh_char_is_digit(ch) || ch == lh_char_map_underscore)
               ? lh_bool_true
               : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_WORD_H */
