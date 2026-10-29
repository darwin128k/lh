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

    lh_ui_web_html_attr_set_name(lh_addr_of(a), name);
    lh_ui_web_html_attr_set_value(lh_addr_of(a), value);
    lh_ptr_deref(out) = a;
}

const lh_str_ptr
lh_ui_web_html_attr_get_name(const lh_ui_web_html_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->name;
}

lh_void
lh_ui_web_html_attr_set_name(lh_ui_web_html_attr_t *self, const lh_str_ptr name)
{
    lh_assert_runtime_ref(self);
    self->name = name;
}

const lh_str_ptr
lh_ui_web_html_attr_get_value(const lh_ui_web_html_attr_t *self)
{
    lh_assert_runtime_ref(self);
    return self->value;
}

lh_void
lh_ui_web_html_attr_set_value(lh_ui_web_html_attr_t *self, const lh_str_ptr value)
{
    lh_assert_runtime_ref(self);
    self->value = value;
}
