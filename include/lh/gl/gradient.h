/**
 * @file gradient.h
 * @brief Two colors on an axis of `0..255`: ::lh_gl_gradient_t.
 *
 * `0` is the first color and `255` is the second. A fraction between them mixes
 * the channels, including alpha (::lh_gl_color_mix). The gradient does not know
 * which way a rectangle is painted. ::lh_gl_gradient_linear_frac lays the axis
 * over a box: `0` degrees points up, `90` to the right, `180` down.
 * ::lh_gl_gradient_radial_frac puts `0` at the middle of the box and the second
 * color toward the corners. ::lh_gl_gradient_fill_t stores the angle and which
 * of the two placements to use.
 *
 * The colors are stored here, not borrowed. Two fills can share one gradient
 * by pointing at it.
 */

#ifndef LH_GL_GRADIENT_H
#define LH_GL_GRADIENT_H

#include <lh/gl/color.h>
#include <lh/gl/gradient/fields.h>
#include <lh/gl/scalar.h>

/**
 * @struct lh_gl_gradient
 * @brief A start color and an end color. Fields via ::lh_gl_gradient_fields.
 */
struct lh_gl_gradient
{
    /** Members: ::lh_gl_gradient_fields. */
    lh_gl_gradient_fields(lh_gl_color_t);
};
/** @copybrief lh_gl_gradient */
typedef struct lh_gl_gradient lh_gl_gradient_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief First color. */
const lh_gl_color_t *
lh_gl_gradient_get_from(const lh_gl_gradient_t *self);

/** @brief Mutable first color. */
lh_gl_color_t *
lh_gl_gradient_get_from_mut(lh_gl_gradient_t *self);

/** @brief Second color. */
const lh_gl_color_t *
lh_gl_gradient_get_to(const lh_gl_gradient_t *self);

/** @brief Mutable second color. */
lh_gl_color_t *
lh_gl_gradient_get_to_mut(lh_gl_gradient_t *self);

/** @brief Copy @p color over the first color. */
lh_void
lh_gl_gradient_set_from(lh_gl_gradient_t *self, const lh_gl_color_t *color);

/** @brief Copy @p color over the second color. */
lh_void
lh_gl_gradient_set_to(lh_gl_gradient_t *self, const lh_gl_color_t *color);

/** @brief Store @p from and @p to. */
lh_void
lh_gl_gradient_init(lh_gl_gradient_t *self, const lh_gl_color_t *from, const lh_gl_color_t *to);

/**
 * @brief Color at @p frac. `0` is the first color, `255` is the second.
 *
 * @p out receives the mix. @p self and @p out are not null.
 */
lh_void
lh_gl_gradient_color_at(const lh_gl_gradient_t *self, lh_gl_color_channel_t frac,
                        lh_gl_color_t *out);

/**
 * @brief Where the pixel (@p x, @p y) sits on a linear axis over a box of
 *        @p width by @p height. `0 .. 255`.
 *
 * The box origin is (0, 0). @p angle is degrees: `0` up, `90` right, `180` down.
 * The ends of the axis meet the corners the line reaches.
 */
lh_gl_color_channel_t
lh_gl_gradient_linear_frac(lh_gl_scalar_t angle, lh_gl_scalar_t width, lh_gl_scalar_t height,
                           lh_gl_scalar_t x, lh_gl_scalar_t y);

/**
 * @brief Where the pixel (@p x, @p y) sits on an ellipse the shape of a box of
 *        @p width by @p height. `0 .. 255`.
 *
 * The box origin is (0, 0). The middle is `0`. A corner of a large box is `255`.
 */
lh_gl_color_channel_t
lh_gl_gradient_radial_frac(lh_gl_scalar_t width, lh_gl_scalar_t height, lh_gl_scalar_t x,
                           lh_gl_scalar_t y);

/**
 * @brief Pixel @p along on an axis of @p length, scaled by 1024. Zero when
 *        @p length is not positive. The middle sample is 0.
 */
lh_s64_t
lh_gl_gradient_radial_axis(lh_gl_scalar_t along, lh_gl_scalar_t length);

/**
 * @brief ::lh_gl_gradient_radial_frac from `ux*ux + uy*uy`, the squares of
 *        ::lh_gl_gradient_radial_axis.
 */
lh_gl_color_channel_t
lh_gl_gradient_radial_frac_from_square(lh_u64_t square);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_H */
