/**
 * @file attr.c
 * @brief Implementation of `lh/ui/web/html/attr.h`.
 */

#include <lh/ui/web/html/attr.h>

lh_ui_web_html_attr_t
lh_ui_web_html_attr(const lh_char_t *name, const lh_char_t *value)
{
    lh_ui_web_html_attr_t a;

    a.name = name;
    a.value = value;
    return a;
}
