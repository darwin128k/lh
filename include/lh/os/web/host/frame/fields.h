/**
 * @file fields.h
 * @brief Member fields of ::lh_os_web_host_frame_t.
 */

#ifndef LH_OS_WEB_HOST_FRAME_FIELDS_H
#define LH_OS_WEB_HOST_FRAME_FIELDS_H

/**
 * @def lh_os_web_host_frame_fields(next_type, bytes_type, size_type)
 * @brief One encoded WebSocket frame waiting to be sent:
 *
 * - `next` — the next frame in the queue;
 * - `bytes` — the whole frame, header included (owned);
 * - `len` — bytes in `bytes`;
 * - `off` — bytes of it already sent.
 */
#define lh_os_web_host_frame_fields(next_type, bytes_type, size_type)                              \
    next_type next;                                                                                \
    bytes_type bytes;                                                                              \
    size_type len;                                                                                 \
    size_type off

#endif /* LH_OS_WEB_HOST_FRAME_FIELDS_H */
