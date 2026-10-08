/**
 * @file blur.h
 * @brief Soften what is already on a target: ::lh_ui_blur_rect.
 *
 * The only effect that reads pixels it did not write, which is why it is about a
 * ::lh_ui_pixmap_t and not about a shape: a panel has to blur the picture
 * *behind* it, and the picture is whatever the frame left there.
 *
 * Two box passes, rows then columns, each averaging `2 * radius + 1` neighbours
 * with the edge clamped. Two boxes make a triangle under the hood, so the result
 * is soft without being a gaussian — and unlike a gaussian it costs two reads and
 * four adds per pixel per pass, because the window slides instead of being added
 * up again.
 *
 * The two passes are separate functions because each is useful on its own and
 * because this tree keeps no file-scope state: a caller that already has a buffer
 * of ::lh_ui_blur_scratch_size bytes can pass it in and pay no allocation.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_BLUR_H
#define LH_UI_BLUR_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/blur.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

/**
 * @struct lh_ui_blur_window
 * @typedef lh_ui_blur_window_t
 * @brief The colour of a whole window summed in four channels, so a run is two
 *        reads per pixel rather than four calls.
 */
struct lh_ui_blur_window
{
    lh_s32_t r;
    lh_s32_t g;
    lh_s32_t b;
    lh_s32_t a;
};
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
 * @brief Bytes of scratch ::lh_ui_blur_rect needs for @p rect: one RGBA8 pixel
 *        per pixel, both passes go through the same buffer.
 */
lh_usize_t
lh_ui_blur_scratch_size(const lh_ui_rect_t *rect);

/**
 * @brief Blur @p rect along its rows.
 *
 * Writes `width * height` RGBA8 pixels into @p scratch — the picture the column
 * pass reads. Nothing is written to @p pixmap here.
 */
lh_void
lh_ui_blur_rows(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, lh_u8_t *scratch);

/**
 * @brief Blur @p rect down its columns, reading @p scratch, writing @p pixmap.
 *
 * The other half of ::lh_ui_blur_rect: this is the pass that writes, and @p
 * scratch has to be what ::lh_ui_blur_rows left in it.
 */
lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch);

/**
 * @brief Blur @p rect of @p pixmap in place, @p radius pixels out.
 *
 * Cuts @p rect to the pixmap first, so a rect hanging over the edge blurs the
 * part that is there and stops. An empty rect, or radius `0`, leaves @p pixmap
 * alone.
 *
 * @p scratch is the caller's buffer for the row pass, and it has to be
 * ::lh_ui_blur_scratch_size of the rect (of the cut rect, which is never bigger).
 * The blur does not allocate: it is a drawing step on somebody else's frame, and
 * a frame is not the place to reach for a heap. A null @p scratch leaves
 * @p pixmap alone.
 */
lh_void
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_u8_t *scratch);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_H */