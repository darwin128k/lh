/**
 * @file rawtext.h
 * @brief Which HTML elements hold raw text: `script` and `style`.
 *
 * The body of a raw-text element is not markup: `<`, `&` and everything else
 * stand for themselves up to the end tag (`</script>`), which is how a script
 * can say `a < b` unescaped. The tree keeps such a body as one
 * ::lh_web_xml_kind_raw child, the writer writes it as it is, and the reader
 * (::lh_web_html_reader_read_rawtext) reads it up to the end tag. The escapable
 * raw-text elements (`title`, `textarea`) are read as ordinary elements.
 */

#ifndef LH_WEB_HTML_RAWTEXT_H
#define LH_WEB_HTML_RAWTEXT_H

#include <lh/core/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when an element named @p name holds raw text in HTML
 *        (`script`, `style`), whatever the ASCII case of the name.
 *
 * @param name The tag name; read only.
 */
lh_bool_t
lh_web_html_rawtext_is_name(lh_str_view_t name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_RAWTEXT_H */
