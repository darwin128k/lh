/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_style_t.
 */

#ifndef LH_GL_STYLE_FIELDS_H
#define LH_GL_STYLE_FIELDS_H

/**
 * @def lh_gl_style_fields(box_type, typography_type, pointer_type, motion_type)
 * @brief Paint recipe for one entity, in four parts.
 *
 * - `box` (::lh_gl_style_box_t): the fill, its corner radius and radii, the
 *   padding, the shadow, and the border with its paint;
 * - `typography` (::lh_gl_style_typography_t): the font, the text paint, the
 *   alignment, the overflow and the wrap;
 * - `pointer` (::lh_gl_style_pointer_t): the corner radius hits are tested
 *   against and the pressed look;
 * - `motion` (::lh_gl_style_motion_t): the transition time and its easing.
 *
 * Each part is a struct of its own with its own level-1 accessors; the style
 * holds them by value, so a style is still one block that several entities
 * share.
 *
 * @param box_type        ::lh_gl_style_box_t.
 * @param typography_type ::lh_gl_style_typography_t.
 * @param pointer_type    ::lh_gl_style_pointer_t.
 * @param motion_type     ::lh_gl_style_motion_t.
 */
#define lh_gl_style_fields(box_type, typography_type, pointer_type, motion_type)                   \
    box_type box;                                                                                  \
    typography_type typography;                                                                    \
    pointer_type pointer;                                                                          \
    motion_type motion

#endif /* LH_GL_STYLE_FIELDS_H */
