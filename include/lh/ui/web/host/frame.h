/**
 * @file frame.h
 * @brief ::lh_ui_web_host_frame_t — one encoded WebSocket frame waiting to be sent.
 */

#ifndef LH_UI_WEB_HOST_FRAME_H
#define LH_UI_WEB_HOST_FRAME_H

#include <lh/config.h>
#include <lh/os/controller.h>
#include <lh/os/net/socket.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/view.h>
#include <lh/ui/web/host/client/fields.h>
#include <lh/ui/web/host/frame/fields.h>
#include <lh/ui/web/host/stream/fields.h>
#include <lh/ui/web/ws.h>

/**
 * @struct lh_ui_web_host_frame
 * @brief One encoded WebSocket frame waiting to be sent.
 */
struct lh_ui_web_host_frame
{
    /** Members: ::lh_ui_web_host_frame_fields. */
    lh_ui_web_host_frame_fields(struct lh_ui_web_host_frame *, lh_byte_ptr, lh_uint_t);
};
/** @copybrief lh_ui_web_host_frame */
typedef struct lh_ui_web_host_frame lh_ui_web_host_frame_t;

#endif /* LH_UI_WEB_HOST_FRAME_H */
