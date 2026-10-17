/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_style_t.
 */

#ifndef LH_UI_STYLE_FIELDS_H
#define LH_UI_STYLE_FIELDS_H

/**
 * @def lh_ui_style_fields(paint_type, radius_type, font_type, insets_type, shadow_type, radii_type)
 * @brief Paint recipe for one entity.
 *
 * `fill` is held by value; the empty paint means no fill. `radius` rounds the
 * corners of the fill (`0` = square), `radii` rounds each of them on its own (the
 * fill is drawn with these; `radius` is the smallest of them, which is what a
 * clipping parent cuts its children with), and `hit_radius` rounds the corners of
 * what a hit test accepts (`0` = the whole rect — a rounded look is pressed by
 * its rect, which is what a finger aims at). `padding` keeps content (a label's
 * text, a layout's children) that far inside the rect, per side. `font` (not owned, may be
 * ::lh_null) and `text` (a paint by value; empty = no text) are how text is drawn, and `align_h` /
 * `align_v` are where inside the padded box it starts. `border` is how many rows each edge paints
 * in `border_fill` after the children (0, and an empty paint, paint nothing): a 1px line under a
 * row is that edge, not an entity of its own. `shadow` is cast by the fill before it (the empty one
 * casts nothing), and `pressed` is the style in effect while the entity is pressed (not owned,
 * ::lh_null = it looks the same either way). `overflow` is what a line of text too wide for the
 * padded box draws (::lh_ui_text_overflow_t), and `wrap` whether the text is broken
 * into lines that fit it (::lh_ui_text_wrap_t). `transition` is how long, in
 * milliseconds, an entity takes to reach this style from the one it had
 * (`0`, the default, means it reaches it at once), and `easing` the curve it moves
 * on (::lh_ui_easing_t). The style itself is what several entities share.
 *
 * @param paint_type  Type of the fill and text paints.
 * @param radius_type Type of the corner radius.
 * @param font_type   Type of the font pointed at.
 * @param insets_type ::lh_ui_insets_t.
 * @param shadow_type ::lh_ui_shadow_t.
 * @param radii_type  ::lh_ui_radii_t.
 */
#define lh_ui_style_fields(paint_type, radius_type, font_type, insets_type, shadow_type,           \
                           radii_type)                                                             \
    paint_type fill;                                                                               \
    radius_type radius;                                                                            \
    radius_type hit_radius;                                                                        \
    radii_type radii;                                                                              \
    insets_type padding;                                                                           \
    const font_type *font;                                                                         \
    paint_type text;                                                                               \
    lh_ui_text_align_h_t align_h;                                                                  \
    lh_ui_text_align_v_t align_v;                                                                  \
    lh_ui_text_overflow_t overflow;                                                                \
    lh_ui_text_wrap_t wrap;                                                                        \
    shadow_type shadow;                                                                            \
    insets_type border;                                                                            \
    paint_type border_fill;                                                                        \
    const struct lh_ui_style *pressed;                                                             \
    lh_u16_t transition;                                                                           \
    lh_ui_easing_t easing

#endif /* LH_UI_STYLE_FIELDS_H */
