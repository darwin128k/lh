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
 * @def LH_ENTITY_EVENT_GET_LOCAL_MATRIX
 * @brief Asks a spatial entity for the matrix from its own space into its
 *        parent's; `param` is the ::lh_math_mat4_t to fill. The most derived
 *        spatial class answers and stops the event, so a 3D entity answers
 *        with its full 3D transform and its 2D base stays out.
 */
#define LH_ENTITY_EVENT_GET_LOCAL_MATRIX 2U

/**
 * @def LH_ENTITY_EVENT_DRAW
 * @brief Sent (never bubbled) to each visible entity as a screen is drawn,
 *        after its background (::lh_entity_2d_draw_background) and before its
 *        children; `param` is the ::lh_ui_canvas_t, already clipped to where
 *        the entity may draw, with its depth left at the background's so this
 *        paint covers that same surface. Paint here to draw on top of the style.
 */
#define LH_ENTITY_EVENT_DRAW 3U

/**
 * @def LH_ENTITY_EVENT_POINTER_DOWN
 * @brief A pointer button was pressed over the entity
 *        (::lh_entity_screen_send_pointer); `param` is the `const lh_math_vec2_t *`
 *        screen position. Bubbles with ::lh_entity_flags_event_bubble.
 */
#define LH_ENTITY_EVENT_POINTER_DOWN 4U

/**
 * @def LH_ENTITY_EVENT_POINTER_UP
 * @brief A pointer button was released over the entity; as
 *        ::LH_ENTITY_EVENT_POINTER_DOWN.
 */
#define LH_ENTITY_EVENT_POINTER_UP 5U

/**
 * @def LH_ENTITY_EVENT_POINTER_MOVE
 * @brief The pointer moved over the entity; as ::LH_ENTITY_EVENT_POINTER_DOWN.
 */
#define LH_ENTITY_EVENT_POINTER_MOVE 6U

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
