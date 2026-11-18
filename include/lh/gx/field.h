/**
 * @file field.h
 * @brief A one-line text field: ::lh_gx_field_t.
 *
 * A **composition** of four entities, like the other controls:
 *
 * - the **box**, of ::lh_gx_field_class: the box class's look, the pointer
 *   (`input`) and the keys (`key`) — the only control so far that takes the
 *   focus (`lh/gx/focus.h`);
 * - the **view**, the box less ::LH_GX_FIELD_PADDING on the left and the right:
 *   it draws nothing and its children are cut to it, so the text scrolls
 *   inside it and never over the border;
 * - the **text**, a text entity whose string is the application's buffer;
 * - the **caret**, ::LH_GX_FIELD_CARET_WIDTH wide and a line high, shown only
 *   while the field has the focus.
 *
 * The text is UTF-8 in a buffer the application owns, with a fixed capacity:
 * a character that does not fit is refused, never cut. The cursor is a byte
 * offset that always stands between two characters, and every step, insertion
 * and deletion moves by whole characters (`lh/core/str/utf8.h`).
 *
 * What it does with input:
 *
 * - a press puts the cursor at the boundary between characters nearest to the
 *   pointer (and the focus comes to the field, the focus's own affair);
 * - the focus puts on `focus_style` and shows the caret, the blur takes them
 *   off;
 * - a typed character is inserted at the cursor; Backspace and Delete remove
 *   the character before and after it; Left, Right, Home and End move it —
 *   each change of the text calls `on_change`, and Enter calls `on_submit`;
 * - the view scrolls so that the cursor is always in it.
 *
 * Every change is reported (moves and invalidations), so the renderer redraws
 * the field and nothing else.
 *
 * Ownership: the struct is the application's and gx never stores it. The
 * application attaches the box (::lh_gx_field_get_box_mut) where the field
 * goes. The buffer, the styles and the reactions' context are the
 * application's and must outlive the field.
 *
 * Usage:
 * @code
 * static lh_char_t name[64] = "";
 *
 * lh_gx_field_init(&screen->name, &theme.field, &theme.focused, &theme.caret, 16, 40, 200, 24,
 *                  name, sizeof(name), on_name, on_name_done, screen);
 * lh_gx_entity_attach(&screen->root, lh_gx_field_get_box_mut(&screen->name));
 * @endcode
 */

#ifndef LH_GX_FIELD_H
#define LH_GX_FIELD_H

#include <lh/gx/class.h>
#include <lh/gx/entity.h>
#include <lh/gx/event.h>
#include <lh/gx/field/fields.h>
#include <lh/gx/field/fn.h>
#include <lh/gx/style.h>

/**
 * @def LH_GX_FIELD_PADDING
 * @brief Pixels between the box's edge and the text, on the left and the right.
 */
#define LH_GX_FIELD_PADDING 4

/**
 * @def LH_GX_FIELD_CARET_WIDTH
 * @brief How wide the cursor is, in pixels.
 */
#define LH_GX_FIELD_CARET_WIDTH 1

/**
 * @struct lh_gx_field
 * @brief A text field. Members: ::lh_gx_field_fields.
 */
struct lh_gx_field
{
    /** Members: ::lh_gx_field_fields. */
    lh_gx_field_fields(lh_gx_entity_t, lh_gx_style_t, lh_char_t, lh_usize_t, lh_s32_t,
                       lh_gx_field_fn);
};
/** @copybrief lh_gx_field */
typedef struct lh_gx_field lh_gx_field_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The class of a field's box: the box's `paint` and `covers`, its own
 *        `input` (::lh_gx_field_input) and `key` (::lh_gx_field_key).
 */
extern const lh_gx_class_t lh_gx_field_class;

/**
 * @brief The box: the entity the application attaches (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_field_get_box(const lh_gx_field_t *self);

/**
 * @brief The box, to attach (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_field_get_box_mut(lh_gx_field_t *self);

/**
 * @brief The view the text scrolls in (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_field_get_view(const lh_gx_field_t *self);

/**
 * @brief The view, to place (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_field_get_view_mut(lh_gx_field_t *self);

/**
 * @brief The text entity (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value; a member is never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_field_get_text_entity(const lh_gx_field_t *self);

/**
 * @brief The text entity, to place (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value; a member is never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_field_get_text_entity_mut(lh_gx_field_t *self);

/**
 * @brief The cursor's entity (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
const lh_gx_entity_t *
lh_gx_field_get_caret(const lh_gx_field_t *self);

/**
 * @brief The cursor's entity, to place and show (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The member itself, never ::lh_null.
 */
lh_gx_entity_t *
lh_gx_field_get_caret_mut(lh_gx_field_t *self);

/**
 * @brief The look at rest: the box's and the text's (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
const lh_gx_style_t *
lh_gx_field_get_style(const lh_gx_field_t *self);

/**
 * @brief Store the look at rest (level 1); nothing is redrawn.
 *
 * @param self  The field; checked by ::lh_assert_runtime_ref.
 * @param style The value to store.
 */
lh_void
lh_gx_field_set_style(lh_gx_field_t *self, const lh_gx_style_t *style);

/**
 * @brief The box's look while it has the focus (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
const lh_gx_style_t *
lh_gx_field_get_focus_style(const lh_gx_field_t *self);

/**
 * @brief Store the look with the focus (level 1); nothing is redrawn.
 *
 * @param self  The field; checked by ::lh_assert_runtime_ref.
 * @param style The value to store.
 */
lh_void
lh_gx_field_set_focus_style(lh_gx_field_t *self, const lh_gx_style_t *style);

/**
 * @brief The cursor's look (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
const lh_gx_style_t *
lh_gx_field_get_caret_style(const lh_gx_field_t *self);

/**
 * @brief Store the cursor's look (level 1); nothing is redrawn.
 *
 * @param self  The field; checked by ::lh_assert_runtime_ref.
 * @param style The value to store.
 */
lh_void
lh_gx_field_set_caret_style(lh_gx_field_t *self, const lh_gx_style_t *style);

/**
 * @brief The buffer, UTF-8 ending in `0`: the text (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
const lh_char_t *
lh_gx_field_get_text(const lh_gx_field_t *self);

/**
 * @brief The buffer, to write into (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_char_t *
lh_gx_field_get_buffer_mut(const lh_gx_field_t *self);

/**
 * @brief Store the buffer (level 1); nothing is measured or redrawn.
 *
 * @param self   The field; checked by ::lh_assert_runtime_ref.
 * @param buffer The value to store.
 */
lh_void
lh_gx_field_set_buffer(lh_gx_field_t *self, lh_char_t *buffer);

/**
 * @brief How many bytes the buffer has, the `0` included (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_usize_t
lh_gx_field_get_capacity(const lh_gx_field_t *self);

/**
 * @brief Store how many bytes the buffer has (level 1).
 *
 * @param self     The field; checked by ::lh_assert_runtime_ref.
 * @param capacity The value to store.
 */
lh_void
lh_gx_field_set_capacity(lh_gx_field_t *self, lh_usize_t capacity);

/**
 * @brief How many bytes of text the buffer holds, the `0` excluded (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_usize_t
lh_gx_field_get_length(const lh_gx_field_t *self);

/**
 * @brief Store how many bytes of text there are (level 1).
 *
 * @param self   The field; checked by ::lh_assert_runtime_ref.
 * @param length The value to store.
 */
lh_void
lh_gx_field_set_length(lh_gx_field_t *self, lh_usize_t length);

/**
 * @brief The byte offset the cursor stands at (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_usize_t
lh_gx_field_get_cursor(const lh_gx_field_t *self);

/**
 * @brief Store the cursor (level 1); it is not checked nor drawn — that is
 *        ::lh_gx_field_place_cursor.
 *
 * @param self   The field; checked by ::lh_assert_runtime_ref.
 * @param cursor The value to store.
 */
lh_void
lh_gx_field_set_cursor(lh_gx_field_t *self, lh_usize_t cursor);

/**
 * @brief How many pixels of text are scrolled off the view's left (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_s32_t
lh_gx_field_get_scroll(const lh_gx_field_t *self);

/**
 * @brief Store the scroll (level 1); nothing moves.
 *
 * @param self   The field; checked by ::lh_assert_runtime_ref.
 * @param scroll The value to store.
 */
lh_void
lh_gx_field_set_scroll(lh_gx_field_t *self, lh_s32_t scroll);

/**
 * @brief The reaction to a change of the text (level 1); ::lh_null for none.
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_gx_field_fn *
lh_gx_field_get_on_change(const lh_gx_field_t *self);

/**
 * @brief Store the reaction to a change (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @param fn   The value to store.
 */
lh_void
lh_gx_field_set_on_change(lh_gx_field_t *self, lh_gx_field_fn *fn);

/**
 * @brief The reaction to Enter (level 1); ::lh_null for none.
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_gx_field_fn *
lh_gx_field_get_on_submit(const lh_gx_field_t *self);

/**
 * @brief Store the reaction to Enter (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @param fn   The value to store.
 */
lh_void
lh_gx_field_set_on_submit(lh_gx_field_t *self, lh_gx_field_fn *fn);

/**
 * @brief What the reactions are given (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_ptr
lh_gx_field_get_context(const lh_gx_field_t *self);

/**
 * @brief Store what the reactions are given (level 1).
 *
 * @param self    The field; checked by ::lh_assert_runtime_ref.
 * @param context The value to store.
 */
lh_void
lh_gx_field_set_context(lh_gx_field_t *self, lh_ptr context);

/**
 * @brief Whether the field has the focus (level 1).
 *
 * @param self The field; checked by ::lh_assert_runtime_ref.
 * @return The value.
 */
lh_bool_t
lh_gx_field_is_focused(const lh_gx_field_t *self);

/**
 * @brief Store whether the field has the focus (level 1); nothing is redrawn —
 *        that is ::lh_gx_field_show_focus.
 *
 * @param self    The field; checked by ::lh_assert_runtime_ref.
 * @param focused The value to store.
 */
lh_void
lh_gx_field_set_focused(lh_gx_field_t *self, lh_bool_t focused);

/**
 * @brief The field whose box is @p entity: the box's content.
 *
 * @param entity The box of a field; must not be null.
 */
lh_gx_field_t *
lh_gx_field_from_entity(lh_gx_entity_t *entity);

/**
 * @brief The font the text is in: the look at rest's.
 *
 * @param self The field; must not be null.
 * @return The font, or ::lh_null when there is none — the text is then not
 *         drawn and every width is `0`.
 */
const lh_gx_font_t *
lh_gx_field_get_font(const lh_gx_field_t *self);

/**
 * @brief Call @p fn with the field and its context; nothing for ::lh_null.
 *
 * @param self The field; must not be null.
 * @param fn   ::lh_gx_field_get_on_change or ::lh_gx_field_get_on_submit.
 */
lh_void
lh_gx_field_call(lh_gx_field_t *self, lh_gx_field_fn *fn);

/**
 * @brief Show whether the field has the focus: the focus look and the caret
 *        while it has it, the look at rest without. The box is reported to be
 *        redrawn.
 *
 * @param self    The field; must not be null.
 * @param focused Whether it has the focus.
 */
lh_void
lh_gx_field_show_focus(lh_gx_field_t *self, lh_bool_t focused);

/**
 * @brief How wide the text before the cursor is: where the cursor stands from
 *        the text's left, in pixels.
 *
 * @param self The field; must not be null.
 */
lh_s32_t
lh_gx_field_get_caret_x(const lh_gx_field_t *self);

/**
 * @brief The boundary between characters nearest to @p at: the byte offset a
 *        press there puts the cursor at.
 *
 * Only the point's x counts. Left of the text it is `0`, right of it the
 * length.
 *
 * @param self The field; must be on a screen. Must not be null.
 * @param at   A point, screen coordinates; must not be null.
 */
lh_usize_t
lh_gx_field_get_cursor_at(const lh_gx_field_t *self, const lh_gx_point_t *at);

/**
 * @brief Scroll so that the cursor is in the view: a cursor past the right edge
 *        brings the text left, one before the left edge brings it right, and a
 *        text that fits is not scrolled at all.
 *
 * @param self The field; must not be null.
 */
lh_void
lh_gx_field_keep_caret_shown(lh_gx_field_t *self);

/**
 * @brief Place the view, the text and the cursor for the scroll. The moves are
 *        reported, so a field on a screen is redrawn where they changed.
 *
 * @param self The field; must not be null.
 */
lh_void
lh_gx_field_lay_out(lh_gx_field_t *self);

/**
 * @brief After a change of the text or the cursor: keep the cursor shown, lay
 *        the field out, and report the view to be redrawn.
 *
 * @param self The field; must not be null.
 */
lh_void
lh_gx_field_refresh(lh_gx_field_t *self);

/**
 * @brief Put the cursor at @p cursor and refresh the field.
 *
 * @param self   The field; must not be null.
 * @param cursor A byte offset between two characters, at most the length.
 */
lh_void
lh_gx_field_place_cursor(lh_gx_field_t *self, lh_usize_t cursor);

/**
 * @brief Insert the character @p code at the cursor and move the cursor past
 *        it. No reaction.
 *
 * @param self The field; must not be null.
 * @param code A Unicode code point.
 * @return ::lh_bool_false when it is not a character UTF-8 can hold or the
 *         buffer has no room for all its bytes: nothing is inserted.
 */
lh_bool_t
lh_gx_field_insert(lh_gx_field_t *self, lh_u32_t code);

/**
 * @brief Remove the bytes from @p from to @p to and put the cursor at @p from.
 *        No reaction.
 *
 * @param self The field; must not be null.
 * @param from A boundary between characters.
 * @param to   A boundary at or after @p from, at most the length.
 */
lh_void
lh_gx_field_erase(lh_gx_field_t *self, lh_usize_t from, lh_usize_t to);

/**
 * @brief Move the cursor one character left, or right; nothing at the start,
 *        or the end.
 *
 * @param self    The field; must not be null.
 * @param forward ::lh_bool_true for right.
 */
lh_void
lh_gx_field_step(lh_gx_field_t *self, lh_bool_t forward);

/**
 * @brief Make the text @p text from the program: copied into the buffer —
 *        as much of it as fits, whole characters only — the cursor at its end.
 *        No reaction.
 *
 * @param self The field; must not be null.
 * @param text UTF-8 ending in `0`; must not be null.
 */
lh_void
lh_gx_field_set_text(lh_gx_field_t *self, const lh_char_t *text);

/**
 * @brief `input` of the field class (::lh_gx_class_input_fn): a press puts the
 *        cursor where the pointer is (::lh_gx_field_get_cursor_at).
 *
 * @param entity The box of a field.
 * @param event  The event.
 */
lh_void
lh_gx_field_input(lh_gx_entity_t *entity, const lh_gx_event_t *event);

/**
 * @brief A field at @p x, @p y, @p width by @p height (relative to the owner it
 *        will be attached to), over the application's @p buffer.
 *
 * The buffer must hold a string ending in `0` already (an empty one is
 * `""`); the cursor goes to its end. The view, the text and the cursor are
 * attached and laid out; the box is attached to nothing — that is the
 * application's.
 *
 * @param self        The field; must not be null.
 * @param style       The look at rest — box and text; not owned.
 * @param focus_style The box's look with the focus; not owned.
 * @param caret_style The cursor's look; not owned.
 * @param x           Left edge, relative to the owner.
 * @param y           Top edge, relative to the owner.
 * @param width       Width.
 * @param height      Height.
 * @param buffer      The application's buffer; not owned.
 * @param capacity    How many bytes it has, the `0` included; at least `1`.
 * @param on_change   The reaction to a change of the text, or ::lh_null.
 * @param on_submit   The reaction to Enter, or ::lh_null.
 * @param context     What the reactions are given.
 */
lh_void
lh_gx_field_init(lh_gx_field_t *self, const lh_gx_style_t *style, const lh_gx_style_t *focus_style,
                 const lh_gx_style_t *caret_style, lh_gx_scalar_t x, lh_gx_scalar_t y,
                 lh_gx_scalar_t width, lh_gx_scalar_t height, lh_char_t *buffer,
                 lh_usize_t capacity, lh_gx_field_fn *on_change, lh_gx_field_fn *on_submit,
                 lh_ptr context);

/**
 * @brief Remove the character before the cursor (Backspace). No reaction.
 *
 * @param self The field; must not be null.
 * @return ::lh_bool_false at the start of the text: nothing is removed.
 */
lh_bool_t
lh_gx_field_erase_before(lh_gx_field_t *self);

/**
 * @brief Remove the character after the cursor (Delete). No reaction.
 *
 * @param self The field; must not be null.
 * @return ::lh_bool_false at the end of the text: nothing is removed.
 */
lh_bool_t
lh_gx_field_erase_after(lh_gx_field_t *self);

/**
 * @brief Act on @p key: Left, Right, Home and End move the cursor; Backspace
 *        and Delete remove a character and call `on_change`; Enter calls
 *        `on_submit`. Any other key does nothing.
 *
 * @param self The field; must not be null.
 * @param key  The key.
 */
lh_void
lh_gx_field_press_key(lh_gx_field_t *self, lh_gx_key_t key);

/**
 * @brief `key` of the field class (::lh_gx_class_key_fn): the focus and the
 *        blur show it (::lh_gx_field_show_focus), a text is inserted and calls
 *        `on_change`, a key goes to ::lh_gx_field_press_key.
 *
 * @param entity The box of a field.
 * @param event  The event.
 */
lh_void
lh_gx_field_key(lh_gx_entity_t *entity, const lh_gx_event_t *event);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_FIELD_H */
