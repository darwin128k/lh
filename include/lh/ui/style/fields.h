/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(color_type)
 * @brief How an entity is painted. `bg_color` is the background and
 *        `text_color` is the glyphs a label draws on it. Zero alpha is
 *        not painted.
 *
 * The rasterizer reads these fields directly. There is no style list and no
 * property search on the draw path: this is already the resolved description
 * of the fill, which is the part LVGL builds a draw descriptor for before
 * touching pixels.
 *
 * @param color_type Type of `bg_color` (::lh_ui_color_t).
 */
#define lh_ui_style_fields(color_type)                                                             \
    color_type bg_color;                                                                           \
    color_type text_color

#endif /* LH_UI_STYLE_FIELDS_H */
