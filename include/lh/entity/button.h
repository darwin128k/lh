/**
 * @file button.h
 * @brief An entity that clicks (LVGL's `lv_btn`).
 *
 * It is a 2D entity. Its box is the hit area and the style paints it.
 * The words on it are a ::lh_entity_label_t placed inside it, not a string
 * stored on the button. That label has to let pointer events bubble, so a
 * press on the words reaches the button. A press followed by a release
 * over the same button sends ::LH_ENTITY_EVENT_CLICKED to it. While it is
 * down, and a pressed style was given, that style paints the box instead.
 */

#ifndef LH_ENTITY_BUTTON_H
#define LH_ENTITY_BUTTON_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>

/**
 * @struct lh_entity_button
 * @brief A 2D entity that reports a click.
 */
struct lh_entity_button
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *);
    const lh_ui_style_t *pressed_style;
    const lh_ui_style_t *rest_style;
    lh_bool_t pressed;
};
typedef struct lh_entity_button lh_entity_button_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_button_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_button_class;

/**
 * @brief True while the pointer is down on @p self.
 */
lh_bool_t
lh_entity_button_is_pressed(const lh_entity_button_t *self);

/**
 * @brief The style used while @p self is down, or ::lh_null for none.
 */
const lh_ui_style_t *
lh_entity_button_get_pressed_style(const lh_entity_button_t *self);

/**
 * @brief Use @p style while @p self is down. ::lh_null keeps the ordinary
 *        style. Not owned, and not copied.
 */
lh_void
lh_entity_button_set_pressed_style(lh_entity_button_t *self, const lh_ui_style_t *style);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_BUTTON_H */
