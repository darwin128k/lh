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
 * the last call — the rest of @p pixels is what the target already shows.
 *
 * @param context The target's own data.
 * @param pixels  The pixels drawn into; the caller's, valid for the call.
 * @param drawn   The rectangles that changed, in @p pixels' coordinates; at
 *                least one.
 */
typedef lh_void(lh_gx_present_show_fn)(lh_ptr context, const lh_gx_pixmap_t *pixels,
                                       const lh_gx_damage_t *drawn);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_PRESENT_SHOW_FN_H */
