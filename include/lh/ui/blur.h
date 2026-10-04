/**
 * @file blur.h
 * @brief A blur effect (::lh_ui_effect_t).
 *
 * Averages the pixels already in the box. It does not read past the box, and
 * a radius of zero paints nothing. The record is not copied and not owned.
 */

#ifndef LH_UI_BLUR_H
#define LH_UI_BLUR_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/effect.h>

/**
 * @struct lh_ui_blur
 * @brief A blur effect.
 */
struct lh_ui_blur
{
    lh_ui_effect_fields(const lh_ui_effect_class_t *, const lh_ui_effect_t *);
    lh_int_t radius;
};
typedef struct lh_ui_blur lh_ui_blur_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A blur of radius zero.
 */
lh_void
lh_ui_blur_init(lh_ui_blur_t *self);

/**
 * @brief @p self as an effect.
 */
const lh_ui_effect_t *
lh_ui_blur_effect(const lh_ui_blur_t *self);

/**
 * @brief Blur radius in pixels.
 */
lh_int_t
lh_ui_blur_get_radius(const lh_ui_blur_t *self);

/**
 * @brief Set the blur radius. Negative becomes zero. Does not redraw
 *        by itself.
 */
lh_void
lh_ui_blur_set_radius(lh_ui_blur_t *self, lh_int_t radius);

/**
 * @brief Blur the pixels of @p canvas inside @p rect by @p radius.
 *        Zero and an empty rect do nothing.
 */
lh_void
lh_ui_blur_apply(lh_ui_canvas_t *canvas, lh_math_rect_t rect, lh_int_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_BLUR_H */
