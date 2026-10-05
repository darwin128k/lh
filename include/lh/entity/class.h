/**
 * @file class.h
 * @brief One kind of entity: ::lh_entity_class_t.
 *
 * The class holds the event function. Every instance of that kind points
 * at the same class. The function is not copied onto the instance.
 */

#ifndef LH_ENTITY_CLASS_H
#define LH_ENTITY_CLASS_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/class/fields.h>
#include <lh/entity/event.h>
#include <lh/entity/face/cb.h>
#include <lh/void.h>

struct lh_entity;

/**
 * @struct lh_entity_class
 * @typedef lh_entity_class_t
 * @brief The event function of one kind, and the class it extends.
 */
struct lh_entity_class
{
    lh_entity_class_fields(lh_entity_face_cb, struct lh_entity_class);
};
typedef struct lh_entity_class lh_entity_class_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_entity_t. Its event fills and strokes the rectangle.
 *
 * It has no base class.
 */
extern const lh_entity_class_t lh_entity_class;

/**
 * @brief Call the event function of the class @p class_p extends.
 *
 * Returns without calling when that class has no base.
 */
lh_void
lh_entity_class_event_base(const lh_entity_class_t *class_p, const struct lh_entity *self,
                           const lh_entity_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_CLASS_H */
