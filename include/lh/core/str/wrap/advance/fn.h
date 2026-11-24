/**
 * @file fn.h
 * @brief Function type ::lh_str_wrap_advance_fn.
 */

#ifndef LH_CORE_STR_WRAP_ADVANCE_FN_H
#define LH_CORE_STR_WRAP_ADVANCE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_str_wrap_advance_fn
 * @brief How far the pen moves past @p code: its advance, in whole pixels; `0` for a
 *        code the font has no glyph for.
 *
 * The one thing breaking a text into lines needs from a font: the core knows no
 * font, so whoever breaks the text says how wide a code is — gl from its font.
 *
 * @param context The caller's: the font.
 * @param code    A Unicode code point.
 */
typedef lh_s32_t(lh_str_wrap_advance_fn)(lh_ptr context, lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_STR_WRAP_ADVANCE_FN_H */
