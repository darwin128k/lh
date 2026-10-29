/**
 * @file map.h
 * @brief Lower and upper case mapping of ::lh_wchar_t buffers in place.
 */

#ifndef LH_UTIL_WSTR_CASE_MAP_H
#define LH_UTIL_WSTR_CASE_MAP_H

#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/wstr/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Replace each of the first @p size wide characters
 *        of @p str with ::lh_wchar_to_lower.
 *
 * In-place; uses Unicode simple lowercase mapping (see ::lh_wchar_to_lower),
 * not ::lh_wchar_fold_case.
 *
 * @param str  Buffer to transform.
 * @param size Number of ::lh_wchar_t elements to map (not bytes).
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 *
 * @see lh_wchar_to_lower
 */
lh_wstr_ptr
lh_wstr_ptr_to_lower(lh_wstr_ptr str, lh_usize_t size);

/**
 * @brief Replace each of the first @p size wide characters
 *        of @p str with ::lh_wchar_to_upper.
 *
 * @param str  Buffer to transform.
 * @param size Number of ::lh_wchar_t elements to map (not bytes).
 *
 * @return Pointer one past the last element updated (@p str + @p size).
 *
 * @see lh_wchar_to_upper
 */
lh_wstr_ptr
lh_wstr_ptr_to_upper(lh_wstr_ptr str, lh_usize_t size);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_WSTR_CASE_MAP_H */
