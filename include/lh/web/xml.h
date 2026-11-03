/**
 * @file xml.h
 * @brief XML markup: the tree of nodes and attributes, the escaping, the
 *        names, the writer that prints the tree and the reader that builds it.
 *
 * A document is a tree of ::lh_web_xml_node_t - elements with their
 * ::lh_web_xml_attr_t, text, comments, CDATA sections, processing
 * instructions, the doctype - that owns everything in it. It is turned into
 * text by ::lh_web_xml_writer_try_write and built from text by
 * ::lh_web_xml_reader_read. HTML (`lh/web/html.h`) is written and read on
 * top of the same tree.
 *
 * @see lh/web/xml/node.h
 * @see lh/web/xml/attr.h
 * @see lh/web/xml/escape.h
 * @see lh/web/xml/name.h
 * @see lh/web/xml/writer.h
 * @see lh/web/xml/reader.h
 */

#ifndef LH_WEB_XML_H
#define LH_WEB_XML_H

#include <lh/web/xml/escape.h>
#include <lh/web/xml/reader.h>
#include <lh/web/xml/writer.h>

#endif /* LH_WEB_XML_H */
