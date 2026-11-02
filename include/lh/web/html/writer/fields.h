/**
 * @file fields.h
 * @brief Member fields of ::lh_web_html_writer_t.
 */

#ifndef LH_WEB_HTML_WRITER_FIELDS_H
#define LH_WEB_HTML_WRITER_FIELDS_H

/**
 * @def lh_web_html_writer_fields(out_type)
 * @brief Where the markup goes.
 *
 * `out` is the string the markup is appended to; the writer does not own it.
 *
 * @param out_type Type of `out` (::lh_str_t pointer).
 */
#define lh_web_html_writer_fields(out_type) out_type out

#endif /* LH_WEB_HTML_WRITER_FIELDS_H */
