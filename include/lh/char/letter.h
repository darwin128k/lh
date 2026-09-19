/**
 * @file letter.h
 * @brief ASCII letter test (`A`–`Z`, `a`–`z`).
 *
 * Locale-independent, same policy as ::lh_char_is_digit. Drive letters and
 * other callers that mean “Latin alphabetic byte” go through this, not a
 * path-local helper.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_LETTER_H
#define LH_CHAR_LETTER_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is an ASCII letter (`A`–`Z` or `a`–`z`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is a letter, ::lh_bool_false otherwise.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_letter(lh_char_t ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ? lh_bool_true
                                                                  : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_LETTER_H */
