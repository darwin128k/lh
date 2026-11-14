/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_entity_class_t.
 */

#ifndef LH_GL_ENTITY_CLASS_FIELDS_H
#define LH_GL_ENTITY_CLASS_FIELDS_H

/**
 * @def lh_gl_entity_class_fields(kind_type, draw_fn_type, opaque_type)
 * @brief The kind of an entity, the call that draws it, and whether it covers
 *        its rectangle.
 *
 * @param kind_type    ::lh_gl_entity_class_kind_t: `kind`, entity or layer.
 * @param draw_fn_type ::lh_gl_entity_class_draw_fn; the member is `draw_cb`,
 *                     the call that draws an entity of this class (may be
 *                     ::lh_null: the entity draws nothing).
 * @param opaque_type  ::lh_bool_t: `opaque`. True when an entity of this class
 *                     covers every pixel of its rectangle, so a sibling fully
 *                     behind it can be skipped.
 */
#define lh_gl_entity_class_fields(kind_type, draw_fn_type, opaque_type)                            \
    kind_type kind;                                                                                \
    draw_fn_type *draw_cb;                                                                         \
    opaque_type opaque

#endif /* LH_GL_ENTITY_CLASS_FIELDS_H */