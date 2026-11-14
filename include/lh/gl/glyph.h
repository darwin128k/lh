/**
 * @file glyph.h
 * @brief One borrowed glyph: ::lh_gl_glyph_t.
 *
 * Width, height, the pen step and the ink's top, plus coverage bytes the caller
 * owns. A font holds these in a row. Nothing here draws.
 */

#ifndef LH_GL_GLYPH_H
#define LH_GL_GLYPH_H

#include <lh/core/byte.h>
#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/gl/glyph/fields.h>
#include <lh/gl/scalar.h>

/**
 * @struct lh_gl_glyph
 * @brief Ink and advance. Fields via ::lh_gl_glyph_fields.
 */
struct lh_gl_glyph
{
    /** Members: ::lh_gl_glyph_fields. */
    lh_gl_glyph_fields(lh_gl_scalar_t, lh_byte_t);
};
/** @copybrief lh_gl_glyph */
typedef struct lh_gl_glyph lh_gl_glyph_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Ink width in pixels. */
lh_gl_scalar_t
lh_gl_glyph_get_width(const lh_gl_glyph_t *self);

/** @brief Ink height in pixels. */
lh_gl_scalar_t
lh_gl_glyph_get_height(const lh_gl_glyph_t *self);

/** @brief How far the pen moves after this glyph. */
lh_gl_scalar_t
lh_gl_glyph_get_advance(const lh_gl_glyph_t *self);

/** @brief Ink top relative to the baseline. Up is negative. */
lh_gl_scalar_t
lh_gl_glyph_get_top(const lh_gl_glyph_t *self);

/** @brief Coverage bytes, or ::lh_null when the glyph has no ink. */
const lh_byte_t *
lh_gl_glyph_get_bits(const lh_gl_glyph_t *self);

/** @brief Store the ink width. */
lh_void
lh_gl_glyph_set_width(lh_gl_glyph_t *self, lh_gl_scalar_t width);

/** @brief Store the ink height. */
lh_void
lh_gl_glyph_set_height(lh_gl_glyph_t *self, lh_gl_scalar_t height);

/** @brief Store the pen step. */
lh_void
lh_gl_glyph_set_advance(lh_gl_glyph_t *self, lh_gl_scalar_t advance);

/** @brief Store the ink top relative to the baseline. */
lh_void
lh_gl_glyph_set_top(lh_gl_glyph_t *self, lh_gl_scalar_t top);

/** @brief Borrow @p bits. @p bits may be ::lh_null. The bytes are not copied. */
lh_void
lh_gl_glyph_set_bits(lh_gl_glyph_t *self, const lh_byte_t *bits);

/** @brief Store the ink. @p bits may be ::lh_null. The bytes are not copied. */
lh_void
lh_gl_glyph_init(lh_gl_glyph_t *self, lh_gl_scalar_t width, lh_gl_scalar_t height,
                 lh_gl_scalar_t advance, lh_gl_scalar_t top, const lh_byte_t *bits);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GLYPH_H */
