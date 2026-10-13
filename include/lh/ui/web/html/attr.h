/**
 * @file attr.h
 * @brief One HTML attribute for ::lh_ui_web_html tag nodes.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_HTML_ATTR_H
#define LH_UI_WEB_HTML_ATTR_H

#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/html/attr.h requires LH_LIBRARY_OPTION_UI"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/** `name="value"`, or bare `name` when @p value is ::lh_null. */
typedef struct lh_ui_web_html_attr
{
    const lh_char_t *name;
    const lh_char_t *value;
} lh_ui_web_html_attr_t;

lh_ui_web_html_attr_t
lh_ui_web_html_attr(const lh_char_t *name, const lh_char_t *value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HTML_ATTR_H */
