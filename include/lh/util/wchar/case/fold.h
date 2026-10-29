/**
 * @file fold.h
 * @brief Case folding of ::lh_wchar_t for case-insensitive comparison.
 */

#ifndef LH_UTIL_WCHAR_CASE_FOLD_H
#define LH_UTIL_WCHAR_CASE_FOLD_H

#include <lh/compiler/extern/c.h>
#include <lh/wchar.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fold one code unit for case-insensitive comparison
 *        (Unicode CaseFolding subset).
 *
 * Uses a sorted table from UCD CaseFolding:
 *   status C, S, and single-code-point F only;
 *   status T omitted.
 *
 * Code points that fold to multiple scalars (e.g. full fold of ß)
 * are not represented — @p c is returned unchanged for those.
 *
 * @note Comparing folded ::lh_wchar_t units is not full UTF-16 semantic
 *       case fold for supplementary characters (surrogate pairs).
 *
 *       Works per code unit,
 *       like the other ::lh_wstr_ptr_* helpers.
 *
 * @see lh_wchar_to_lower
 * @see lh_wchar_to_upper
 */
lh_wchar_t
lh_wchar_fold_case(lh_wchar_t c);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_WCHAR_CASE_FOLD_H */
