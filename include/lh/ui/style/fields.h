/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(paint_type, radius_type, font_type, insets_type)
 * @brief Paint recipe for one entity.
 *
 * `fill` is held by value; the empty paint means no fill. `radius` rounds the
 * corners of the fill (`0` = square). `padding` keeps content (a label's
 * text, a layout's children) that far inside the rect, per side. `font` (not owned, may be ::lh_null)
 * and `text` (a paint by value; empty = no text) are how text is drawn, and
 * `align_h` / `align_v` are where inside the padded box it starts. The
 * style itself is what several entities share.
 *
 * @param paint_type  Type of the fill and text paints.
 * @param radius_type Type of the corner radius.
 * @param font_type   Type of the font pointed at.
 * @param insets_type ::lh_ui_insets_t.
 */
#define lh_ui_style_fields(paint_type, radius_type, font_type, insets_type)                         \
    paint_type fill;                                                                                \
    radius_type radius;                                                                             \
    insets_type padding;                                                                            \
    const font_type *font;                                                                          \
    paint_type text;                                                                                \
    lh_ui_text_align_h_t align_h;                                                                   \
    lh_ui_text_align_v_t align_v

#endif /* LH_UI_STYLE_FIELDS_H */
