/**
 * @file fn.h
 * @brief Function type ::lh_gx_font_get_ascent_fn.
 */

#ifndef LH_GX_FONT_GET_ASCENT_FN_H
#define LH_GX_FONT_GET_ASCENT_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_font_get_ascent_fn
 * @brief How far the baseline is below the top of a line, in pixels.
 *
 * Required.
 *
 * @param context The font's own data.
 * @return Pixels.
 */
typedef lh_s32_t(lh_gx_font_get_ascent_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FONT_GET_ASCENT_FN_H */
