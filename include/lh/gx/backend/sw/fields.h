/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_backend_sw_t.
 */

#ifndef LH_GX_BACKEND_SW_FIELDS_H
#define LH_GX_BACKEND_SW_FIELDS_H

/**
 * @def lh_gx_backend_sw_fields(pixmap_type, point_type)
 * @brief Where the software backend draws: the pixels, and where they sit on
 *        the screen.
 *
 * `target` is the caller's pixmap; the backend writes into its memory and owns
 * none of it. `origin` is the screen position of the target's top-left pixel:
 * `0, 0` for a buffer the size of the screen, the top of the strip for a strip
 * buffer that the renderer moves down the screen.
 *
 * @param pixmap_type Type of the target (::lh_gx_pixmap_t).
 * `shadow` is the caller's mask the backend keeps a shadow's alphas in for the
 * next one like it (::lh_gx_backend_sw_shadow_t), ::lh_null for none.
 *
 * @param point_type  Type of the origin (::lh_math_point_t, whole pixels).
 * @param shadow_type Type of the shadow mask (::lh_gx_backend_sw_shadow_t).
 */
#define lh_gx_backend_sw_fields(pixmap_type, point_type, shadow_type)                              \
    pixmap_type *target;                                                                           \
    point_type origin;                                                                             \
    shadow_type *shadow

#endif /* LH_GX_BACKEND_SW_FIELDS_H */
