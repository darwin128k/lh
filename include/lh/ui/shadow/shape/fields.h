/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_shadow_shape_t.
 */

#ifndef LH_UI_SHADOW_SHAPE_FIELDS_H
#define LH_UI_SHADOW_SHAPE_FIELDS_H

/**
 * @def lh_ui_shadow_shape_fields(rect_type, scalar_type)
 * @brief A shadow over one box, worked out once: the box (whose wholly covered pixels
 *        the shadow leaves to the fill) and its corner, the shifted box's middle and
 *        half sizes less the corner, the corner and the fade in
 *        ::LH_UI_RADIUS_SUBPIXEL units, the peak, and the fade as a slope: `start`
 *        and `step` in 16.16, so a pixel's fade is a multiply and a shift.
 *
 * @param rect_type   ::lh_ui_rect_t.
 * @param scalar_type ::lh_ui_scalar_t.
 */
#define lh_ui_shadow_shape_fields(rect_type, scalar_type)                                           \
    rect_type box;                                                                                  \
    scalar_type corner;                                                                             \
    lh_s32_t box_x0;                                                                                \
    lh_s32_t box_x1;                                                                                \
    lh_s32_t box_y0;                                                                                \
    lh_s32_t box_y1;                                                                                \
    lh_s32_t cx;                                                                                    \
    lh_s32_t cy;                                                                                    \
    lh_s32_t hx;                                                                                    \
    lh_s32_t hy;                                                                                    \
    lh_s32_t r;                                                                                     \
    lh_s32_t spread;                                                                                \
    lh_s32_t peak;                                                                                  \
    lh_s64_t start;                                                                                 \
    lh_s64_t step

#endif /* LH_UI_SHADOW_SHAPE_FIELDS_H */
