/**
 * @file fn.h
 * @brief Function type ::lh_gx_font_get_glyph_fn.
 */

#ifndef LH_GX_FONT_GET_GLYPH_FN_H
#define LH_GX_FONT_GET_GLYPH_FN_H

#include <lh/gx/glyph.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_font_get_glyph_fn
 * @brief The glyph of @p code, into @p out.
 *
 * Required. A code the font has no glyph for answers false and leaves @p out
 * alone: the text skips it and the pen does not move.
 *
 * @param context The font's own data.
 * @param code    The code point.
 * @param out     Receives the glyph; must not be null.
 * @return True when the font has a glyph for @p code.
 */
typedef lh_bool_t(lh_gx_font_get_glyph_fn)(lh_ptr context, lh_u32_t code, lh_gx_glyph_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FONT_GET_GLYPH_FN_H */
