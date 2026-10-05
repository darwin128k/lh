/**
 * @file fn.h
 * @brief The face of an entity: the function it calls for an event.
 *
 * Not a pointer type by itself. ::lh_entity_face_cb is the pointer.
 */

#ifndef LH_ENTITY_FACE_FN_H
#define LH_ENTITY_FACE_FN_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/event.h>
#include <lh/void.h>

struct lh_entity;

/**
 * @typedef lh_entity_face_fn
 * @brief Function type an entity calls with itself and one event.
 *
 * @p self is the entity. When the entity is the first field of a wider
 * object, @p self is that object.
 */
LH_COMPILER_EXTERN_C_BEGIN
typedef lh_void(lh_entity_face_fn)(const struct lh_entity *self, const lh_entity_event_t *event);
LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_FACE_FN_H */
