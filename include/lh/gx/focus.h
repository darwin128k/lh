/**
 * @file focus.h
 * @brief Where the keys go: ::lh_gx_focus_t.
 *
 * The keyboard has no point: a key goes to the entity that has the focus, one
 * at a time per screen. The focus is one word of state, like the pointer
 * (`lh/gx/pointer.h`), and settles only on an entity that takes keys — whose
 * class fills `key` (::lh_gx_entity_can_take_keys).
 *
 * - A press gives the focus to the entity it landed on, or to its nearest
 *   owner that takes keys, so a press on a field's caption focuses the field;
 *   a press on anything else — or on nothing — takes the focus away
 *   (::lh_gx_focus_press).
 * - A move of the focus tells both sides: the entity that had it hears a blur,
 *   the one that gets it a focus event, in that order (::lh_gx_focus_move).
 * - A key or a text goes to the entity with the focus (::lh_gx_focus_take); with
 *   no focus it goes nowhere.
 *
 * Ownership: the focus is the application's, beside its pointer; it points at
 * the focused entity and owns nothing.
 */

#ifndef LH_GX_FOCUS_H
#define LH_GX_FOCUS_H

#include <lh/gx/entity.h>
#include <lh/gx/event.h>
#include <lh/gx/focus/fields.h>

/**
 * @struct lh_gx_focus
 * @brief The entity with the focus. Members: ::lh_gx_focus_fields.
 */
struct lh_gx_focus
{
    /** Members: ::lh_gx_focus_fields. */
    lh_gx_focus_fields(lh_gx_entity_t);
};
/** @copybrief lh_gx_focus */
typedef struct lh_gx_focus lh_gx_focus_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entity with the focus (level 1).
 *
 * @param self The focus; checked by ::lh_assert_runtime_ref.
 * @return The entity, or ::lh_null for none.
 */
lh_gx_entity_t *
lh_gx_focus_get_focused(const lh_gx_focus_t *self);

/**
 * @brief Store the entity with the focus (level 1); nobody is told — that is
 *        ::lh_gx_focus_move.
 *
 * @param self   The focus; checked by ::lh_assert_runtime_ref.
 * @param entity The entity, or ::lh_null; not owned.
 */
lh_void
lh_gx_focus_set_focused(lh_gx_focus_t *self, lh_gx_entity_t *entity);

/**
 * @brief The entity a press on @p entity focuses: @p entity itself or its
 *        nearest owner that takes keys.
 *
 * @param entity Where the press landed, or ::lh_null for nothing.
 * @return The entity, or ::lh_null when none of them takes keys.
 */
lh_gx_entity_t *
lh_gx_focus_find(lh_gx_entity_t *entity);

/**
 * @brief Tell @p entity that it got or lost the focus: an event of @p kind
 *        (::lh_gx_event_kind_focus or ::lh_gx_event_kind_blur) to its `key` slot.
 *
 * @param entity The entity; must not be null.
 * @param kind   ::lh_gx_event_kind_focus or ::lh_gx_event_kind_blur.
 */
lh_void
lh_gx_focus_send_to(lh_gx_entity_t *entity, lh_gx_event_kind_t kind);

/**
 * @brief A focus on nothing.
 *
 * @param self The focus; must not be null.
 */
lh_void
lh_gx_focus_init(lh_gx_focus_t *self);

/**
 * @brief Give the focus to @p entity: the entity that had it hears a blur, then
 *        @p entity hears a focus. Nothing when @p entity has it already.
 *
 * The focus is stored before either is told, so a reaction may move it again.
 *
 * @param self   The focus; must not be null.
 * @param entity An entity that takes keys, or ::lh_null to focus nothing.
 */
lh_void
lh_gx_focus_move(lh_gx_focus_t *self, lh_gx_entity_t *entity);

/**
 * @brief Hand a key or a text event to the entity with the focus.
 *
 * @param self  The focus; must not be null.
 * @param event A key or a text event; must not be null.
 * @return ::lh_bool_true when an entity had the focus and got the event.
 */
lh_bool_t
lh_gx_focus_take(const lh_gx_focus_t *self, const lh_gx_event_t *event);

/**
 * @brief The pointer went down on @p entity: the focus moves to what it finds
 *        there (::lh_gx_focus_find), or away.
 *
 * @param self   The focus; must not be null.
 * @param entity What the press landed on (the pointer's target), or ::lh_null.
 */
lh_void
lh_gx_focus_press(lh_gx_focus_t *self, lh_gx_entity_t *entity);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FOCUS_H */
