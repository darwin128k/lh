/**
 * @file writer.h
 * @brief Write an ::lh_web_html_node_t tree as HTML markup (::lh_web_html_writer_t).
 *
 * The tree holds no markup, only what the page is; the writer is the one place
 * that turns it into text, appended to an ::lh_str_t it does not own:
 *
 * - the doctype as `<!doctype html>`;
 * - an element as `<name attrs>`, its children, `</name>`;
 * - a void element as `<name attrs>`, and nothing else;
 * - an attribute as ` name="value"`, or ` name` when bare;
 * - text with ::lh_web_html_escape_append (`&`, `<`, `>`), attribute values
 *   the same way in their own context (`&`, `<`, `"`);
 * - raw markup as it is.
 *
 * Nothing is added between nodes: no line breaks, no indentation, so the
 * markup is exactly the tree. Tag and attribute names are checked with
 * ::lh_web_html_name_is_valid, since they cannot be escaped.
 *
 * ::lh_web_html_writer_try_write reports a tree it cannot write - a node of
 * no kind, a name that is not valid, a void element with children - by
 * returning false; the text appended up to that point stays in the output.
 * ::lh_web_html_writer_write is the same with a runtime check.
 *
 * Example usage:
 * @code{.c}
 * lh_web_html_writer_t writer;
 * lh_str_t text;
 *
 * lh_str_init(&text);
 * lh_web_html_writer_init(&writer, &text);
 * if (lh_web_html_writer_try_write(&writer, root)) {
 *     // text: the markup of root and everything under it
 * }
 * lh_str_deinit(&text);
 * @endcode
 */

#ifndef LH_WEB_HTML_WRITER_H
#define LH_WEB_HTML_WRITER_H

#include <lh/web/html/node.h>
#include <lh/web/html/writer/fields.h>

/**
 * @struct lh_web_html_writer
 * @brief The output. Fields via ::lh_web_html_writer_fields.
 */
struct lh_web_html_writer
{
    /** Members: ::lh_web_html_writer_fields. */
    lh_web_html_writer_fields(lh_str_t *);
};
/** @copybrief lh_web_html_writer */
typedef struct lh_web_html_writer lh_web_html_writer_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The string @p self appends to, to change. */
lh_str_t *
lh_web_html_writer_get_out_mut(lh_web_html_writer_t *self);

/** Make @p out the string @p self appends to. */
lh_void
lh_web_html_writer_set_out(lh_web_html_writer_t *self, lh_str_t *out);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start writing at the end of @p out. @p out is appended to, never
 *        cleared, and stays the caller's.
 */
lh_void
lh_web_html_writer_init(lh_web_html_writer_t *self, lh_str_t *out);

/* ── text ────────────────────────────────────────────────────────────────── */

/** Append @p ch as it is. */
lh_void
lh_web_html_writer_put_char(lh_web_html_writer_t *self, lh_char_t ch);

/** Append @p text as it is. */
lh_void
lh_web_html_writer_put_view(lh_web_html_writer_t *self, lh_str_view_t text);

/**
 * @brief Append @p name as it is when ::lh_web_html_name_is_valid accepts it.
 * @return False, appending nothing, when it does not.
 */
lh_bool_t
lh_web_html_writer_try_put_name(lh_web_html_writer_t *self, const lh_str_t *name);

/* ── markup ──────────────────────────────────────────────────────────────── */

/**
 * @brief Append @p attr as ` name="value"` (the value escaped), or ` name`
 *        when it is bare.
 * @return False when the name is not valid.
 */
lh_bool_t
lh_web_html_writer_try_write_attr(lh_web_html_writer_t *self, const lh_web_html_attr_t *attr);

/**
 * @brief Append the end tag of the element @p node, `</name>`.
 * @return False when the name is not valid.
 */
lh_bool_t
lh_web_html_writer_try_write_close(lh_web_html_writer_t *self, const lh_web_html_node_t *node);

/**
 * @brief Append the start tag of the element or void element @p node:
 *        `<name`, every attribute in order, `>`.
 * @return False when a name is not valid.
 */
lh_bool_t
lh_web_html_writer_try_write_open(lh_web_html_writer_t *self, const lh_web_html_node_t *node);

/**
 * @brief Append every child of @p node in order, each with
 *        ::lh_web_html_writer_try_write.
 * @return False at the first child that cannot be written.
 */
lh_bool_t
lh_web_html_writer_try_write_children(lh_web_html_writer_t *self, const lh_web_html_node_t *node);

/**
 * @brief Append @p node, whatever kind it is, children and all.
 * @return False when the tree cannot be written: a node of kind
 *         ::lh_web_html_kind_none, a tag or attribute name that is not valid,
 *         a void element with children. What was appended before stays.
 */
lh_bool_t
lh_web_html_writer_try_write(lh_web_html_writer_t *self, const lh_web_html_node_t *node);

/**
 * @brief ::lh_web_html_writer_try_write, checked: a tree that cannot be
 *        written is a runtime check failure
 *        (::lh_runtime_error_code_invalid_argument).
 */
lh_void
lh_web_html_writer_write(lh_web_html_writer_t *self, const lh_web_html_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_WRITER_H */
