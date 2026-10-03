/**
 * @file fn.h
 * @brief Callback signatures for ::lh_entity_t.
 */

#ifndef LH_ENTITY_FN_H
#define LH_ENTITY_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_entity;
struct lh_entity_event;

/**
 * @typedef lh_entity_constructor_fn
 * @brief Set up the fields a class adds, right after the entity is created.
 *
 * Runs base class first, so a derived constructor sees its base already set
 * up. The entity is already among its parent's children.
 */
typedef lh_void(lh_entity_constructor_fn)(struct lh_entity *self);

/**
 * @typedef lh_entity_destructor_fn
 * @brief Release what a class acquired, right before the entity is freed.
 *
 * Runs derived class first. The entity's children are still alive.
 */
typedef lh_void(lh_entity_destructor_fn)(struct lh_entity *self);

/**
 * @typedef lh_entity_event_fn
 * @brief A class's own handler invoked when an event reaches one of its entities.
 *
 * Runs derived class first, then each base, before the entity's handlers;
 * ::lh_entity_event_stop ends the chain.
 */
typedef lh_void(lh_entity_event_fn)(struct lh_entity *self, struct lh_entity_event *event);

/**
 * @typedef lh_entity_handler_fn
 * @brief A handler added to one entity with ::lh_entity_add_handler.
 *
 * @param event     The event; ::lh_entity_event_get_current is the entity
 *                  the handler was added to.
 * @param user_data What was passed to ::lh_entity_add_handler.
 */
typedef lh_void(lh_entity_handler_fn)(struct lh_entity_event *event, lh_ptr user_data);

#endif /* LH_ENTITY_FN_H */
