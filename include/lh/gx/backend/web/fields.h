/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_backend_web_t.
 */

#ifndef LH_GX_BACKEND_WEB_FIELDS_H
#define LH_GX_BACKEND_WEB_FIELDS_H

/**
 * @def lh_gx_backend_web_fields(writer_type, size_type)
 * @brief Where the commands go, and how many the current frame has.
 *
 * `writer` is the JSON writer the commands are appended through (the caller's;
 * it owns the text), `count` how many commands the frame opened by
 * ::lh_gx_backend_web_begin holds so far — the first one goes in without a
 * comma before it.
 *
 * @param writer_type Type of the writer (::lh_web_json_writer_t).
 * @param size_type   Type of the count (::lh_usize_t).
 */
#define lh_gx_backend_web_fields(writer_type, size_type)                                           \
    writer_type *writer;                                                                           \
    size_type count

#endif /* LH_GX_BACKEND_WEB_FIELDS_H */
