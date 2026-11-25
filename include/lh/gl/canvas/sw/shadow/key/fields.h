/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_canvas_sw_shadow_key_t.
 */

#ifndef LH_GL_CANVAS_SW_SHADOW_KEY_FIELDS_H
#define LH_GL_CANVAS_SW_SHADOW_KEY_FIELDS_H

/**
 * @def lh_gl_canvas_sw_shadow_key_fields()
 * @brief The nine numbers a shadow mask is made for: its size, the corner, the
 *        box that casts it, the spread, the offset and the peak alpha.
 */
#define lh_gl_canvas_sw_shadow_key_fields()                                                        \
    lh_s32_t width;                                                                                \
    lh_s32_t height;                                                                               \
    lh_s32_t corner;                                                                               \
    lh_s32_t box_w;                                                                                \
    lh_s32_t box_h;                                                                                \
    lh_s32_t spread;                                                                               \
    lh_s32_t offset_x;                                                                             \
    lh_s32_t offset_y;                                                                             \
    lh_s32_t peak

#endif /* LH_GL_CANVAS_SW_SHADOW_KEY_FIELDS_H */
