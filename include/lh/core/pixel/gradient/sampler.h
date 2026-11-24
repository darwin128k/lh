/**
 * @file sampler.h
 * @brief The positions of a gradient along a row of a box: ::lh_pixel_gradient_sampler_t.
 *
 * ::lh_pixel_gradient_get_frac_at gives the position of one pixel with a few
 * multiplications and a division (a square root for a radial gradient). Along a
 * row the position moves by the same amount from pixel to pixel, so the sampler
 * carries it — the quotient and its remainder — and gives the position of every
 * pixel of a run with additions only, the same positions to the last pixel. The
 * positions are bytes; the colour table (`lh/core/pixel/gradient/table.h`) turns
 * them into colours, so the sampler does not care what a colour is.
 *
 * Positions, not colours, so that one walk serves every colour type: `0xAARRGGBB`
 * words, gl's ::lh_gl_color_t. The price is a second pass
 * over the row, the look-up: against gl's walk that wrote its colours directly,
 * on `bench/lvgl/` (i7-3770K, 2026-11-19, run alternately with the old build)
 * a radial frame of 24 cards went from 1003–1051 to 1063–1123 microseconds and
 * a diagonal one by 3–5 %; a vertical one, which is a column, did not change.
 *
 * A value on the stack of whoever draws the box: made for one gradient over one
 * box (::lh_pixel_gradient_sampler_init), then asked for rows
 * (::lh_pixel_gradient_sampler_row) and columns (::lh_pixel_gradient_sampler_column)
 * of it. It keeps nothing of the gradient but its shape and direction.
 */

#ifndef LH_CORE_PIXEL_GRADIENT_SAMPLER_H
#define LH_CORE_PIXEL_GRADIENT_SAMPLER_H

#include <lh/core/pixel/gradient.h>
#include <lh/core/pixel/gradient/sampler/fields.h>

/**
 * @struct lh_pixel_gradient_sampler
 * @brief A gradient laid over one box. Members: ::lh_pixel_gradient_sampler_fields.
 */
struct lh_pixel_gradient_sampler
{
    /** Members: ::lh_pixel_gradient_sampler_fields. */
    lh_pixel_gradient_sampler_fields(lh_pixel_gradient_shape_t, lh_s64_t);
};
/** @copybrief lh_pixel_gradient_sampler */
typedef struct lh_pixel_gradient_sampler lh_pixel_gradient_sampler_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The gradient's shape (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_pixel_gradient_shape_t
lh_pixel_gradient_sampler_get_shape(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the gradient's shape (level 1).
 *
 * @param self  The sampler; checked by ::lh_assert_runtime_ref.
 * @param shape The shape.
 */
lh_void
lh_pixel_gradient_sampler_set_shape(lh_pixel_gradient_sampler_t *self,
                                    lh_pixel_gradient_shape_t shape);

/**
 * @brief The box's width (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_width(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the box's width (level 1).
 *
 * @param self  The sampler; checked by ::lh_assert_runtime_ref.
 * @param width Pixels.
 */
lh_void
lh_pixel_gradient_sampler_set_width(lh_pixel_gradient_sampler_t *self, lh_s64_t width);

/**
 * @brief The box's height (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_height(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the box's height (level 1).
 *
 * @param self   The sampler; checked by ::lh_assert_runtime_ref.
 * @param height Pixels.
 */
lh_void
lh_pixel_gradient_sampler_set_height(lh_pixel_gradient_sampler_t *self, lh_s64_t height);

/**
 * @brief The box's middle column, doubled (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_middle_x(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the box's middle column, doubled (level 1).
 *
 * @param self     The sampler; checked by ::lh_assert_runtime_ref.
 * @param middle_x `2 * left + width`.
 */
lh_void
lh_pixel_gradient_sampler_set_middle_x(lh_pixel_gradient_sampler_t *self, lh_s64_t middle_x);

/**
 * @brief The box's middle row, doubled (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_middle_y(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the box's middle row, doubled (level 1).
 *
 * @param self     The sampler; checked by ::lh_assert_runtime_ref.
 * @param middle_y `2 * top + height`.
 */
lh_void
lh_pixel_gradient_sampler_set_middle_y(lh_pixel_gradient_sampler_t *self, lh_s64_t middle_y);

/**
 * @brief The sine of the direction, times 4096 (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_sine(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the sine of the direction (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 * @param sine Times 4096 (::lh_pixel_gradient_sine).
 */
lh_void
lh_pixel_gradient_sampler_set_sine(lh_pixel_gradient_sampler_t *self, lh_s64_t sine);

/**
 * @brief The cosine of the direction, times 4096 (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_cosine(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the cosine of the direction (level 1).
 *
 * @param self   The sampler; checked by ::lh_assert_runtime_ref.
 * @param cosine Times 4096.
 */
lh_void
lh_pixel_gradient_sampler_set_cosine(lh_pixel_gradient_sampler_t *self, lh_s64_t cosine);

/**
 * @brief The box seen along the direction, doubled, times 4096 (level 1).
 *
 * @param self The sampler; checked by ::lh_assert_runtime_ref.
 */
lh_s64_t
lh_pixel_gradient_sampler_get_length(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief Store the box seen along the direction (level 1).
 *
 * @param self   The sampler; checked by ::lh_assert_runtime_ref.
 * @param length `(width * |sine| + height * |cosine|) * 2`.
 */
lh_void
lh_pixel_gradient_sampler_set_length(lh_pixel_gradient_sampler_t *self, lh_s64_t length);

/**
 * @brief @p count positions of `0`: a flat box's row.
 *
 * @param self  The sampler; not read (every position is the first).
 * @param count How many.
 * @param out   Room for @p count positions.
 */
lh_void
lh_pixel_gradient_sampler_flat_run(const lh_pixel_gradient_sampler_t *self, lh_s32_t count,
                                   lh_byte_t *out);

/**
 * @brief Lay @p gradient over @p rect: its shape and direction, the box's size and
 *        middle.
 *
 * @param self     The sampler; checked by the setters.
 * @param gradient The gradient; must not be null. Read, not kept.
 * @param rect     The box, whole pixels; must not be null.
 */
lh_void
lh_pixel_gradient_sampler_init(lh_pixel_gradient_sampler_t *self,
                               const lh_pixel_gradient_t *gradient, const lh_math_rect_t *rect);

/**
 * @brief Whether every pixel of the box is at position `0`: an empty box, or a
 *        linear gradient over a box with no length along its direction.
 *
 * @param self The sampler; checked by the getters.
 */
lh_bool_t
lh_pixel_gradient_sampler_is_flat(const lh_pixel_gradient_sampler_t *self);

/**
 * @brief @p count positions of a linear gradient, from the pixel whose centre is
 *        @p dx, @p dy from the middle (both doubled), each next one @p move further
 *        along the scale `(2 * along + length) * 255` — what
 *        ::lh_pixel_gradient_get_frac_at gives, carried as a quotient rounded down
 *        and its remainder instead of divided again. Below zero the two round
 *        differently, and both give `0`.
 *
 * @param self  The sampler; checked by the getters.
 * @param dx    The first pixel's column from the middle, doubled.
 * @param dy    Its row from the middle, doubled.
 * @param move  What one pixel on adds: `4 * sine * 255` along a row,
 *              `-4 * cosine * 255` down a column.
 * @param count How many pixels.
 * @param out   Room for @p count positions.
 */
lh_void
lh_pixel_gradient_sampler_linear_run(const lh_pixel_gradient_sampler_t *self, lh_s64_t dx,
                                     lh_s64_t dy, lh_s64_t move, lh_s32_t count, lh_byte_t *out);

/**
 * @brief The positions of a radial gradient from column @p x0 to @p x1 of the row
 *        @p dy from the middle (doubled): `dx * 1024 / width` carried from pixel
 *        to pixel as a quotient and its remainder; the squared distance then
 *        crosses the steps of the scale at a table of the least squares that
 *        reach them, walked to from the last pixel's step — no square root.
 *
 * @param self The sampler; checked by the getters.
 * @param x0   The first column.
 * @param x1   One past the last; nothing when not past @p x0.
 * @param dy   The row from the middle, doubled.
 * @param out  Room for `x1 - x0` positions.
 */
lh_void
lh_pixel_gradient_sampler_radial_run(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0,
                                     lh_s32_t x1, lh_s64_t dy, lh_byte_t *out);

/**
 * @brief A row of a radial gradient: two pixels the same distance either side of
 *        the middle are the same position, so the half from the middle on is
 *        walked (::lh_pixel_gradient_sampler_radial_run) and each pixel left of
 *        the middle is its mirror's — walked too when the mirror is past the end
 *        of the row.
 *
 * @param self The sampler; checked by the getters.
 * @param x0   The first column.
 * @param x1   One past the last.
 * @param dy   The row from the middle, doubled.
 * @param out  Room for `x1 - x0` positions.
 */
lh_void
lh_pixel_gradient_sampler_radial_row(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0,
                                     lh_s32_t x1, lh_s64_t dy, lh_byte_t *out);

/**
 * @brief The positions of row @p y from column @p x0 to @p x1 — flat, radial or
 *        linear as the box and the gradient are. Columns outside the box are
 *        taken as the gradient goes on past it.
 *
 * @param self The sampler; checked by the getters.
 * @param x0   The first column.
 * @param x1   One past the last, at least @p x0.
 * @param y    The row.
 * @param out  Room for `x1 - x0` positions; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_pixel_gradient_sampler_row(const lh_pixel_gradient_sampler_t *self, lh_s32_t x0, lh_s32_t x1,
                              lh_s32_t y, lh_byte_t *out);

/**
 * @brief The positions of column @p x from row @p y0 to @p y1: a linear gradient
 *        walked down (a pixel down moves `-2 * cosine` along the line); a radial
 *        or flat one a row of one pixel at a time.
 *
 * @param self The sampler; checked by the getters.
 * @param x    The column.
 * @param y0   The first row.
 * @param y1   One past the last, at least @p y0.
 * @param out  Room for `y1 - y0` positions; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_pixel_gradient_sampler_column(const lh_pixel_gradient_sampler_t *self, lh_s32_t x, lh_s32_t y0,
                                 lh_s32_t y1, lh_byte_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_PIXEL_GRADIENT_SAMPLER_H */
