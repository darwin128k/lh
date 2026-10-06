/**
 * @file class.h
 * @brief One kind of entity: ::lh_ui_entity_class_t.
 *
 * The class holds the event function. Every instance of that kind points
 * at the same class. The function is not copied onto the instance.
 */

#ifndef LH_UI_ENTITY_CLASS_H
#define LH_UI_ENTITY_CLASS_H

#include <lh/compiler/extern/c.h>
#include <lh/ui/entity/class/fields.h>
#include <lh/ui/entity/event.h>
#include <lh/ui/entity/face/cb.h>
#include <lh/void.h>

struct lh_ui_entity;

/**
 * @struct lh_ui_entity_class
 * @typedef lh_ui_entity_class_t
 * @brief The event function of one kind, and the class it extends.
 */
struct lh_ui_entity_class
{
    lh_ui_entity_class_fields(lh_ui_entity_face_cb, struct lh_ui_entity_class);
};
typedef struct lh_ui_entity_class lh_ui_entity_class_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Class of ::lh_ui_entity_t.
 *
 * It has no base class.
 */
extern const lh_ui_entity_class_t lh_ui_entity_class;

/**
 * @brief Call the event function of the class @p class_p extends.
 *
 * Returns without calling when that class has no base.
 */
lh_void
lh_ui_entity_class_event_base(const lh_ui_entity_class_t *class_p, const struct lh_ui_entity *self,
                           const lh_ui_entity_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_CLASS_H */
