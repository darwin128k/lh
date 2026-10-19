/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_client_t.
 */

#ifndef LH_UI_WEB_HOST_CLIENT_FIELDS_H
#define LH_UI_WEB_HOST_CLIENT_FIELDS_H

/**
 * @def lh_ui_web_host_client_fields(socket_type, bytes_type, size_type, frame_type, tick_type,
 *                                   bool_type)
 * @brief One WebSocket client: its socket, what it sent, and what is queued for it:
 *
 * - `sock` — the connection, non-blocking;
 * - `rx`, `rx_len` — received bytes not yet parsed (::LH_UI_WEB_HOST_WS_RX) and how many;
 * - `msg`, `msg_len`, `msg_cap`, `msg_op`, `msg_on` — a fragmented message being
 *   assembled: its bytes, length, capacity, opcode, and whether one is in progress;
 * - `qhead`, `qtail`, `queued` — the frames to send, first and last, and their bytes
 *   against ::LH_UI_WEB_HOST_WS_QMAX;
 * - `seen` — tree generation of the last frame it got;
 * - `last_rx_ms`, `last_ping_ms` — when it last sent anything, when it was last pinged;
 * - `used` — this slot holds a client;
 * - `closing` — a close was exchanged; dropped once the queue drains;
 * - `synced` — got the last frame sent, so it holds every cover that frame assumed.
 */
#define lh_ui_web_host_client_fields(socket_type, bytes_type, size_type, frame_type, tick_type,    \
                                     bool_type)                                                    \
    socket_type sock;                                                                              \
    bytes_type rx;                                                                                 \
    size_type rx_len;                                                                              \
    bytes_type msg;                                                                                \
    size_type msg_len;                                                                             \
    size_type msg_cap;                                                                             \
    size_type msg_op;                                                                              \
    bool_type msg_on;                                                                              \
    frame_type qhead;                                                                              \
    frame_type qtail;                                                                              \
    size_type queued;                                                                              \
    size_type seen;                                                                                \
    tick_type last_rx_ms;                                                                          \
    tick_type last_ping_ms;                                                                        \
    bool_type used;                                                                                \
    bool_type closing;                                                                             \
    bool_type synced

#endif /* LH_UI_WEB_HOST_CLIENT_FIELDS_H */
