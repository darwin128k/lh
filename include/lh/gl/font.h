/**
 * @file font.h
 * @brief A borrowed row of glyphs: ::lh_gl_font_t.
 *
 * One contiguous run of codes, each a ::lh_gl_glyph_t the caller owns. The
 * lookup is the code minus the first code. A code outside the run has no glyph.
 * Line height and ascent are the only metrics. Drawing a string is
 * ::lh_gl_text_t.
 */

#ifndef LH_GL_FONT_H
#define LH_GL_FONT_H

#include <lh/gl/font/fields.h>
#include <lh/gl/glyph.h>

/**
 * @struct lh_gl_font
 * @brief Glyphs over one run of codes. Fields via ::lh_gl_font_fields.
 */
struct lh_gl_font
{
    /** Members: ::lh_gl_font_fields. */
    lh_gl_font_fields(lh_gl_glyph_t, lh_u32_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_font */
typedef struct lh_gl_font lh_gl_font_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief The glyph row, or ::lh_null. */
const lh_gl_glyph_t *
lh_gl_font_get_glyphs(const lh_gl_font_t *self);

/** @brief Code of the first glyph. */
lh_u32_t
lh_gl_font_get_first(const lh_gl_font_t *self);

/** @brief How many glyphs the row holds. */
lh_u32_t
lh_gl_font_get_count(const lh_gl_font_t *self);

/** @brief Height of one line, in pixels. */
lh_gl_scalar_t
lh_gl_font_get_line_height(const lh_gl_font_t *self);

/** @brief Baseline down from the top of the line, in pixels. */
lh_gl_scalar_t
lh_gl_font_get_ascent(const lh_gl_font_t *self);

/** @brief Borrow @p glyphs. @p glyphs may be ::lh_null. The row is not copied. */
lh_void
lh_gl_font_set_glyphs(lh_gl_font_t *self, const lh_gl_glyph_t *glyphs);

/** @brief Store the code of the first glyph. */
lh_void
lh_gl_font_set_first(lh_gl_font_t *self, lh_u32_t first);

/** @brief Store how many glyphs the row holds. */
lh_void
lh_gl_font_set_count(lh_gl_font_t *self, lh_u32_t count);

/** @brief Store the line height. */
lh_void
lh_gl_font_set_line_height(lh_gl_font_t *self, lh_gl_scalar_t line_height);

/** @brief Store the ascent. */
lh_void
lh_gl_font_set_ascent(lh_gl_font_t *self, lh_gl_scalar_t ascent);

/**
 * @brief Glyph of @p code, or ::lh_null when the run does not hold it.
 *
 * The pointer is into the borrowed row.
 */
const lh_gl_glyph_t *
lh_gl_font_get_glyph(const lh_gl_font_t *self, lh_u32_t code);

/**
 * @brief Fill @p self over @p glyphs. The row is not copied.
 *
 * @p count entries start at code @p first.
 */
lh_void
lh_gl_font_init(lh_gl_font_t *self, const lh_gl_glyph_t *glyphs, lh_u32_t first, lh_u32_t count,
                lh_gl_scalar_t line_height, lh_gl_scalar_t ascent);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_FONT_H */
