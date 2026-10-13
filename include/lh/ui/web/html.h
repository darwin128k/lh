/**
 * @file html.h
 * @brief HTML markup entities: construct a tree, then serialize to bytes.
 *
 * Same shape as ::lh_ui_web_x for expressions — the page is built as nodes on
 * the stack; ::lh_ui_web_html_emit / ::lh_ui_web_html_open / ::lh_ui_web_html_close
 * write text only at the end. Children and attribute arrays must outlive emit.
 *
 * Attributes live in ::lh/ui/web/html/attr.h.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_HTML_H
#define LH_UI_WEB_HTML_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/ui/web/html/attr.h>
#include <lh/ui/web/js.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/html.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

typedef enum lh_ui_web_html_kind
{
    lh_ui_web_html_kind_doctype = 0,
    lh_ui_web_html_kind_text,
    lh_ui_web_html_kind_raw,
    lh_ui_web_html_kind_tag
} lh_ui_web_html_kind_t;

/**
 * @brief One markup node.
 *
 * `tag.void_el` is for HTML void elements (`meta`, …) — no children, no close.
 */
typedef struct lh_ui_web_html
{
    lh_ui_web_html_kind_t kind;
    union
    {
        const lh_char_t *text;
        const lh_char_t *raw;
        struct
        {
            const lh_char_t *name;
            const lh_ui_web_html_attr_t *attrs;
            lh_u16_t nattrs;
            const struct lh_ui_web_html *const *kids;
            lh_u16_t nkids;
            lh_bool_t void_el;
        } tag;
    } u;
} lh_ui_web_html_t;

/** `<!doctype html>` */
lh_ui_web_html_t
lh_ui_web_html_doctype(void);

/** Text node — `&`, `<`, `>` escaped on emit. */
lh_ui_web_html_t
lh_ui_web_html_text(const lh_char_t *s);

/** Raw bytes inside a parent (e.g. CSS) — not escaped. */
lh_ui_web_html_t
lh_ui_web_html_raw(const lh_char_t *s);

/**
 * @brief Element node.
 *
 * @param void_el When true, emit `<name …>` with no children and no close tag.
 */
lh_ui_web_html_t
lh_ui_web_html_tag(const lh_char_t *name, const lh_ui_web_html_attr_t *attrs, lh_u16_t nattrs,
                   const lh_ui_web_html_t *const *kids, lh_u16_t nkids, lh_bool_t void_el);

/** Serialize a full node (doctype / text / raw / element with kids). */
lh_void
lh_ui_web_html_emit(lh_ui_web_js_t *out, const lh_ui_web_html_t *node);

/** Emit only `<tag attrs>` — for elements whose body is filled elsewhere (script). */
lh_void
lh_ui_web_html_open(lh_ui_web_js_t *out, const lh_ui_web_html_t *node);

/** Emit `</tag>`. */
lh_void
lh_ui_web_html_close(lh_ui_web_js_t *out, const lh_ui_web_html_t *node);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HTML_H */
