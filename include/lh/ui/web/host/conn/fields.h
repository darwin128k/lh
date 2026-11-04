/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_conn_t.
 */

#ifndef LH_UI_WEB_HOST_CONN_FIELDS_H
#define LH_UI_WEB_HOST_CONN_FIELDS_H

#include <lh/ui/web/host/capacity.h>

/**
 * @def lh_ui_web_host_conn_fields()
 * @brief A connection's socket, phase and start; its request buffer and parse;
 *        what the head said about the body; and the pipe of a streaming body.
 */
#define lh_ui_web_host_conn_fields()                                                               \
    lh_os_net_socket_t sock;                                                                       \
    lh_ui_web_host_conn_phase_t phase;                                                             \
    lh_u64_t start;                                                                                \
    lh_http_parser_t parser;                                                                       \
    lh_char_t buf[LH_UI_WEB_HOST_REQUEST_CAPACITY];                                                \
    lh_usize_t received;                                                                           \
    lh_usize_t head_size;                                                                          \
    lh_http_request_t request;                                                                     \
    lh_http_header_t headers[LH_UI_WEB_HOST_HEADER_MAX];                                           \
    lh_bool_t websocket;                                                                           \
    lh_bool_t content_gzip;                                                                        \
    lh_bool_t chunked;                                                                             \
    lh_bool_t transfer_gzip;                                                                       \
    lh_usize_t body_size;                                                                          \
    lh_ui_web_host_pipe_t pipe

#endif /* LH_UI_WEB_HOST_CONN_FIELDS_H */
