/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(color_type, int_type)
 * @brief How an entity is painted. `bg_color` is the background and
 *        `text_color` is the glyphs a label draws on it. `border_color` with a
 *        non-zero `border_width` is the outline, and `radius` rounds the
 *        corners. Zero alpha is not painted, zero width is no outline and zero
 *        radius is a square.
 *
 * The rasterizer reads these fields directly. There is no style list and no
 * property search on the draw path: this is already the resolved description
 * of the fill, which is the part LVGL builds a draw descriptor for before
 * touching pixels.
 *
 * @param color_type Type of `bg_color` (::lh_ui_color_t).
 * @param int_type   Type of `border_width` and `radius` (::lh_int_t).
 */
#define lh_ui_style_fields(color_type, int_type)                                                    \
    color_type bg_color;                                                                           \
    color_type text_color;                                                                         \
    color_type border_color;                                                                       \
    int_type border_width;                                                                         \
    int_type radius

#endif /* LH_UI_STYLE_FIELDS_H */
