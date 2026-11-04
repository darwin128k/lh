/**
 * @file escape.h
 * @brief XML escaping: the reference that stands for a character, and text
 *        appended with those references in place.
 *
 * Two contexts need different sets. Character data escapes `&`, `<` and `>`,
 * so it can neither open a tag, start a reference nor close a CDATA section,
 * and CR as `&#13;`, which a reader would otherwise turn into LF. An
 * attribute value - always written in double quotes - escapes `&`, `<` and
 * `"`, and tab, LF and CR as `&#9;`, `&#10;` and `&#13;`, which a reader
 * would otherwise turn into spaces (attribute-value normalization). Every
 * other byte, UTF-8 included, goes as it is.
 *
 * The same sets are right for HTML, which reads every one of these references
 * back as the character, so the writer (::lh_web_xml_writer_try_write)
 * escapes with these functions in both dialects: there is one escaper.
 *
 * Example usage:
 * @code{.c}
 * lh_str_t out;
 *
 * lh_str_init(&out);
 * lh_web_xml_escape_append(&out, lh_str_view_lit("a<b & \"c\""), lh_bool_true);
 * // out: a&lt;b &amp; &quot;c&quot;
 * lh_str_deinit(&out);
 * @endcode
 */

#ifndef LH_WEB_XML_ESCAPE_H
#define LH_WEB_XML_ESCAPE_H

#include <lh/core/str.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The reference that stands for @p ch in that context.
 *
 * @param ch      The character to look up.
 * @param in_attr True for an attribute value, false for character data.
 * @return The reference, a view of static text (`&amp;`, `&lt;`, `&gt;`,
 *         `&quot;`, `&#9;`, `&#10;`, `&#13;`); the empty view when @p ch goes
 *         as it is.
 */
lh_str_view_t
lh_web_xml_escape_get_entity(lh_char_t ch, lh_bool_t in_attr);

/**
 * @brief True when @p ch has to be written as a reference in that context
 *        (::lh_web_xml_escape_get_entity is not empty).
 */
lh_bool_t
lh_web_xml_escape_is_needed(lh_char_t ch, lh_bool_t in_attr);

/**
 * @brief Append @p text to @p out with every character that needs it replaced
 *        by its reference. Runs between two references are appended at once.
 *
 * @param out     String appended to; never cleared.
 * @param text    Text to append; read only, may be empty.
 * @param in_attr True for an attribute value, false for character data.
 */
lh_void
lh_web_xml_escape_append(lh_str_t *out, lh_str_view_t text, lh_bool_t in_attr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_XML_ESCAPE_H */
