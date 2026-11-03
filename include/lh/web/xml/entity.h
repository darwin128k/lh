/**
 * @file entity.h
 * @brief The five entities XML predefines: `lt`, `gt`, `amp`, `quot`, `apos`.
 *
 * A reference `&name;` in text or in an attribute value stands for a
 * character; XML defines five names without a DTD, and this reader knows no
 * others (a DTD's own entities are not expanded). The reader
 * (::lh_web_xml_reader_read_reference) looks the name up here; the writer
 * produces the same names through ::lh_web_xml_escape_get_entity.
 */

#ifndef LH_WEB_XML_ENTITY_H
#define LH_WEB_XML_ENTITY_H

#include <lh/str/view.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The character the predefined entity @p name stands for (`lt` is
 *        `<`, ...); NUL when @p name is not one of the five. Case matters.
 *
 * @param name The name between `&` and `;`; read only.
 */
lh_char_t
lh_web_xml_entity_get_char(lh_str_view_t name);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_ENTITY_H */
