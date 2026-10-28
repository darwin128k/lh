/**
 * @file client.h
 * @brief ::lh_ui_web_host_client_t — one WebSocket client: its socket, what it sent, and what is queued for it.
 */

#ifndef LH_UI_WEB_HOST_CLIENT_H
#define LH_UI_WEB_HOST_CLIENT_H

#include <lh/config.h>
#include <lh/net/port.h>
#include <lh/os/controller.h>
#include <lh/os/input/sink/cb.h>
#include <lh/os/net/socket.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/view.h>
#include <lh/ui/web/host/client/fields.h>
#include <lh/ui/web/host/frame/fields.h>
#include <lh/ui/web/host/stream/fields.h>
#include <lh/ui/web/ws.h>
#include <lh/ui/web/host/frame.h>

/**
 * @struct lh_ui_web_host_client
 * @brief One WebSocket client: its socket, what it sent, and what is queued for it.
 */
struct lh_ui_web_host_client
{
    /** Members: ::lh_ui_web_host_client_fields. */
    lh_ui_web_host_client_fields(lh_os_net_socket_t, lh_byte_ptr, lh_uint_t,
                                 lh_ui_web_host_frame_t *, lh_tick_t, lh_bool_t);
};
/** @copybrief lh_ui_web_host_client */
typedef struct lh_ui_web_host_client lh_ui_web_host_client_t;

#endif /* LH_UI_WEB_HOST_CLIENT_H */
