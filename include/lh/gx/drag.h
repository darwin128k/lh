/**
 * @file drag.h
 * @brief The drag gesture: ::lh_gx_drag_t.
 *
 * Press an entity and move the pointer: everything the pointer does until it
 * is released belongs to the drag, wherever it goes — the pointer
 * (`lh/gx/pointer.h`) captures the entity the press landed on. A slider's
 * knob follows it; a scroll view would scroll by it. The drag holds whether
 * one is in progress and where the pointer is; the control that has one feeds
 * it the events its `input` slot gets (::lh_gx_drag_take) and reacts to the
 * step it answers. Unlike the tap (`lh/gx/tap.h`), where the drag ends does not
 * matter: a release anywhere is its end, and a cancel asks the control to undo
 * it.
 *
 * Ownership: the drag is a field of the control; it points at nothing.
 */

#ifndef LH_GX_DRAG_H
#define LH_GX_DRAG_H

#include <lh/gx/drag/fields.h>
#include <lh/gx/drag/step.h>
#include <lh/gx/event.h>

/**
 * @struct lh_gx_drag
 * @brief Whether a drag is in progress, and where. Members: ::lh_gx_drag_fields.
 */
struct lh_gx_drag
{
    /** Members: ::lh_gx_drag_fields. */
    lh_gx_drag_fields(lh_bool_t, lh_gx_point_t);
};
/** @copybrief lh_gx_drag */
typedef struct lh_gx_drag lh_gx_drag_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether a drag is in progress (level 1).
 *
 * @param self The drag; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gx_drag_is_active(const lh_gx_drag_t *self);

/**
 * @brief Store whether a drag is in progress (level 1).
 *
 * @param self   The drag; checked by ::lh_assert_runtime_ref.
 * @param active The flag.
 */
lh_void
lh_gx_drag_set_active(lh_gx_drag_t *self, lh_bool_t active);

/**
 * @brief Where the pointer was at the drag's last event (level 1).
 *
 * @param self The drag; checked by ::lh_assert_runtime_ref.
 * @return The point, screen coordinates; never ::lh_null.
 */
const lh_gx_point_t *
lh_gx_drag_get_at(const lh_gx_drag_t *self);

/**
 * @brief Store where the pointer is (level 1).
 *
 * @param self The drag; checked by ::lh_assert_runtime_ref.
 * @param at   The point, screen coordinates.
 */
lh_void
lh_gx_drag_set_at(lh_gx_drag_t *self, lh_gx_point_t at);

/**
 * @brief A drag with none in progress, at `(0, 0)`.
 *
 * @param self The drag; must not be null.
 */
lh_void
lh_gx_drag_init(lh_gx_drag_t *self);

/**
 * @brief Feed @p event to the drag: a press begins it, a move while it goes on
 *        moves it, a release ends it and a cancel takes it away. Every step
 *        but a cancel stores where the pointer is (::lh_gx_drag_get_at).
 *
 * @param self  The drag; must not be null.
 * @param event The event the control's `input` slot got; must not be null.
 * @return What the event did (::lh_gx_drag_step_t); ::lh_gx_drag_step_none for
 *         anything but a press when no drag is in progress.
 */
lh_gx_drag_step_t
lh_gx_drag_take(lh_gx_drag_t *self, const lh_gx_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_DRAG_H */
