/**
 * @file font.h
 * @brief A bitmap font the canvas can draw. The bytes come from memory.
 *
 * A font does not open a file and does not ask the operating system for
 * glyphs. It wraps a ::lh_memory_view_t the caller already holds: a static
 * table, a buffer, or bytes embedded in the program. The same view is what
 * a window, a browser canvas, and a small display draw from. A new label
 * and the other text widgets start with ::lh_ui_font_get_default, which is
 * ::LH_LIBRARY_OPTION_UI_FONT (::lh_ui_font_roboto unless the build names
 * another). A label points at a font the way an entity points at a style;
 * it does not own it.
 */

#ifndef LH_UI_FONT_H
#define LH_UI_FONT_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/math/rect.h>
#include <lh/math/vec2.h>
#include <lh/memory/view.h>
#include <lh/numeric/types.h>
#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/font/fields.h>
#include <lh/void.h>

/**
 * @struct lh_ui_font
 * @brief Fields via ::lh_ui_font_fields. Bitmaps are packed like LVGL's
 *        font converter: 1, 2, 4 or 8 bits per pixel, high bit first.
 */
struct lh_ui_font
{
    lh_ui_font_fields(lh_memory_view_t, lh_int_t);
};
typedef struct lh_ui_font lh_ui_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Roboto Regular at 16 pixels, glyphs 32..126, 4 bits per pixel.
 *        Baked from the TTF; the file is not opened at runtime.
 *        This is ::LH_LIBRARY_OPTION_UI_FONT unless the build names another.
 */
extern const lh_ui_font_t lh_ui_font_roboto;

/**
 * @brief The font a new label, field, list, spin box, keyboard, and tab
 *        title start with: ::LH_LIBRARY_OPTION_UI_FONT.
 */
const lh_ui_font_t *
lh_ui_font_get_default(void);

/**
 * @brief Point @p self at glyph memory. The bytes are not copied.
 *
 * @p glyphs must cover `count * row_bytes * glyph_height` bytes, with
 * `row_bytes = (glyph_width * bpp + 7) / 8`. @p bpp is 1, 2, 4 or 8.
 * @p advances is one byte per glyph, or ::lh_null when every glyph
 * advances by @p glyph_width. @p first is the character code of the
 * first glyph.
 */
lh_void
lh_ui_font_init(lh_ui_font_t *self, const lh_memory_view_t *glyphs, const lh_memory_view_t *advances,
                lh_int_t glyph_width, lh_int_t glyph_height, lh_int_t bpp, lh_int_t first,
                lh_int_t count);

/**
 * @brief The glyph memory @p self wraps.
 */
lh_memory_view_t
lh_ui_font_get_glyphs(const lh_ui_font_t *self);

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
 * @brief Tight pixels of @p text, relative to the point ::lh_ui_font_draw
 *        uses. The origin is the top-left of the ink, so it can sit inside
 *        the cell. A null or blank string is an empty rectangle.
 */
lh_math_rect_t
lh_ui_font_ink(const lh_ui_font_t *self, const lh_char_t *text);

/**
 * @brief Draw @p text at (@p x, @p y) in @p color. Stays inside the canvas
 *        clip. A newline moves to the next line under @p x.
 */
lh_void
lh_ui_font_draw(const lh_ui_font_t *self, lh_ui_canvas_t *canvas, lh_int_t x, lh_int_t y,
                const lh_char_t *text, lh_ui_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_FONT_H */
