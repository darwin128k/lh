/**
 * @file attr.c
 * @brief Implementation of `lh/ui/web/html/attr.h`.
 */

#include <lh/ui/web/html/attr.h>
#include <lh/assert/runtime.h>

lh_void
lh_ui_web_html_attr(const lh_str_ptr name, const lh_str_ptr value, lh_ui_web_html_attr_t *out)
{
    lh_assert_runtime_ref(out);
    lh_ui_web_html_attr_t a;

    a.name = name;
    a.value = value;
    *out = a;
    return;
}
