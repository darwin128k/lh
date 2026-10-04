/**
 * @file glass.h
 * @brief A glass effect (::lh_ui_effect_t): blur the box, then tint it.
 *
 * The blur is ::lh_ui_blur_apply. The tint is a straight color laid over the
 * blurred pixels; its alpha is how milky the glass is. The record is not
 * copied and not owned.
 */

#ifndef LH_UI_GLASS_H
#define LH_UI_GLASS_H

#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ui/color.h>
#include <lh/ui/effect.h>

/**
 * @struct lh_ui_glass
 * @brief A glass effect.
 */
struct lh_ui_glass
{
    lh_ui_effect_fields(const lh_ui_effect_class_t *, const lh_ui_effect_t *);
    lh_int_t radius;
    lh_ui_color_t tint;
};
typedef struct lh_ui_glass lh_ui_glass_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Clear glass: radius zero and a transparent tint.
 */
lh_void
lh_ui_glass_init(lh_ui_glass_t *self);

/**
 * @brief @p self as an effect.
 */
const lh_ui_effect_t *
lh_ui_glass_effect(const lh_ui_glass_t *self);

/**
 * @brief Blur radius in pixels.
 */
lh_int_t
lh_ui_glass_get_radius(const lh_ui_glass_t *self);

/**
 * @brief Set the blur radius. Negative becomes zero. Does not redraw
 *        by itself.
 */
lh_void
lh_ui_glass_set_radius(lh_ui_glass_t *self, lh_int_t radius);

/**
 * @brief Color laid over the blurred box.
 */
lh_ui_color_t
lh_ui_glass_get_tint(const lh_ui_glass_t *self);

/**
 * @brief Set the tint. Does not redraw by itself.
 */
lh_void
lh_ui_glass_set_tint(lh_ui_glass_t *self, lh_ui_color_t tint);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_GLASS_H */
