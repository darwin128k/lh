/**
 * @file writer.h
 * @brief Write an ::lh_json_node_t tree as JSON text (::lh_json_writer_t).
 *
 * The job of cJSON's `cJSON_Print` / `cJSON_PrintUnformatted`: the text is
 * appended to an ::lh_str_t, compact (indent `0`) or laid out with the
 * given number of spaces per level.
 *
 * - Strings are escaped: `"` and `\` with a backslash, `\b \f \n \r \t` by
 *   name, every other control character as `\u00XX`. `/` and all bytes from
 *   U+0080 up are written as they are.
 * - Numbers are written as their text, so a tree read from a text writes the
 *   same digits back.
 * - Members are written in order, duplicate keys included.
 * - A node holding nothing (::lh_json_kind_none) is written as `null`.
 *
 * Example usage:
 * @code{.c}
 * lh_json_writer_t writer;
 * lh_str_t text;
 *
 * lh_str_init(&text);
 * lh_json_writer_init(&writer, &text, 2U);
 * lh_json_writer_write(&writer, &root);   // text: the tree, two spaces a level
 * lh_str_deinit(&text);
 * @endcode
 */

#ifndef LH_JSON_WRITER_H
#define LH_JSON_WRITER_H

#include <lh/json/node.h>
#include <lh/json/writer/fields.h>

/**
 * @struct lh_json_writer
 * @brief Output plus layout. Fields via ::lh_json_writer_fields.
 */
struct lh_json_writer
{
    /** Members: ::lh_json_writer_fields. */
    lh_json_writer_fields(lh_str_t *, lh_usize_t);
};
/** @copybrief lh_json_writer */
typedef struct lh_json_writer lh_json_writer_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The string @p self appends to, to change. */
lh_str_t *
lh_json_writer_get_out_mut(lh_json_writer_t *self);

/** Make @p out the string @p self appends to. */
lh_void
lh_json_writer_set_out(lh_json_writer_t *self, lh_str_t *out);

/** How many spaces one level takes; `0` for compact text. */
lh_usize_t
lh_json_writer_get_indent(const lh_json_writer_t *self);

/** Make @p indent how many spaces one level takes; `0` for compact text. */
lh_void
lh_json_writer_set_indent(lh_json_writer_t *self, lh_usize_t indent);

/** How many arrays and objects are open where @p self stands. */
lh_usize_t
lh_json_writer_get_depth(const lh_json_writer_t *self);

/** Make @p depth how many arrays and objects are open. */
lh_void
lh_json_writer_set_depth(lh_json_writer_t *self, lh_usize_t depth);

/** True when @p self lays the text out over lines (indent above `0`). */
lh_bool_t
lh_json_writer_is_pretty(const lh_json_writer_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start writing at the end of @p out, @p indent spaces a level (`0`:
 *        compact). @p out is appended to, never cleared.
 */
lh_void
lh_json_writer_init(lh_json_writer_t *self, lh_str_t *out, lh_usize_t indent);

/* ── text ────────────────────────────────────────────────────────────────── */

/** Append @p ch. */
lh_void
lh_json_writer_put_char(lh_json_writer_t *self, lh_char_t ch);

/** Append @p text as it is. */
lh_void
lh_json_writer_put_view(lh_json_writer_t *self, lh_str_view_t text);

/** Pretty text: a line break and the indent of the current depth. Compact: nothing. */
lh_void
lh_json_writer_put_newline(lh_json_writer_t *self);

/** Append the escape that stands for @p ch inside a string: `\n`, `\u001F`, ... */
lh_void
lh_json_writer_put_escape(lh_json_writer_t *self, lh_char_t ch);

/** True when @p ch must be escaped inside a string: `"`, `\` and every control character. */
lh_bool_t
lh_json_writer_needs_escape(lh_char_t ch);

/** Append @p text as a JSON string: quotes around, escapes inside. */
lh_void
lh_json_writer_put_string(lh_json_writer_t *self, lh_str_view_t text);

/* ── values ──────────────────────────────────────────────────────────────── */

/** Append the array @p node, each element on its own line when pretty. */
lh_void
lh_json_writer_write_array(lh_json_writer_t *self, const lh_json_node_t *node);

/** Append the object @p node, each member on its own line when pretty. */
lh_void
lh_json_writer_write_object(lh_json_writer_t *self, const lh_json_node_t *node);

/** Append the value @p node, whatever kind it is, children and all. */
lh_void
lh_json_writer_write(lh_json_writer_t *self, const lh_json_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_JSON_WRITER_H */
