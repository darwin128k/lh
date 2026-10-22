/**
 * @file fields.h
 * @brief Member fields of ::lh_json_writer_t.
 */

#ifndef LH_JSON_WRITER_FIELDS_H
#define LH_JSON_WRITER_FIELDS_H

/**
 * @def lh_json_writer_fields(out_type, size_type)
 * @brief Where the text goes and how it is laid out.
 *
 * `out` is the string the text is appended to, `indent` how many spaces one
 * level of nesting takes (`0`: everything on one line, no spaces), `depth`
 * how many arrays and objects are open.
 *
 * @param out_type  Type of `out` (::lh_str_t pointer).
 * @param size_type Type of `indent` and `depth` (::lh_usize_t).
 */
#define lh_json_writer_fields(out_type, size_type)                                                 \
    out_type out;                                                                                  \
    size_type indent;                                                                              \
    size_type depth

#endif /* LH_JSON_WRITER_FIELDS_H */
