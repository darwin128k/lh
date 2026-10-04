/**
 * @file font.h
 * @brief A bitmap font the canvas can draw. No operating-system font.
 *
 * The same glyphs are used on a window, in a browser canvas, and on a
 * small display: the font only names which pixels are ink. ::lh_ui_font_basic
 * covers ASCII 32..126 at 8 by 8 pixels. A label points at a font the way
 * an entity points at a style; it does not own it.
 */

#ifndef LH_UI_FONT_H
#define LH_UI_FONT_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/vec2.h>
#include <lh/numeric/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>

/**
 * @struct lh_ui_font
 * @brief A fixed set of bitmap glyphs. Fields stay inside the font module.
 */
struct lh_ui_font
{
    lh_int_t glyph_width;
    lh_int_t glyph_height;
    lh_int_t first;
    lh_int_t count;
    const lh_byte_t *glyphs;
};
typedef struct lh_ui_font lh_ui_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Built-in 8 by 8 ASCII font, glyphs 32..126.
 */
extern const lh_ui_font_t lh_ui_font_basic;

/**
 * @brief Width of one glyph, in pixels.
 */
lh_int_t
lh_ui_font_get_glyph_width(const lh_ui_font_t *self);

/**
 * @brief Height of one glyph, in pixels. A line is one glyph tall.
 */
lh_int_t
lh_ui_font_get_glyph_height(const lh_ui_font_t *self);

/**
 * @brief Width and height @p text occupies. A newline starts another line.
 *        A null or empty string occupies nothing.
 */
lh_math_vec2_t
lh_ui_font_measure(const lh_ui_font_t *self, const lh_char_t *text);

/**
 * @brief Draw @p text at (@p x, @p y) in @p color. Stays inside the canvas
 *        clip. A newline moves to the next line under @p x.
 */
lh_void
lh_ui_font_draw(const lh_ui_font_t *self, lh_ui_canvas_t *canvas, lh_int_t x, lh_int_t y,
                const lh_char_t *text, lh_ui_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
