/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_font_t.
 */

#ifndef LH_GX_FONT_FIELDS_H
#define LH_GX_FONT_FIELDS_H

#include <lh/core/ptr.h>

/**
 * @def lh_gx_font_fields(get_glyph_fn, get_ascent_fn, get_line_height_fn)
 * @brief The slots of a font, and the font's own data they are called with.
 *
 * Unlike a canvas, a font carries its context: a style points at one font, and
 * the font is the whole of what a text needs to ask for its glyphs. `context` is
 * the font's own data — a baked table (::lh_font_t), a rasterizer's state — and
 * is not owned.
 *
 * @param get_glyph_fn       ::lh_gx_font_get_glyph_fn.
 * @param get_ascent_fn      ::lh_gx_font_get_ascent_fn.
 * @param get_line_height_fn ::lh_gx_font_get_line_height_fn.
 */
#define lh_gx_font_fields(get_glyph_fn, get_ascent_fn, get_line_height_fn)                         \
    get_glyph_fn *get_glyph;                                                                       \
    get_ascent_fn *get_ascent;                                                                     \
    get_line_height_fn *get_line_height;                                                           \
    lh_ptr context

#endif /* LH_GX_FONT_FIELDS_H */
