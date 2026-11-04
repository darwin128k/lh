/**
 * @file reader.h
 * @brief Read an HTML text into an ::lh_web_xml_node_t tree (::lh_web_html_reader_t).
 *
 * HTML is read on top of XML: the reader holds an ::lh_web_xml_reader_t and
 * reads with its steps - names, references, quoted values, text, comments,
 * end tags, the doctype body - adding only what HTML spells differently:
 *
 * - void elements (::lh_web_html_void_is_name) have no end tag and become
 *   ::lh_web_xml_kind_empty; `<x/>` is taken as empty for any element;
 * - the bodies of `script` and `style` (::lh_web_html_rawtext_is_name) are
 *   read up to their end tag as one ::lh_web_xml_kind_raw child;
 * - an attribute may be bare (`disabled`) or unquoted (`width=980`);
 * - tag names and the `<!doctype` keyword fold ASCII case; names are kept as
 *   written;
 * - a document may hold any number of top-level elements, comments and a
 *   doctype; whitespace between them is not kept.
 *
 * The result is the tree the writer writes back in the HTML dialect
 * (::lh_web_xml_writer_set_dialect), so the markup lh writes reads back into
 * the same tree and writes again byte for byte.
 *
 * It is a reader of well-formed HTML, not the HTML Living Standard's
 * tree-construction algorithm: no end tag is implied (`<p>a<p>b` is an
 * error), nothing is reparented or recovered, references are XML's (the five
 * predefined entities and `&#...;`; `&nbsp;` is an error), and the first error
 * stops the read with an ::lh_web_xml_error_t
 * (::lh_web_html_reader_get_error).
 *
 * Example usage:
 * @code{.c}
 * lh_web_html_reader_t reader;
 * lh_web_xml_node_t doc;
 *
 * lh_web_xml_node_init(&doc);
 * lh_web_html_reader_init(&reader, lh_str_view_lit("<!doctype html><p>a<br>b</p>"));
 * if (lh_web_html_reader_read(&reader, &doc)) {
 *     // doc: the doctype, then p with the text "a", the void br and the text "b"
 * }
 * lh_web_xml_node_deinit(&doc);
 * @endcode
 */

#ifndef LH_WEB_HTML_READER_H
#define LH_WEB_HTML_READER_H

#include <lh/core/web/html/reader/fields.h>
#include <lh/core/web/xml/reader.h>

/**
 * @struct lh_web_html_reader
 * @brief The XML reader underneath. Fields via ::lh_web_html_reader_fields.
 */
struct lh_web_html_reader
{
    /** Members: ::lh_web_html_reader_fields. */
    lh_web_html_reader_fields(lh_web_xml_reader_t);
};
/** @copybrief lh_web_html_reader */
typedef struct lh_web_html_reader lh_web_html_reader_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The XML reader @p self reads with, to read. */
lh_web_xml_reader_t *
lh_web_html_reader_get_xml_mut(lh_web_html_reader_t *self);

/** The XML reader @p self reads with: its cursor tells where the read stands. */
const lh_web_xml_reader_t *
lh_web_html_reader_get_xml(const lh_web_html_reader_t *self);

/** The error that stopped the read, or ::lh_web_xml_error_none. */
lh_web_xml_error_t
lh_web_html_reader_get_error(const lh_web_html_reader_t *self);

/* ── init ────────────────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first character, past a UTF-8 byte
 *        order mark. Nothing is owned; @p text must outlive the read.
 */
lh_void
lh_web_html_reader_init(lh_web_html_reader_t *self, lh_str_view_t text);

/* ── steps ───────────────────────────────────────────────────────────────── */

/**
 * @brief Read the unquoted attribute value at the position - up to
 *        whitespace, a quote, `=`, `<`, `>` or a backtick - and append it to
 *        @p out, references decoded.
 *
 * @return ::lh_bool_false when the value is empty or a reference is bad.
 */
lh_bool_t
lh_web_html_reader_read_unquoted_into(lh_web_html_reader_t *self, lh_str_t *out);

/**
 * @brief Read the body of the raw-text element @p node (`script`, `style`) up
 *        to its end tag, without the end tag, into one raw child; no child for
 *        an empty body.
 *
 * @return ::lh_bool_false when the end tag never comes.
 */
lh_bool_t
lh_web_html_reader_read_rawtext(lh_web_html_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the attribute at the position into a new attribute of @p node:
 *        `name`, `name=value`, `name="value"` or `name='value'`. A bare name
 *        makes a bare attribute (::lh_web_xml_attr_set_bare).
 *
 * @return ::lh_bool_false on a bad name, a bad value, or a name @p node
 *         already has (::lh_web_xml_error_duplicate_attr).
 */
lh_bool_t
lh_web_html_reader_read_attr(lh_web_html_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the element at the position into @p node: the start tag with
 *        its attributes, then - unless it is void or ends in `/>` - its body
 *        (raw text or content) and its end tag, matched without regard to
 *        ASCII case.
 */
lh_bool_t
lh_web_html_reader_read_element(lh_web_html_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the children of the element @p node up to its end tag: text,
 *        elements and comments, each a new child in order. Stops in front of
 *        `</`.
 */
lh_bool_t
lh_web_html_reader_read_content(lh_web_html_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the whole text as one document into @p node.
 *
 * @p node must be freshly initialized (::lh_web_xml_node_init) and becomes a
 * document whose children are the top-level doctype, comments and elements.
 * On false it may hold what was read before the error;
 * ::lh_web_xml_node_deinit it either way.
 *
 * @return ::lh_bool_true if the whole text was read; ::lh_bool_false
 *         otherwise (see ::lh_web_html_reader_get_error), text outside every
 *         element being ::lh_web_xml_error_trailing.
 */
lh_bool_t
lh_web_html_reader_read(lh_web_html_reader_t *self, lh_web_xml_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_READER_H */
