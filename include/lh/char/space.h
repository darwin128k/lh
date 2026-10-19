/**
 * @file space.h
 * @brief ASCII whitespace test (space, `\t`, `\n`, `\v`, `\f`, `\r`).
 *
 * Locale-independent, same policy as ::lh_char_is_digit: the characters C
 * `isspace` accepts in the "C" locale. The same set, as an array, is what
 * every trim in lh strips (::lh_str_ptr_get_whitespace).
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_SPACE_H
#define LH_CHAR_SPACE_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/char/map.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is ASCII whitespace.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true for space, `\t`, `\n`, `\v`, `\f` or `\r`,
 *         otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_whitespace(lh_char_t ch)
{
    return (ch == lh_char_map_space || (ch >= lh_char_map_ht && ch <= lh_char_map_cr))
               ? lh_bool_true
               : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_SPACE_H */
