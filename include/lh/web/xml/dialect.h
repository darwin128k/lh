/**
 * @file dialect.h
 * @brief Which markup language a tree is written in (::lh_web_xml_dialect_t):
 *        XML or HTML.
 *
 * HTML is XML with different tagging rules, so one tree
 * (::lh_web_xml_node_t) and one writer (::lh_web_xml_writer_t) serve both,
 * and the dialect is the writer's flag. It changes only what the two
 * languages spell differently:
 *
 * - an empty element is `<br/>` in XML and the void element `<br>` in HTML;
 * - a bare attribute (`<input disabled>`) exists only in HTML;
 * - the doctype is `<!DOCTYPE ...>` in XML and `<!doctype ...>` in HTML;
 * - CDATA sections and processing instructions exist only in XML.
 *
 * Names, escaping, text, comments and raw markup are the same in both. The
 * HTML reader (`lh/web/html/reader.h`) is the other half: it reads HTML's
 * rules on top of the XML reader's steps.
 */

#ifndef LH_WEB_XML_DIALECT_H
#define LH_WEB_XML_DIALECT_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_web_xml_dialect_t
 * @brief The language a writer writes. Alias for ::lh_u8_t; values are the
 *        `lh_web_xml_dialect_*` constants below.
 */
typedef lh_u8_t lh_web_xml_dialect_t;

/**
 * @def lh_web_xml_dialect_xml
 * @brief XML 1.0: empty elements as `<name/>`, no bare attributes.
 */
#define lh_web_xml_dialect_xml 0U

/**
 * @def lh_web_xml_dialect_html
 * @brief HTML: void elements as `<name>`, bare attributes, `<!doctype ...>`,
 *        no CDATA sections or processing instructions.
 */
#define lh_web_xml_dialect_html 1U

#endif /* LH_WEB_XML_DIALECT_H */
