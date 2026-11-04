/**
 * @file char.h
 * @brief Which code points XML allows in a document (the `Char` production).
 *
 * XML 1.0 allows tab, LF, CR, and every code point from U+0020 up, except the
 * surrogates and U+FFFE / U+FFFF. A character reference (`&#...;`) must name
 * one of them: the reader (::lh_web_xml_reader_read_reference) refuses
 * `&#0;`, `&#xD800;` or `&#x110000;` with this test.
 */

#ifndef LH_WEB_XML_CHAR_H
#define LH_WEB_XML_CHAR_H

#include <lh/core/bool.h>
#include <lh/core/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief True when @p code is a character XML allows: `#x9 | #xA | #xD |
 *        [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]`.
 */
lh_bool_t
lh_web_xml_char_is_valid(lh_u32_t code);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_CHAR_H */
