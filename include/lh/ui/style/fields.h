/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(paint_type, radius_type, font_type)
 * @brief Paint recipe for one entity.
 *
 * `fill` is held by value; the empty paint means no fill. `radius` rounds the
 * corners of the fill (`0` = square). `font` (not owned, may be ::lh_null)
 * and `text` (a paint by value; empty = no text) are how text is drawn. The
 * style itself is what several entities share.
 *
 * @param paint_type  Type of the fill and text paints.
 * @param radius_type Type of the corner radius.
 * @param font_type   Type of the font pointed at.
 */
#define lh_ui_style_fields(paint_type, radius_type, font_type)                                      \
    paint_type fill;                                                                                \
    radius_type radius;                                                                             \
    const font_type *font;                                                                          \
    paint_type text

#endif /* LH_UI_STYLE_FIELDS_H */
