/**
 * @file wrap.h
 * @brief A text broken into lines at a width, one line at a time: ::lh_str_wrap_t.
 *
 * A UTF-8 text is laid out line after line: a line takes as many words as fit
 * in the width, breaks at the last space before the edge, and the next line
 * starts past the spaces it broke at. A line feed ends a line wherever it is. A
 * word wider than the whole width takes a line of its own, as far as it fits —
 * and its first letter always does, so every line moves the text on. A space
 * may hang past the edge: it is where the line breaks, not ink. A width of `0`
 * breaks only at line feeds.
 *
 * How wide a code is, is the caller's to say (::lh_str_wrap_advance_fn): the
 * core knows no font. gl and gx measure with their own fonts and break the same
 * way (::lh_str_wrap_measure), so their lines are the same lines.
 *
 * A value on the stack of whoever lays the text out; it points into the text,
 * which must outlive it.
 */

#ifndef LH_CORE_STR_WRAP_H
#define LH_CORE_STR_WRAP_H

#include <lh/core/bool.h>
#include <lh/core/char.h>
#include <lh/core/str/wrap/advance/fn.h>
#include <lh/core/str/wrap/fields.h>

/**
 * @struct lh_str_wrap
 * @brief One line of a broken text. Members: ::lh_str_wrap_fields.
 */
struct lh_str_wrap
{
    /** Members: ::lh_str_wrap_fields. */
    lh_str_wrap_fields(lh_char_t, lh_s32_t);
};
/** @copybrief lh_str_wrap */
typedef struct lh_str_wrap lh_str_wrap_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The line's first byte (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_str_wrap_get_begin(const lh_str_wrap_t *self);

/**
 * @brief Store the line's first byte (level 1).
 *
 * @param self  The line; checked by ::lh_assert_runtime_ref.
 * @param begin Into the text.
 */
lh_void
lh_str_wrap_set_begin(lh_str_wrap_t *self, const lh_char_t *begin);

/**
 * @brief One past the line's last drawn byte (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_str_wrap_get_end(const lh_str_wrap_t *self);

/**
 * @brief Store one past the line's last drawn byte (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 * @param end  Into the text.
 */
lh_void
lh_str_wrap_set_end(lh_str_wrap_t *self, const lh_char_t *end);

/**
 * @brief Where the next line begins, or ::lh_null when the text ends with this one
 *        (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
const lh_char_t *
lh_str_wrap_get_next(const lh_str_wrap_t *self);

/**
 * @brief Store where the next line begins (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 * @param next Into the text, or ::lh_null.
 */
lh_void
lh_str_wrap_set_next(lh_str_wrap_t *self, const lh_char_t *next);

/**
 * @brief The sum of the line's advances, whole pixels (level 1).
 *
 * @param self The line; checked by ::lh_assert_runtime_ref.
 */
lh_s32_t
lh_str_wrap_get_width(const lh_str_wrap_t *self);

/**
 * @brief Store the sum of the line's advances (level 1).
 *
 * @param self  The line; checked by ::lh_assert_runtime_ref.
 * @param width Pixels.
 */
lh_void
lh_str_wrap_set_width(lh_str_wrap_t *self, lh_s32_t width);

/**
 * @brief An empty line: no text, no width.
 *
 * @param self The line; checked by the setters.
 */
lh_void
lh_str_wrap_init(lh_str_wrap_t *self);

/**
 * @brief Whether @p code ends a line wherever it is: the text's end (`0`) or a line
 *        feed.
 *
 * @param code A Unicode code point.
 */
lh_bool_t
lh_str_wrap_is_line_end(lh_u32_t code);

/**
 * @brief Where the line after one that ends at @p end begins: past a line feed; past
 *        the spaces it broke at when it was broken to a width; ::lh_null when the
 *        text ends there.
 *
 * @param end   One past the line's last drawn byte.
 * @param width The width it was broken to; `0` breaks only at line feeds.
 * @return The next line's first byte, or ::lh_null.
 */
const lh_char_t *
lh_str_wrap_get_next_of(const lh_char_t *end, lh_s32_t width);

/**
 * @brief Break the line that begins at @p text to @p width: as many words as fit,
 *        the line's width, and where the next line begins.
 *
 * @param self    The line; checked by the setters.
 * @param text    The line's first byte, UTF-8; ::lh_null for no line.
 * @param width   The width to break to, whole pixels; `0` breaks only at line
 *                feeds.
 * @param advance How wide a code is; must not be null.
 * @param context Handed to @p advance: the font.
 * @return False for no text (@p self an empty line); true otherwise.
 */
lh_bool_t
lh_str_wrap_measure(lh_str_wrap_t *self, const lh_char_t *text, lh_s32_t width,
                    lh_str_wrap_advance_fn *advance, lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_CORE_STR_WRAP_H */
