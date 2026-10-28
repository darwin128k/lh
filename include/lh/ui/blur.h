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

#include <lh/ui/pixmap.h>
#include <lh/ui/blur/window.h>
#include <lh/ui/blur/pass.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/blur.h requires LH_LIBRARY_OPTION_UI (CMake: -DLH_LIBRARY_OPTION_UI=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

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

/** Rows the column pass has to already hold: the rect, and up to @p radius rows
   past it on each side, cut to the pixmap. The scratch is addressed from @p y0. */
lh_void
lh_ui_blur_band(const lh_ui_pixmap_t *pixmap, lh_s32_t top, lh_s32_t height, lh_s32_t radius,
                lh_s32_t *y0, lh_s32_t *y1);

/** One row, blurred across. The window reads the pixmap, out to @p radius past
   the rect and no further than the pixmap; @p out receives only the rect. */
lh_void
lh_ui_blur_row_x(const lh_ui_pixmap_t *pixmap, lh_s32_t y, lh_s32_t left, lh_s32_t width,
                 lh_s32_t radius, lh_u8_t *out);

/**
 * @brief Blur @p rect along its rows, reading up to @p radius pixels past it.
 *
 * Writes the rect's columns, and up to @p radius rows above and below it, as
 * RGBA8 into @p scratch — the picture the column pass reads. Nothing is written
 * to @p pixmap here. The band is addressed from the first row that exists, so
 * a rect on the top of the pixmap has no rows above it.
 */
lh_void
lh_ui_blur_rows(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius,
                lh_u8_t *scratch);

/** Write a blurred pixel, cut to the rounded @p shape when there is one: kept out of its square
 * corners, blended on its edge. */
lh_void
lh_ui_blur_put(lh_ui_pixmap_t *pixmap, lh_s32_t x, lh_s32_t y, const lh_ui_color_t *color,
               const lh_ui_rect_t *shape, lh_ui_scalar_t corner);

/**
 * @brief The colour of one RGBA scratch @p sample (four bytes) into @p out.
 */
lh_void
lh_ui_blur_sample_color(const lh_u8_t *sample, lh_ui_color_t *out);

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
lh_ui_blur_columns(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_s32_t radius,
                   const lh_u8_t *scratch, const lh_ui_rect_t *shape, lh_ui_scalar_t corner);

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
lh_ui_blur_rect(lh_ui_pixmap_t *pixmap, const lh_ui_rect_t *rect, lh_ui_scalar_t radius,
                lh_u8_t *scratch);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_H */