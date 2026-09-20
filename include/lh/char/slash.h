/**
 * @file slash.h
 * @brief ASCII solidus (`/`) and reverse solidus (`\`) tests.
 *
 * Path separators go through ::lh_char_is_path_sep, which combines these.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_SLASH_H
#define LH_CHAR_SLASH_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/char/map.h>
#include <lh/compiler/extern/c.h>
#include <lh/compiler/os.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is `/`.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is ::lh_char_map_slash, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_slash(lh_char_t ch)
{
    return (ch == lh_char_map_slash) ? lh_bool_true : lh_bool_false;
}

/**
 * @brief Test whether @p ch is `\`.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is ::lh_char_map_backslash, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_backslash(lh_char_t ch)
{
    return (ch == lh_char_map_backslash) ? lh_bool_true : lh_bool_false;
}

/**
 * @brief Test whether @p ch is a directory separator for the target OS.
 *
 * Windows accepts `/` and `\`; elsewhere only `/`. Only needs
 * ::LH_COMPILER_OS (a compile-time platform fact), not ::LH_LIBRARY_OPTION_OS.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch separates directories on this OS,
 *         otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_path_sep(lh_char_t ch)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return (lh_char_is_slash(ch) || lh_char_is_backslash(ch)) ? lh_bool_true : lh_bool_false;
#else
    return lh_char_is_slash(ch);
#endif
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_SLASH_H */
