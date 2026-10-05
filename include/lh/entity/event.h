/**
 * @file event.h
 * @brief One call handed to an entity's face: ::lh_entity_event_t.
 *
 * The event lives for that call. The entity does not keep it.
 */

#ifndef LH_ENTITY_EVENT_H
#define LH_ENTITY_EVENT_H

#include <lh/compiler/extern/c.h>
#include <lh/entity/event/fields.h>

/**
 * @enum lh_entity_event_code
 * @brief What the face was asked to do.
 */
typedef enum lh_entity_event_code
{
    lh_entity_event_draw = 0
} lh_entity_event_code_t;

/**
 * @struct lh_entity_event
 * @typedef lh_entity_event_t
 * @brief A draw call: the code.
 */
struct lh_entity_event
{
    lh_entity_event_fields(lh_entity_event_code_t);
};
typedef struct lh_entity_event lh_entity_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Which event @p self is.
 */
lh_entity_event_code_t
lh_entity_event_get_code(const lh_entity_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_EVENT_H */
