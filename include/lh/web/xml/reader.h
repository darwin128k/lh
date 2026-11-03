/**
 * @file reader.h
 * @brief Read an XML text into an ::lh_web_xml_node_t tree (::lh_web_xml_reader_t).
 *
 * A recursive-descent reader of XML 1.0 documents, built from small public
 * steps (`read_name_into`, `read_reference`, `read_attr`, `read_element`,
 * ...) on an ::lh_str_cursor_t; the HTML reader (`lh/web/html/reader.h`)
 * composes the same steps with HTML's rules. What it does:
 *
 * - Elements, empty elements (`<a/>`, kept as ::lh_web_xml_kind_empty, so
 *   they write back the same), attributes in single or double quotes, text,
 *   comments, CDATA sections, processing instructions (the XML declaration
 *   among them) and the doctype, all kept in the tree in order.
 * - The five predefined entities and character references (`&#38;`,
 *   `&#x26;`) are decoded to UTF-8; a reference to a character XML does not
 *   allow (::lh_web_xml_char_is_valid) is an error.
 * - Line ends are normalized as XML says: CR LF and a lone CR become LF; in
 *   an attribute value tab, CR and LF become a space.
 * - Start and end tags must match; one attribute name may appear once per
 *   tag; one root element, with only comments, processing instructions and
 *   whitespace around it (and the doctype before it).
 * - A UTF-8 byte order mark at the start of the text is skipped.
 *
 * What it does not do: no DTD processing - the doctype is kept as text and
 * its entities are not expanded - no namespaces (a prefix is part of the
 * name), no check of characters in text against the `Char` production, and
 * names are checked only in their ASCII part (::lh_web_xml_name_is_valid).
 * Whitespace between top-level nodes is not kept; whitespace inside elements
 * is text and is kept.
 *
 * The first error stops the read: ::lh_web_xml_reader_get_error says what
 * and the position of ::lh_web_xml_reader_get_cursor where.
 *
 * Example usage:
 * @code{.c}
 * lh_web_xml_reader_t reader;
 * lh_web_xml_node_t doc;
 *
 * lh_web_xml_node_init(&doc);
 * lh_web_xml_reader_init(&reader, lh_str_view_lit("<a x=\"1\">b &amp; c</a>"));
 * if (lh_web_xml_reader_read(&reader, &doc)) {
 *     // doc: a document holding the element a, its attribute x and the text "b & c"
 * }
 * lh_web_xml_node_deinit(&doc);
 * @endcode
 */

#ifndef LH_WEB_XML_READER_H
#define LH_WEB_XML_READER_H

#include <lh/str/cursor.h>
#include <lh/web/xml/error.h>
#include <lh/web/xml/node.h>
#include <lh/web/xml/reader/fields.h>

#ifndef LH_WEB_XML_READER_DEPTH_MAX
/**
 * @def LH_WEB_XML_READER_DEPTH_MAX
 * @brief How many elements may be open at once; deeper is
 *        ::lh_web_xml_error_too_deep. Each level is a C stack frame on the way
 *        in and on ::lh_web_xml_node_deinit, so a small stack wants it lower.
 *        The same default as ::LH_WEB_JSON_READER_DEPTH_MAX.
 */
#    define LH_WEB_XML_READER_DEPTH_MAX 1000U
#endif

/**
 * @struct lh_web_xml_reader
 * @brief Cursor, depth and error. Fields via ::lh_web_xml_reader_fields.
 */
struct lh_web_xml_reader
{
    /** Members: ::lh_web_xml_reader_fields. */
    lh_web_xml_reader_fields(lh_str_cursor_t, lh_usize_t, lh_web_xml_error_t);
};
/** @copybrief lh_web_xml_reader */
typedef struct lh_web_xml_reader lh_web_xml_reader_t;

/* ── accessors ───────────────────────────────────────────────────────────── */

LH_COMPILER_EXTERN_C_BEGIN

/** The cursor @p self reads with, to move. */
lh_str_cursor_t *
lh_web_xml_reader_get_cursor_mut(lh_web_xml_reader_t *self);

/** The cursor @p self reads with: the text, and after an error the offending position. */
const lh_str_cursor_t *
lh_web_xml_reader_get_cursor(const lh_web_xml_reader_t *self);

/** How many elements are open where @p self stands. */
lh_usize_t
lh_web_xml_reader_get_depth(const lh_web_xml_reader_t *self);

/** Make @p depth how many elements are open. */
lh_void
lh_web_xml_reader_set_depth(lh_web_xml_reader_t *self, lh_usize_t depth);

/** The error that stopped the read, or ::lh_web_xml_error_none. */
lh_web_xml_error_t
lh_web_xml_reader_get_error(const lh_web_xml_reader_t *self);

/** Make @p error the error of @p self; anything but ::lh_web_xml_error_none stops the read. */
lh_void
lh_web_xml_reader_set_error(lh_web_xml_reader_t *self, lh_web_xml_error_t error);

/* ── init and state ──────────────────────────────────────────────────────── */

/**
 * @brief Start reading @p text from its first character, past a UTF-8 byte
 *        order mark.
 *
 * The reader owns nothing; @p text must outlive it. The tree it builds
 * copies what it keeps, so the text may go once the read is done.
 */
lh_void
lh_web_xml_reader_init(lh_web_xml_reader_t *self, lh_str_view_t text);

/**
 * @brief Stop on @p error.
 *
 * @return ::lh_bool_false, so a step can `return lh_web_xml_reader_fail(...)`.
 */
lh_bool_t
lh_web_xml_reader_fail(lh_web_xml_reader_t *self, lh_web_xml_error_t error);

/** True when an error stopped the read. */
lh_bool_t
lh_web_xml_reader_has_error(const lh_web_xml_reader_t *self);

/** Close the innermost open element. */
lh_void
lh_web_xml_reader_leave(lh_web_xml_reader_t *self);

/* ── characters ──────────────────────────────────────────────────────────── */

/**
 * @brief Move past one line end at the position - CR LF, CR or LF.
 *
 * @return ::lh_bool_true if there was one.
 */
lh_bool_t
lh_web_xml_reader_skip_newline(lh_web_xml_reader_t *self);

/**
 * @brief Stop on what is at the position: ::lh_web_xml_error_unexpected_end at
 *        the end of the text, ::lh_web_xml_error_unexpected_char otherwise.
 *
 * @return ::lh_bool_false.
 */
lh_bool_t
lh_web_xml_reader_fail_unexpected(lh_web_xml_reader_t *self);

/**
 * @brief Open one more element: ::lh_web_xml_error_too_deep past
 *        ::LH_WEB_XML_READER_DEPTH_MAX.
 *
 * @return ::lh_bool_true if the read may go deeper.
 */
lh_bool_t
lh_web_xml_reader_enter(lh_web_xml_reader_t *self);

/**
 * @brief Read the name at the position - a start character, then name
 *        characters (`lh/web/xml/name.h`) - and append it to @p out.
 *
 * @return ::lh_bool_false when no name starts there (::lh_web_xml_error_bad_name,
 *         ::lh_web_xml_error_unexpected_end at the end of the text).
 */
lh_bool_t
lh_web_xml_reader_read_name_into(lh_web_xml_reader_t *self, lh_str_t *out);

/**
 * @brief Read what follows `&#` - decimal digits, or `x` and hex digits, then
 *        `;` - and append the character as UTF-8 to @p out.
 *
 * @return ::lh_bool_false without digits or `;`, or for a character XML does
 *         not allow (::lh_web_xml_error_bad_reference).
 */
lh_bool_t
lh_web_xml_reader_read_char_reference(lh_web_xml_reader_t *self, lh_str_t *out);

/**
 * @brief Move past @p ch if it is at the position, or stop as
 *        ::lh_web_xml_reader_fail_unexpected.
 *
 * @return ::lh_bool_true if @p ch was there.
 */
lh_bool_t
lh_web_xml_reader_expect_char(lh_web_xml_reader_t *self, lh_char_t ch);

/**
 * @brief Read the reference at the position (`&`, a predefined entity or
 *        `#` and a character reference, `;`) and append the character it
 *        stands for to @p out.
 *
 * @return ::lh_bool_false on anything else (::lh_web_xml_error_bad_reference).
 */
lh_bool_t
lh_web_xml_reader_read_reference(lh_web_xml_reader_t *self, lh_str_t *out);

/**
 * @brief Read character data up to the next `<` or the end of the text and
 *        append it to @p out, references decoded and line ends normalized.
 *
 * @return ::lh_bool_false on a bad reference.
 */
lh_bool_t
lh_web_xml_reader_read_text_into(lh_web_xml_reader_t *self, lh_str_t *out);

/**
 * @brief Read the quoted attribute value at the position (`"..."` or
 *        `'...'`) and append it to @p out: references decoded, tab and line
 *        ends turned into spaces.
 *
 * @return ::lh_bool_false without an opening or closing quote, on `<` in the
 *         value, or on a bad reference.
 */
lh_bool_t
lh_web_xml_reader_read_attr_value_into(lh_web_xml_reader_t *self, lh_str_t *out);

/**
 * @brief Read the comment at the position (`<!--` ... `-->`) into @p node.
 *
 * @return ::lh_bool_false when it does not end or holds `--`
 *         (::lh_web_xml_error_bad_comment).
 */
lh_bool_t
lh_web_xml_reader_read_comment(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/** Read the CDATA section at the position (`<![CDATA[` ... `]]>`) into @p node. */
lh_bool_t
lh_web_xml_reader_read_cdata(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the processing instruction at the position (`<?target text?>`)
 *        into @p node; whitespace after the target is not part of the text.
 */
lh_bool_t
lh_web_xml_reader_read_pi(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read what follows the `<!DOCTYPE` keyword - whitespace, then the
 *        text up to the `>` that ends the declaration, skipping `>` inside
 *        quotes and inside an internal subset `[...]` - into @p node, a
 *        doctype. HTML reads its own keyword (any case) and comes here.
 */
lh_bool_t
lh_web_xml_reader_read_doctype_body(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/** Read the doctype at the position (`<!DOCTYPE` ...) into @p node. */
lh_bool_t
lh_web_xml_reader_read_doctype(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the end tag of the element @p node at the position: `</`, its
 *        name, whitespace, `>`. XML compares the name exactly; HTML passes
 *        @p ignore_case, since its tag names fold ASCII case.
 *
 * @return ::lh_bool_false when the name differs (::lh_web_xml_error_mismatched_tag).
 */
lh_bool_t
lh_web_xml_reader_read_close(lh_web_xml_reader_t *self, const lh_web_xml_node_t *node,
                             lh_bool_t ignore_case);

/* ── nodes ───────────────────────────────────────────────────────────────── */

/**
 * @brief Read `name = "value"` at the position into a new attribute of
 *        @p node.
 *
 * @return ::lh_bool_false on a bad name, a missing `=`, a bad value, or a
 *         name @p node already has (::lh_web_xml_error_duplicate_attr).
 */
lh_bool_t
lh_web_xml_reader_read_attr(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/** Read character data at the position into @p node, which becomes text. */
lh_bool_t
lh_web_xml_reader_read_text(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the children of the element @p node up to its end tag: text,
 *        elements, comments, CDATA sections, processing instructions, each a
 *        new child in order. Stops in front of `</`.
 */
lh_bool_t
lh_web_xml_reader_read_content(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the element at the position into @p node: the start tag with
 *        its attributes, and either `/>` (@p node becomes an empty element) or
 *        `>`, the content and the matching end tag.
 */
lh_bool_t
lh_web_xml_reader_read_element(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

/**
 * @brief Read the whole text as one document into @p node.
 *
 * @p node must be freshly initialized (::lh_web_xml_node_init) and becomes a
 * document whose children are the top-level nodes. On false it may hold what
 * was read before the error; ::lh_web_xml_node_deinit it either way.
 *
 * @return ::lh_bool_true if the text is one well-formed document;
 *         ::lh_bool_false otherwise (see ::lh_web_xml_reader_get_error).
 */
lh_bool_t
lh_web_xml_reader_read(lh_web_xml_reader_t *self, lh_web_xml_node_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_READER_H */
