/**
 * @file event.h
 * @brief One call handed to an entity's face: ::lh_ui_entity_event_t.
 *
 * The event lives for that call. The entity does not keep it.
 */

#ifndef LH_UI_ENTITY_EVENT_H
#define LH_UI_ENTITY_EVENT_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/ptr.h>
#include <lh/ui/entity/event/fields.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/void.h>

struct lh_ui_canvas;
struct lh_ui_entity_transform;

/**
 * @enum lh_ui_entity_event_code
 * @brief What the face was asked to do.
 */
typedef enum lh_ui_entity_event_code
{
    lh_ui_entity_event_draw = 0,    /**< Draw on the canvas in the context. */
    lh_ui_entity_event_click = 1,   /**< Primary click; context is ::lh_ui_point_t *. */
    lh_ui_entity_event_children = 2, /**< How are the children placed? Context is the
                                          ::lh_ui_entity_transform_t * to fill, preset
                                          to no offset, no clip. */
    lh_ui_entity_event_visible = 3,  /**< Is the entity shown? Context is an
                                          ::lh_bool_t * preset to true; a class may
                                          set it to false. */
    lh_ui_entity_event_measure = 4,  /**< What does the content cover? Context is
                                          the ::lh_ui_rect_t * bounds, preset to the
                                          children bounds; a class may grow it. */
    lh_ui_entity_event_press = 5,    /**< Pointer went down on it; context is the
                                          ::lh_ui_point_t * in its rect space. */
    lh_ui_entity_event_release = 6,  /**< The pointer that went down on it came up
                                          (wherever); context as press. */
    lh_ui_entity_event_scroll_begin = 7, /**< A container starts moving under a
                                              gesture (drag, wheel, keys); no context. */
    lh_ui_entity_event_scroll = 8,   /**< A container's scroll changed; no context. */
    lh_ui_entity_event_scroll_end = 9, /**< The gesture is over (and any glide after
                                            it); no context. */
    lh_ui_entity_event_focusable = 10, /**< May it take keyboard focus? Context is an
                                            ::lh_bool_t * preset to false. */
    lh_ui_entity_event_focus = 11,   /**< It took the focus; no context. */
    lh_ui_entity_event_defocus = 12, /**< It lost the focus; no context. */
    lh_ui_entity_event_key = 13,     /**< A key or a character while focused; context
                                          is the const ::lh_ui_key_input_t *. */
    lh_ui_entity_event_clickable = 14 /**< Does the pointer belong to it, or to what it
                                           holds? Context is an ::lh_bool_t * preset to
                                           false: a caption and a picture say no, and a
                                           button says yes, so the pointer over a button's
                                           own content reaches the button. */
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
lh_ui_point_t
lh_ui_entity_event_get_point(const lh_ui_entity_event_t *self);

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

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_ENTITY_EVENT_H */
