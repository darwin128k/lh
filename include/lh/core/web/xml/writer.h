/**
 * @file writer.h
 * @brief Write an ::lh_web_xml_node_t tree as XML or HTML markup
 *        (::lh_web_xml_writer_t).
 *
 * The tree holds no markup, only what the document is; the writer is the one
 * place that turns it into text, appended to an ::lh_str_t it does not own.
 * Its dialect (::lh_web_xml_dialect_t, XML unless
 * ::lh_web_xml_writer_set_dialect says HTML) decides the few things the two
 * languages spell differently:
 *
 * - a document as its children, one after another;
 * - an element as `<name attrs>`, its children, `</name>`;
 * - an empty element as `<name attrs/>` in XML, `<name attrs>` (a void
 *   element) in HTML;
 * - an attribute as ` name="value"`; a bare one as ` name`, in HTML only;
 * - text with ::lh_web_xml_escape_append (`&`, `<`, `>`, CR), attribute
 *   values the same way in their own context (`&`, `<`, `"`, tab, LF, CR);
 * - raw markup as it is;
 * - a comment as `<!--text-->`, a CDATA section as `<![CDATA[text]]>`, a
 *   processing instruction as `<?target text?>` (`<?target?>` without text),
 *   a doctype as `<!DOCTYPE text>` (`<!doctype text>` in HTML; HTML has no
 *   CDATA sections or processing instructions and refuses them).
 *
 * Nothing is added between nodes: no line breaks, no indentation, so the
 * markup is exactly the tree, and a tree read by ::lh_web_xml_reader_read
 * writes back the text it was read from, up to the quotes of attribute values,
 * the spelling of references and the whitespace between top-level nodes.
 *
 * Names are checked with ::lh_web_xml_name_is_valid, since they cannot be
 * escaped; neither can the bodies of comments, CDATA sections and processing
 * instructions, so the writer refuses a body that would end its node early.
 * Every step is public, so a producer of a markup this writer does not know
 * composes them instead of copying them.
 *
 * ::lh_web_xml_writer_try_write reports a tree it cannot write by returning
 * false; the text appended up to that point stays in the output.
 * ::lh_web_xml_writer_write is the same with a runtime check.
 *
 * Example usage:
 * @code{.c}
 * lh_web_xml_writer_t writer;
 * lh_str_t text;
 *
 * lh_str_init(&text);
 * lh_web_xml_writer_init(&writer, &text);
 * if (lh_web_xml_writer_try_write(&writer, root)) {
 *     // text: the markup of root and everything under it
 * }
 * lh_str_deinit(&text);
 * @endcode
 */

#ifndef LH_WEB_XML_WRITER_H
#define LH_WEB_XML_WRITER_H

#include <lh/core/web/xml/dialect.h>
#include <lh/core/web/xml/node.h>
#include <lh/core/web/xml/writer/fields.h>

/**
 * @struct lh_web_xml_writer
 * @brief The output and the dialect. Fields via ::lh_web_xml_writer_fields.
 */
struct lh_web_xml_writer
{
    /** Members: ::lh_web_xml_writer_fields. */
    lh_web_xml_writer_fields(lh_str_t *, lh_web_xml_dialect_t);
};
/** @copybrief lh_web_xml_writer */
typedef struct lh_web_xml_writer lh_web_xml_writer_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The string @p self appends to, to change. */
lh_str_t *
lh_web_xml_writer_get_out_mut(lh_web_xml_writer_t *self);

/** Make @p out the string @p self appends to. */
lh_void
lh_web_xml_writer_set_out(lh_web_xml_writer_t *self, lh_str_t *out);

/** The language @p self writes. */
lh_web_xml_dialect_t
lh_web_xml_writer_get_dialect(const lh_web_xml_writer_t *self);

/** Make @p self write @p dialect from now on. */
lh_void
lh_web_xml_writer_set_dialect(lh_web_xml_writer_t *self, lh_web_xml_dialect_t dialect);

/** True when @p self writes HTML (::lh_web_xml_dialect_html). */
lh_bool_t
lh_web_xml_writer_is_html(const lh_web_xml_writer_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start writing XML at the end of @p out. @p out is appended to, never
 *        cleared, and stays the caller's. ::lh_web_xml_writer_set_dialect
 *        switches to HTML.
 */
lh_void
lh_web_xml_writer_init(lh_web_xml_writer_t *self, lh_str_t *out);

/* ── text ────────────────────────────────────────────────────────────────── */

/** Append @p ch as it is. */
lh_void
lh_web_xml_writer_put_char(lh_web_xml_writer_t *self, lh_char_t ch);

/** Append @p text as it is. */
lh_void
lh_web_xml_writer_put_view(lh_web_xml_writer_t *self, lh_str_view_t text);

/**
 * @brief Append @p text escaped for its context (::lh_web_xml_escape_append):
 *        @p in_attr true for an attribute value, false for character data.
 */
lh_void
lh_web_xml_writer_put_escaped(lh_web_xml_writer_t *self, const lh_str_t *text, lh_bool_t in_attr);

/** Append @p text as it is. */
lh_void
lh_web_xml_writer_put_str(lh_web_xml_writer_t *self, const lh_str_t *text);

/**
 * @brief Append @p name as it is when ::lh_web_xml_name_is_valid accepts it.
 * @return False, appending nothing, when it does not.
 */
lh_bool_t
lh_web_xml_writer_try_put_name(lh_web_xml_writer_t *self, const lh_str_t *name);

/* ── markup ──────────────────────────────────────────────────────────────── */

/** Append the value of @p attr as `="value"`, escaped. Its name is not written. */
lh_void
lh_web_xml_writer_write_attr_value(lh_web_xml_writer_t *self, const lh_web_xml_attr_t *attr);

/**
 * @brief Append the comment @p node as `<!--text-->`.
 * @return False when the text holds `--` or ends with `-`.
 */
lh_bool_t
lh_web_xml_writer_try_write_comment(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append the CDATA section @p node as `<![CDATA[text]]>`.
 * @return False when the text holds `]]>`.
 */
lh_bool_t
lh_web_xml_writer_try_write_cdata(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append @p attr as ` name="value"`, or ` name` when it is bare.
 * @return False when the name is not valid, or @p attr is bare and the
 *         dialect is XML, which has no bare attributes.
 */
lh_bool_t
lh_web_xml_writer_try_write_attr(lh_web_xml_writer_t *self, const lh_web_xml_attr_t *attr);

/**
 * @brief Append the end tag of the element @p node, `</name>`.
 * @return False when the name is not valid.
 */
lh_bool_t
lh_web_xml_writer_try_write_close(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append the doctype @p node as `<!DOCTYPE text>`, or `<!doctype text>`
 *        in HTML. The text is written as it is.
 */
lh_void
lh_web_xml_writer_write_doctype(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append the processing instruction @p node as `<?target text?>`, or
 *        `<?target?>` when the text is empty.
 * @return False when the target is not a valid name or the text holds `?>`.
 */
lh_bool_t
lh_web_xml_writer_try_write_pi(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append the start of the tag of the element @p node: `<name` and
 *        every attribute in order, without the closing `>` or `/>`.
 * @return False when a name is not valid or an attribute is bare.
 */
lh_bool_t
lh_web_xml_writer_try_write_open(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append @p node, whatever kind it is, children and all.
 * @return False when the tree cannot be written: a node of kind
 *         ::lh_web_xml_kind_none, a name that is not valid, an empty element
 *         with children, a comment, CDATA section or processing instruction
 *         whose text would end it early; in XML a bare attribute, in HTML a
 *         CDATA section or a processing instruction. What was appended
 *         before stays.
 */
lh_bool_t
lh_web_xml_writer_try_write(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief Append every child of @p node in order, each with
 *        ::lh_web_xml_writer_try_write.
 * @return False at the first child that cannot be written.
 */
lh_bool_t
lh_web_xml_writer_try_write_children(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

/**
 * @brief ::lh_web_xml_writer_try_write, checked: a tree that cannot be
 *        written is a runtime check failure
 *        (::lh_runtime_error_code_invalid_argument).
 */
lh_void
lh_web_xml_writer_write(lh_web_xml_writer_t *self, const lh_web_xml_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_WRITER_H */
