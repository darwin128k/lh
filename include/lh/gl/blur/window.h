/**
 * @file window.h
 * @brief ::lh_gl_blur_window_t — one sliding average, four channels.
 *
 * A blur walks a row or a column by adding the pixel that enters the window
 * and subtracting the pixel that leaves it. The sums live here so that walk
 * does not add the window up again at every pixel. Red, green and blue are
 * stored already multiplied by alpha: a fully transparent pixel still holds
 * colour bytes, and those bytes are not part of the picture.
 */

#ifndef LH_GL_BLUR_WINDOW_H
#define LH_GL_BLUR_WINDOW_H

#include <lh/core/bool.h>
#include <lh/core/size.h>
#include <lh/gl/blur/window/fields.h>
#include <lh/gl/color.h>
#include <lh/gl/scalar.h>

/**
 * @struct lh_gl_blur_window
 * @brief Four running sums. Fields via ::lh_gl_blur_window_fields.
 */
struct lh_gl_blur_window
{
    /** Members: ::lh_gl_blur_window_fields. */
    lh_gl_blur_window_fields(lh_s32_t);
};
/** @copybrief lh_gl_blur_window */
typedef struct lh_gl_blur_window lh_gl_blur_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/** @brief Sum of premultiplied red. */
lh_s32_t
lh_gl_blur_window_get_r(const lh_gl_blur_window_t *self);

/** @brief Store the sum of premultiplied red. */
lh_void
lh_gl_blur_window_set_r(lh_gl_blur_window_t *self, lh_s32_t r);

/** @brief Sum of premultiplied green. */
lh_s32_t
lh_gl_blur_window_get_g(const lh_gl_blur_window_t *self);

/** @brief Store the sum of premultiplied green. */
lh_void
lh_gl_blur_window_set_g(lh_gl_blur_window_t *self, lh_s32_t g);

/** @brief Sum of premultiplied blue. */
lh_s32_t
lh_gl_blur_window_get_b(const lh_gl_blur_window_t *self);

/** @brief Store the sum of premultiplied blue. */
lh_void
lh_gl_blur_window_set_b(lh_gl_blur_window_t *self, lh_s32_t b);

/** @brief Sum of alpha. */
lh_s32_t
lh_gl_blur_window_get_a(const lh_gl_blur_window_t *self);

/** @brief Store the sum of alpha. */
lh_void
lh_gl_blur_window_set_a(lh_gl_blur_window_t *self, lh_s32_t a);

/** @brief All four sums zero. */
lh_void
lh_gl_blur_window_init(lh_gl_blur_window_t *self);

/** @brief Add @p pixel. Colour is multiplied by its alpha first. */
lh_void
lh_gl_blur_window_take(lh_gl_blur_window_t *self, const lh_gl_color_t *pixel);

/** @brief Subtract @p pixel, the one leaving the window. */
lh_void
lh_gl_blur_window_drop(lh_gl_blur_window_t *self, const lh_gl_color_t *pixel);

/**
 * @brief Add @p count colours starting at @p sample, @p stride colours apart.
 *        A count of `0` or less adds nothing.
 */
lh_void
lh_gl_blur_window_take_colors(lh_gl_blur_window_t *self, const lh_gl_color_t *sample,
                              lh_usize_t stride, lh_gl_scalar_t count);

/**
 * @brief Subtract @p count colours starting at @p sample, @p stride colours apart.
 */
lh_void
lh_gl_blur_window_drop_colors(lh_gl_blur_window_t *self, const lh_gl_color_t *sample,
                              lh_usize_t stride, lh_gl_scalar_t count);

/**
 * @brief Channel @p channel (`0` red .. `3` alpha) averaged over @p count
 *        pixels, rounded half up and held to `0 .. 255`.
 *
 * Alpha divides by @p count. A colour channel divides by the alpha sum, so a
 * transparent pixel does not tint the average. When every pixel in the window
 * is opaque the channel divides by @p count instead, which is the same number
 * without the extra rounding of dividing the weights back out.
 */
lh_gl_color_channel_t
lh_gl_blur_window_average(const lh_gl_blur_window_t *self, lh_gl_scalar_t channel,
                          lh_gl_scalar_t count);

/**
 * @brief One channel of an average held in running sums, rounded half up and
 *        held to `0 .. 255`.
 *
 * @p is_alpha selects the alpha sum in @p sum: it divides by @p count. A colour
 * channel divides by @p alpha_sum, except when every pixel is opaque
 * (`@p alpha_sum == 255 * count`), which divides by @p count after taking the
 * factor of 255 back out. A @p count of `0` or below is `0`.
 */
lh_gl_color_channel_t
lh_gl_blur_window_average_sum(lh_s32_t sum, lh_s32_t alpha_sum, lh_gl_scalar_t count,
                              lh_bool_t is_alpha);

/** @brief All four channels of ::lh_gl_blur_window_average into @p out. */
lh_void
lh_gl_blur_window_average_color(const lh_gl_blur_window_t *self, lh_gl_scalar_t count,
                                lh_gl_color_t *out);

/**
 * @brief Write the average of four running sums into @p out.
 *        @p count is how many pixels those sums cover.
 */
lh_void
lh_gl_blur_window_write_average(lh_s32_t red, lh_s32_t green, lh_s32_t blue, lh_s32_t alpha,
                                lh_gl_scalar_t count, lh_gl_color_t *out);

/**
 * @brief Slide a window of @p radius along @p count samples and write each average.
 *
 * Index @p i is the colour `samples[(i - lo) * stride]`. Indices run over
 * `[lo, hi)`. Output @p n, `0 .. count`, is the average at index `first + n`
 * and is written `out_stride` colours after the previous one. The sums stay in
 * this call: a row does not go through ::lh_gl_blur_window_take.
 */
lh_void
lh_gl_blur_window_slide(const lh_gl_color_t *samples, lh_usize_t stride, lh_gl_scalar_t lo,
                        lh_gl_scalar_t hi, lh_gl_scalar_t radius, lh_gl_scalar_t first,
                        lh_gl_scalar_t count, lh_gl_color_t *out, lh_usize_t out_stride);

/**
 * @brief The window `[at - radius, at + radius + 1)` cut to `[lo, hi)`.
 *        @p win0 and @p win1 must not be null.
 */
lh_void
lh_gl_blur_window_span(lh_gl_scalar_t at, lh_gl_scalar_t radius, lh_gl_scalar_t lo,
                       lh_gl_scalar_t hi, lh_gl_scalar_t *win0, lh_gl_scalar_t *win1);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_BLUR_WINDOW_H */
