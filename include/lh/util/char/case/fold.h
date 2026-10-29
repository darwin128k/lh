/**
 * @file fold.h
 * @brief Case folding of ::lh_char_t beyond ASCII (the table in case/fold.c).
 */

#ifndef LH_UTIL_CHAR_CASE_FOLD_H
#define LH_UTIL_CHAR_CASE_FOLD_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold a non-ASCII ::lh_char_t (table path). Prefer ::lh_char_fold_case.
 *
 * @see lh_char_fold_case
 */
lh_char_t
lh_char_fold_case_nonascii(lh_char_t c);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_CHAR_CASE_FOLD_H */
