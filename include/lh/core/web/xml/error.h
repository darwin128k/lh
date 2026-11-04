/**
 * @file error.h
 * @brief Why an XML (or HTML) text could not be read (::lh_web_xml_error_t).
 *
 * The XML reader (::lh_web_xml_reader_read) stops at the first error and
 * keeps it; the HTML reader, built on it, reports through the same codes.
 */

#ifndef LH_WEB_XML_ERROR_H
#define LH_WEB_XML_ERROR_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/str/ptr.h>

/**
 * @typedef lh_web_xml_error_t
 * @brief Syntax error found by a reader. Alias for ::lh_u8_t; values are the
 *        `lh_web_xml_error_*` constants below.
 */
typedef lh_u8_t lh_web_xml_error_t;

/**
 * @def lh_web_xml_error_none
 * @brief No error.
 */
#define lh_web_xml_error_none 0U

/**
 * @def lh_web_xml_error_unexpected_end
 * @brief The text ends inside a tag, a reference, a comment or an element.
 */
#define lh_web_xml_error_unexpected_end 1U

/**
 * @def lh_web_xml_error_unexpected_char
 * @brief A character that cannot stand there: a missing `=`, `>` or quote,
 *        `<` inside an attribute value, an attribute not set off by
 *        whitespace.
 */
#define lh_web_xml_error_unexpected_char 2U

/**
 * @def lh_web_xml_error_bad_name
 * @brief An element, attribute or target name that does not start with a
 *        name character (::lh_web_xml_name_is_start_char).
 */
#define lh_web_xml_error_bad_name 3U

/**
 * @def lh_web_xml_error_bad_reference
 * @brief A `&` that does not start a reference this reader knows: an entity
 *        other than the five predefined, a character reference without
 *        digits, without `;`, or to a character XML does not allow.
 */
#define lh_web_xml_error_bad_reference 4U

/**
 * @def lh_web_xml_error_bad_comment
 * @brief A comment with `--` inside or a `-` before its `-->`.
 */
#define lh_web_xml_error_bad_comment 5U

/**
 * @def lh_web_xml_error_mismatched_tag
 * @brief An end tag whose name is not the name of the open element.
 */
#define lh_web_xml_error_mismatched_tag 6U

/**
 * @def lh_web_xml_error_duplicate_attr
 * @brief The same attribute name twice in one start tag.
 */
#define lh_web_xml_error_duplicate_attr 7U

/**
 * @def lh_web_xml_error_too_deep
 * @brief Elements nested deeper than ::LH_WEB_XML_READER_DEPTH_MAX.
 */
#define lh_web_xml_error_too_deep 8U

/**
 * @def lh_web_xml_error_no_root
 * @brief A document without a root element.
 */
#define lh_web_xml_error_no_root 9U

/**
 * @def lh_web_xml_error_trailing
 * @brief Something other than comments, processing instructions and
 *        whitespace outside the root element: text, a second root, a
 *        doctype after the root.
 */
#define lh_web_xml_error_trailing 10U

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief A short English description of @p error, for a log line.
 *
 * @return A NUL-terminated static string; `"unknown error"` for a value
 *         that is not one of the constants above.
 */
const lh_str_ptr
lh_web_xml_error_get_message(lh_web_xml_error_t error);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_ERROR_H */
