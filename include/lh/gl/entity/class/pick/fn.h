/**
 * @file fn.h
 * @brief How an entity answers a press: the call a class provides.
 */

#ifndef LH_GL_ENTITY_CLASS_PICK_FN_H
#define LH_GL_ENTITY_CLASS_PICK_FN_H

#include <lh/gl/entity/decl.h>
#include <lh/gl/point.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gl_entity_class_pick_fn
 * @brief Function type `lh_bool_t (const lh_gl_entity_t *, lh_gl_point_t, lh_gl_scalar_t)` —
 *        whether this entity takes the press.
 *
 * @p point is in root coordinates. @p slop is the finger thickness in pixels,
 * the same for every entity. True takes the press. False lets the search
 * continue to whoever is behind. Not a pointer type by itself; use
 * ::lh_gl_entity_class_pick_cb.
 */
typedef lh_bool_t(lh_gl_entity_class_pick_fn)(const lh_gl_entity_t *entity, lh_gl_point_t point,
                                              lh_gl_scalar_t slop);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_ENTITY_CLASS_PICK_FN_H */
