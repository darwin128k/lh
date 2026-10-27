/**
 * @file reader.h
 * @brief Read a text line by line and know which line you are on
 *        (::lh_str_line_reader_t).
 *
 * The cursor under any line-based format: it hands out the next line
 * (ends as in ::lh_str_view_split_next_line) and counts them, so a parser
 * on top can say "line 12" when something is wrong. What a line means is
 * the parser's business.
 *
 * Owns nothing: the text must outlive the reader and every line it gives.
 */

#ifndef LH_STR_LINE_READER_H
#define LH_STR_LINE_READER_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/size.h>
#include <lh/str/line/reader/fields.h>
#include <lh/str/view.h>

/**
 * @struct lh_str_line_reader
 * @brief Text plus position. Fields via ::lh_str_line_reader_fields.
 */
struct lh_str_line_reader
{
    /** Members: ::lh_str_line_reader_fields. */
    lh_str_line_reader_fields(lh_str_view_t, lh_usize_t);
};
/** @copybrief lh_str_line_reader */
typedef struct lh_str_line_reader lh_str_line_reader_t;

LH_COMPILER_EXTERN_C_BEGIN

/* ── accessors ───────────────────────────────────────────────────────────── */

/** The whole text @p self reads. */
const lh_str_view_t *
lh_str_line_reader_get_text(const lh_str_line_reader_t *self);

/** Make @p text the whole text @p self reads. Does not move the position. */
lh_void
lh_str_line_reader_set_text(lh_str_line_reader_t *self, lh_str_view_t text);

/** The offset in the text where the next line starts, to change. */
lh_usize_t *
lh_str_line_reader_get_pos_mut(lh_str_line_reader_t *self);

/** Make @p pos the offset in the text where the next line starts. */
lh_void
lh_str_line_reader_set_pos(lh_str_line_reader_t *self, lh_usize_t pos);

/**
 * @brief The 1-based number of the last line read; `0` before the first.
 */
lh_usize_t
lh_str_line_reader_get_number(const lh_str_line_reader_t *self);

/** Make @p number the number of the last line read. */
lh_void
lh_str_line_reader_set_number(lh_str_line_reader_t *self, lh_usize_t number);

/* ── read ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first line.
 */
lh_void
lh_str_line_reader_init(lh_str_line_reader_t *self, lh_str_view_t text);

/**
 * @brief Read the next line into @p line, without its end.
 *
 * Example usage:
 * @code{.c}
 * lh_str_line_reader_t reader;
 * lh_str_view_t line;
 *
 * lh_str_line_reader_init(&reader, text);
 * while (lh_str_line_reader_next(&reader, &line)) {
 *     // lh_str_line_reader_get_number(&reader) is this line's number
 * }
 * @endcode
 *
 * @return ::lh_bool_true if @p line was filled; ::lh_bool_false at the end
 *         of the text, with @p line untouched.
 */
lh_bool_t
lh_str_line_reader_next(lh_str_line_reader_t *self, lh_str_view_t *line);

LH_COMPILER_EXTERN_C_END

#endif /* LH_STR_LINE_READER_H */
