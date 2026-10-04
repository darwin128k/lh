/**
 * @file tabs.h
 * @brief A rounded bar of buttons. One of them is active.
 *
 * The bar stays square until ::lh_entity_tabs_set_radius. The active button
 * is a plain fill of the text color. ::lh_entity_tabs_add adds a button.
 * The buttons share the bar. A press selects that button and sends
 * ::LH_ENTITY_EVENT_CLICKED.
 * Pages are a separate entity.
 */

#ifndef LH_ENTITY_TABS_H
#define LH_ENTITY_TABS_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/2d.h>
#include <lh/numeric/types.h>
#include <lh/ui/font.h>
#include <lh/ui/style.h>

/**
 * @struct lh_entity_tabs
 * @brief Buttons, and which one is active.
 */
struct lh_entity_tabs
{
    lh_entity_fields(lh_entity_class_t, lh_list_node_t, lh_list_t, lh_entity_flags_t);
    lh_entity_2d_fields(lh_math_vec2_t, lh_float_t, const lh_ui_style_t *, const lh_ui_effect_t *);
    const lh_ui_font_t *font;
    const lh_ui_style_t *tab_style;
    lh_ui_style_t on_title;
    lh_int_t index;
    lh_int_t radius;
};
typedef struct lh_entity_tabs lh_entity_tabs_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_tabs_t, derived from ::lh_entity_2d_class.
 */
extern const lh_entity_class_t lh_entity_tabs_class;

/**
 * @brief Font of the titles. Not owned.
 */
lh_void
lh_entity_tabs_set_font(lh_entity_tabs_t *self, const lh_ui_font_t *font);

/**
 * @brief Style of the bar. The active button is filled with the text color.
 *        Not owned.
 */
lh_void
lh_entity_tabs_set_style(lh_entity_tabs_t *self, const lh_ui_style_t *style);

/**
 * @brief Which button is active. The first one added is 0.
 */
lh_int_t
lh_entity_tabs_get_index(const lh_entity_tabs_t *self);

/**
 * @brief Select button @p index.
 */
lh_void
lh_entity_tabs_set_index(lh_entity_tabs_t *self, lh_int_t index);

/**
 * @brief Add a button titled @p title. @p title is not copied.
 */
lh_void
lh_entity_tabs_add(lh_entity_tabs_t *self, const lh_char_t *title);

/**
 * @brief Corner radius of @p self, in pixels. 0 is a square.
 */
lh_int_t
lh_entity_tabs_get_radius(const lh_entity_tabs_t *self);

/**
 * @brief Round the bar of @p self by @p radius pixels. 0 keeps the square.
 */
lh_void
lh_entity_tabs_set_radius(lh_entity_tabs_t *self, lh_int_t radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_TABS_H */
