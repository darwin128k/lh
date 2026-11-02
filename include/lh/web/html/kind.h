/**
 * @file kind.h
 * @brief What an ::lh_web_html_node_t is: a doctype, an element, a void
 *        element, text or raw markup.
 *
 * The kind decides which parts of a node the writer reads. An element has a
 * tag name, attributes and children, and is written with its close tag. A
 * void element (`meta`, `br`, ...) has a tag name and attributes but no
 * children and no close tag. Text is escaped on the way out; raw markup is
 * written as it is, which is what the bodies of `style` and `script` need.
 * The kind is chosen by the node's setters (::lh_web_html_node_set_element,
 * ...), never written by hand.
 */

#ifndef LH_WEB_HTML_KIND_H
#define LH_WEB_HTML_KIND_H

#include <lh/numeric/fixed/types.h>

/**
 * @typedef lh_web_html_kind_t
 * @brief Discriminator of ::lh_web_html_node_t. Alias for ::lh_u8_t; values are
 *        the `lh_web_html_kind_*` constants below.
 */
typedef lh_u8_t lh_web_html_kind_t;

/**
 * @def lh_web_html_kind_none
 * @brief Nothing yet. What ::lh_web_html_node_init leaves; the writer refuses it.
 */
#define lh_web_html_kind_none 0U

/**
 * @def lh_web_html_kind_doctype
 * @brief `<!doctype html>`.
 */
#define lh_web_html_kind_doctype 1U

/**
 * @def lh_web_html_kind_element
 * @brief `<name attrs>children</name>`.
 */
#define lh_web_html_kind_element 2U

/**
 * @def lh_web_html_kind_void
 * @brief `<name attrs>`: a void element, without children and close tag.
 */
#define lh_web_html_kind_void 3U

/**
 * @def lh_web_html_kind_text
 * @brief Text, escaped by the writer (`&`, `<`, `>`).
 */
#define lh_web_html_kind_text 4U

/**
 * @def lh_web_html_kind_raw
 * @brief Markup written as it is: the body of `style` or `script`.
 */
#define lh_web_html_kind_raw 5U

#endif /* LH_WEB_HTML_KIND_H */
