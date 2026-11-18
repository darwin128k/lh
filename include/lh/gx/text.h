/**
 * @file text.h
 * @brief A line of text: ::lh_gx_text_class.
 *
 * An entity of this class draws the UTF-8 string its content points at
 * (::lh_gx_entity_set_content), one line from the top-left corner of its place,
 * in its style's `ink`, with its style's font (::lh_gx_style_get_font). The
 * baseline is the font's ascent below the top; each glyph's mask is laid at its
 * own top from the baseline, and the pen moves by its advance. A code the font
 * has no glyph for is skipped, and the line ends at the first line feed.
 *
 * The glyphs go to the canvas as coverage masks: each row of a glyph's packed
 * mask (`lh/core/pixel/mask.h`) is widened to 8 bits and laid through
 * `fill_mask`. A glyph is at most ::LH_GX_TEXT_GLYPH_MAX pixels wide — the row is
 * on the stack.
 *
 * Text is never opaque, so the class has no `covers`. Ownership: the string is
 * the application's; the entity only points at it, and a change of it is the
 * application's to report (::lh_gx_entity_invalidate).
 */

#ifndef LH_GX_TEXT_H
#define LH_GX_TEXT_H

#include <lh/gx/class.h>
#include <lh/gx/entity.h>
#include <lh/gx/paint.h>
#include <lh/gx/style.h>

/**
 * @def LH_GX_TEXT_GLYPH_MAX
 * @brief The widest glyph a text draws, in pixels: one row of it is on the stack.
 */
#define LH_GX_TEXT_GLYPH_MAX 64

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a line of text: `paint` ::lh_gx_text_paint.
 */
extern const lh_gx_class_t lh_gx_text_class;

/**
 * @brief How wide @p text is in @p font, up to its first line feed: the advances
 *        of its glyphs, a code with no glyph adding nothing.
 *
 * @param font The font; must not be null.
 * @param text The string, UTF-8, ending in `0`; must not be null.
 * @return Pixels.
 */
lh_s32_t
lh_gx_text_get_width(const lh_gx_font_t *font, const lh_char_t *text);

/**
 * @brief Lay @p glyph in @p color with its pen at @p x on @p baseline: each row of
 *        its mask through `fill_mask`, cut by the paint's clip.
 *
 * Nothing for a glyph with no ink. Columns past ::LH_GX_TEXT_GLYPH_MAX are not
 * drawn.
 *
 * @param paint    The paint; must not be null.
 * @param glyph    The glyph; must not be null.
 * @param x        The pen's column.
 * @param baseline The baseline's row.
 * @param color    `0xAARRGGBB`.
 */
lh_void
lh_gx_text_paint_glyph(const lh_gx_paint_t *paint, const lh_gx_glyph_t *glyph, lh_s32_t x,
                       lh_s32_t baseline, lh_gx_color_t color);

/**
 * @brief `paint` of the text class (::lh_gx_class_paint_fn): the entity's string in
 *        its style's font and `ink`.
 *
 * Nothing when the entity has no style, the style no font, or the entity no
 * string.
 *
 * @param entity The entity.
 * @param paint  The paint.
 */
lh_void
lh_gx_text_paint(const lh_gx_entity_t *entity, const lh_gx_paint_t *paint);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_TEXT_H */
