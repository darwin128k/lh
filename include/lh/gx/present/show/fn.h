/**
 * @file fn.h
 * @brief Function type ::lh_gx_present_show_fn.
 */

#ifndef LH_GX_PRESENT_SHOW_FN_H
#define LH_GX_PRESENT_SHOW_FN_H

#include <lh/gx/damage.h>
#include <lh/gx/pixmap.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_present_show_fn
 * @brief Make the rectangles @p drawn of @p pixels visible: the frame just drawn
 *        reaches the screen.
 *
 * Required. What "visible" means is the target's: a window asks its system to
 * repaint those rectangles and copies the pixels when it does; a panel sends
 * them over its bus; a test writes them down. Only @p drawn has changed since
 * the last call.
 *
 * The pixels need not be the whole screen: a board with little memory draws a
 * band of rows at a time (`lh/gx/strip.h`), so @p at says where the pixels'
 * top-left pixel is on the screen, and @p drawn is on the screen too — the
 * pixel of the screen point `(x, y)` is the pixel `(x - at.x, y - at.y)` of
 * @p pixels. A window's surface is the whole screen, at `(0, 0)`.
 *
 * @param context The target's own data.
 * @param pixels  The pixels drawn into; the caller's, valid for the call.
 * @param at      Where @p pixels' top-left pixel is on the screen.
 * @param drawn   The rectangles that changed, screen coordinates, inside
 *                @p pixels; at least one.
 */
typedef lh_void(lh_gx_present_show_fn)(lh_ptr context, const lh_gx_pixmap_t *pixels,
                                       lh_gx_point_t at, const lh_gx_damage_t *drawn);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_PRESENT_SHOW_FN_H */
