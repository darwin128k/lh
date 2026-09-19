/**
 * @file dot.h
 * @brief ASCII full stop (`.`) test.
 *
 * Locale-independent. String `.` / `..` names go through
 * `lh/util/str/ptr/dot.h`; this is the one-byte kernel.
 *
 * Every function here is ::LH_ATTRIBUTE_FORCE_INLINE.
 */

#ifndef LH_CHAR_DOT_H
#define LH_CHAR_DOT_H

#include <lh/attribute/force_inline.h>
#include <lh/bool.h>
#include <lh/char.h>
#include <lh/char/map.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Test whether @p ch is `.`.
 *
 * @param ch Character to test.
 * @return ::lh_bool_true if @p ch is ::lh_char_map_period, otherwise ::lh_bool_false.
 */
LH_ATTRIBUTE_FORCE_INLINE
lh_bool_t
lh_char_is_dot(lh_char_t ch)
{
    return (ch == lh_char_map_period) ? lh_bool_true : lh_bool_false;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_CHAR_DOT_H */
