/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_theme_t.
 */

#ifndef LH_UI_THEME_FIELDS_H
#define LH_UI_THEME_FIELDS_H

/**
 * @def lh_ui_theme_fields(style_type, color_type, bool_type)
 * @brief The styles a theme hands to entities, plus the accent and the
 *        border color. `dark` is the choice that filled the neutrals.
 *
 * @param style_type Type of a style (::lh_ui_style_t).
 * @param color_type Type of a color (::lh_ui_color_t).
 * @param bool_type  Type of `dark` (::lh_bool_t).
 */
#define lh_ui_theme_fields(style_type, color_type, bool_type)                                      \
    style_type surface;                                                                            \
    style_type text;                                                                               \
    style_type button;                                                                             \
    style_type button_pressed;                                                                     \
    style_type primary_style;                                                                      \
    color_type primary;                                                                            \
    color_type border;                                                                             \
    bool_type dark

#endif /* LH_UI_THEME_FIELDS_H */
