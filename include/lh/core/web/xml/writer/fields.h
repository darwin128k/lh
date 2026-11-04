/**
 * @file fields.h
 * @brief Member fields of ::lh_web_xml_writer_t.
 */

#ifndef LH_WEB_XML_WRITER_FIELDS_H
#define LH_WEB_XML_WRITER_FIELDS_H

/**
 * @def lh_web_xml_writer_fields(out_type, dialect_type)
 * @brief Where the markup goes, and in which language.
 *
 * `out` is the string the markup is appended to; the writer does not own it.
 * `dialect` says whether the tree is written as XML or as HTML.
 *
 * @param out_type     Type of `out` (::lh_str_t pointer).
 * @param dialect_type Type of `dialect` (::lh_web_xml_dialect_t).
 */
#define lh_web_xml_writer_fields(out_type, dialect_type)                                           \
    out_type out;                                                                                  \
    dialect_type dialect

#endif /* LH_WEB_XML_WRITER_FIELDS_H */
