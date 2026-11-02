/**
 * @file escape.h
 * @brief HTML escaping: the entity that stands for a character, and text
 *        appended with those entities in place.
 *
 * Two contexts need different sets. Element text escapes `&`, `<` and `>`, so
 * it can neither open a tag nor start a character reference. An attribute
 * value - always written in double quotes - escapes `&`, `<` and `"`, so it
 * cannot end its quotes early. Every other byte, UTF-8 included, goes as it is.
 *
 * The writer (::lh_web_html_writer_try_write) escapes every text node and
 * every attribute value with these functions; they are public so any other
 * producer of markup does the same, not its own copy.
 *
 * Example usage:
 * @code{.c}
 * lh_str_t out;
 *
 * lh_str_init(&out);
 * lh_web_html_escape_append(&out, lh_str_view_lit("a<b & \"c\""), lh_bool_true);
 * // out: a&lt;b &amp; &quot;c&quot;
 * lh_str_deinit(&out);
 * @endcode
 */

#ifndef LH_WEB_HTML_ESCAPE_H
#define LH_WEB_HTML_ESCAPE_H

#include <lh/str.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The entity that stands for @p ch: `&amp;`, `&lt;`, `&gt;` in text,
 *        `&amp;`, `&lt;`, `&quot;` in an attribute value (@p in_attr).
 *
 * @param ch      The character to look up.
 * @param in_attr True for an attribute value, false for element text.
 * @return The entity, a view of static text; the empty view when @p ch goes
 *         as it is.
 */
lh_str_view_t
lh_web_html_escape_get_entity(lh_char_t ch, lh_bool_t in_attr);

/**
 * @brief True when @p ch has to be written as an entity in that context
 *        (::lh_web_html_escape_get_entity is not empty).
 */
lh_bool_t
lh_web_html_escape_is_needed(lh_char_t ch, lh_bool_t in_attr);

/**
 * @brief Append @p text to @p out with every character that needs it replaced
 *        by its entity. Runs between two escapes are appended at once.
 *
 * @param out     String appended to; never cleared.
 * @param text    Text to append; read only, may be empty.
 * @param in_attr True for an attribute value, false for element text.
 */
lh_void
lh_web_html_escape_append(lh_str_t *out, lh_str_view_t text, lh_bool_t in_attr);

LH_COMPILER_EXTERN_C_END

#endif /* LH_WEB_HTML_ESCAPE_H */
