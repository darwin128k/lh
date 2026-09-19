/**
 * @file dot.h
 * @brief NUL-terminated `.` and `..` name tests.
 *
 * Directory listings skip these names. The checks themselves are C-string
 * predicates, not OS/fs helpers.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_UTIL_STR_PTR_DOT_H
#define LH_UTIL_STR_PTR_DOT_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/str/ptr.h>
#include <lh/util/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p name is `.`.
 *
 * @param name NUL-terminated name; must not be null.
 * @return ::lh_bool_true if @p name is `.`, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_is_dot(lh_str_cptr name)
{
    return (lh_ptr_deref(name) == '.' &&
            lh_ptr_deref(lh_ptr_add_by_offset(const lh_char_t, name, 1U)) == '\0')
               ? lh_bool_true
               : lh_bool_false;
}

/**
 * @brief Test whether @p name is `..`.
 *
 * First character must be `.`; the remainder is ::lh_str_ptr_is_dot.
 *
 * @param name NUL-terminated name; must not be null.
 * @return ::lh_bool_true if @p name is `..`, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_str_ptr_is_double_dot(lh_str_cptr name)
{
    return (lh_ptr_deref(name) == '.' &&
            lh_str_ptr_is_dot(lh_ptr_add_by_offset(const lh_char_t, name, 1U)))
               ? lh_bool_true
               : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_UTIL_STR_PTR_DOT_H */
