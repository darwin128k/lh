/**
 * @file attr.h
 * @brief One HTML attribute for ::lh_ui_web_html tag nodes.
 *
 * Requires ::LH_LIBRARY_OPTION_UI.
 */

#ifndef LH_UI_WEB_HTML_ATTR_H
#define LH_UI_WEB_HTML_ATTR_H

#include <lh/str/ptr.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/html/attr.h requires LH_LIBRARY_OPTION_UI"
#endif

/**
 * @struct lh_ui_web_html_attr
 * @brief `name="value"`, or bare `name` when `value` is ::lh_null.
 */
typedef struct lh_ui_web_html_attr
{
    const lh_str_ptr name;  /**< Attribute name. Not owned. */
    const lh_str_ptr value; /**< Attribute value, or ::lh_null for a bare attribute. Not owned. */
} lh_ui_web_html_attr_t;    /**< An HTML attribute: name and optional value. */

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief An attribute of @p name and @p value. Neither string is copied.
 *
 * @param name  Attribute name.
 * @param value Attribute value, or ::lh_null for a bare attribute.
 * @param out   Receives the attribute.
 */
lh_void
lh_ui_web_html_attr(const lh_str_ptr name, const lh_str_ptr value, lh_ui_web_html_attr_t *out);

/**
 * @brief The name of the attribute (level 1).
 *
 * @param self The attribute; checked by ::lh_assert_runtime_ref.
 */
const lh_str_ptr
lh_ui_web_html_attr_get_name(const lh_ui_web_html_attr_t *self);

/**
 * @brief Store the name of the attribute (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The attribute; checked by ::lh_assert_runtime_ref.
 * @param name The new value.
 */
lh_void
lh_ui_web_html_attr_set_name(lh_ui_web_html_attr_t *self, const lh_str_ptr name);

/**
 * @brief The value of the attribute (level 1).
 *
 * @param self The attribute; checked by ::lh_assert_runtime_ref.
 */
const lh_str_ptr
lh_ui_web_html_attr_get_value(const lh_ui_web_html_attr_t *self);

/**
 * @brief Store the value of the attribute (level 1).
 *
 * A level-1 setter: the value is stored as given and is not validated.
 *
 * @param self The attribute; checked by ::lh_assert_runtime_ref.
 * @param value The new value.
 */
lh_void
lh_ui_web_html_attr_set_value(lh_ui_web_html_attr_t *self, const lh_str_ptr value);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HTML_ATTR_H */
