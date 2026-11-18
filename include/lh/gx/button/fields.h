/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_button_t.
 */

#ifndef LH_GX_BUTTON_FIELDS_H
#define LH_GX_BUTTON_FIELDS_H

#include <lh/core/bool.h>
#include <lh/core/ptr.h>

/**
 * @def lh_gx_button_fields(entity_type, style_type, click_fn, tap_type)
 * @brief The two entities of a button, its two looks, its reaction and its state.
 *
 * - `box` — the button itself: a box that takes input (::lh_gx_button_class);
 *   its content points back at the button;
 * - `caption` — a text (::lh_gx_text_class) owned by the box, centred in it;
 *   its content is the string;
 * - `style` — how it looks at rest; `pressed_style` — how it looks while it is
 *   pressed, or ::lh_null to look the same;
 * - `on_click`, `context` — the application's reaction and its data;
 * - `tap` — the gesture on the box (::lh_gx_tap_t): whether a press on it is
 *   in progress.
 *
 * None of the pointers owns: the styles are a theme's, the string, the reaction
 * and its context are the application's.
 *
 * @param entity_type Type of an entity (::lh_gx_entity_t).
 * @param style_type  Type of a style (::lh_gx_style_t).
 * @param click_fn    Type of the reaction (::lh_gx_button_click_fn).
 * @param tap_type    Type of the gesture (::lh_gx_tap_t).
 */
#define lh_gx_button_fields(entity_type, style_type, click_fn, tap_type)                           \
    entity_type box;                                                                               \
    entity_type caption;                                                                           \
    const style_type *style;                                                                       \
    const style_type *pressed_style;                                                               \
    click_fn *on_click;                                                                            \
    lh_ptr context;                                                                                \
    tap_type tap

#endif /* LH_GX_BUTTON_FIELDS_H */
