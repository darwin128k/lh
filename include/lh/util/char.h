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

#include <lh/attribute/force_inline.h>
#include <lh/cast/static.h>
#include <lh/util/char/case/fold.h>

/** @brief Code of character @p c as type @p T. */
#define lh_char_ord_to(T, c) lh_cast_static(T, c)
/** @brief Code of character @p c as ::lh_sint_t. */
#define lh_char_ord(c) lh_char_ord_to(lh_sint_t, c)

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold one ::lh_char_t for case-insensitive comparison
 *        (Unicode CaseFolding subset, single-byte scalars only).
 *
 * ASCII A-Z is folded inline; bytes 128..255 use ::lh_char_fold_case_nonascii.
 * Same policy as ::lh_wchar_fold_case in @c wchar/case/fold.c,
 * restricted to table rows where both code points fit in 0..255.
 * Per code unit; not UTF-8 scalar folding.
 *
 * @see lh_wchar_fold_case
 * @see lh_char_fold_case_nonascii
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_char_t
lh_char_fold_case(lh_char_t c)
{
    const lh_uchar_t u = lh_cast_static(lh_uchar_t, c);

    if (u < 128U)
    {
        if (u >= 65U && u <= 90U)
        {
            return lh_cast_static(lh_char_t, u + 32U);
        }
        return c;
    }
    return lh_char_fold_case_nonascii(c);
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_CHAR_H */
