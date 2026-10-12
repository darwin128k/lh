/**
 * @file host.h
 * @brief Serve one ::lh_ui_view_t over HTTP as canvas JSON. No Python.
 *
 * GET `/` is a Canvas2D player. GET `/ui.json` is ::lh_ui_view_draw through
 * ::lh_ui_canvas_backend_web. GET or POST `/ui/click?x=&y=` is press then
 * release on that view, the same pair a window delivers. The host does not
 * know whose tree it is: cfg plugs the configurator, Paladin plugs its own window.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_HOST_H
#define LH_UI_WEB_HOST_H

#include <lh/bool.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/net/port.h>
#include <lh/size.h>
#include <lh/ui/view.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/host.h requires LH_LIBRARY_OPTION_OS"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Listen on 127.0.0.1:@p port and serve @p view. Does not return on success.
 *
 * @p json is the caller's scratch for one frame; it is overwritten each GET
 * `/ui.json`. @p width and @p height are the canvas the player should match.
 *
 * @return ::lh_bool_false when the socket would not listen (or @p view is null).
 */
lh_bool_t
lh_ui_web_host_run(lh_ui_view_t *view, unsigned width, unsigned height, char *json, lh_usize_t json_cap,
                   lh_net_port_t port);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_HOST_H */
