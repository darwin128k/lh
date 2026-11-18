/**
 * @file checkbox.h
 * @brief A checkbox: ::lh_gx_checkbox_t, a control made of four entities.
 *
 * Like the button (`lh/gx/button.h`), a checkbox is a **composition**, not a
 * kind of entity:
 *
 * - the **row**, an entity of ::lh_gx_checkbox_class: it draws nothing and
 *   takes input, so a press anywhere on it — the square or the caption —
 *   is a press on the checkbox;
 * - the **square**, a box at the row's left, as high as the row;
 * - the **tick**, a smaller box inside the square, shown only while the
 *   checkbox is checked (::lh_gx_entity_set_shown, so only the square is
 *   redrawn when it changes);
 * - the **caption**, a text right of the square, centred in the row's height.
 *
 * The gesture is the tap (`lh/gx/tap.h`): a press and a release on the row
 * toggle the checkbox and call the application's reaction; a release
 * elsewhere or a cancel does nothing. The row's content points back at this
 * struct, so the `input` slot finds the checkbox from its entity.
 *
 * Ownership: the struct is the application's and gx never stores it. The
 * application attaches the row (::lh_gx_checkbox_get_row_mut) where the
 * checkbox goes and detaches it before the memory goes. The string, the
 * styles and the reaction's context are the application's and must outlive
 * the checkbox.
 *
 * Usage:
 * @code
 * lh_gx_checkbox_init(&screen->sound, &theme.field, &theme.accent, 16, 40, 160, 20,
 *                     "Sound", lh_bool_true, on_sound, screen);
 * lh_gx_entity_attach(&screen->root, lh_gx_checkbox_get_row_mut(&screen->sound));
 * @endcode
 */

#ifndef LH_GX_CHECKBOX_H
#define LH_GX_CHECKBOX_H

#include <lh/gx/checkbox/change/fn.h>
#include <lh/gx/checkbox/fields.h>
#include <lh/gx/class.h>
#include <lh/gx/style.h>
#include <lh/gx/tap.h>

/**
 * @struct lh_gx_checkbox
 * @brief A checkbox. Members: ::lh_gx_checkbox_fields.
 */
struct lh_gx_checkbox
{
    /** Members: ::lh_gx_checkbox_fields. */
    lh_gx_checkbox_fields(lh_gx_entity_t, lh_gx_style_t, lh_gx_checkbox_change_fn, lh_gx_tap_t);
};
/** @copybrief lh_gx_checkbox */
typedef struct lh_gx_checkbox lh_gx_checkbox_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a checkbox's row: no `paint`, no `covers`, its own
 *        `input` (::lh_gx_checkbox_input).
 */
extern const lh_gx_class_t lh_gx_checkbox_class;

/**
 * @brief The row: the entity the application attaches (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_checkbox_get_row(const lh_gx_checkbox_t *self);

/**
 * @brief The row, to attach (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_checkbox_get_row_mut(lh_gx_checkbox_t *self);

/**
 * @brief The square (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_checkbox_get_square(const lh_gx_checkbox_t *self);

/**
 * @brief The square, to change (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_checkbox_get_square_mut(lh_gx_checkbox_t *self);

/**
 * @brief The tick (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_checkbox_get_tick(const lh_gx_checkbox_t *self);

/**
 * @brief The tick, to show or hide (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_checkbox_get_tick_mut(lh_gx_checkbox_t *self);

/**
 * @brief The caption (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_checkbox_get_caption(const lh_gx_checkbox_t *self);

/**
 * @brief The caption, to change (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_checkbox_get_caption_mut(lh_gx_checkbox_t *self);

/**
 * @brief The square's and the caption's style (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_checkbox_get_style(const lh_gx_checkbox_t *self);

/**
 * @brief Store the square's and the caption's style (level 1); the entities
 *        are not changed.
 *
 * @param self  The checkbox; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_checkbox_set_style(lh_gx_checkbox_t *self, const lh_gx_style_t *style);

/**
 * @brief The tick's style (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_checkbox_get_tick_style(const lh_gx_checkbox_t *self);

/**
 * @brief Store the tick's style (level 1); the tick is not changed.
 *
 * @param self  The checkbox; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_checkbox_set_tick_style(lh_gx_checkbox_t *self, const lh_gx_style_t *style);

/**
 * @brief The application's reaction to a change (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The reaction, or ::lh_null.
 */
lh_gx_checkbox_change_fn *
lh_gx_checkbox_get_on_change(const lh_gx_checkbox_t *self);

/**
 * @brief Store the application's reaction to a change (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @param fn   The reaction, or ::lh_null.
 */
lh_void
lh_gx_checkbox_set_on_change(lh_gx_checkbox_t *self, lh_gx_checkbox_change_fn *fn);

/**
 * @brief What the reaction is given (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_checkbox_get_context(const lh_gx_checkbox_t *self);

/**
 * @brief Store what the reaction is given (level 1).
 *
 * @param self    The checkbox; checked by ::lh_assert_runtime_ref.
 * @param context The application's data.
 */
lh_void
lh_gx_checkbox_set_context(lh_gx_checkbox_t *self, lh_ptr context);

/**
 * @brief The gesture on the row (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_tap_t *
lh_gx_checkbox_get_tap(const lh_gx_checkbox_t *self);

/**
 * @brief The gesture on the row, to feed (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_tap_t *
lh_gx_checkbox_get_tap_mut(lh_gx_checkbox_t *self);

/**
 * @brief Whether the checkbox is checked (level 1).
 *
 * @param self The checkbox; checked by ::lh_assert_runtime_ref.
 */
lh_bool_t
lh_gx_checkbox_is_checked(const lh_gx_checkbox_t *self);

/**
 * @brief Store the state (level 1); the tick is not shown or hidden — that is
 *        ::lh_gx_checkbox_check.
 *
 * @param self    The checkbox; checked by ::lh_assert_runtime_ref.
 * @param checked The state.
 */
lh_void
lh_gx_checkbox_set_checked(lh_gx_checkbox_t *self, lh_bool_t checked);

/**
 * @brief The checkbox whose row is @p entity: the row's content.
 *
 * @param entity The row of a checkbox; must not be null.
 * @return The checkbox.
 */
lh_gx_checkbox_t *
lh_gx_checkbox_from_entity(lh_gx_entity_t *entity);

/**
 * @brief The caption's string.
 *
 * @param self The checkbox; must not be null.
 * @return The string, or ::lh_null for none.
 */
const lh_char_t *
lh_gx_checkbox_get_text(const lh_gx_checkbox_t *self);

/**
 * @brief Call the application's reaction with the current state; nothing when
 *        there is none.
 *
 * @param self The checkbox; must not be null.
 */
lh_void
lh_gx_checkbox_change(lh_gx_checkbox_t *self);

/**
 * @brief Make @p checked the state and show or hide the tick to match; the
 *        reaction is not called.
 *
 * Showing or hiding the tick is reported, so only the square is redrawn.
 *
 * @param self    The checkbox; must not be null.
 * @param checked The state.
 */
lh_void
lh_gx_checkbox_check(lh_gx_checkbox_t *self, lh_bool_t checked);

/**
 * @brief Lay the square, the tick and the caption out in the row: the square
 *        as high as the row at its left, the tick inset by a quarter of that,
 *        the caption half a square right of it and centred in the row's
 *        height (empty with no font or no string).
 *
 * @param self The checkbox; must not be null.
 */
lh_void
lh_gx_checkbox_lay_out(lh_gx_checkbox_t *self);

/**
 * @brief The user's toggle: the other state (::lh_gx_checkbox_check), then the
 *        reaction (::lh_gx_checkbox_change).
 *
 * @param self The checkbox; must not be null.
 */
lh_void
lh_gx_checkbox_toggle(lh_gx_checkbox_t *self);

/**
 * @brief Make @p text the caption, lay it out again, and report the row to be
 *        redrawn.
 *
 * @param self The checkbox; must not be null.
 * @param text The string, UTF-8, or ::lh_null for none; not owned.
 */
lh_void
lh_gx_checkbox_set_text(lh_gx_checkbox_t *self, const lh_char_t *text);

/**
 * @brief `input` of the checkbox class (::lh_gx_class_input_fn): the event goes
 *        to the checkbox's tap (::lh_gx_tap_take), and a tap toggles it
 *        (::lh_gx_checkbox_toggle).
 *
 * @param entity The row of a checkbox.
 * @param event  The event.
 */
lh_void
lh_gx_checkbox_input(lh_gx_entity_t *entity, const lh_gx_event_t *event);

/**
 * @brief A checkbox at @p x, @p y, @p width by @p height (relative to the owner
 *        it will be attached to), with the caption @p text, the state
 *        @p checked and the reaction @p on_change given @p context.
 *
 * The square, the tick and the caption are attached to the row and laid out
 * (::lh_gx_checkbox_lay_out); the row is attached to nothing — that is the
 * application's (::lh_gx_checkbox_get_row_mut).
 *
 * @param self       The checkbox; must not be null.
 * @param style      The square's fill and border, the caption's ink and font;
 *                   not owned.
 * @param tick_style The tick's fill; not owned.
 * @param x          Left edge, relative to the owner.
 * @param y          Top edge, relative to the owner.
 * @param width      Width of the whole row, caption included.
 * @param height     Height; the square is this high and as wide.
 * @param text       The caption, UTF-8, or ::lh_null; not owned.
 * @param checked    The state to start in.
 * @param on_change  The reaction, or ::lh_null.
 * @param context    What the reaction is given.
 */
lh_void
lh_gx_checkbox_init(lh_gx_checkbox_t *self, const lh_gx_style_t *style,
                    const lh_gx_style_t *tick_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                    lh_gx_scalar_t width, lh_gx_scalar_t height, const lh_char_t *text,
                    lh_bool_t checked, lh_gx_checkbox_change_fn *on_change, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_CHECKBOX_H */
