/**
 * @file void.h
 * @brief Which HTML elements are void: they never have children or an end
 *        tag (`<br>`, `<meta charset="utf-8">`).
 *
 * A void element is kept in the tree as ::lh_web_xml_kind_empty, and the
 * writer in the HTML dialect writes it as a start tag alone. The reader
 * (::lh_web_html_reader_read_element) needs the list, since `<br>` gives no
 * sign in the markup that nothing follows: the element names below, ASCII
 * case folded - `area`, `base`, `br`, `col`, `embed`, `hr`, `img`, `input`,
 * `link`, `meta`, `source`, `track`, `wbr` (the HTML Living Standard's list).
 */

#ifndef LH_WEB_HTML_VOID_H
#define LH_WEB_HTML_VOID_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when an element named @p name is void in HTML, whatever the
 *        ASCII case of the name.
 *
 * @param name The tag name; read only.
 */
lh_bool_t
lh_web_html_void_is_name(lh_str_view_t name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_VOID_H */
