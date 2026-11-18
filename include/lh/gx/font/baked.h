/**
 * @file baked.h
 * @brief A font of baked glyph tables: the font contract over ::lh_font_t.
 *
 * The core's ::lh_font_t is a bitmap font as data — cropped masks, advances, tops
 * and the runs of code points they cover, baked from a TTF by `scripts/font.py`
 * (Roboto ships that way). These calls are the slots of ::lh_gx_font_t over such
 * a table; ::lh_gx_font_baked_init makes a gx font of one. The table is the
 * font's context and is not owned.
 */

#ifndef LH_GX_FONT_BAKED_H
#define LH_GX_FONT_BAKED_H

#include <lh/core/font.h>
#include <lh/gx/font.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief `get_glyph` over a baked table (::lh_gx_font_get_glyph_fn).
 *
 * @param context The table (::lh_font_t).
 * @param code    The code point.
 * @param out     Receives the glyph.
 * @return True when a run of the table holds @p code.
 */
lh_bool_t
lh_gx_font_baked_get_glyph(lh_ptr context, lh_u32_t code, lh_gx_glyph_t *out);

/**
 * @brief `get_ascent` over a baked table (::lh_gx_font_get_ascent_fn).
 *
 * @param context The table (::lh_font_t).
 * @return Pixels.
 */
lh_s32_t
lh_gx_font_baked_get_ascent(lh_ptr context);

/**
 * @brief `get_line_height` over a baked table (::lh_gx_font_get_line_height_fn).
 *
 * @param context The table (::lh_font_t).
 * @return Pixels.
 */
lh_s32_t
lh_gx_font_baked_get_line_height(lh_ptr context);

/**
 * @brief A gx font over the baked @p table.
 *
 * @param self  The font; must not be null.
 * @param table The baked table; not owned, it must outlive the font.
 */
lh_void
lh_gx_font_baked_init(lh_gx_font_t *self, const lh_font_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FONT_BAKED_H */
