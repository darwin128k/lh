/**
 * @file kind.h
 * @brief What an ::lh_web_xml_node_t is: a document, an element, an empty
 *        element, text, raw markup, a comment, a CDATA section, a processing
 *        instruction or a document type declaration.
 *
 * The kind decides which parts of a node a writer reads. A document has only
 * children: the top-level nodes, written one after another. An element has a
 * name, attributes and children and is written with its end tag. An empty
 * element has a name and attributes but can have no children: XML writes it
 * as `<name/>`, HTML - where it is a void element (`meta`, `br`) - as
 * `<name>`. Text is escaped on the way out; raw markup is written as it is.
 * A comment, a CDATA section, a processing instruction (`<?target data?>`)
 * and a document type declaration keep their text as it is meant and are
 * written in their own brackets. The kind is chosen by the node's setters
 * (::lh_web_xml_node_set_element, ...), never written by hand.
 *
 * The same kinds serve HTML (`lh/core/web/html/`), which is written and read on
 * top of this tree.
 */

#ifndef LH_WEB_XML_KIND_H
#define LH_WEB_XML_KIND_H

#include <lh/core/numeric/fixed/types.h>

/**
 * @typedef lh_web_xml_kind_t
 * @brief Discriminator of ::lh_web_xml_node_t. Alias for ::lh_u8_t; values are
 *        the `lh_web_xml_kind_*` constants below.
 */
typedef lh_u8_t lh_web_xml_kind_t;

/**
 * @def lh_web_xml_kind_none
 * @brief Nothing yet. What ::lh_web_xml_node_init leaves; a writer refuses it.
 */
#define lh_web_xml_kind_none 0U

/**
 * @def lh_web_xml_kind_document
 * @brief The whole document: its children are the top-level nodes (prolog,
 *        root element, trailing comments), written in order.
 */
#define lh_web_xml_kind_document 1U

/**
 * @def lh_web_xml_kind_element
 * @brief `<name attrs>children</name>`.
 */
#define lh_web_xml_kind_element 2U

/**
 * @def lh_web_xml_kind_empty
 * @brief `<name attrs/>` in XML, `<name attrs>` in HTML (a void element):
 *        an element that has no children and no end tag.
 */
#define lh_web_xml_kind_empty 3U

/**
 * @def lh_web_xml_kind_text
 * @brief Character data, kept unescaped and escaped by the writer.
 */
#define lh_web_xml_kind_text 4U

/**
 * @def lh_web_xml_kind_raw
 * @brief Markup written as it is: a ready fragment, or the body of an HTML
 *        `style` or `script`.
 */
#define lh_web_xml_kind_raw 5U

/**
 * @def lh_web_xml_kind_comment
 * @brief `<!--text-->`.
 */
#define lh_web_xml_kind_comment 6U

/**
 * @def lh_web_xml_kind_cdata
 * @brief `<![CDATA[text]]>`: character data written without escaping.
 */
#define lh_web_xml_kind_cdata 7U

/**
 * @def lh_web_xml_kind_pi
 * @brief `<?name text?>`: a processing instruction, the target in the name,
 *        the rest in the text. The XML declaration `<?xml version="1.0"?>`
 *        is kept as one.
 */
#define lh_web_xml_kind_pi 8U

/**
 * @def lh_web_xml_kind_doctype
 * @brief `<!DOCTYPE text>`: the document type declaration, its text kept as
 *        written (`html`, `note SYSTEM "note.dtd"`, an internal subset).
 */
#define lh_web_xml_kind_doctype 9U

#endif /* LH_WEB_XML_KIND_H */
