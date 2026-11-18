/**
 * @file event.h
 * @brief One thing the pointer did: ::lh_gx_event_t.
 *
 * An event is what an entity's class hears through its `input` slot
 * (::lh_gx_class_input_fn): a press, a move, a release or a cancel, and the point
 * on the screen where it happened. It is a value: the pointer (::lh_gx_pointer_t)
 * builds it on its stack for one call and nothing keeps it.
 *
 * Where events come from is not gx's business: a window, a touch controller on
 * a board, a test, a browser — each turns its own report into calls of the
 * pointer, which finds the entity and builds the event.
 */

#ifndef LH_GX_EVENT_H
#define LH_GX_EVENT_H

#include <lh/gx/event/fields.h>
#include <lh/gx/event/kind.h>
#include <lh/gx/point.h>

/**
 * @struct lh_gx_event
 * @brief What the pointer did, and where on the screen.
 */
struct lh_gx_event
{
    /** Members: ::lh_gx_event_fields. */
    lh_gx_event_fields(lh_gx_event_kind_t, lh_gx_point_t);
};
/** @copybrief lh_gx_event */
typedef struct lh_gx_event lh_gx_event_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 */
lh_gx_event_kind_t
lh_gx_event_get_kind(const lh_gx_event_t *self);

/**
 * @brief Store what happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param kind What happened.
 */
lh_void
lh_gx_event_set_kind(lh_gx_event_t *self, lh_gx_event_kind_t kind);

/**
 * @brief Where it happened, screen coordinates (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @return The point, inside @p self.
 */
const lh_gx_point_t *
lh_gx_event_get_at(const lh_gx_event_t *self);

/**
 * @brief Store where it happened (level 1).
 *
 * @param self The event; checked by ::lh_assert_runtime_ref.
 * @param at   The point, screen coordinates.
 */
lh_void
lh_gx_event_set_at(lh_gx_event_t *self, lh_gx_point_t at);

/**
 * @brief An event of @p kind at @p at.
 *
 * @param self The event; must not be null.
 * @param kind What happened.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_event_init(lh_gx_event_t *self, lh_gx_event_kind_t kind, lh_gx_point_t at);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_EVENT_H */
