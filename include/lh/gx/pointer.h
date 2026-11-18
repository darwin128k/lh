/**
 * @file pointer.h
 * @brief A finger or a mouse over a tree: ::lh_gx_pointer_t.
 *
 * The pointer turns "it went down here, moved, went up there" into events for
 * the entities (::lh_gx_event_t). A press finds its target once
 * (::lh_gx_hit_find) and **captures** it: the moves, the release and the
 * cancel of that gesture go to the same entity wherever the pointer is — a
 * button pressed and released outside itself hears the release, and decides it
 * was not a click.
 *
 * The pointer is one word of state, the captured entity, and it lives where the
 * application put it, one per finger. Where the reports come from is the
 * application's: a window's mouse, a touch controller on a board, a test, a
 * browser. Each report is one call: ::lh_gx_pointer_press,
 * ::lh_gx_pointer_move, ::lh_gx_pointer_release, ::lh_gx_pointer_cancel.
 *
 * Ownership: the captured entity is the application's. An application that
 * detaches or destroys an entity during a gesture calls ::lh_gx_pointer_cancel
 * first (the entity forgets it was pressed), or ::lh_gx_pointer_set_captured
 * with ::lh_null (nothing is sent).
 */

#ifndef LH_GX_POINTER_H
#define LH_GX_POINTER_H

#include <lh/gx/entity.h>
#include <lh/gx/event.h>
#include <lh/gx/pointer/fields.h>

/**
 * @struct lh_gx_pointer
 * @brief The entity the gesture in progress belongs to.
 */
struct lh_gx_pointer
{
    /** Members: ::lh_gx_pointer_fields. */
    lh_gx_pointer_fields(lh_gx_entity_t);
};
/** @copybrief lh_gx_pointer */
typedef struct lh_gx_pointer lh_gx_pointer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entity the gesture in progress belongs to (level 1).
 *
 * @param self The pointer; checked by ::lh_assert_runtime_ref.
 * @return The entity, or ::lh_null.
 */
lh_gx_entity_t *
lh_gx_pointer_get_captured(const lh_gx_pointer_t *self);

/**
 * @brief Store the entity the gesture belongs to (level 1); nothing is sent.
 *
 * @param self   The pointer; checked by ::lh_assert_runtime_ref.
 * @param entity The entity, or ::lh_null; not owned.
 */
lh_void
lh_gx_pointer_set_captured(lh_gx_pointer_t *self, lh_gx_entity_t *entity);

/**
 * @brief Build an event of @p kind at @p at and hand it to @p entity
 *        (::lh_gx_entity_take_input); nothing when @p entity is ::lh_null.
 *
 * @param entity The entity, or ::lh_null.
 * @param kind   What happened.
 * @param at     Where, screen coordinates.
 */
lh_void
lh_gx_pointer_send_to(lh_gx_entity_t *entity, lh_gx_event_kind_t kind, const lh_gx_point_t *at);

/**
 * @brief A pointer with no gesture in progress.
 *
 * @param self The pointer; must not be null.
 */
lh_void
lh_gx_pointer_init(lh_gx_pointer_t *self);

/**
 * @brief Send an event of @p kind at @p at to the captured entity; nothing when
 *        there is none. The gesture goes on.
 *
 * @param self The pointer; must not be null.
 * @param kind What happened.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_pointer_send(const lh_gx_pointer_t *self, lh_gx_event_kind_t kind, const lh_gx_point_t *at);

/**
 * @brief End the gesture: forget the captured entity, then send it an event of
 *        @p kind at @p at; nothing is sent when there is none.
 *
 * The entity is forgotten first, so what it does when it hears the end — a
 * click that opens another screen, a new press — finds the pointer free.
 *
 * @param self The pointer; must not be null.
 * @param kind ::lh_gx_event_kind_release or ::lh_gx_event_kind_cancel.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_pointer_end(lh_gx_pointer_t *self, lh_gx_event_kind_t kind, const lh_gx_point_t *at);

/**
 * @brief The pointer moved to @p at while down: a move to the captured entity.
 *
 * @param self The pointer; must not be null.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_pointer_move(const lh_gx_pointer_t *self, const lh_gx_point_t *at);

/**
 * @brief The pointer went up at @p at: the release to the captured entity, and
 *        the gesture is over.
 *
 * @param self The pointer; must not be null.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_pointer_release(lh_gx_pointer_t *self, const lh_gx_point_t *at);

/**
 * @brief The gesture is over without a release — the window lost the mouse, the
 *        screen is changing: a cancel to the captured entity, at `(0, 0)`.
 *
 * @param self The pointer; must not be null.
 */
lh_void
lh_gx_pointer_cancel(lh_gx_pointer_t *self);

/**
 * @brief The pointer went down at @p at: cancel a gesture still in progress,
 *        find the target under @p top (::lh_gx_hit_find), capture it and send it
 *        the press.
 *
 * A press on nothing that takes input captures nothing, and the rest of that
 * gesture goes nowhere.
 *
 * @param self The pointer; must not be null.
 * @param top  The tree, usually the screen; must not be null.
 * @param at   Where, screen coordinates.
 */
lh_void
lh_gx_pointer_press(lh_gx_pointer_t *self, lh_gx_entity_t *top, const lh_gx_point_t *at);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_POINTER_H */
