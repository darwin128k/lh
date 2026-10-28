/**
 * @file event.h
 * @brief One call handed to an entity's face: ::lh_ui_entity_event_t.
 *
 * The event lives for that call. The entity does not keep it.
 */

#ifndef LH_UI_ENTITY_EVENT_H
#define LH_UI_ENTITY_EVENT_H

#include <lh/ui/entity/event/fields.h>
#include <lh/ui/rect.h>
#include <lh/ui/entity/event/code.h>

LH_COMPILER_EXTERN_C_BEGIN

struct lh_ui_canvas;
struct lh_ui_entity_transform;

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_ui_entity_event
 * @brief The code, and the context that goes with it.
 */
struct lh_ui_entity_event
{
    /** Members: ::lh_ui_entity_event_fields. */
    lh_ui_entity_event_fields(lh_ui_entity_event_code_t, lh_ptr);
};
/** @copybrief lh_ui_entity_event */
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
 * @brief True for the pointer events (click, press, release), whose context
 *        is a point.
 */
lh_bool_t
lh_ui_entity_event_is_pointer(const lh_ui_entity_event_t *self);

/**
 * @brief The answer slot of ::lh_ui_entity_event_focusable (preset false).
 */
lh_bool_t *
lh_ui_entity_event_get_focusable(const lh_ui_entity_event_t *self);

/**
 * @brief The answer slot of ::lh_ui_entity_event_clickable (preset false).
 */
lh_bool_t *
lh_ui_entity_event_get_clickable(const lh_ui_entity_event_t *self);

/**
 * @brief The key or character of ::lh_ui_entity_event_key.
 */
const struct lh_ui_key_input *
lh_ui_entity_event_get_key(const lh_ui_entity_event_t *self);

/**
 * @brief Pointer location in the rect space of the entity. @p self must be
 *        ::lh_ui_entity_event_click, ::lh_ui_entity_event_press or
 *        ::lh_ui_entity_event_release.
 */
lh_void
lh_ui_entity_event_get_point(const lh_ui_entity_event_t *self, lh_ui_point_t *out);

/**
 * @brief Answer a children event fills. @p self must be
 *        ::lh_ui_entity_event_children.
 */
struct lh_ui_entity_transform *
lh_ui_entity_event_get_transform(const lh_ui_entity_event_t *self);

/**
 * @brief Answer a visible event fills. @p self must be
 *        ::lh_ui_entity_event_visible.
 */
lh_bool_t *
lh_ui_entity_event_get_visible(const lh_ui_entity_event_t *self);

/**
 * @brief Content bounds a measure event fills. @p self must be
 *        ::lh_ui_entity_event_measure.
 */
lh_ui_rect_t *
lh_ui_entity_event_get_bounds(const lh_ui_entity_event_t *self);

/**
 * @brief Rows a baseline event answers in. @p self must be
 *        ::lh_ui_entity_event_baseline.
 */
lh_ui_scalar_t *
lh_ui_entity_event_get_baseline(const lh_ui_entity_event_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_EVENT_H */
