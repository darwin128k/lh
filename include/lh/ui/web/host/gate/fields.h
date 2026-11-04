/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_gate_t.
 */

#ifndef LH_UI_WEB_HOST_GATE_FIELDS_H
#define LH_UI_WEB_HOST_GATE_FIELDS_H

/**
 * @def lh_ui_web_host_gate_fields(socket_type, flag_type)
 * @brief The socket and the permission to receive from it once.
 *
 * @param socket_type ::lh_os_net_socket_t: `socket` points at it, borrowed.
 * @param flag_type   ::lh_bool_t: `ready`, set when a poll found the socket
 *                    readable, cleared by the receive it allows.
 */
#define lh_ui_web_host_gate_fields(socket_type, flag_type)                                         \
    socket_type *socket;                                                                           \
    flag_type ready

#endif /* LH_UI_WEB_HOST_GATE_FIELDS_H */
