/**
 * @file input.h
 * @brief The pointer lines a browser sends to the gx web application.
 *
 * The page of `lh/gx/web/page.h` sends the pointer as one websocket text line
 * per event: a letter for the kind, then the point in canvas pixels, the fields
 * separated by one space:
 *
 * - `p x y` — pressed (::lh_gx_event_kind_press);
 * - `m x y` — moved while pressed (::lh_gx_event_kind_move);
 * - `r x y` — released (::lh_gx_event_kind_release);
 * - `c` — the gesture was taken away (::lh_gx_event_kind_cancel).
 *
 * The coordinates are whole numbers and may be negative: a captured pointer
 * goes on reporting outside the canvas. The functions here only read a line
 * into an ::lh_gx_event_t; ::lh_gx_pointer_take does the rest. Nothing is
 * allocated and nothing is kept.
 */

#ifndef LH_GX_WEB_INPUT_H
#define LH_GX_WEB_INPUT_H

#include <lh/gx/event.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The kind of event the letter @p code stands for: `p`, `m`, `r`, `c`.
 *
 * @param code The first character of a line.
 * @param out  Receives the kind; untouched on failure. Must not be null.
 * @return ::lh_bool_true when @p code is one of the four letters.
 */
lh_bool_t
lh_gx_web_input_try_get_kind(lh_char_t code, lh_gx_event_kind_t *out);

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
 * @brief Read one pointer line into an event.
 *
 * A cancel is the letter alone and is at `(0, 0)`; the other kinds take
 * exactly two coordinates. Anything else — an unknown letter, a missing or
 * extra field, a field that is not a number — is refused.
 *
 * @param text The line; not required to be NUL-terminated. May be ::lh_null
 *             when @p size is `0`.
 * @param size How many characters the line has.
 * @param out  Receives the event; untouched on failure. Must not be null.
 * @return ::lh_bool_true when the line is a pointer line.
 */
lh_bool_t
lh_gx_web_input_try_parse(const lh_char_t *text, lh_usize_t size, lh_gx_event_t *out);

LH_COMPILER_EXTERN_C_END

#endif /* LH_GX_WEB_INPUT_H */
