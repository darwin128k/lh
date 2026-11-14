/**
 * @file color.h
 * @brief One pixel: ::lh_gl_color_t, 8-bit RGBA, straight alpha.
 *
 * The canvas hands this to its target. The target converts it to the device
 * format. The color knows nothing about a surface or a layer.
 */

#ifndef LH_GL_COLOR_H
#define LH_GL_COLOR_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>
#include <lh/gl/color/fields.h>

/**
 * @typedef lh_gl_color_channel_t
 * @brief One 8-bit channel, `0..255`.
 */
typedef lh_u8_t lh_gl_color_channel_t;

/**
 * @struct lh_gl_color
 * @brief Straight RGBA. Alpha 255 covers the pixel.
 */
struct lh_gl_color
{
    /** Members: ::lh_gl_color_fields. */
    lh_gl_color_fields(lh_gl_color_channel_t);
};
/** @copybrief lh_gl_color */
typedef struct lh_gl_color lh_gl_color_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Fill @p self from channels in `0..255`. */
lh_void
lh_gl_color_init(lh_gl_color_t *self, lh_gl_color_channel_t r, lh_gl_color_channel_t g,
                 lh_gl_color_channel_t b, lh_gl_color_channel_t a);

/** @brief Red channel. */
lh_gl_color_channel_t
lh_gl_color_get_r(const lh_gl_color_t *self);

/** @brief Green channel. */
lh_gl_color_channel_t
lh_gl_color_get_g(const lh_gl_color_t *self);

/** @brief Blue channel. */
lh_gl_color_channel_t
lh_gl_color_get_b(const lh_gl_color_t *self);

/** @brief Alpha channel. 255 covers the pixel. */
lh_gl_color_channel_t
lh_gl_color_get_a(const lh_gl_color_t *self);

/** @brief Store the red channel. */
lh_void
lh_gl_color_set_r(lh_gl_color_t *self, lh_gl_color_channel_t r);

/** @brief Store the green channel. */
lh_void
lh_gl_color_set_g(lh_gl_color_t *self, lh_gl_color_channel_t g);

/** @brief Store the blue channel. */
lh_void
lh_gl_color_set_b(lh_gl_color_t *self, lh_gl_color_channel_t b);

/** @brief Store the alpha channel. */
lh_void
lh_gl_color_set_a(lh_gl_color_t *self, lh_gl_color_channel_t a);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_COLOR_H */
