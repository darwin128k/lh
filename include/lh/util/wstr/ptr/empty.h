/**
 * @file empty.h
 * @brief NUL-terminated wide "no text" test that accepts a null pointer.
 *
 * Wide counterpart of `lh/util/str/ptr/empty.h`.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_UTIL_WSTR_PTR_EMPTY_H
#define LH_UTIL_WSTR_PTR_EMPTY_H

#include <lh/attribute/force_inline.h>
#include <lh/char/map.h>
#include <lh/util/wstr/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p text is null or `L""`.
 *
 * @param text NUL-terminated wide text, or ::lh_null.
 * @return ::lh_bool_true if there is no text, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_wstr_ptr_is_empty(const lh_wstr_ptr text)
{
    return (lh_null_eq(text) || lh_ptr_deref(text) == lh_wstr_ptr_cat_va(lh_char_map_nul)) ? lh_bool_true
                                                                             : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_WSTR_PTR_EMPTY_H */
