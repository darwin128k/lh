/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_style_box_t.
 */

#ifndef LH_GL_STYLE_BOX_FIELDS_H
#define LH_GL_STYLE_BOX_FIELDS_H

/**
 * @def lh_gl_style_box_fields(paint_type, insets_type, shadow_type, radii_type)
 * @brief The box of a style: what is painted under, around and on the edge of an entity.
 *
 * The fill (a paint held by value; the empty paint is no fill), its corner
 * radii, the padding that keeps content inside the rect, the shadow cast by
 * the fill, and the border painted after the children in its own paint. The
 * smallest of the `radii`, which is what a clipping parent cuts its children
 * with, is read from them (::lh_gl_style_get_radius), not kept. A box is a
 * part of ::lh_gl_style_t and lives inside it (::lh_gl_style_get_box).
 *
 * @param paint_type   ::lh_gl_paint_t.
 * @param insets_type  ::lh_gl_insets_t.
 * @param shadow_type  ::lh_gl_shadow_t.
 * @param radii_type   ::lh_gl_radii_t.
 */
#define lh_gl_style_box_fields(paint_type, insets_type, shadow_type, radii_type)                   \
    paint_type fill;                                                                               \
    radii_type radii;                                                                              \
    insets_type padding;                                                                           \
    shadow_type shadow;                                                                            \
    insets_type border;                                                                            \
    paint_type border_fill

#endif /* LH_GL_STYLE_BOX_FIELDS_H */
