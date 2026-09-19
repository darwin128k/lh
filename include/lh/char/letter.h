/**
 * @file letter.h
 * @brief ASCII letter tests (`A`–`Z`, `a`–`z`).
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
 * @brief Test whether @p ch is an ASCII uppercase letter (`A`–`Z`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is uppercase, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_upper(lh_char_t ch)
{
    return (ch >= 'A' && ch <= 'Z') ? lh_bool_true : lh_bool_false;
}

/**
 * @brief Test whether @p ch is an ASCII lowercase letter (`a`–`z`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is lowercase, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_lower(lh_char_t ch)
{
    return (ch >= 'a' && ch <= 'z') ? lh_bool_true : lh_bool_false;
}

/**
 * @brief Test whether @p ch is an ASCII letter (`A`–`Z` or `a`–`z`).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is a letter, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_letter(lh_char_t ch)
{
    return (lh_char_is_upper(ch) || lh_char_is_lower(ch)) ? lh_bool_true : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_LETTER_H */
