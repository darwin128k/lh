/**
 * @file parser.h
 * @brief Read an INI text one entry at a time (::lh_ini_parser_t).
 *
 * A pull parser: each ::lh_ini_parser_next hands back the next section
 * header or key/value pair as an ::lh_ini_entry_t whose text points into
 * the INI text. Nothing is allocated and nothing is stored — looking keys
 * up, rejecting duplicates, reading `true` or `42` out of a value is the
 * caller's business.
 *
 * The dialect, line by line (lines end in `\n`, `\r\n` or `\r`; see
 * ::lh_str_view_split_next_line):
 *
 * - Surrounding whitespace is ignored on every line, key and value.
 * - An empty line, or one whose first character is `;` or `#`, is skipped.
 *   Comments take the whole line: `key = a ; b` has the value `a ; b`.
 * - `[name]` starts section `name`. Every pair must be in a section.
 * - `key = value` is a pair. The first `=` splits it, so the value may hold
 *   more. The value may be empty. No quotes, escapes or continuation lines:
 *   the value is the rest of the line, as written.
 * - A UTF-8 byte order mark at the start of the text is skipped.
 *
 * The first line that fits none of these stops the parse:
 * ::lh_ini_parser_next returns ::lh_bool_false from then on, and
 * ::lh_ini_parser_get_error and ::lh_ini_parser_get_line say what and where.
 *
 * Example usage:
 * @code{.c}
 * lh_ini_parser_t parser;
 * lh_ini_entry_t entry;
 *
 * lh_ini_parser_init(&parser, text);
 * while (lh_ini_parser_next(&parser, &entry)) {
 *     if (lh_ini_entry_has_kind(&entry, lh_ini_entry_kind_pair)) {
 *         lh_str_view_t key = lh_ini_entry_get_key(&entry);
 *         lh_str_view_t value = lh_ini_entry_get_value(&entry);
 *     }
 * }
 * if (lh_ini_parser_has_error(&parser)) {
 *     // lh_ini_parser_get_line(&parser), lh_ini_parser_get_error(&parser)
 * }
 * @endcode
 */

#ifndef LH_INI_PARSER_H
#define LH_INI_PARSER_H

#include <lh/ini/entry.h>
#include <lh/ini/error.h>
#include <lh/ini/parser/fields.h>
#include <lh/str/line/reader.h>

/**
 * @struct lh_ini_parser
 * @brief A line reader plus INI state. Fields via ::lh_ini_parser_fields.
 */
struct lh_ini_parser
{
    /** Members: ::lh_ini_parser_fields. */
    lh_ini_parser_fields(lh_str_line_reader_t, lh_str_view_t, lh_ini_error_t);
};
/** @copybrief lh_ini_parser */
typedef struct lh_ini_parser lh_ini_parser_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The line reader under @p self, to change. */
lh_str_line_reader_t *
lh_ini_parser_get_reader_mut(lh_ini_parser_t *self);

/**
 * @brief The 1-based number of the last line read. Right after
 *        ::lh_ini_parser_next it is the line of that entry, or of the
 *        error; at the end of the text, the last line (blank lines and
 *        comments count). `0` before the first call.
 */
lh_usize_t
lh_ini_parser_get_line(const lh_ini_parser_t *self);

/**
 * @brief The name of the last section header read; empty before the first.
 *
 * While reading, every entry already carries its section
 * (::lh_ini_entry_get_section). This is for when there is no entry: after
 * a syntax error, it names the section the bad line is in, next to
 * ::lh_ini_parser_get_line.
 */
lh_str_view_t
lh_ini_parser_get_section(const lh_ini_parser_t *self);

/** Make @p section the section the next pairs of @p self are in. */
void
lh_ini_parser_set_section(lh_ini_parser_t *self, lh_str_view_t section);

/**
 * @brief The error that stopped the parse, or ::lh_ini_error_none.
 */
lh_ini_error_t
lh_ini_parser_get_error(const lh_ini_parser_t *self);

/** Make @p error the error of @p self; anything but ::lh_ini_error_none stops the parse. */
void
lh_ini_parser_set_error(lh_ini_parser_t *self, lh_ini_error_t error);

/**
 * @brief Stop on @p error: this and every later ::lh_ini_parser_next returns false.
 *
 * @return ::lh_bool_false, so a reader can `return lh_ini_parser_fail(...)`.
 */
lh_bool_t
lh_ini_parser_fail(lh_ini_parser_t *self, lh_ini_error_t error);

/**
 * @brief True when a syntax error stopped the parse.
 */
lh_bool_t
lh_ini_parser_has_error(const lh_ini_parser_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first line.
 *
 * The parser owns nothing; @p text must outlive it and every entry it
 * returns.
 */
void
lh_ini_parser_init(lh_ini_parser_t *self, lh_str_view_t text);

/* ── lines ───────────────────────────────────────────────────────────────── */

/** True when the trimmed @p line is empty or a whole-line comment: nothing to read. */
lh_bool_t
lh_ini_parser_is_skipped(const lh_str_view_t *line);

/**
 * @brief Read the trimmed @p line, which starts with `[`, as a `[name]` header
 *        into @p entry and make it the current section.
 *
 * @return ::lh_bool_true if @p entry was filled; ::lh_bool_false on a bad or
 *         empty name, which stops the parse (see ::lh_ini_parser_fail).
 */
lh_bool_t
lh_ini_parser_read_section(lh_ini_parser_t *self, const lh_str_view_t *line, lh_ini_entry_t *entry);

/**
 * @brief Read the trimmed, not skipped, non-header @p line as `key = value`
 *        in the current section into @p entry.
 *
 * @return ::lh_bool_true if @p entry was filled; ::lh_bool_false on a missing
 *         `=`, an empty key or no section yet, which stops the parse.
 */
lh_bool_t
lh_ini_parser_read_pair(lh_ini_parser_t *self, const lh_str_view_t *line, lh_ini_entry_t *entry);

/**
 * @brief Read the next header or pair into @p entry.
 *
 * @return ::lh_bool_true if @p entry was filled; ::lh_bool_false at the end
 *         of the text or on a syntax error (then and on every later call;
 *         see ::lh_ini_parser_has_error). @p entry is untouched on false.
 */
lh_bool_t
lh_ini_parser_next(lh_ini_parser_t *self, lh_ini_entry_t *entry);

LH_COMPILER_EXTERN_C_END

#endif /* LH_INI_PARSER_H */
