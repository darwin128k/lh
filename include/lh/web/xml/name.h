/**
 * @file name.h
 * @brief Which element, attribute and processing-instruction names XML accepts.
 *
 * A name is written into the markup as it is - it cannot be escaped - so the
 * writer (::lh_web_xml_writer_try_write) checks every name with
 * ::lh_web_xml_name_is_valid, and the reader
 * (::lh_web_xml_reader_read_name_into) reads a name as the longest run of
 * name characters.
 *
 * The ASCII part follows the `Name` production of XML 1.0 exactly: a letter,
 * `_` or `:` first, then letters, digits, `-`, `.`, `_` and `:`. Every byte
 * from 0x80 up is taken as a name character, so a UTF-8 name (`имя`) passes;
 * which non-ASCII code points XML lists is not checked. Nothing that could
 * end a tag or start another one (space, quotes, `=`, `<`, `>`, `/`, `?`,
 * `!`) is ever a name character.
 */

#ifndef LH_WEB_XML_NAME_H
#define LH_WEB_XML_NAME_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/** True when @p ch may start a name: an ASCII letter, `_`, `:`, or a byte from 0x80 up. */
lh_bool_t
lh_web_xml_name_is_start_char(lh_char_t ch);

/**
 * @brief True when @p ch may stand inside a name: a start character, an ASCII
 *        digit, `-` or `.`.
 */
lh_bool_t
lh_web_xml_name_is_char(lh_char_t ch);

/**
 * @brief True when @p name may stand as a name: not empty, a start character
 *        first (::lh_web_xml_name_is_start_char), then only name characters
 *        (::lh_web_xml_name_is_char).
 *
 * @param name The name to check; read only.
 */
lh_bool_t
lh_web_xml_name_is_valid(lh_str_view_t name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_NAME_H */
