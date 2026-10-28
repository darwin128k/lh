/**
 * @file window.h
 * @brief ::lh_ui_blur_window_t — the colour of a whole window summed in four channels, so a run is two reads per pixel rather than four calls. Red, green and blue are premultiplied by alpha; alpha is the straight sum.
 */

#ifndef LH_UI_BLUR_WINDOW_H
#define LH_UI_BLUR_WINDOW_H

#include <lh/ui/pixmap.h>
#include <lh/compiler/extern/c.h>

/**
 * @struct lh_ui_blur_window
 * @brief The colour of a whole window summed in four channels, so a run is two
 *        reads per pixel rather than four calls. Red, green and blue are
 *        premultiplied by alpha; alpha is the straight sum.
 */
struct lh_ui_blur_window
{
    lh_s32_t r; /**< Sum of premultiplied red. */
    lh_s32_t g; /**< Sum of premultiplied green. */
    lh_s32_t b; /**< Sum of premultiplied blue. */
    lh_s32_t a; /**< Sum of alpha. */
};
/** @copybrief lh_ui_blur_window */
typedef struct lh_ui_blur_window lh_ui_blur_window_t;

LH_COMPILER_EXTERN_C_BEGIN

/** Add @p pixel to the running sum. */
lh_void
lh_ui_blur_window_take(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel);

/** Take @p pixel back out — the pixel leaving the window as it slides. */
lh_void
lh_ui_blur_window_drop(lh_ui_blur_window_t *self, const lh_ui_color_t *pixel);

/**
 * @brief Channel @p channel (`0` red .. `3` alpha) of the average of @p count
 *        pixels, rounded and held to `0 .. 255`.
 */
lh_u8_t
lh_ui_blur_window_average(const lh_ui_blur_window_t *self, lh_s32_t channel, lh_s32_t count);

/**
 * @brief Add @p count scratch samples, @p stride bytes apart from @p sample, to
 *        the running sum. A @p count of `0` or less adds nothing.
 */
lh_void
lh_ui_blur_window_take_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride,
                               lh_s32_t count);

/**
 * @brief Take @p count scratch samples, @p stride bytes apart from @p sample,
 *        back out of the running sum. A @p count of `0` or less takes nothing.
 */
lh_void
lh_ui_blur_window_drop_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride,
                               lh_s32_t count);

/**
 * @brief The average of the window, all four channels
 *        (::lh_ui_blur_window_average), over @p count pixels into @p out.
 */
lh_void
lh_ui_blur_window_average_color(const lh_ui_blur_window_t *self, lh_s32_t count,
                                lh_ui_color_t *out);

/**
 * @brief The window `[at - radius, at + radius + 1)` cut to `[lo, hi)`, into
 *        @p win0 and @p win1. Neither may be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_blur_window_span(lh_s32_t at, lh_s32_t radius, lh_s32_t lo, lh_s32_t hi, lh_s32_t *win0,
                       lh_s32_t *win1);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_WINDOW_H */
