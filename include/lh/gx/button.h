/**
 * @file button.h
 * @brief A button: ::lh_gx_button_t, the first control made of entities.
 *
 * A button is not a kind of entity; it is a **composition** of two:
 *
 * - the **box**, an entity of ::lh_gx_button_class — the card of the box class
 *   (::lh_gx_box_paint, ::lh_gx_box_covers) with an `input` slot of its own;
 * - the **caption**, an entity of ::lh_gx_text_class owned by the box and
 *   centred in it. The text takes no input, so a press on the caption is a press
 *   on the box (::lh_gx_hit_find passes it over).
 *
 * The box's content points back at this struct, so the `input` slot finds the
 * button's state and reaction from the entity it is given. One style serves both
 * entities: the box takes its fill, border and radius, the caption its ink and
 * font.
 *
 * What it does with input: a press makes it pressed (and puts on
 * `pressed_style`, if there is one); a release on the button is a click — it is
 * released and the application's reaction is called; a release elsewhere or a
 * cancel only releases it. The look changes are reported (the box is
 * invalidated), so the renderer redraws the button and nothing else.
 *
 * Ownership: the struct is the application's — a field of its screen, an
 * element of its array — and gx never stores it. The application attaches the
 * box (::lh_gx_button_get_box_mut) where the button goes, and detaches it
 * before the memory goes. The string, the styles and the reaction's context are
 * the application's and must outlive the button.
 *
 * Usage:
 * @code
 * lh_gx_button_init(&screen->ok, &theme[my_style_button], 24, 72, 120, 36, "OK",
 *                   on_ok, screen);
 * lh_gx_entity_attach(&screen->root, lh_gx_button_get_box_mut(&screen->ok));
 * @endcode
 */

#ifndef LH_GX_BUTTON_H
#define LH_GX_BUTTON_H

#include <lh/gx/button/click/fn.h>
#include <lh/gx/button/fields.h>
#include <lh/gx/class.h>
#include <lh/gx/style.h>
#include <lh/gx/tap.h>

/**
 * @struct lh_gx_button
 * @brief A box that takes input, owning a caption; its looks, reaction and state.
 */
struct lh_gx_button
{
    /** Members: ::lh_gx_button_fields. */
    lh_gx_button_fields(lh_gx_entity_t, lh_gx_style_t, lh_gx_button_click_fn, lh_gx_tap_t);
};
/** @copybrief lh_gx_button */
typedef struct lh_gx_button lh_gx_button_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a button's box: `paint` ::lh_gx_box_paint, `covers`
 *        ::lh_gx_box_covers, `input` ::lh_gx_button_input.
 */
extern const lh_gx_class_t lh_gx_button_class;

/**
 * @brief The box: the entity the application attaches (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The entity, inside @p self.
 */
const lh_gx_entity_t *
lh_gx_button_get_box(const lh_gx_button_t *self);

/**
 * @brief The box, mutable: to attach it, move it, show or hide it (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The entity, inside @p self.
 */
lh_gx_entity_t *
lh_gx_button_get_box_mut(lh_gx_button_t *self);

/**
 * @brief The caption: the text inside the box (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The entity, inside @p self.
 */
const lh_gx_entity_t *
lh_gx_button_get_caption(const lh_gx_button_t *self);

/**
 * @brief The caption, mutable (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The entity, inside @p self.
 */
lh_gx_entity_t *
lh_gx_button_get_caption_mut(lh_gx_button_t *self);

/**
 * @brief How the button looks at rest (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_button_get_style(const lh_gx_button_t *self);

/**
 * @brief Store how the button looks at rest (level 1); the entities keep the look
 *        they have until ::lh_gx_button_set_look.
 *
 * @param self  The button; checked by ::lh_assert_runtime_ref.
 * @param style The style; not owned.
 */
lh_void
lh_gx_button_set_style(lh_gx_button_t *self, const lh_gx_style_t *style);

/**
 * @brief How the button looks while pressed, or ::lh_null to look the same
 *        (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 */
const lh_gx_style_t *
lh_gx_button_get_pressed_style(const lh_gx_button_t *self);

/**
 * @brief Store how the button looks while pressed (level 1).
 *
 * @param self  The button; checked by ::lh_assert_runtime_ref.
 * @param style The style, or ::lh_null to look the same; not owned.
 */
lh_void
lh_gx_button_set_pressed_style(lh_gx_button_t *self, const lh_gx_style_t *style);

/**
 * @brief The application's reaction to a click, or ::lh_null (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 */
lh_gx_button_click_fn *
lh_gx_button_get_on_click(const lh_gx_button_t *self);

/**
 * @brief Store the reaction to a click (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @param fn   The reaction, or ::lh_null for none.
 */
lh_void
lh_gx_button_set_on_click(lh_gx_button_t *self, lh_gx_button_click_fn *fn);

/**
 * @brief What the reaction is given (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_gx_button_get_context(const lh_gx_button_t *self);

/**
 * @brief Store what the reaction is given (level 1).
 *
 * @param self    The button; checked by ::lh_assert_runtime_ref.
 * @param context The application's; not owned.
 */
lh_void
lh_gx_button_set_context(lh_gx_button_t *self, lh_ptr context);

/**
 * @brief The gesture on the button (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_tap_t *
lh_gx_button_get_tap(const lh_gx_button_t *self);

/**
 * @brief The gesture on the button, to feed (level 1).
 *
 * @param self The button; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_tap_t *
lh_gx_button_get_tap_mut(lh_gx_button_t *self);

/**
 * @brief The button whose box is @p entity: the box's content.
 *
 * @param entity The box of a button; must not be null.
 * @return The button.
 */
lh_gx_button_t *
lh_gx_button_from_entity(lh_gx_entity_t *entity);

/**
 * @brief True while a press on the button is in progress (::lh_gx_tap_is_pressed).
 *
 * @param self The button; must not be null.
 */
lh_bool_t
lh_gx_button_is_pressed(const lh_gx_button_t *self);

/**
 * @brief The caption's string.
 *
 * @param self The button; must not be null.
 * @return The string, or ::lh_null for none.
 */
const lh_char_t *
lh_gx_button_get_text(const lh_gx_button_t *self);

/**
 * @brief Call the reaction to a click with the context; nothing when there is none.
 *
 * @param self The button; must not be null.
 */
lh_void
lh_gx_button_click(lh_gx_button_t *self);

/**
 * @brief Put on @p style: the box and the caption both take it, and the box is
 *        reported to be redrawn (::lh_gx_entity_invalidate).
 *
 * @param self  The button; must not be null.
 * @param style The style; not owned.
 */
lh_void
lh_gx_button_set_look(lh_gx_button_t *self, const lh_gx_style_t *style);

/**
 * @brief Place the caption in the middle of the box: as wide as its string in the
 *        caption's font (::lh_gx_text_get_width), as tall as a line of it.
 *
 * Called by the button when its string changes; the application calls it after
 * it resizes the box or changes the font. A caption with no font or no string is
 * left empty at the box's corner. A string wider than the box starts at its left
 * edge. Nothing is reported.
 *
 * @param self The button; must not be null.
 */
lh_void
lh_gx_button_center_caption(lh_gx_button_t *self);

/**
 * @brief The button was pressed: put on the pressed look if it has one.
 *
 * Only the look; whether a press is in progress is the tap's
 * (::lh_gx_button_get_tap_mut).
 *
 * @param self The button; must not be null.
 */
lh_void
lh_gx_button_press(lh_gx_button_t *self);

/**
 * @brief The press ended: the look at rest again, and — when @p click — the
 *        reaction.
 *
 * @param self  The button; must not be null.
 * @param click Whether the release was on the button.
 */
lh_void
lh_gx_button_release(lh_gx_button_t *self, lh_bool_t click);

/**
 * @brief Make @p text the caption, centre it, and report the box to be redrawn.
 *
 * @param self The button; must not be null.
 * @param text The string, UTF-8, or ::lh_null for none; not owned.
 */
lh_void
lh_gx_button_set_text(lh_gx_button_t *self, const lh_char_t *text);

/**
 * @brief `input` of the button class (::lh_gx_class_input_fn): the event goes to
 *        the button's tap (::lh_gx_tap_take); its step down presses the button
 *        (::lh_gx_button_press), up releases it, and a tap releases it with a
 *        click (::lh_gx_button_release).
 *
 * @param entity The box of a button.
 * @param event  The event.
 */
lh_void
lh_gx_button_input(lh_gx_entity_t *entity, const lh_gx_event_t *event);

/**
 * @brief A button of @p style at @p x, @p y, @p width by @p height (relative to
 *        the owner it will be attached to), with the caption @p text and the
 *        reaction @p on_click given @p context.
 *
 * The caption is attached to the box and centred; the box is attached to
 * nothing — that is the application's (::lh_gx_button_get_box_mut). No pressed
 * look (::lh_gx_button_set_pressed_style).
 *
 * @param self     The button; must not be null.
 * @param style    How it looks; not owned.
 * @param x        Left edge, relative to the owner.
 * @param y        Top edge, relative to the owner.
 * @param width    Width.
 * @param height   Height.
 * @param text     The caption, UTF-8, or ::lh_null; not owned.
 * @param on_click The reaction, or ::lh_null.
 * @param context  What the reaction is given.
 */
lh_void
lh_gx_button_init(lh_gx_button_t *self, const lh_gx_style_t *style, lh_gx_scalar_t x,
                  lh_gx_scalar_t y, lh_gx_scalar_t width, lh_gx_scalar_t height,
                  const lh_char_t *text, lh_gx_button_click_fn *on_click, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_BUTTON_H */
