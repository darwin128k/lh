/**
 * @file cursor.h
 * @brief A reading position in a borrowed text (::lh_str_cursor_t): what is
 *        next, move on, skip what matches.
 *
 * A cursor is the part every hand-written reader of a text format shares: it
 * holds the text as a view and the offset of the next character, and answers
 * the questions a recursive-descent reader asks - is the text over, what is
 * the next character, does the text go on with this word, where is the next
 * occurrence of that one. It owns nothing: the text must outlive it. It does
 * not know any grammar; a reader (::lh_web_xml_reader_t, ...) is built on it
 * and adds its own rules and its own errors.
 *
 * Reading past the end is not an error at this level: ::lh_str_cursor_peek
 * gives NUL there, so a reader tests ::lh_str_cursor_is_end when NUL could be
 * real text. The position never goes past the end of the text:
 * ::lh_str_cursor_advance stops there.
 *
 * Example usage:
 * @code{.c}
 * lh_str_cursor_t cursor;
 *
 * lh_str_cursor_init(&cursor, lh_str_view_lit("  <a>"));
 * lh_str_cursor_skip_whitespace(&cursor);
 * if (lh_str_cursor_skip_char(&cursor, lh_char_map_less)) {
 *     // cursor stands at "a>"
 * }
 * @endcode
 */

#ifndef LH_STR_CURSOR_H
#define LH_STR_CURSOR_H

#include <lh/str/cursor/fields.h>
#include <lh/str/view.h>

/**
 * @struct lh_str_cursor
 * @brief The text and the position. Fields via ::lh_str_cursor_fields.
 */
struct lh_str_cursor
{
    /** Members: ::lh_str_cursor_fields. */
    lh_str_cursor_fields(lh_str_view_t, lh_usize_t);
};
/** @copybrief lh_str_cursor */
typedef struct lh_str_cursor lh_str_cursor_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The whole text @p self reads. */
const lh_str_view_t *
lh_str_cursor_get_text(const lh_str_cursor_t *self);

/** Make @p text the whole text @p self reads. Does not move the position. */
lh_void
lh_str_cursor_set_text(lh_str_cursor_t *self, lh_str_view_t text);

/** The offset of the next character to read. */
lh_usize_t
lh_str_cursor_get_pos(const lh_str_cursor_t *self);

/**
 * @brief Make @p pos the offset of the next character to read. Not checked:
 *        @p pos must not be past the end of the text.
 */
lh_void
lh_str_cursor_set_pos(lh_str_cursor_t *self, lh_usize_t pos);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first character. @p text is borrowed
 *        and must outlive @p self; nothing is allocated.
 */
lh_void
lh_str_cursor_init(lh_str_cursor_t *self, lh_str_view_t text);

/* ── position ────────────────────────────────────────────────────────────── */

/** True when @p self has read the whole text. */
lh_bool_t
lh_str_cursor_is_end(const lh_str_cursor_t *self);

/** How many characters are left from the position to the end of the text. */
lh_usize_t
lh_str_cursor_get_left(const lh_str_cursor_t *self);

/** The character @p ahead places past the position; NUL past the end of the text. */
lh_char_t
lh_str_cursor_peek(const lh_str_cursor_t *self, lh_usize_t ahead);

/**
 * @brief Move the position @p count characters on, but never past the end of
 *        the text.
 */
lh_void
lh_str_cursor_advance(lh_str_cursor_t *self, lh_usize_t count);

/**
 * @brief The text from the position to its end, into @p out (a view of the
 *        same storage).
 */
lh_void
lh_str_cursor_get_rest(const lh_str_cursor_t *self, lh_str_view_t *out);

/**
 * @brief The text from offset @p start up to the position, into @p out (a
 *        view of the same storage): what was read since @p start.
 *
 * @p start must not be past the position.
 */
lh_void
lh_str_cursor_get_since(const lh_str_cursor_t *self, lh_usize_t start, lh_str_view_t *out);

/* ── matching ────────────────────────────────────────────────────────────── */

/** The character at the position; NUL at the end of the text. */
lh_char_t
lh_str_cursor_get_char(const lh_str_cursor_t *self);

/** True when the text goes on with @p word at the position (@p ignore_case: ASCII case folded). */
lh_bool_t
lh_str_cursor_is_word(const lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case);

/**
 * @brief How far from the position the next @p word starts (@p ignore_case:
 *        ASCII case folded).
 *
 * @return The distance in characters, 0 when @p word is right at the
 *         position; ::LH_STR_VIEW_INVALID when the rest of the text does not
 *         hold it.
 */
lh_usize_t
lh_str_cursor_find(const lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case);

/** Move past @p ch if it is at the position; false, and no move, if not. */
lh_bool_t
lh_str_cursor_skip_char(lh_str_cursor_t *self, lh_char_t ch);

/** Move past @p word if the text goes on with it; false, and no move, if not. */
lh_bool_t
lh_str_cursor_skip_word(lh_str_cursor_t *self, lh_str_view_t word, lh_bool_t ignore_case);

/**
 * @brief Move past the whitespace at the position (::lh_char_is_whitespace).
 *
 * @return How many characters were skipped.
 */
lh_usize_t
lh_str_cursor_skip_whitespace(lh_str_cursor_t *self);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_CURSOR_H */
