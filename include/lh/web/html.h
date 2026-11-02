/**
 * @file html.h
 * @brief HTML markup: the tree of nodes and attributes, the escaping, and the
 *        writer that prints the tree.
 *
 * A page is built as a tree of ::lh_web_html_node_t - the doctype, elements
 * with their ::lh_web_html_attr_t, text and raw markup - that owns everything
 * in it, and turned into text once, by ::lh_web_html_writer_try_write. No
 * markup is written by hand.
 *
 * @see lh/web/html/node.h
 * @see lh/web/html/attr.h
 * @see lh/web/html/escape.h
 * @see lh/web/html/name.h
 * @see lh/web/html/writer.h
 */

#ifndef LH_WEB_HTML_H
#define LH_WEB_HTML_H

#include <lh/web/html/escape.h>
#include <lh/web/html/name.h>
#include <lh/web/html/writer.h>

#endif /* LH_WEB_HTML_H */
