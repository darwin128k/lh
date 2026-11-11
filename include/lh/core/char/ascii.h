/**
 * @file ascii.h
 * @brief Whether a ::lh_char_t is ASCII (0..127).
 *
 * The code unit is read as an unsigned byte, so the answer does not depend on
 * whether `char` is signed. Callers that split "ASCII, handled inline" from
 * "128..255, handled by a table" (case mapping, case folding) ask this.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_ASCII_H
#define LH_CHAR_ASCII_H

#include <lh/core/attribute/force_inline.h>
#include <lh/core/char/map.h>
#include <lh/core/bool.h>
#include <lh/core/cast/static.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is ASCII: its unsigned byte is at most `DEL` (0x7F).
 *
 * @param ch Character to test.
 * @return ::lh_bool_true for 0..127, ::lh_bool_false for 128..255.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_ascii(lh_char_t ch)
{
    return (lh_cast_static(lh_uchar_t, ch) <= lh_cast_static(lh_uchar_t, lh_char_map_del))
               ? lh_bool_true
               : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_ASCII_H */
