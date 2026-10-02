/**
 * @file event.h
 * @brief An event sent to an entity (::lh_entity_send_event).
 */

#ifndef LH_ENTITY_EVENT_H
#define LH_ENTITY_EVENT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/entity/event/fields.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/void.h>

struct lh_entity;

/**
 * @def LH_ENTITY_EVENT_DELETE
 * @brief Sent to an entity (never bubbled) as it is being deleted, before
 *        its classes' destructors run.
 */
#define LH_ENTITY_EVENT_DELETE 1U

/**
 * @def LH_ENTITY_EVENT_USER
 * @brief First code free for the application's own events.
 */
#define LH_ENTITY_EVENT_USER 0x1000U

/**
 * @struct lh_entity_event
 * @brief Fields via ::lh_entity_event_fields.
 */
struct lh_entity_event
{
    lh_entity_event_fields(lh_uint_t, struct lh_entity, lh_ptr, lh_bool_t);
};
typedef struct lh_entity_event lh_entity_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What happened: an `LH_ENTITY_EVENT_*` code or an application one.
 */
lh_uint_t
lh_entity_event_get_code(const lh_entity_event_t *self);

/**
 * @brief The entity the event was sent to.
 */
struct lh_entity *
lh_entity_event_get_target(const lh_entity_event_t *self);

/**
 * @brief The entity being notified now: the target, or one of its ancestors
 *        while the event bubbles up.
 */
struct lh_entity *
lh_entity_event_get_current(const lh_entity_event_t *self);

/**
 * @brief The sender's extra data (::lh_null if none).
 */
lh_ptr
lh_entity_event_get_param(const lh_entity_event_t *self);

/**
 * @brief Notify no one else: no further class, handler or ancestor.
 */
lh_void
lh_entity_event_stop(lh_entity_event_t *self);

/**
 * @brief True once ::lh_entity_event_stop was called.
 */
lh_bool_t
lh_entity_event_is_stopped(const lh_entity_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_ENTITY_EVENT_H */
