/**
 * @file event.h
 * @brief One call handed to an entity's face: ::lh_ui_entity_event_t.
 *
 * The event lives for that call. The entity does not keep it.
 */

#ifndef LH_UI_ENTITY_EVENT_H
#define LH_UI_ENTITY_EVENT_H

#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/entity/event/fields.h>
#include <lh/ui/point.h>
#include <lh/void.h>

struct lh_ui_canvas;

/**
 * @enum lh_ui_entity_event_code
 * @brief What the face was asked to do.
 */
typedef enum lh_ui_entity_event_code
{
    lh_ui_entity_event_draw = 0, /**< Draw on the canvas in the context. */
    lh_ui_entity_event_click = 1 /**< Primary click; context is ::lh_ui_point_t *. */
} lh_ui_entity_event_code_t;

/**
 * @struct lh_ui_entity_event
 * @typedef lh_ui_entity_event_t
 * @brief The code, and the context that goes with it.
 */
struct lh_ui_entity_event
{
    lh_ui_entity_event_fields(lh_ui_entity_event_code_t, lh_ptr);
};
typedef struct lh_ui_entity_event lh_ui_entity_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Fill @p self with @p code and @p context. The context is not owned.
 */
lh_void
lh_ui_entity_event_init(lh_ui_entity_event_t *self, lh_ui_entity_event_code_t code, lh_ptr context);

/**
 * @brief Which event @p self is.
 */
lh_ui_entity_event_code_t
lh_ui_entity_event_get_code(const lh_ui_entity_event_t *self);

/**
 * @brief Context of @p self, or ::lh_null.
 */
lh_ptr
lh_ui_entity_event_get_context(const lh_ui_entity_event_t *self);

/**
 * @brief Canvas a draw event draws on, or ::lh_null.
 *
 * @p self must be ::lh_ui_entity_event_draw.
 */
struct lh_ui_canvas *
lh_ui_entity_event_get_canvas(const lh_ui_entity_event_t *self);

/**
 * @brief Click location. @p self must be ::lh_ui_entity_event_click.
 */
lh_ui_point_t
lh_ui_entity_event_get_point(const lh_ui_entity_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_EVENT_H */
