/**
 * @file gradient.h
 * @brief Two colors on an axis of `0..255`: ::lh_gl_gradient_t.
 *
 * `0` is the first color and `255` is the second. A fraction between them mixes
 * the channels, including alpha (::lh_gl_color_mix). The gradient does not know
 * which way a rectangle is painted: a ::lh_gl_gradient_fill_t paints it from
 * the first color at the top row to the second at the bottom.
 *
 * The colors are stored here, not borrowed. Two fills can share one gradient
 * by pointing at it.
 */

#ifndef LH_GL_GRADIENT_H
#define LH_GL_GRADIENT_H

#include <lh/gl/color.h>
#include <lh/gl/gradient/fields.h>

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_H */
