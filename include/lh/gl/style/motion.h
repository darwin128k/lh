/**
 * @file motion.h
 * @brief How a style is reached over time: the transition and its easing: ::lh_gl_style_motion_t.
 *
 * `transition` is how long, in milliseconds, an entity takes to reach this
 * style from the one it had (`0` reaches it at once), and `easing` the curve
 * it moves on (::lh_gl_easing_t). A part of ::lh_gl_style_t that lives inside
 * it (::lh_gl_style_get_motion).
 */

#ifndef LH_GL_STYLE_MOTION_H
#define LH_GL_STYLE_MOTION_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/numeric/fixed/types.h>
#include <lh/core/void.h>
#include <lh/gl/easing.h>
#include <lh/gl/style/motion/fields.h>

/**
 * @struct lh_gl_style_motion
 * @brief How a style is reached over time: the transition and its easing.
 */
struct lh_gl_style_motion
{
    /** Members: ::lh_gl_style_motion_fields. */
    lh_gl_style_motion_fields(lh_gl_easing_t);
};
/** @copybrief lh_gl_style_motion */
typedef struct lh_gl_style_motion lh_gl_style_motion_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief How long, in milliseconds, reaching the style takes (level 1).
 *
 * @param self The motion; checked by ::lh_assert_runtime_ref.
 */
lh_u16_t
lh_gl_style_motion_get_transition(const lh_gl_style_motion_t *self);

/**
 * @brief Store how long, in milliseconds, reaching the style takes (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The motion; checked by ::lh_assert_runtime_ref.
 * @param transition The new value.
 */
lh_void
lh_gl_style_motion_set_transition(lh_gl_style_motion_t *self, lh_u16_t transition);

/**
 * @brief The curve the transition moves on (level 1).
 *
 * @param self The motion; checked by ::lh_assert_runtime_ref.
 */
lh_gl_easing_t
lh_gl_style_motion_get_easing(const lh_gl_style_motion_t *self);

/**
 * @brief Store the curve the transition moves on (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The motion; checked by ::lh_assert_runtime_ref.
 * @param easing The new value.
 */
lh_void
lh_gl_style_motion_set_easing(lh_gl_style_motion_t *self, lh_gl_easing_t easing);

/**
 * @brief Make @p self the part of a fresh style (::lh_gl_style_init): At once, on the default curve
 * (::lh_gl_easing_ease).
 *
 * @param self The motion; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gl_style_motion_init(lh_gl_style_motion_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_STYLE_MOTION_H */
