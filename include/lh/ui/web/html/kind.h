/**
 * @file kind.h
 * @brief ::lh_ui_web_html_kind_t — what an ::lh_ui_web_html_t node is, and so which member of its union is set.
 */

#ifndef LH_UI_WEB_HTML_KIND_H
#define LH_UI_WEB_HTML_KIND_H

/**
 * @enum lh_ui_web_html_kind
 * @brief What an ::lh_ui_web_html_t node is, and so which member of its union is set.
 */
typedef enum lh_ui_web_html_kind
{
    lh_ui_web_html_kind_doctype = 0, /**< `<!doctype html>`. */
    lh_ui_web_html_kind_text,        /**< Text, HTML-escaped on emit. */
    lh_ui_web_html_kind_raw,         /**< Markup written as is. */
    lh_ui_web_html_kind_tag          /**< An element with attributes and children. */
} lh_ui_web_html_kind_t;             /**< What an ::lh_ui_web_html_t node is. */

#endif /* LH_UI_WEB_HTML_KIND_H */
