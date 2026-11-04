/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_host_t.
 */

#ifndef LH_UI_WEB_HOST_FIELDS_H
#define LH_UI_WEB_HOST_FIELDS_H

#include <lh/ui/web/host/capacity.h>

/**
 * @def LH_UI_WEB_HOST_POLL_MAX
 * @brief Poll entries of the host: the listener, the wake signal, every
 *        websocket client and every HTTP connection.
 */
#define LH_UI_WEB_HOST_POLL_MAX (2u + LH_UI_WEB_HOST_WS_MAX + LH_UI_WEB_HOST_CONN_MAX)

/**
 * @def lh_ui_web_host_fields()
 * @brief The application; the listener and wake signal; the loop; the
 *        websocket clients and their last frame; the HTTP connections; the poll
 *        entries of the last wait (entry 0 the listener, 1 the wake signal, then
 *        the clients, then the connections, slot for slot).
 */
#define lh_ui_web_host_fields()                                                                    \
    lh_ui_web_app_t *app;                                                                          \
    lh_os_net_socket_t listener;                                                                   \
    lh_os_wake_t wake;                                                                             \
    lh_exec_loop_t *loop;                                                                          \
    lh_ui_web_host_client_t clients[LH_UI_WEB_HOST_WS_MAX];                                        \
    lh_ui_web_host_stream_t stream;                                                                \
    lh_ui_web_host_conn_t conns[LH_UI_WEB_HOST_CONN_MAX];                                          \
    lh_os_net_socket_poll_t events[LH_UI_WEB_HOST_POLL_MAX]

#endif /* LH_UI_WEB_HOST_FIELDS_H */
