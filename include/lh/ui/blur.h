/**
 * @file blur.h
 * @brief Soften what is already on a target: ::lh_ui_blur_rect.
 *
 * The only effect that reads pixels it did not write, which is why it is about a
 * ::lh_ui_pixmap_t and not about a shape: a panel has to blur the picture
 * *behind* it, and the picture is whatever the frame left there.
 *
 * Two box passes, rows then columns, each averaging `2 * radius + 1` neighbours.
 * The window is clamped to the pixmap, not to the rect: what is behind a panel
 * does not stop at the panel's edge, and only the rect is written. Two boxes
 * make a triangle under the hood, so the result is soft without being a gaussian
 * — and unlike a gaussian it costs two reads and four adds per pixel per pass,
 * because the window slides instead of being added up again.
 *
 * A channel is summed premultiplied, colour times alpha. A fully transparent
 * pixel still holds colour bytes, and averaging those straight is what tints
 * the blur with a colour nobody can see.
 *
 * The two passes are separate functions because each is useful on its own and
 * because this tree keeps no file-scope state: a caller that already has a buffer
 * of ::lh_ui_blur_scratch_size bytes can pass it in and pay no allocation.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_BLUR_H
#define LH_UI_BLUR_H

#include <lh/bool.h>
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

/**
 * @struct lh_ui_blur_pass
 * @brief What the column pass of ::lh_ui_blur_columns reads for every column:
 *        filled by ::lh_ui_blur_pass_init.
 */
struct lh_ui_blur_pass
{
    const lh_u8_t *scratch;      /**< What ::lh_ui_blur_rows left, addressed from row `y0`. */
    const lh_ui_rect_t *shape;   /**< Rounded shape to cut the write to, or ::lh_null. */
    lh_ui_scalar_t corner;       /**< Its corner radius, clamped; `0` with no shape. */
    lh_s32_t left;               /**< Rect left column. */
    lh_s32_t top;                /**< Rect top row. */
    lh_s32_t width;              /**< Rect width. */
    lh_s32_t height;             /**< Rect height. */
    lh_s32_t radius;             /**< Blur radius in pixels. */
    lh_s32_t y0;                 /**< First row held in the scratch. */
    lh_s32_t y1;                 /**< One past the last row held in the scratch. */
};
/** @copybrief lh_ui_blur_pass */
typedef struct lh_ui_blur_pass lh_ui_blur_pass_t;

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
 * @brief Bytes of scratch ::lh_ui_blur_rect needs: the rect, plus @p radius rows
 *        above it and below it, four bytes a pixel.
 *
 * @p radius `0` (or negative) is the rect alone. Rows that fall outside the
 * pixmap are not stored, but this count does not know the pixmap, so both
 * margins are always included. The column pass reads what the row pass wrote,
 * and that band is taller than the rect precisely so a pixel on the rect's edge
 * can see the picture past it.
 */
lh_usize_t
lh_ui_blur_scratch_size(const lh_ui_rect_t *rect, lh_s32_t radius);

/**
 * @brief Blur @p rect along its rows, reading up to @p radius pixels past it.
 *
 * Writes the rect's columns, and up to @p radius rows above and below it, as
 * RGBA8 into @p scratch — the picture the column pass reads. Nothing is written
 * to @p pixmap here. The band is addressed from the first row that exists, so
 * a rect on the top of the pixmap has no rows above it.
 */
lh_void
lh_ui_blur_rows(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, lh_u8_t *scratch);

/**
 * @brief The colour of one RGBA scratch @p sample (four bytes) into @p out.
 */
lh_void
lh_ui_blur_sample_color(const lh_u8_t *sample, lh_ui_color_t *out);

/**
 * @brief Add @p count scratch samples, @p stride bytes apart from @p sample, to
 *        the running sum. A @p count of `0` or less adds nothing.
 */
lh_void
lh_ui_blur_window_take_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride, lh_s32_t count);

/**
 * @brief Take @p count scratch samples, @p stride bytes apart from @p sample,
 *        back out of the running sum. A @p count of `0` or less takes nothing.
 */
lh_void
lh_ui_blur_window_drop_samples(lh_ui_blur_window_t *self, const lh_u8_t *sample, lh_usize_t stride, lh_s32_t count);

/**
 * @brief The average of the window, all four channels
 *        (::lh_ui_blur_window_average), over @p count pixels into @p out.
 */
lh_void
lh_ui_blur_window_average_color(const lh_ui_blur_window_t *self, lh_s32_t count, lh_ui_color_t *out);

/**
 * @brief The window `[at - radius, at + radius + 1)` cut to `[lo, hi)`, into
 *        @p win0 and @p win1. Neither may be null (::lh_assert_runtime_ref).
 */
lh_void
lh_ui_blur_window_span(lh_s32_t at, lh_s32_t radius, lh_s32_t lo, lh_s32_t hi, lh_s32_t *win0, lh_s32_t *win1);

/**
 * @brief Prepare the column pass of @p rect over @p pixmap into @p out
 *        (parameters as ::lh_ui_blur_columns). False when there is nothing to
 *        write: a negative @p radius, null @p scratch or an empty rect.
 *
 * @p out must not be null (::lh_assert_runtime_ref).
 */
lh_bool_t
lh_ui_blur_pass_init(const lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch,
                     const lh_ui_rect_t *shape, lh_ui_scalar_t corner, lh_ui_blur_pass_t *out);

/**
 * @brief Blur column @p x (from the rect's left) of a prepared @p pass down
 *        into @p pixmap, cut to the pass's shape.
 */
lh_void
lh_ui_blur_column(lh_ui_pixmap_t *pixmap, const lh_ui_blur_pass_t *pass, lh_s32_t x);

/**
 * @brief Blur @p rect down its columns, reading @p scratch, writing @p pixmap.
 *
 * The other half of ::lh_ui_blur_rect: this is the pass that writes, and @p
 * scratch has to be what ::lh_ui_blur_rows left in it. Only the rect is written.
 *
 * @p shape and @p corner cut that write to a rounded rect. ::lh_null @p shape,
 * or a @p corner of `0`, writes every pixel. A pixel the rounded shape does not
 * cover is left as it was, and one it covers in part is replaced in that part:
 * the blur of a glass panel is the panel, and the square corners of its box
 * are not part of it.
 */
lh_void
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius, const lh_u8_t *scratch,
                   const lh_ui_rect_t *shape, lh_ui_scalar_t corner);

/**
 * @brief Blur @p rect of @p pixmap in place, @p radius pixels out.
 *
 * Cuts @p rect to the pixmap first, so a rect hanging over the edge blurs the
 * part that is there and stops. An empty rect, or radius `0`, leaves @p pixmap
 * alone.
 *
 * @p scratch is the caller's buffer for the row pass, and it has to be
 * ::lh_ui_blur_scratch_size of the rect and @p radius (of the cut rect, which is
 * never bigger).
 * The blur does not allocate: it is a drawing step on somebody else's frame, and
 * a frame is not the place to reach for a heap. A null @p scratch leaves
 * @p pixmap alone.
 */
lh_void
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius, lh_u8_t *scratch);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_H */