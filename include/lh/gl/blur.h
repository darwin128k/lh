/**
 * @file blur.h
 * @brief Soften pixels already on a ::lh_gl_pixmap_t, and frost a panel over them.
 *
 * Two box passes, rows then columns. Each pixel is the average of
 * `2 * radius + 1` neighbours, and the window slides: the next pixel adds the
 * one that enters and drops the one that leaves. The window is clamped to the
 * pixmap, not to the rectangle, so a panel sees the picture past its own edge.
 * Only the rectangle is written.
 *
 * The scratch is the caller's. ::lh_gl_blur_scratch_count says how many
 * ::lh_gl_color_t it has to hold: the rectangle, plus @p radius rows above it
 * and below it. Nothing here allocates.
 *
 * Glass is that blur, written only where the rounded panel covers, then the
 * tint painted over those pixels. The column is one slide of those rows. A
 * square corner the arc misses stays the picture that was there. A blur of
 * `0` is the tint alone. An opaque window sums straight channels and divides
 * by one factor for the whole run (`lh_gl_blur_window_round_factor`).
 */

#ifndef LH_GL_BLUR_H
#define LH_GL_BLUR_H

#include <lh/core/size.h>
#include <lh/gl/pixmap.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Colours of scratch a blur of @p rect at @p radius needs.
 *
 * An empty rectangle returns `0`. A radius of `0` or below is the rectangle
 * alone. Both margins are counted even when the rectangle sits on the pixmap
 * edge: this does not know the pixmap.
 */
lh_usize_t
lh_gl_blur_scratch_count(const lh_gl_rect_t *rect, lh_gl_scalar_t radius);

/**
 * @brief The rows the column pass has to already hold, cut to the pixmap.
 *        @p y0 and @p y1 must not be null. The scratch is addressed from @p y0.
 */
lh_void
lh_gl_blur_band(const lh_gl_pixmap_t *pixmap, lh_gl_scalar_t top, lh_gl_scalar_t height,
                lh_gl_scalar_t radius, lh_gl_scalar_t *y0, lh_gl_scalar_t *y1);

/**
 * @brief Blur one pixmap row across. @p out receives @p width colours, the
 *        rectangle only. The window reads up to @p radius pixels past it.
 */
lh_void
lh_gl_blur_row(const lh_gl_pixmap_t *pixmap, lh_gl_scalar_t y, lh_gl_scalar_t left,
               lh_gl_scalar_t width, lh_gl_scalar_t radius, lh_gl_color_t *out);

/**
 * @brief Blur @p rect along its rows into @p scratch. Nothing is written to
 *        @p pixmap. A radius below `0`, an empty rectangle, or a null scratch
 *        leaves the scratch alone.
 */
lh_void
lh_gl_blur_rows(const lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                lh_gl_color_t *scratch);

/**
 * @brief Write one blurred colour. A @p shape with @p corner above `0` skips a
 *        pixel the rounded rectangle does not cover. @p tint, when set, is
 *        then painted over that pixel.
 */
lh_void
lh_gl_blur_put(lh_gl_pixmap_t *pixmap, lh_gl_scalar_t x, lh_gl_scalar_t y,
               const lh_gl_color_t *color, const lh_gl_rect_t *shape, lh_gl_scalar_t corner,
               const lh_gl_color_t *tint);

/**
 * @brief Blur column @p x (from the rectangle's left) of @p scratch down into
 *        @p pixmap. @p scratch is what ::lh_gl_blur_rows wrote, addressed from
 *        @p y0. One slide covers the rows the rounded @p shape includes, or
 *        the whole column when @p shape is null. @p tint is painted over those
 *        rows afterwards.
 */
lh_void
lh_gl_blur_column(lh_gl_pixmap_t *pixmap, const lh_gl_color_t *scratch, lh_gl_scalar_t left,
                  lh_gl_scalar_t top, lh_gl_scalar_t width, lh_gl_scalar_t height,
                  lh_gl_scalar_t radius, lh_gl_scalar_t y0, lh_gl_scalar_t y1, lh_gl_scalar_t x,
                  const lh_gl_rect_t *shape, lh_gl_scalar_t corner, const lh_gl_color_t *tint);

/**
 * @brief Blur @p rect of @p pixmap in place. Cuts @p rect to the pixmap first.
 *        An empty cut, a radius of `0` or below, or a null scratch writes nothing.
 */
lh_void
lh_gl_blur_rect(lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *rect, lh_gl_scalar_t radius,
                lh_gl_color_t *scratch);

/**
 * @brief Frost @p panel: blur it by @p blur, write only where the rounded
 *        rectangle of @p corner covers, then paint @p tint over those pixels.
 *
 * @p clip, when set, further limits the write. The blur still reads past that
 *        limit, up to @p blur pixels, and no further than the pixmap. A null
 *        @p tint blurs and does not paint. A @p blur of `0` or below paints the
 *        tint alone and does not read @p scratch.
 */
lh_void
lh_gl_glass(lh_gl_pixmap_t *pixmap, const lh_gl_rect_t *panel, lh_gl_scalar_t corner,
            lh_gl_scalar_t blur, const lh_gl_color_t *tint, lh_gl_color_t *scratch,
            const lh_gl_rect_t *clip);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_BLUR_H */
