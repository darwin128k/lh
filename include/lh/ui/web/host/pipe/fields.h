/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_pipe_t.
 */

#ifndef LH_UI_WEB_HOST_PIPE_FIELDS_H
#define LH_UI_WEB_HOST_PIPE_FIELDS_H

#include <lh/ui/web/host/capacity.h>

/**
 * @def lh_ui_web_host_pipe_fields()
 * @brief The route, the time it started, and the reader chain of the body.
 *
 * `route` and `start`; `wire` (::LH_UI_WEB_HOST_REQUEST_CAPACITY bytes of wire
 * followed by ::LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY of chunk line); the chain
 * gate → `gate_io` → `body` (with `decoder` when chunked) → `body_io` →
 * (`gzip` → `gzip_io` when `gzip_on`) → `tap`, each link reading the one
 * before it by address, so the record must not move once begun; `draining`
 * once the handler stopped and the host validates the rest.
 */
#define lh_ui_web_host_pipe_fields()                                                               \
    const lh_ui_web_stream_route_t *route;                                                         \
    lh_u64_t start;                                                                                \
    lh_char_t wire[LH_UI_WEB_HOST_REQUEST_CAPACITY + LH_UI_WEB_HOST_CHUNK_LINE_CAPACITY];          \
    lh_http_chunked_decoder_t decoder;                                                             \
    lh_http_body_reader_t body;                                                                    \
    lh_io_reader_t body_io;                                                                        \
    lh_ui_web_host_gate_t gate;                                                                    \
    lh_io_reader_t gate_io;                                                                        \
    lh_ui_web_host_gzip_workspace_t gzip;                                                          \
    lh_io_reader_t gzip_io;                                                                        \
    lh_bool_t gzip_on;                                                                             \
    lh_ui_web_host_tap_t tap;                                                                      \
    lh_bool_t draining

#endif /* LH_UI_WEB_HOST_PIPE_FIELDS_H */
