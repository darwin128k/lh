/**
 * @file fields.h
 * @brief Member fields of ::lh_web_html_reader_t.
 */

#ifndef LH_WEB_HTML_READER_FIELDS_H
#define LH_WEB_HTML_READER_FIELDS_H

/**
 * @def lh_web_html_reader_fields(xml_type)
 * @brief The XML reader the HTML reader reads with.
 *
 * `xml` holds the text, the position, the depth and the error; every step
 * HTML shares with XML is that reader's.
 *
 * @param xml_type Type of `xml` (::lh_web_xml_reader_t).
 */
#define lh_web_html_reader_fields(xml_type) xml_type xml

#endif /* LH_WEB_HTML_READER_FIELDS_H */
