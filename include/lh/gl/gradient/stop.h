/**
 * @file stop.h
 * @brief One gradient stop: ::lh_gl_gradient_stop_t (color + frac `0..255`).
 */

#ifndef LH_GL_GRADIENT_STOP_H
#define LH_GL_GRADIENT_STOP_H

#include <lh/gl/color.h>
#include <lh/gl/gradient/stop/fields.h>

/**
 * @struct lh_gl_gradient_stop
 * @brief A color on the virtual gradient axis `0..255`.
 */
struct lh_gl_gradient_stop
{
    /** Members: ::lh_gl_gradient_stop_fields. */
    lh_gl_gradient_stop_fields(lh_gl_color_t, lh_byte_t);
};
/** @copybrief lh_gl_gradient_stop */
typedef struct lh_gl_gradient_stop lh_gl_gradient_stop_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self from @p color at position @p frac (`0..255`).
 */
lh_void
lh_gl_gradient_stop_init(lh_gl_gradient_stop_t *self, lh_gl_color_t color, lh_byte_t frac);

/**
 * @brief Color of @p self.
 */
const lh_gl_color_t *
lh_gl_gradient_stop_get_color(const lh_gl_gradient_stop_t *self);

/**
 * @brief Mutable color of @p self.
 */
lh_gl_color_t *
lh_gl_gradient_stop_get_color_mut(lh_gl_gradient_stop_t *self);

/**
 * @brief Position of @p self on the `0..255` axis.
 */
lh_byte_t
lh_gl_gradient_stop_get_frac(const lh_gl_gradient_stop_t *self);

/**
 * @brief Replace the color of @p self.
 */
lh_void
lh_gl_gradient_stop_set_color_v(lh_gl_gradient_stop_t *self, lh_gl_color_t color);

/**
 * @brief Replace the frac of @p self.
 */
lh_void
lh_gl_gradient_stop_set_frac(lh_gl_gradient_stop_t *self, lh_byte_t frac);

/**
 * @brief Store the color of the stop (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The stop; checked by ::lh_assert_runtime_ref.
 * @param color The new value.
 */
lh_void
lh_gl_gradient_stop_set_color(lh_gl_gradient_stop_t *self, lh_gl_color_t color);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_STOP_H */
