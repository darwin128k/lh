/**
 * @file tap.h
 * @brief The tap gesture: ::lh_gx_tap_t.
 *
 * Press an entity, release it on the entity: that is a tap — a button's click,
 * a checkbox's toggle. Release it elsewhere, or have the gesture cancelled, and
 * it is not. The tap holds only whether a press is in progress; the control
 * that has one feeds it the events its `input` slot gets
 * (::lh_gx_tap_take) and reacts to the step it answers — a pressed look on
 * ::lh_gx_tap_step_down, the look at rest on ::lh_gx_tap_step_up, the reaction
 * on ::lh_gx_tap_step_tap. What a tap means is the control's, not the tap's.
 *
 * The pointer (`lh/gx/pointer.h`) captures the entity a gesture starts on, so
 * the release reaches it wherever it happens; the tap decides from the point
 * whether that was on the entity (::lh_gx_hit_is_on, its class's `hit`
 * included).
 *
 * Ownership: the tap is a field of the control; it points at nothing.
 */

#ifndef LH_GX_TAP_H
#define LH_GX_TAP_H

#include <lh/gx/entity.h>
#include <lh/gx/event.h>
#include <lh/gx/tap/fields.h>
#include <lh/gx/tap/step.h>

/**
 * @struct lh_gx_tap
 * @brief Whether a press is in progress. Members: ::lh_gx_tap_fields.
 */
struct lh_gx_tap
{
    /** Members: ::lh_gx_tap_fields. */
    lh_gx_tap_fields(lh_bool_t);
};
/** @copybrief lh_gx_tap */
typedef struct lh_gx_tap lh_gx_tap_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Whether a press is in progress (level 1).
 *
 * @param self The tap; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gx_tap_is_pressed(const lh_gx_tap_t *self);

/**
 * @brief Store whether a press is in progress (level 1).
 *
 * @param self    The tap; checked by ::lh_assert_runtime_ref.
 * @param pressed The flag.
 */
lh_void
lh_gx_tap_set_pressed(lh_gx_tap_t *self, lh_bool_t pressed);

/**
 * @brief A tap with no press in progress.
 *
 * @param self The tap; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_gx_tap_init(lh_gx_tap_t *self);

/**
 * @brief End the press in progress: the step is ::lh_gx_tap_step_tap when
 *        @p on, else ::lh_gx_tap_step_up; ::lh_gx_tap_step_none when no press
 *        was in progress.
 *
 * @param self The tap; must not be null.
 * @param on   Whether the gesture ended on the entity.
 * @return The step.
 */
lh_gx_tap_step_t
lh_gx_tap_end(lh_gx_tap_t *self, lh_bool_t on);

/**
 * @brief Feed @p event, which @p entity got, to the tap: a press begins the
 *        gesture, a release ends it — a tap when it is on @p entity — and a
 *        cancel ends it without one. A move changes nothing.
 *
 * @param self   The tap; must not be null.
 * @param entity The entity the control's `input` slot was called for; it must
 *               be on a screen, for the release is tested against where it is
 *               on the screen.
 * @param event  The event; must not be null.
 * @return What the event did (::lh_gx_tap_step_t).
 */
lh_gx_tap_step_t
lh_gx_tap_take(lh_gx_tap_t *self, const lh_gx_entity_t *entity, const lh_gx_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_TAP_H */
