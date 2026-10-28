/**
 * @file stream.h
 * @brief ::lh_ui_web_host_stream_t — what the stream of WS frames shares between iterations.
 */

#ifndef LH_UI_WEB_HOST_STREAM_H
#define LH_UI_WEB_HOST_STREAM_H

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
#include <lh/ui/web/host/client.h>

/**
 * @struct lh_ui_web_host_stream
 * @brief What the stream of WS frames shares between iterations.
 */
struct lh_ui_web_host_stream
{
    /** Members: ::lh_ui_web_host_stream_fields. */
    lh_ui_web_host_stream_fields(lh_ui_canvas_web_glyphs_t, lh_str_ptr, lh_usize_t);
};
/** @copybrief lh_ui_web_host_stream */
typedef struct lh_ui_web_host_stream lh_ui_web_host_stream_t;

#endif /* LH_UI_WEB_HOST_STREAM_H */
