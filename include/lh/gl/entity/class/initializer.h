/**
 * @file initializer.h
 * @brief Brace-enclosed initializer macro for ::lh_gl_entity_class_t.
 */

#ifndef LH_GL_ENTITY_CLASS_INITIALIZER_H
#define LH_GL_ENTITY_CLASS_INITIALIZER_H

#include <lh/core/initializer.h>

/**
 * @def lh_gl_entity_class_initializer(kind, draw_fn, opaque)
 * @brief Produces a brace-enclosed initializer for ::lh_gl_entity_class_t.
 *
 * Expands to ::lh_initializer with the arguments in the field order of
 * ::lh_gl_entity_class_fields. A class is a `const` table that lives in flash
 * and is shared by every entity of that kind.
 *
 * @param kind    One of the `lh_gl_entity_class_kind_*` constants.
 * @param draw_fn ::lh_gl_entity_class_draw_fn, or ::lh_null for an entity that
 *                draws nothing.
 * @param opaque  ::lh_bool_true when every entity of the class covers its
 *                rectangle; ::lh_bool_false when siblings behind it still show.
 *
 * Example usage:
 * @code{.c}
 * static const lh_gl_entity_class_t label_class = lh_gl_entity_class_initializer(
 *     lh_gl_entity_class_kind_entity,  // kind
 *     label_draw,                      // draw
 *     lh_bool_false);                  // opaque
 * @endcode
 *
 * @see lh_initializer
 * @see lh_gl_entity_class_t
 */
#define lh_gl_entity_class_initializer(kind, draw_fn, opaque)                                      \
    lh_initializer(kind, draw_fn, opaque)

#endif /* LH_GL_ENTITY_CLASS_INITIALIZER_H */