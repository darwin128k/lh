/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_style_motion_t.
 */

#ifndef LH_GL_STYLE_MOTION_FIELDS_H
#define LH_GL_STYLE_MOTION_FIELDS_H

/**
 * @def lh_gl_style_motion_fields(easing_type)
 * @brief How a style is reached over time: the transition and its easing.
 *
 * `transition` is how long, in milliseconds, an entity takes to reach this
 * style from the one it had (`0` reaches it at once), and `easing` the curve
 * it moves on (::lh_gl_easing_t). A part of ::lh_gl_style_t that lives inside
 * it (::lh_gl_style_get_motion).
 *
 * @param easing_type  ::lh_gl_easing_t.
 */
#define lh_gl_style_motion_fields(easing_type)                                                     \
    lh_u16_t transition;                                                                           \
    easing_type easing

#endif /* LH_GL_STYLE_MOTION_FIELDS_H */
