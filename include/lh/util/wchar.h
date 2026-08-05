/**
 * @file wchar.h
 * @brief Wide-character utilities: ordinal casts and Unicode case (fold + simple map).
 *
 * Declarations live in ::lh/wchar/case/fold.h and ::lh/wchar/case/map.h;
 * this header pulls them in together with the ordinal helpers.
 */

#ifndef LH_UTIL_WCHAR_H
#define LH_UTIL_WCHAR_H

#include <lh/attribute/symbol.h>
#include <lh/compiler/extern/c.h>
#include <lh/wchar.h>
#include <lh/cast/static.h>

/**
 * @def lh_wchar_ord_to(T, c)
 * @brief Map wide code unit @p c to integer type @p T (::lh_cast_static).
 */
#define lh_wchar_ord_to(T, c) lh_cast_static(T, c)

/**
 * @def lh_wchar_ord(c)
 * @brief Wide code unit as ::lh_sint_t (same pattern as ::lh_char_ord).
 */
#define lh_wchar_ord(c) lh_wchar_ord_to(lh_sint_t, c)

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
LH_ATTRIBUTE_SYMBOL
lh_wchar_t
lh_wchar_fold_case(lh_wchar_t c);

/**
 * @brief Simple lowercase mapping for one code unit (Unicode UCD, field 13).
 *
 * Uses a sorted table from @c UnicodeData.txt (simple lowercase only).
 * Not the same as ::lh_wchar_fold_case:
 *   folding follows @c CaseFolding.txt for comparison (e.g. some mappings differ).
 *
 * Code points without a simple lowercase mapping
 * (including those that map to multiple scalars)
 * are returned unchanged.
 *
 * Per-code-unit only;
 * not full UTF-16 string lowercasing for supplementary planes (surrogate pairs).
 */
LH_ATTRIBUTE_SYMBOL
lh_wchar_t
lh_wchar_to_lower(lh_wchar_t c);

/**
 * @brief Simple uppercase mapping for one code unit (Unicode UCD, field 12).
 *
 * Same data source and limitations as ::lh_wchar_to_lower.
 */
LH_ATTRIBUTE_SYMBOL
lh_wchar_t
lh_wchar_to_upper(lh_wchar_t c);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_WCHAR_H */
