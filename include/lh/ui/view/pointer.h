/**
 * @file pointer.h
 * @brief One pointer's press in a view: ::lh_ui_view_pointer_t.
 *
 * A view keeps one of these for every pointer id it follows, `0 ..`
 * ::LH_OS_INPUT_POINTER_MAX `- 1`, inside itself: no heap. The mouse is `0`,
 * touch contacts and pens are `1` and up. Each slot has its own pressed entity,
 * its own thumb grab and its own content drag, so two fingers on two buttons press
 * both, and a finger that went down on a list keeps that list while another finger
 * presses somewhere else.
 *
 * The view writes the slots (::lh_ui_view_press_pointer and its siblings); a
 * caller reads them through ::lh_ui_view_get_pointer.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_VIEW_POINTER_H
#define LH_UI_VIEW_POINTER_H

#include <lh/ui/scrollbar.h>
#include <lh/ui/view/pointer/fields.h>

/**
 * @struct lh_ui_view_pointer
 * @brief The press, the grab and the drag of one pointer.
 */
struct lh_ui_view_pointer
{
    /** Members: ::lh_ui_view_pointer_fields. */
    lh_ui_view_pointer_fields(lh_ui_entity_t, lh_ui_container_t, lh_ui_scrollbar_t, lh_ui_scalar_t,
                              lh_ui_point_t);
};
/** @copybrief lh_ui_view_pointer */
typedef struct lh_ui_view_pointer lh_ui_view_pointer_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A pointer with nothing pressed, grabbed or dragged, at the origin, still
 *        (::lh_ui_view_pointer_idle_initializer).
 */
lh_void
lh_ui_view_pointer_init(lh_ui_view_pointer_t *self);

/**
 * @brief A press begins at @p point: the session of @p self is cleared
 *        (::lh_ui_view_pointer_reset), it is pressed, both its points are
 *        @p point and it stands still.
 */
lh_void
lh_ui_view_pointer_start(lh_ui_view_pointer_t *self, lh_ui_point_t point);

/**
 * @brief The press of @p self is over: no target, drag, grab or grab offset, not
 *        pressed and not dragged. Where it was and how fast it went stay.
 */
lh_void
lh_ui_view_pointer_reset(lh_ui_view_pointer_t *self);

/**
 * @brief True while @p self holds a press or a thumb: what a cancel has to undo.
 */
lh_bool_t
lh_ui_view_pointer_is_active(const lh_ui_view_pointer_t *self);

/**
 * @brief The entity @p self went down on, or ::lh_null.
 */
lh_ui_entity_t *
lh_ui_view_pointer_get_target(const lh_ui_view_pointer_t *self);

/**
 * @brief The entity @p self went down on: @p target, ::lh_null for none.
 */
lh_void
lh_ui_view_pointer_set_target(lh_ui_view_pointer_t *self, lh_ui_entity_t *target);

/**
 * @brief The container a content drag of @p self moves, or ::lh_null.
 */
lh_ui_container_t *
lh_ui_view_pointer_get_drag(const lh_ui_view_pointer_t *self);

/**
 * @brief The container a content drag of @p self moves: @p drag, ::lh_null for
 *        none.
 */
lh_void
lh_ui_view_pointer_set_drag(lh_ui_view_pointer_t *self, lh_ui_container_t *drag);

/**
 * @brief The scrollbar whose thumb @p self holds, or ::lh_null.
 */
lh_ui_scrollbar_t *
lh_ui_view_pointer_get_grab(const lh_ui_view_pointer_t *self);

/**
 * @brief How far into the thumb @p self holds it, along the bar.
 */
lh_ui_scalar_t
lh_ui_view_pointer_get_grab_offset(const lh_ui_view_pointer_t *self);

/**
 * @brief @p self holds the thumb of @p grab (::lh_null lets go), @p offset into
 *        it along the bar.
 */
lh_void
lh_ui_view_pointer_set_grab(lh_ui_view_pointer_t *self, lh_ui_scrollbar_t *grab,
                            lh_ui_scalar_t offset);

/**
 * @brief True while a press of @p self is open.
 */
lh_bool_t
lh_ui_view_pointer_is_pressed(const lh_ui_view_pointer_t *self);

/**
 * @brief True once the press of @p self moved past the threshold: its release
 *        is no click.
 */
lh_bool_t
lh_ui_view_pointer_is_dragged(const lh_ui_view_pointer_t *self);

/**
 * @brief Say whether the press of @p self has been dragged.
 */
lh_void
lh_ui_view_pointer_set_dragged(lh_ui_view_pointer_t *self, lh_bool_t dragged);

/**
 * @brief Where the press of @p self went down, into @p out.
 */
lh_void
lh_ui_view_pointer_get_press_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out);

/**
 * @brief Where @p self was last seen, into @p out.
 */
lh_void
lh_ui_view_pointer_get_last_point(const lh_ui_view_pointer_t *self, lh_ui_point_t *out);

/**
 * @brief How fast the drag of @p self goes, per move, into @p out: the speed its
 *        release leaves the content gliding at.
 */
lh_void
lh_ui_view_pointer_get_velocity(const lh_ui_view_pointer_t *self, lh_ui_point_t *out);

/**
 * @brief @p self moves to @p point: the last point follows and the velocity keeps
 *        half of the old speed and half of this step (content follows the
 *        pointer, so a move down is a speed up).
 */
lh_void
lh_ui_view_pointer_track(lh_ui_view_pointer_t *self, lh_ui_point_t point);

/**
 * @brief True when @p point is ::LH_UI_VIEW_DRAG_THRESHOLD or more from where the
 *        press of @p self went down, on either axis.
 */
lh_bool_t
lh_ui_view_pointer_is_past_threshold(const lh_ui_view_pointer_t *self, lh_ui_point_t point);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_VIEW_POINTER_H */
