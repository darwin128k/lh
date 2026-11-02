/**
 * @file name.h
 * @brief Which tag and attribute names the HTML writer accepts.
 *
 * A name is written into the markup as it is - it cannot be escaped - so the
 * writer (::lh_web_html_writer_try_write) checks every tag name and every
 * attribute name with ::lh_web_html_name_is_valid and refuses the tree when
 * one fails. The accepted set is deliberately small: an ASCII letter, then
 * ASCII letters, digits and `-`. It covers every HTML element, custom
 * elements (`my-widget`), and the attributes HTML defines, `data-*` and
 * `aria-*` included; it rejects anything that could end the tag early or
 * start another one (spaces, quotes, `=`, `<`, `>`, `/`).
 */

#ifndef LH_WEB_HTML_NAME_H
#define LH_WEB_HTML_NAME_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p name may stand as a tag or attribute name: not empty,
 *        an ASCII letter first, then only ASCII letters, digits and `-`.
 *
 * @param name The name to check; read only.
 * @return True when the writer can write @p name as it is.
 */
lh_bool_t
lh_web_html_name_is_valid(lh_str_view_t name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_NAME_H */
