/**
 * @file fn.h
 * @brief How an entity draws itself: the call a class provides.
 */

#ifndef LH_GL_ENTITY_CLASS_DRAW_FN_H
#define LH_GL_ENTITY_CLASS_DRAW_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/void.h>
#include <lh/gl/canvas/decl.h>
#include <lh/gl/entity/decl.h>
#include <lh/gl/place.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gl_entity_class_draw_fn
 * @brief Function type of an entity drawing itself.
 *
 * @p entity is the object being drawn, read-only: drawing never changes it.
 * @p place is the origin, the clip and the mask for this call. The canvas does
 * not store them. The call knows nothing about input, styles or drivers. Not a
 * pointer type by itself; use ::lh_gl_entity_class_draw_cb.
 */
typedef lh_void(lh_gl_entity_class_draw_fn)(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas,
                                            const lh_gl_place_t *place);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_ENTITY_CLASS_DRAW_FN_H */