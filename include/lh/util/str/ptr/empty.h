/**
 * @file empty.h
 * @brief NUL-terminated "no text" test that accepts a null pointer.
 *
 * OS calls report a missing name either way: no pointer at all, or a
 * pointer to an empty string (e.g. the main program's `link_map` name).
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_UTIL_STR_PTR_EMPTY_H
#define LH_UTIL_STR_PTR_EMPTY_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char/map.h>
#include <lh/compiler/extern/c.h>
#include <lh/null.h>
#include <lh/str/ptr.h>
#include <lh/util/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p text is null or `""`.
 *
 * @param text NUL-terminated text, or ::lh_null.
 * @return ::lh_bool_true if there is no text, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_is_empty(lh_str_cptr text)
{
    return (lh_null_eq(text) || lh_ptr_deref(text) == lh_char_map_nul) ? lh_bool_true : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_STR_PTR_EMPTY_H */
