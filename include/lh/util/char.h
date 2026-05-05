/**
 * @file char.h
 * @brief Character utility functions and macros.
 *
 * Provides macros and functions for character manipulation,
 * including case folding for ASCII characters.
 *
 * @see lh_char_t
 * @see lh_wchar_fold_case
 */

#ifndef LH_UTIL_CHAR_H
#define LH_UTIL_CHAR_H

#include <lh/cast/static.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/attribute/symbol.h>

#define lh_char_ord_to(T, c) lh_cast_static(T, c)
#define lh_char_ord(c) lh_char_ord_to(lh_sint_t, c)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold one ::lh_char_t for case-insensitive comparison
 *        (Unicode CaseFolding subset, single-byte scalars only).
 *
 * Same policy as ::lh_wchar_fold_case in @c wchar/case/fold.c,
 * restricted to table rows where both code points fit in 0..255.
 * Per code unit; not UTF-8 scalar folding.
 *
 * @see lh_wchar_fold_case
 */
LH_ATTRIBUTE_SYMBOL
lh_char_t lh_char_fold_case(lh_char_t c);

LH_COMPILER_EXTERN_C_END

#endif // LH_UTIL_CHAR_H
