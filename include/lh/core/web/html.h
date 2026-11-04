/**
 * @file html.h
 * @brief HTML on top of XML: the void and raw-text elements, and the reader.
 *
 * A page is an XML tree (::lh_web_xml_node_t) - the doctype, elements with
 * their attributes, text, raw markup - that owns everything in it. It is
 * written by the XML writer in the HTML dialect (::lh_web_xml_writer_set_dialect
 * with ::lh_web_xml_dialect_html) and read by ::lh_web_html_reader_read, which
 * adds HTML's rules to the XML reader's steps. No markup is written by hand.
 *
 * @see lh/core/web/xml.h
 * @see lh/core/web/html/void.h
 * @see lh/core/web/html/rawtext.h
 * @see lh/core/web/html/reader.h
 */

#ifndef LH_WEB_HTML_H
#define LH_WEB_HTML_H

#include <lh/core/web/html/rawtext.h>
#include <lh/core/web/html/reader.h>
#include <lh/core/web/html/void.h>
#include <lh/core/web/xml/writer.h>

#endif /* LH_WEB_HTML_H */
