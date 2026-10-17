/**
 * @file html.h
 * @brief HTML markup entities: construct a tree, then serialize to a buf.
 *
 * Attributes live in ::lh/ui/web/html/attr.h. Serialize with
 * ::lh_ui_web_html_emit / ::lh_ui_web_html_open / ::lh_ui_web_html_close into a
 * ::lh_ui_web_buf_t — not by writing strings by hand.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_HTML_H
#define LH_UI_WEB_HTML_H

#include <lh/config.h>
#include <lh/ui/web/buf.h>
#include <lh/ui/web/html/attr.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/html.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

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

/**
 * @brief One markup node.
 *
 * `tag.void_el` is for HTML void elements (`meta`, …) — no children, no close.
 */
typedef struct lh_ui_web_html
{
    lh_ui_web_html_kind_t kind; /**< Which member of @c u is set. */
    /** The content of the node, by @c kind. */
    union
    {
        const lh_str_ptr text;
        const lh_str_ptr raw;
        struct
        {
            const lh_str_ptr name;
            const lh_ui_web_html_attr_t *attrs;
            lh_u16_t nattrs;
            const struct lh_ui_web_html *const *kids;
            lh_u16_t nkids;
            lh_bool_t void_el;
        } tag;
    } u;
} lh_ui_web_html_t;

/** `<!doctype html>` */
lh_void
lh_ui_web_html_doctype(lh_ui_web_html_t *out);

/** Text node — `&`, `<`, `>` escaped on emit. */
lh_void
lh_ui_web_html_text(const lh_str_ptr s, lh_ui_web_html_t *out);

/** Raw bytes inside a parent (e.g. CSS) — not escaped. */
lh_void
lh_ui_web_html_raw(const lh_str_ptr s, lh_ui_web_html_t *out);

/**
 * @brief Element node.
 *
 * @param name    Tag name.
 * @param attrs   Attributes, in order; may be ::lh_null when @p nattrs is `0`.
 * @param nattrs  How many attributes @p attrs holds.
 * @param kids    Child nodes, in order; may be ::lh_null when @p nkids is `0`.
 * @param nkids   How many children @p kids holds.
 * @param void_el When true, emit `<name …>` with no children and no close tag.
 */
lh_void
lh_ui_web_html_tag(const lh_str_ptr name, const lh_ui_web_html_attr_t *attrs, lh_u16_t nattrs,
                   const lh_ui_web_html_t *const *kids, lh_u16_t nkids, lh_bool_t void_el,
                   lh_ui_web_html_t *out);

/** Serialize a full node (doctype / text / raw / element with kids). */
lh_void
lh_ui_web_html_emit(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

/** Emit only `<tag attrs>` — for elements whose body is filled elsewhere (script). */
lh_void
lh_ui_web_html_open(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

/** Emit `</tag>`. */
lh_void
lh_ui_web_html_close(lh_ui_web_buf_t *out, const lh_ui_web_html_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HTML_H */
