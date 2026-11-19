/**
 * @file strip.h
 * @brief A frame drawn a band of rows at a time: ::lh_gx_strip_draw.
 *
 * A board rarely has the memory for a frame: a 320 by 240 screen in RGB565 is
 * 150 KB, and a small chip has less RAM than that in all. So the screen is
 * drawn through a band — a buffer as wide as the screen and a few rows high —
 * one band at a time: the renderer paints the part of a changed rectangle that
 * falls in the band (::lh_gx_render_paint_area) with the software backend's
 * origin moved to the band's first row, and the present (`lh/gx/present.h`)
 * sends the band before the next one is drawn into the same buffer. Only what
 * changed is drawn and sent.
 *
 * The band's pixels are the caller's — a static array, or a DMA buffer — and
 * so is the present: a panel's bus (SPI, a parallel port, a DMA), or a test.
 * What the buffer costs is the band: 320 pixels by 20 rows in RGB565 is
 * 12.5 KB for any screen height.
 */

#ifndef LH_GX_STRIP_H
#define LH_GX_STRIP_H

#include <lh/gx/backend/sw.h>
#include <lh/gx/present.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Draw what changed under @p render through @p band and show each band
 *        as soon as it is drawn.
 *
 * The damage is taken (::lh_gx_render_take_damage); each changed rectangle is
 * cut into runs of the band's rows, and for each run the backend's origin is
 * moved to it, the run is painted, and `show` is called with the band, its
 * place on the screen and the run as the one rectangle drawn.
 *
 * @param render  The renderer; its canvas is @p sw's. Must not be null.
 * @param sw      The software backend; its target is @p band and its origin is
 *                moved by this call. Must not be null.
 * @param band    The band's pixels: as wide as the screen, at least one row.
 *                Must not be null.
 * @param present The target's table; must be complete.
 * @param context The target's context, handed to `show`.
 * @return ::lh_bool_true when something was drawn and shown.
 */
lh_bool_t
lh_gx_strip_draw(lh_gx_render_t *render, lh_gx_backend_sw_t *sw, const lh_gx_pixmap_t *band,
                 const lh_gx_present_t *present, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_STRIP_H */
