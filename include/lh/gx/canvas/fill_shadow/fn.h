/**
 * @file fn.h
 * @brief Function type ::lh_gx_canvas_fill_shadow_fn.
 */

#ifndef LH_GX_CANVAS_FILL_SHADOW_FN_H
#define LH_GX_CANVAS_FILL_SHADOW_FN_H

#include <lh/gx/shadow.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gx_canvas_fill_shadow_fn
 * @brief Lay the soft @p shadow of @p box, with corners of @p radius, inside @p clip.
 *
 * Optional, and without it no shadow is drawn: a shadow is an effect, not a shape
 * the rest of the drawing comes down to, so there is no fallback through the other
 * slots — a canvas that does not draw shadows links none of their code. Every
 * pixel's alpha is the core's reading of the shadow (::lh_gx_shadow_get_pixel_shadow,
 * `lh/core/pixel/shadow.h`), laid in the shadow's colour over what is there; the
 * pixels @p box covers wholly are left to the box drawn over it.
 *
 * @param context The backend's context — where it draws.
 * @param clip    Nothing outside it is touched; screen coordinates.
 * @param box     The box that casts the shadow; screen coordinates.
 * @param radius  Its corners' radius, pixels, at most half its shorter side.
 * @param shadow  The shadow; the caller's.
 */
typedef lh_void(lh_gx_canvas_fill_shadow_fn)(lh_ptr context, const lh_gx_rect_t *clip,
                                             const lh_gx_rect_t *box, lh_gx_scalar_t radius,
                                             const lh_gx_shadow_t *shadow);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CANVAS_FILL_SHADOW_FN_H */
