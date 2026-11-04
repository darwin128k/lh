/**
 * @file fields.h
 * @brief Member fields of ::lh_web_js_writer_t.
 */

#ifndef LH_WEB_JS_WRITER_FIELDS_H
#define LH_WEB_JS_WRITER_FIELDS_H

/**
 * @def lh_web_js_writer_fields(out_type)
 * @brief Where the script goes.
 *
 * `out` is the string the script is appended to; the writer does not own it.
 *
 * @param out_type Type of `out` (::lh_str_t pointer).
 */
#define lh_web_js_writer_fields(out_type) out_type out

#endif /* LH_WEB_JS_WRITER_FIELDS_H */
