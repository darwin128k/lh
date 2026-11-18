/**
 * @file fields.h
 * @brief Member fields of ::lh_os_web_host_stream_t.
 */

#ifndef LH_OS_WEB_HOST_STREAM_FIELDS_H
#define LH_OS_WEB_HOST_STREAM_FIELDS_H

/**
 * @def lh_os_web_host_stream_fields(text_type, size_type)
 * @brief What the stream of WS frames shares between iterations:
 *
 * - `last`, `last_len`, `last_cap` — the bytes of the last frame sent (owned), how
 *   many, and the capacity.
 */
#define lh_os_web_host_stream_fields(text_type, size_type)                                         \
    text_type last;                                                                                \
    size_type last_len;                                                                            \
    size_type last_cap

#endif /* LH_OS_WEB_HOST_STREAM_FIELDS_H */
