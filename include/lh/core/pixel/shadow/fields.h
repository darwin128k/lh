/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_pixel_shadow_t.
 */

#ifndef LH_CORE_PIXEL_SHADOW_FIELDS_H
#define LH_CORE_PIXEL_SHADOW_FIELDS_H

/**
 * @def lh_pixel_shadow_fields(box_type, length_type, wide_type)
 * @brief A soft shadow laid off one rounded box, as whole numbers a pixel is asked
 *        with.
 *
 * `box` is the box that casts it (whose wholly covered pixels the shadow leaves
 * alone); in ::LH_PIXEL_ROUND_SUBPIXEL units, `cx`, `cy` are the middle of the
 * shifted box, `hx`, `hy` how far its straight edges reach from the middle, `r` its
 * corners' radius and `spread` how far the fade reaches either side of the edge;
 * `peak` is the alpha a spread inside the edge, and `start`, `step` the fade
 * `peak * (spread - d) / (2 * spread)` in 16.16: `(start - d * step) >> 16`.
 *
 * @param box_type    Type of the box (::lh_pixel_round_box_t).
 * @param length_type Type of a length (::lh_s32_t).
 * @param wide_type   Type of the fade's terms (::lh_s64_t).
 */
#define lh_pixel_shadow_fields(box_type, length_type, wide_type)                                   \
    box_type box;                                                                                  \
    length_type cx;                                                                                \
    length_type cy;                                                                                \
    length_type hx;                                                                                \
    length_type hy;                                                                                \
    length_type r;                                                                                 \
    length_type spread;                                                                            \
    length_type peak;                                                                              \
    wide_type start;                                                                               \
    wide_type step

#endif /* LH_CORE_PIXEL_SHADOW_FIELDS_H */
