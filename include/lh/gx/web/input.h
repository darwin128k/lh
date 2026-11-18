/**
 * @file input.h
 * @brief The lines a browser sends to the gx web application: the pointer and
 *        the keyboard.
 *
 * The page of `lh/gx/web/page.h` sends one websocket text line per event: a
 * letter for the kind, then its data, the fields separated by one space:
 *
 * - `p x y` — pressed (::lh_gx_event_kind_press);
 * - `m x y` — moved while pressed (::lh_gx_event_kind_move);
 * - `r x y` — released (::lh_gx_event_kind_release);
 * - `c` — the gesture was taken away (::lh_gx_event_kind_cancel);
 * - `k name` — a key that does not type, by the browser's `KeyboardEvent.key`
 *   name (`ArrowLeft`, `Backspace`, `Enter`, ... ::lh_gx_web_input_try_get_key);
 * - `t code` — a character typed, by its Unicode code point in decimal.
 *
 * The coordinates are whole numbers in canvas pixels and may be negative: a
 * captured pointer goes on reporting outside the canvas. A key the list does
 * not name is refused, so a page may send every key it sees. The functions
 * here only read a line into an ::lh_gx_event_t; the pointer
 * (::lh_gx_pointer_take) and the focus (::lh_gx_focus_take) do the rest.
 * Nothing is allocated and nothing is kept.
 */

#ifndef LH_GX_WEB_INPUT_H
#define LH_GX_WEB_INPUT_H

#include <lh/gx/event.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The kind of event the letter @p code stands for: `p`, `m`, `r`, `c`,
 *        `k`, `t`.
 *
 * @param code The first character of a line.
 * @param out  Receives the kind; untouched on failure. Must not be null.
 * @return ::lh_bool_true when @p code is one of the six letters.
 */
lh_bool_t
lh_gx_web_input_try_get_kind(lh_char_t code, lh_gx_event_kind_t *out);

/**
 * @brief The key the browser names @p name (`KeyboardEvent.key`): `ArrowLeft`,
 *        `ArrowRight`, `ArrowUp`, `ArrowDown`, `Home`, `End`, `Backspace`,
 *        `Delete`, `Enter`, `Escape`, `Tab`.
 *
 * @param name The name; not required to be NUL-terminated. Case counts.
 * @param size How many characters it has.
 * @param out  Receives the key; untouched on failure. Must not be null.
 * @return ::lh_bool_true when @p name is one of those.
 */
lh_bool_t
lh_gx_web_input_try_get_key(const lh_char_t *name, lh_usize_t size, lh_gx_key_t *out);

/**
 * @brief Read one typed character: a code point in decimal that types — not a
 *        control character (below U+0020, or U+007F) and not half of a
 *        surrogate pair — up to U+10FFFF.
 *
 * @param text The field; not required to be NUL-terminated.
 * @param size How many characters the field has.
 * @param out  Receives the code point; untouched on failure. Must not be null.
 * @return ::lh_bool_true when the whole field is such a code point.
 */
lh_bool_t
lh_gx_web_input_try_parse_code(const lh_char_t *text, lh_usize_t size, lh_u32_t *out);

/**
 * @brief Read one coordinate: a whole number, signed, in the 32-bit range.
 *
 * @param text The field; not required to be NUL-terminated.
 * @param size How many characters the field has.
 * @param out  Receives the coordinate; untouched on failure. Must not be null.
 * @return ::lh_bool_true when the whole field is such a number.
 */
lh_bool_t
lh_gx_web_input_try_parse_scalar(const lh_char_t *text, lh_usize_t size, lh_gx_scalar_t *out);

/**
 * @brief Read one line into an event.
 *
 * A cancel is the letter alone and is at `(0, 0)`; a key takes one name and a
 * text one code point; the pointer's other kinds take exactly two coordinates.
 * Anything else — an unknown letter or key, a missing or extra field, a field
 * that is not a number — is refused.
 *
 * @param text The line; not required to be NUL-terminated. May be ::lh_null
 *             when @p size is `0`.
 * @param size How many characters the line has.
 * @param out  Receives the event; untouched on failure. Must not be null.
 * @return ::lh_bool_true when the line is one of the page's lines.
 */
lh_bool_t
lh_gx_web_input_try_parse(const lh_char_t *text, lh_usize_t size, lh_gx_event_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WEB_INPUT_H */
