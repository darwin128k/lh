/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_tap_t.
 */

#ifndef LH_UI_WEB_HOST_TAP_FIELDS_H
#define LH_UI_WEB_HOST_TAP_FIELDS_H

/**
 * @def lh_ui_web_host_tap_fields(reader_type, result_type)
 * @brief The reader passed through and what it said last.
 *
 * @param reader_type ::lh_io_reader_t: `source`, the reader every read goes to.
 * @param result_type ::lh_ssize_t: `last`, the result of the latest read.
 */
#define lh_ui_web_host_tap_fields(reader_type, result_type)                                        \
    reader_type source;                                                                            \
    result_type last

#endif /* LH_UI_WEB_HOST_TAP_FIELDS_H */
