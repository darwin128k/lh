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
 * ::lh_entity_button_set_repeat makes a hold: the click is sent when the
 * press starts and again on each tick while it is held. A corner radius
 * of 0 keeps the square style fill. A larger radius paints a rounded box
 * in that same color.
 * docs/scene.md shows how to use this class and how to write another.
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
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_ui_style_t *pressed_style;
    const lh_ui_style_t *rest_style;
    lh_bool_t pressed;
    lh_bool_t repeat;
    lh_int_t radius;
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

/**
 * @brief True when a held press repeats ::LH_ENTITY_EVENT_CLICKED.
 */
lh_bool_t
lh_entity_button_get_repeat(const lh_entity_button_t *self);

/**
 * @brief Send ::LH_ENTITY_EVENT_CLICKED on the press, and again while it
 *        is held, each time the window tick reaches the screen.
 */
lh_void
lh_entity_button_set_repeat(lh_entity_button_t *self, lh_bool_t repeat);

/**
 * @brief Corner radius of @p self, in pixels. 0 is a square.
 */
lh_int_t
lh_entity_button_get_radius(const lh_entity_button_t *self);

/**
 * @brief Round the box of @p self by @p radius pixels. 0 keeps the square
 *        style fill. The color stays the style's background.
 */
lh_void
lh_entity_button_set_radius(lh_entity_button_t *self, lh_int_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_BUTTON_H */
