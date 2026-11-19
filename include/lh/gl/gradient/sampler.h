/**
 * @file sampler.h
 * @brief A gradient laid over one box, read a row at a time in gl's colours:
 *        ::lh_gl_gradient_sampler_t.
 *
 * The walk is the core's (`lh/core/pixel/gradient/sampler.h`): it gives the
 * position of every pixel of a row with additions only, and the colour table
 * (`lh/core/pixel/gradient/table.h`) the colour of every position. gl draws
 * with ::lh_gl_color_t, so the 256 colours are turned into gl's once, when the
 * sampler is made, and a row is its positions looked up in them. The colours
 * are ::lh_gl_gradient_frac_at and ::lh_gl_gradient_color_at's to the last bit.
 *
 * A value on the stack of the gl canvas that draws the box (about 1.1 KB).
 */

#ifndef LH_GL_GRADIENT_SAMPLER_H
#define LH_GL_GRADIENT_SAMPLER_H

#include <lh/core/pixel/gradient/sampler.h>
#include <lh/gl/gradient.h>
#include <lh/gl/gradient/sampler/fields.h>

/**
 * @def LH_GL_GRADIENT_SAMPLER_CHUNK
 * @brief The positions of a row worked out at a time on the stack, before they
 *        are looked up: a longer row is done in pieces this long.
 */
#define LH_GL_GRADIENT_SAMPLER_CHUNK 256

/**
 * @struct lh_gl_gradient_sampler
 * @brief A gradient over one box. Members: ::lh_gl_gradient_sampler_fields.
 */
struct lh_gl_gradient_sampler
{
    /** Members: ::lh_gl_gradient_sampler_fields. */
    lh_gl_gradient_sampler_fields(lh_pixel_gradient_sampler_t, lh_gl_color_t);
};
/** @copybrief lh_gl_gradient_sampler */
typedef struct lh_gl_gradient_sampler lh_gl_gradient_sampler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The core's sampler: the walk over the box (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
const lh_pixel_gradient_sampler_t *
lh_gl_gradient_sampler_get_sampler(const lh_gl_gradient_sampler_t *self);

/**
 * @brief The core's sampler, to set it up (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_pixel_gradient_sampler_t *
lh_gl_gradient_sampler_get_sampler_mut(lh_gl_gradient_sampler_t *self);

/**
 * @brief The gradient's 256 colours as gl's, `colors[frac]` (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
const lh_gl_color_t *
lh_gl_gradient_sampler_get_colors(const lh_gl_gradient_sampler_t *self);

/**
 * @brief The gradient's 256 colours, to write them (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_gl_color_t *
lh_gl_gradient_sampler_get_colors_mut(lh_gl_gradient_sampler_t *self);

/**
 * @brief Lay @p gradient over @p rect: the core's sampler over the box's whole
 *        pixels (::lh_gl_gradient_get_box_px), and the colour table
 *        (::lh_pixel_gradient_table_fill) turned into gl's colours.
 *
 * @param self     The sampler; checked by the accessors.
 * @param gradient The gradient; checked by ::lh_assert_runtime_ref. Read, not kept.
 * @param rect     The box; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_gradient_sampler_init(lh_gl_gradient_sampler_t *self, const lh_pixel_gradient_t *gradient,
                            const lh_gl_rect_t *rect);

/**
 * @brief Turn @p count positions into gl's colours: `out[i]` is
 *        `colors[levels[i]]`.
 *
 * @param self   The sampler; checked by ::lh_gl_gradient_sampler_get_colors.
 * @param levels The positions; @p count of them.
 * @param count  How many.
 * @param out    Room for @p count colours.
 */
lh_void
lh_gl_gradient_sampler_map(const lh_gl_gradient_sampler_t *self, const lh_byte_t *levels,
                           lh_s32_t count, lh_gl_color_t *out);

/**
 * @brief The colours of row @p y from column @p x0 to @p x1
 *        (::lh_pixel_gradient_sampler_row), a chunk of
 *        ::LH_GL_GRADIENT_SAMPLER_CHUNK at a time.
 *
 * @param self The sampler; checked by the accessors.
 * @param x0   The first column.
 * @param x1   One past the last, at least @p x0.
 * @param y    The row.
 * @param out  Room for `x1 - x0` colours; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_gradient_sampler_row(const lh_gl_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                           lh_s32_t y, lh_gl_color_t *out);

/**
 * @brief The colours of column @p x from row @p y0 to @p y1
 *        (::lh_pixel_gradient_sampler_column), a chunk at a time.
 *
 * @param self The sampler; checked by the accessors.
 * @param x    The column.
 * @param y0   The first row.
 * @param y1   One past the last, at least @p y0.
 * @param out  Room for `y1 - y0` colours; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_gradient_sampler_column(const lh_gl_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0,
                              lh_s32_t y1, lh_gl_color_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_GRADIENT_SAMPLER_H */
