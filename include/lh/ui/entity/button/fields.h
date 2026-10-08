/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_button_t.
 */

#ifndef LH_UI_ENTITY_BUTTON_FIELDS_H
#define LH_UI_ENTITY_BUTTON_FIELDS_H

/**
 * @def lh_ui_button_fields(entity_type, style_type, bool_type, cb_type, ptr_type)
 * @brief The entity this button is, the two styles it has, whether the pointer
 *        is on it, and who to tell about a click.
 *
 * The entity's own style pointer is what the engine reads
 * (::lh_ui_entity_get_style_now), and it always points at one of these two, so
 * the pressed look composes on top of whichever is showing:
 * `hot && pressed` is the pressed style *of the hot style*, when the app
 * declared one, and the hot style itself when it did not. Neither style is
 * owned; neither does the button keep a copy of what the pointer had.
 *
 * @param entity_type Type of the embedded entity.
 * @param style_type  Type of the styles pointed at.
 * @param bool_type   Type of the flag.
 * @param cb_type     ::lh_ui_button_on_click_cb.
 * @param ptr_type    ::lh_ptr.
 */
#define lh_ui_button_fields(entity_type, style_type, bool_type, cb_type, ptr_type)                    \
    entity_type entity;                                                                              \
    const style_type *rest_style;                                                                    \
    const style_type *hot_style;                                                                     \
    bool_type hot;                                                                                   \
    cb_type on_click;                                                                                \
    ptr_type click_context

#endif /* LH_UI_ENTITY_BUTTON_FIELDS_H */