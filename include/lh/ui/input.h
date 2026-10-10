/**
 * @file input.h
 * @brief A line of text that can be typed into: ::lh_ui_input_t.
 *
 * The component the configurator was missing, and the one whose absence cost a day:
 * its add-device dialog had three fields, and each of them was a label, a second
 * label eight pixels wide for the caret, a focus index in the application's own
 * struct and about twenty lines of key handling. The caret was drawn with the
 * **value's** style -- which pads by ten -- inside a box eight wide, so the text
 * began ten pixels into an eight-pixel box and was cut away by the panel's own
 * clip. The one thing on screen that said which field the keyboard was in was the
 * one thing not on screen, and it took a screenshot to see it.
 *
 * An input **is** a ::lh_ui_label_t (::lh_ui_label_class is its base, and the label
 * is its first field), so the text, the font, the padding, the alignment and the
 * drawing are the label's code, unchanged. What the input adds is the three things a
 * label has no place for: a **buffer it can edit**, a **caret**, and the keyboard.
 *
 * Two decisions worth writing down, because both were arrived at the hard way.
 *
 * **The text is a value, not a pointer.** ::lh_ui_label_set_text keeps the address
 * it is given, and the rule that comes with it is that the string must outlive the
 * label; a label handed a local buffer reads freed stack on the next paint, and the
 * window dies with `0xC000041D` inside the window procedure. A field cannot live
 * that way: inserting a character shifts everything after the caret right, and
 * deleting shifts it left. So the application hands over **a buffer and a capacity**
 * and the input edits the bytes itself. The buffer is still not owned -- it belongs
 * to the application, which is where it should be, since `lh` has no allocator --
 * but it is a buffer and not a string, so the lifetime rule is a field of the
 * application's own struct rather than a local.
 *
 * **The caret is an index in bytes and never lands inside a code point.** Moving it
 * left steps back over UTF-8 continuation bytes, and inserting at the end of a buffer
 * that is one byte short writes nothing rather than half a character -- half a code
 * point is not a character, and the next read over it walks off the end of the text.
 *
 * **It does not blink.** The caret is drawn whenever the field holds the focus, and
 * that is what it does; a component that measured a clock it did not own and faded
 * its own caret would be a second, quieter copy of the same mistake. An application
 * that wants a blinking caret gives it one, by drawing the caret itself.
 *
 * Damage, as everywhere in `lh`, is the view's: every setter here changes the text
 * or the caret and **nothing invalidates**. ::lh_ui_input_get_caret_rect is there so
 * that the application can damage exactly the caret and the text that moved with it,
 * rather than the whole field.
 */

#ifndef LH_UI_INPUT_H
#define LH_UI_INPUT_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/numeric/types.h>
#include <lh/ptr.h>
#include <lh/ui/color.h>
#include <lh/ui/entity.h>
#include <lh/ui/label.h>
#include <lh/ui/rect.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/void.h>

/**
 * @struct lh_ui_input
 * @typedef lh_ui_input_t
 * @brief A label, the buffer it edits, and where the caret is.
 */
struct lh_ui_input
{
    lh_ui_label_t label;      /**< The label this input is (first field). */
    lh_char_t *buffer;        /**< The text, edited in place. **Not owned**: the application
                                   owns it and it must outlive the input, and it must be
                                   at least @p capacity + 1 bytes, because the text is
                                   always terminated. See ::lh_ui_input_init. */
    lh_u16_t capacity;        /**< How many bytes may be used, terminator aside. */
    lh_u16_t used;            /**< How many bytes are in use, terminator aside. */
    lh_u16_t caret;           /**< Where the caret is, in bytes from the start. Always on a
                                   code point boundary, and never past @p used. */
    lh_bool_t focused;        /**< True while it holds the focus. The caret is drawn only
                                   when this is true. */
    const lh_ui_color_t *caret_color; /**< What the caret is drawn in, or ::lh_null for the
                                           text's own colour. Not owned. */
};
typedef struct lh_ui_input lh_ui_input_t;

LH_COMPILER_EXTERN_C_BEGIN

/* -- Class ---------------------------------------------------------------- */

extern const lh_ui_entity_class_t lh_ui_input_class;

/**
 * @brief Event function of ::lh_ui_input_class: the label first (its draw, its
 *        measure, its baseline), then ::lh_ui_input_on_focus,
 *        ::lh_ui_input_on_defocus, ::lh_ui_input_on_key, ::lh_ui_input_on_press
 *        and ::lh_ui_input_on_draw_caret.
 */
lh_void
lh_ui_input_event(const struct lh_ui_entity *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_focus and ::lh_ui_entity_event_defocus: remember
 *        which of the two it was. A derived class may call it directly.
 */
lh_void
lh_ui_input_on_focus(const lh_ui_input_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_focusable: yes.
 *
 * This is not a formality. A press puts the focus on
 * `lh_ui_entity_find_focusable` of whatever was hit, ::lh_ui_view_get_next_focus walks
 * the focusables in order for Tab, and a key goes to whoever holds the focus -- so a
 * field that says no cannot be clicked into, cannot be tabbed to and never receives a
 * key, with every setter in this header working perfectly on a widget nobody can
 * reach. The label underneath says no, which is right for a label.
 */
lh_void
lh_ui_input_on_focusable(const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_key, while the field has the focus: typing,
 *        backspace, delete, the arrows, home and end. A key it does not know is
 *        the application's, and it says so with a false answer.
 *
 * @return True when the text or the caret moved. No damage either way.
 */
lh_bool_t
lh_ui_input_on_key(const lh_ui_input_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_press: put the caret on the nearest code point
 *        boundary at or before the point. A press inside the text moves the caret;
 *        a press past its end puts it at the end.
 */
lh_bool_t
lh_ui_input_on_press(const lh_ui_input_t *self, const lh_ui_entity_event_t *event);

/**
 * @brief On ::lh_ui_entity_event_draw, after the label has drawn the text: the
 *        caret, when the field has the focus.
 */
lh_void
lh_ui_input_on_draw_caret(const lh_ui_input_t *self, const lh_ui_entity_event_t *event);

/* -- Lifetime ------------------------------------------------------------- */

/**
 * @brief An empty input at @p rect, editing @p buffer (not owned).
 *
 * @param buffer   At least @p capacity + 1 bytes, and it must outlive the input.
 *                 A buffer of `0` leaves the input with a text it cannot change and
 *                 every editing call answers false -- which is a field that refuses to
 *                 be typed into rather than one that writes past its end.
 * @param capacity How many bytes may be used, terminator aside.
 *
 * The text starts empty and the caret at the start. No styles: the look is the
 * label's, which reads it out of the entity's style like every other label.
 */
lh_void
lh_ui_input_init(lh_ui_input_t *self, lh_ui_rect_t rect, lh_char_t *buffer, lh_u16_t capacity);

/**
 * @brief The entity @p self is: what a tree holds and what a hit test returns.
 */
lh_ui_entity_t *
lh_ui_input_as_entity(lh_ui_input_t *self);

/**
 * @brief The label @p self is -- the text, the font and the drawing, unchanged.
 */
lh_ui_label_t *
lh_ui_input_as_label(lh_ui_input_t *self);

/**
 * @brief The input @p entity is, or ::lh_null when it is not one. The class is
 *        what tells the two apart. A field **is** found as a label too, since it is
 *        one, and so is one found as a button: a field that says "click me" is not
 *        a field.
 */
lh_ui_input_t *
lh_ui_entity_as_input(lh_ui_entity_t *entity);

/* -- Text ----------------------------------------------------------------- */

/**
 * @brief The text @p self holds, terminated, or ::lh_null when it has no buffer.
 *
 * The pointer is @p self's own buffer and changes as it is typed into: keep the
 * **text** if it is wanted, not the address.
 */
const lh_char_t *
lh_ui_input_get_text(const lh_ui_input_t *self);

/**
 * @brief Put @p text in, truncated to what fits, and put the caret at the end.
 *
 * A cut lands on a code point boundary: a string that does not fit loses its last
 * character rather than its last character's second byte.
 *
 * @return False when there was no buffer, which is the one refusal worth reporting.
 */
lh_bool_t
lh_ui_input_set_text(lh_ui_input_t *self, const lh_char_t *text);

/**
 * @brief Empty the text and put the caret at the start.
 */
lh_void
lh_ui_input_clear(lh_ui_input_t *self);

/**
 * @brief How many bytes the text is long, terminator aside.
 *
 * Bytes, not characters: this is what ::lh_ui_input_get_caret is measured in, and a
 * caller that wants characters walks the buffer with ::lh_ui_text_next_code.
 */
lh_u16_t
lh_ui_input_get_length(const lh_ui_input_t *self);

/* -- Editing -------------------------------------------------------------- */

/**
 * @brief Put @p code at the caret and step over it.
 *
 * @return False when the code is not encodable, when there is no room, or when
 *         there is no buffer. Nothing is written in any of those cases: a refused
 *         character leaves the text exactly as it was.
 */
lh_bool_t
lh_ui_input_insert(lh_ui_input_t *self, lh_u32_t code);

/**
 * @brief Delete the code point before the caret and step the caret over what is
 *        now there.
 *
 * @return False at the start of the text, where there is nothing before the caret
 *         to delete.
 */
lh_bool_t
lh_ui_input_backspace(lh_ui_input_t *self);

/**
 * @brief Delete the code point at the caret, leaving the caret where it is.
 *
 * @return False at the end of the text.
 */
lh_bool_t
lh_ui_input_delete_forward(lh_ui_input_t *self);

/* -- Caret ---------------------------------------------------------------- */

/**
 * @brief Where the caret is, in bytes from the start.
 */
lh_u16_t
lh_ui_input_get_caret(const lh_ui_input_t *self);

/**
 * @brief Put the caret at byte @p caret, or ::lh_bool_false when that is past the
 *        end of the text or inside a code point.
 *
 * A refusal leaves the caret where it was, rather than moving it to the nearest
 * boundary: a caller that asked for a byte that is not there has a number wrong,
 * and silently giving it a different one hides that.
 */
lh_bool_t
lh_ui_input_set_caret(lh_ui_input_t *self, lh_u16_t caret);

/** @brief Put the caret before the first character. */
lh_void
lh_ui_input_home(lh_ui_input_t *self);

/** @brief Put the caret after the last character. */
lh_void
lh_ui_input_end(lh_ui_input_t *self);

/**
 * @brief Where the caret is drawn: a column on the text's own line, from the top of
 *        the ink to the bottom of it.
 *
 * An empty field has no ink, and its caret is drawn over the font's ascent instead --
 * a caret whose height comes from the text is **no caret at all** in an empty field,
 * which is the state a field is in for exactly as long as it takes to notice it.
 *
 * An empty rect when the field has no font or no buffer.
 */
lh_ui_rect_t
lh_ui_input_get_caret_rect(const lh_ui_input_t *self);

/**
 * @brief What the caret is drawn in (not owned), or ::lh_null for the text's own
 *        colour.
 */
const lh_ui_color_t *
lh_ui_input_get_caret_color(const lh_ui_input_t *self);

/**
 * @brief Draw the caret in @p color (not owned), or in ::lh_null for the text's own.
 */
lh_void
lh_ui_input_set_caret_color(lh_ui_input_t *self, const lh_ui_color_t *color);

/**
 * @brief True while @p self holds the focus.
 */
lh_bool_t
lh_ui_input_is_focused(const lh_ui_input_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_INPUT_H */