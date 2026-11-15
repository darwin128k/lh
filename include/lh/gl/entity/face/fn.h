/**
 * @file fn.h
 * @brief The face of an entity: the function it calls for an event.
 *
 * Not a pointer type by itself. ::lh_gl_entity_face_cb is the pointer.
 */

#ifndef LH_GL_ENTITY_FACE_FN_H
#define LH_GL_ENTITY_FACE_FN_H

#include <lh/gl/entity/event.h>

struct lh_gl_entity;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_gl_entity_face_fn
 * @brief Function type an entity calls with itself and one event.
 *
 * @p self is the entity. When the entity is the first field of a wider
 * object, @p self is that object.
 */
typedef lh_void(lh_gl_entity_face_fn)(const struct lh_gl_entity *self,
                                      const lh_gl_entity_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GL_ENTITY_FACE_FN_H */
