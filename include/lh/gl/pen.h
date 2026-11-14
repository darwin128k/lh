/**
 * @file pen.h
 * @brief A stroke: ::lh_gl_pen_t, a color and a thickness.
 *
 * The pen is a value. It does not know which shape it will stroke.
 */

#ifndef LH_GL_PEN_H
#define LH_GL_PEN_H

#include <lh/gl/scalar.h>
#include <lh/gl/color.h>
#include <lh/gl/pen/fields.h>

/**
 * @struct lh_gl_pen
 * @brief Stroke color and thickness.
 */
struct lh_gl_pen
{
    /** Members: ::lh_gl_pen_fields. */
    lh_gl_pen_fields(lh_gl_color_t, lh_gl_scalar_t);
};
/** @copybrief lh_gl_pen */
typedef struct lh_gl_pen lh_gl_pen_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Fill @p self. @p color is copied. */
lh_void
lh_gl_pen_init(lh_gl_pen_t *self, lh_gl_color_t color, lh_gl_scalar_t width);

/** @brief Borrowed color, valid while @p self lives. */
const lh_gl_color_t *
lh_gl_pen_get_color(const lh_gl_pen_t *self);

/** @brief Copy @p color into @p self. */
lh_void
lh_gl_pen_set_color(lh_gl_pen_t *self, lh_gl_color_t color);

/** @brief Stroke thickness. */
lh_gl_scalar_t
lh_gl_pen_get_width(const lh_gl_pen_t *self);

/** @brief Store the stroke thickness. */
lh_void
lh_gl_pen_set_width(lh_gl_pen_t *self, lh_gl_scalar_t width);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_PEN_H */
