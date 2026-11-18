/**
 * @file slider.h
 * @brief A slider: ::lh_gx_slider_t, a value picked by dragging a knob.
 *
 * A **composition** of four entities, like the other controls:
 *
 * - the **row**, an entity of ::lh_gx_slider_class: it draws nothing and takes
 *   input, so a press anywhere on the slider — the rail, the knob, beside the
 *   rail — begins a drag;
 * - the **rail**, a thin box across the row, a quarter of its height, centred;
 * - the **fill**, a box over the rail from its start to the knob's centre: how
 *   much of the range is taken;
 * - the **knob**, a box as high as the row and as wide (round by its style).
 *
 * The value is a whole number in `[min, max]`. The knob's left edge goes from
 * `0` at `min` to `width - height` at `max`, so its centre runs along the rail.
 * The gesture is the drag (`lh/gx/drag.h`): a press puts the knob's centre
 * under the pointer, a move follows it — the value is the nearest one to
 * where the pointer is, held in the range — and a release leaves it there. A
 * cancel puts back the value the drag began from. Every change the user makes
 * calls the application's reaction; a move of the knob is reported, so the
 * renderer redraws only where it was and where it is.
 *
 * Ownership: the struct is the application's and gx never stores it. The
 * application attaches the row (::lh_gx_slider_get_row_mut) where the slider
 * goes. The styles and the reaction's context are the application's and must
 * outlive the slider.
 *
 * Usage:
 * @code
 * lh_gx_slider_init(&screen->volume, &theme.rail, &theme.accent, &theme.knob, 16, 40, 200, 20,
 *                   0, 100, 50, on_volume, screen);
 * lh_gx_entity_attach(&screen->root, lh_gx_slider_get_row_mut(&screen->volume));
 * @endcode
 */

#ifndef LH_GX_SLIDER_H
#define LH_GX_SLIDER_H

#include <lh/gx/class.h>
#include <lh/gx/drag.h>
#include <lh/gx/entity.h>
#include <lh/gx/slider/change/fn.h>
#include <lh/gx/slider/fields.h>
#include <lh/gx/style.h>

/**
 * @struct lh_gx_slider
 * @brief A slider. Members: ::lh_gx_slider_fields.
 */
struct lh_gx_slider
{
    /** Members: ::lh_gx_slider_fields. */
    lh_gx_slider_fields(lh_gx_entity_t, lh_gx_style_t, lh_s32_t, lh_gx_slider_change_fn,
                        lh_gx_drag_t);
};
/** @copybrief lh_gx_slider */
typedef struct lh_gx_slider lh_gx_slider_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a slider's row: no `paint`, no `covers`, its own `input`
 *        (::lh_gx_slider_input).
 */
extern const lh_gx_class_t lh_gx_slider_class;

/**
 * @brief The row: the entity the application attaches (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_slider_get_row(const lh_gx_slider_t *self);

/**
 * @brief The row, to attach (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_slider_get_row_mut(lh_gx_slider_t *self);

/**
 * @brief The rail (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_slider_get_rail(const lh_gx_slider_t *self);

/**
 * @brief The rail, to place (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_slider_get_rail_mut(lh_gx_slider_t *self);

/**
 * @brief The fill (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_slider_get_fill(const lh_gx_slider_t *self);

/**
 * @brief The fill, to place (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_slider_get_fill_mut(lh_gx_slider_t *self);

/**
 * @brief The knob (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_slider_get_knob(const lh_gx_slider_t *self);

/**
 * @brief The knob, to place (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_slider_get_knob_mut(lh_gx_slider_t *self);

/**
 * @brief The rail's style (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_slider_get_style(const lh_gx_slider_t *self);

/**
 * @brief Store the rail's style (level 1); the rail is not changed.
 *
 * @param self  The slider; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_slider_set_style(lh_gx_slider_t *self, const lh_gx_style_t *style);

/**
 * @brief The fill's style (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_slider_get_accent_style(const lh_gx_slider_t *self);

/**
 * @brief Store the fill's style (level 1); the fill is not changed.
 *
 * @param self  The slider; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_slider_set_accent_style(lh_gx_slider_t *self, const lh_gx_style_t *style);

/**
 * @brief The knob's style (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_slider_get_knob_style(const lh_gx_slider_t *self);

/**
 * @brief Store the knob's style (level 1); the knob is not changed.
 *
 * @param self  The slider; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_slider_set_knob_style(lh_gx_slider_t *self, const lh_gx_style_t *style);

/**
 * @brief The least value (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_slider_get_min(const lh_gx_slider_t *self);

/**
 * @brief Store the least value (level 1); nothing is laid out again.
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @param min  The least value.
 */
lh_void
lh_gx_slider_set_min(lh_gx_slider_t *self, lh_s32_t min);

/**
 * @brief The greatest value (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_slider_get_max(const lh_gx_slider_t *self);

/**
 * @brief Store the greatest value (level 1); nothing is laid out again.
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @param max  The greatest value.
 */
lh_void
lh_gx_slider_set_max(lh_gx_slider_t *self, lh_s32_t max);

/**
 * @brief The value (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_slider_get_value(const lh_gx_slider_t *self);

/**
 * @brief Store the value (level 1); it is not held in the range and the knob
 *        does not move — that is ::lh_gx_slider_slide.
 *
 * @param self  The slider; checked by ::lh_assert_runtime_ref.
 * @param value The value.
 */
lh_void
lh_gx_slider_set_value(lh_gx_slider_t *self, lh_s32_t value);

/**
 * @brief The value the drag in progress began from (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_gx_slider_get_start(const lh_gx_slider_t *self);

/**
 * @brief Store the value a drag began from (level 1).
 *
 * @param self  The slider; checked by ::lh_assert_runtime_ref.
 * @param start The value.
 */
lh_void
lh_gx_slider_set_start(lh_gx_slider_t *self, lh_s32_t start);

/**
 * @brief The application's reaction to a change (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The reaction, or ::lh_null.
 */
lh_gx_slider_change_fn *
lh_gx_slider_get_on_change(const lh_gx_slider_t *self);

/**
 * @brief Store the application's reaction to a change (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @param fn   The reaction, or ::lh_null.
 */
lh_void
lh_gx_slider_set_on_change(lh_gx_slider_t *self, lh_gx_slider_change_fn *fn);

/**
 * @brief What the reaction is given (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_slider_get_context(const lh_gx_slider_t *self);

/**
 * @brief Store what the reaction is given (level 1).
 *
 * @param self    The slider; checked by ::lh_assert_runtime_ref.
 * @param context The application's data.
 */
lh_void
lh_gx_slider_set_context(lh_gx_slider_t *self, lh_ptr context);

/**
 * @brief The gesture on the row (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_drag_t *
lh_gx_slider_get_drag(const lh_gx_slider_t *self);

/**
 * @brief The gesture on the row, to feed (level 1).
 *
 * @param self The slider; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_drag_t *
lh_gx_slider_get_drag_mut(lh_gx_slider_t *self);

/**
 * @brief The slider whose row is @p entity: the row's content.
 *
 * @param entity The row of a slider; must not be null.
 * @return The slider.
 */
lh_gx_slider_t *
lh_gx_slider_from_entity(lh_gx_entity_t *entity);

/**
 * @brief How far the knob can travel: the row's width less its height, in
 *        whole pixels; `0` when the row is not wider than it is high.
 *
 * @param self The slider; must not be null.
 */
lh_s64_t
lh_gx_slider_get_travel(const lh_gx_slider_t *self);

/**
 * @brief Call the application's reaction with the value; nothing when there
 *        is none.
 *
 * @param self The slider; must not be null.
 */
lh_void
lh_gx_slider_change(lh_gx_slider_t *self);

/**
 * @brief Where the knob's left edge is for the value, from the row's left:
 *        `0` at `min`, the travel at `max`, the nearest whole pixel between.
 *
 * @param self The slider; must not be null.
 * @return The offset, in whole pixels; `0` for an empty range or no travel.
 */
lh_s64_t
lh_gx_slider_get_offset(const lh_gx_slider_t *self);

/**
 * @brief The value whose knob centre is nearest to @p at, held in the range.
 *
 * Only the point's x counts: a pointer above or below the slider still drags
 * it.
 *
 * @param self The slider; must be on a screen. Must not be null.
 * @param at   A point, screen coordinates; must not be null.
 * @return The value, in `[min, max]`; `min` for no travel.
 */
lh_s32_t
lh_gx_slider_get_value_at(const lh_gx_slider_t *self, const lh_gx_point_t *at);

/**
 * @brief Place the rail, the fill and the knob for the value. The moves are
 *        reported (::lh_gx_entity_move), so a slider on a screen redraws where
 *        the fill and the knob were and where they are.
 *
 * @param self The slider; must not be null.
 */
lh_void
lh_gx_slider_lay_out(lh_gx_slider_t *self);

/**
 * @brief Move the slider to @p value, held in the range, and lay it out. The
 *        reaction is not called.
 *
 * @param self  The slider; must not be null.
 * @param value The value; one outside the range is the nearer end.
 * @return ::lh_bool_true when the value changed.
 */
lh_bool_t
lh_gx_slider_slide(lh_gx_slider_t *self, lh_s32_t value);

/**
 * @brief A slider at @p x, @p y, @p width by @p height (relative to the owner
 *        it will be attached to), over `[min, max]` at @p value, with the
 *        reaction @p on_change given @p context.
 *
 * The rail, the fill and the knob are attached to the row in that order (the
 * knob in front) and laid out; the row is attached to nothing — that is the
 * application's (::lh_gx_slider_get_row_mut).
 *
 * @param self         The slider; must not be null.
 * @param style        The rail's look; not owned.
 * @param accent_style The fill's look; not owned.
 * @param knob_style   The knob's look; not owned.
 * @param x            Left edge, relative to the owner.
 * @param y            Top edge, relative to the owner.
 * @param width        Width.
 * @param height       Height; the knob is this high and as wide.
 * @param min          The least value.
 * @param max          The greatest value; at least @p min.
 * @param value        The value to start at; held in the range.
 * @param on_change    The reaction, or ::lh_null.
 * @param context      What the reaction is given.
 */
lh_void
lh_gx_slider_init(lh_gx_slider_t *self, const lh_gx_style_t *style,
                  const lh_gx_style_t *accent_style, const lh_gx_style_t *knob_style,
                  lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t width, lh_gx_scalar_t height,
                  lh_s32_t min, lh_s32_t max, lh_s32_t value, lh_gx_slider_change_fn *on_change,
                  lh_ptr context);

/**
 * @brief The user moved the slider to @p value: ::lh_gx_slider_slide, and the
 *        reaction when the value changed.
 *
 * @param self  The slider; must not be null.
 * @param value The value.
 */
lh_void
lh_gx_slider_move(lh_gx_slider_t *self, lh_s32_t value);

/**
 * @brief `input` of the slider class (::lh_gx_class_input_fn): the event goes
 *        to the slider's drag (::lh_gx_drag_take). A begin keeps the value to
 *        go back to, and a begin, a move or an end moves the slider to the
 *        pointer (::lh_gx_slider_get_value_at); a cancel moves it back.
 *
 * @param entity The row of a slider.
 * @param event  The event.
 */
lh_void
lh_gx_slider_input(lh_gx_entity_t *entity, const lh_gx_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_SLIDER_H */
