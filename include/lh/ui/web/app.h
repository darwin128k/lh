/**
 * @file app.h
 * @brief ::lh_ui_web_app_t — what a web application serves.
 *
 * The host is the transport: it listens, it reads the requests, it writes the
 * answers and it keeps the websocket clients. What is served is the
 * application's business: its routes (the REST surface it chooses to have),
 * its websocket path and the frames it pushes.
 *
 * Nothing is built in. An application that wants nothing is an application
 * with no routes; the desktop has no use for a REST surface, and the host
 * does not insist on one.
 *
 * Requires ::LH_LIBRARY_OPTION_UI and ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_UI_WEB_APP_H
#define LH_UI_WEB_APP_H

#include <lh/config.h>
#include <lh/http/request.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/web/app/fields.h>
#include <lh/ui/web/app/route.h>

#if !LH_LIBRARY_OPTION_UI
#    error "lh/ui/web/app.h requires LH_LIBRARY_OPTION_UI"
#endif
#if !LH_LIBRARY_OPTION_OS
#    error "lh/ui/web/app.h requires LH_LIBRARY_OPTION_OS"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The bytes of one frame, for the websocket clients.
 *
 * The application draws into its own buffer and hands the bytes over; the
 * transport never knows what a frame is made of.
 *
 * @param ctx    The application's context.
 * @param glyphs The font masks the frame uses; the transport owns them and
 *               resets them when a client missed a frame.
 * @param bytes  Receives the frame.
 * @param len    Receives its length.
 */
typedef lh_void (*lh_ui_web_app_frame_fn)(lh_ptr ctx, lh_ui_canvas_web_glyphs_t *glyphs,
                                          const lh_str_ptr *bytes, lh_usize_t *len);

/**
 * @brief What one text message from a client means.
 *
 * @param ctx  The application's context.
 * @param text The message.
 * @param len  Its length.
 */
typedef lh_void (*lh_ui_web_app_message_fn)(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len);

/**
 * @brief What the application does on a tick.
 *
 * @param ctx The application's context.
 */
typedef lh_void (*lh_ui_web_app_tick_fn)(lh_ptr ctx);

LH_COMPILER_EXTERN_C_END

/**
 * @struct lh_ui_web_app
 * @brief What a web application serves: its routes, its websocket and the
 *        callbacks that feed it.
 */
struct lh_ui_web_app
{
    /** Members: ::lh_ui_web_app_fields. */
    lh_ui_web_app_fields(const lh_ui_web_route_t *, lh_usize_t, const lh_str_ptr,
                         lh_ui_web_app_frame_fn, lh_ui_web_app_message_fn, lh_ui_web_app_tick_fn);
};
/** @copybrief lh_ui_web_app */
typedef struct lh_ui_web_app lh_ui_web_app_t;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The route table (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
const lh_ui_web_route_t *
lh_ui_web_app_get_routes(const lh_ui_web_app_t *self);

/**
 * @brief Store the route table (level 1).
 *
 * A level-1 setter: the pointer is stored as given and is not validated. The
 * table is not owned and must outlive the application. Its count is stored
 * apart, with ::lh_ui_web_app_set_nroutes.
 *
 * @param self   The application; checked by ::lh_assert_runtime_ref.
 * @param routes The new table.
 */
lh_void
lh_ui_web_app_set_routes(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes);

/**
 * @brief How many routes the application has (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_app_get_nroutes(const lh_ui_web_app_t *self);

/**
 * @brief Store how many routes the application has (level 1).
 *
 * A level-1 setter: the count is stored as given.
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param nroutes The new count.
 */
lh_void
lh_ui_web_app_set_nroutes(lh_ui_web_app_t *self, lh_usize_t nroutes);

/**
 * @brief The path the websocket answers on (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
const lh_str_ptr
lh_ui_web_app_get_ws_path(const lh_ui_web_app_t *self);

/**
 * @brief Store the path the websocket answers on (level 1).
 *
 * A level-1 setter: the pointer is stored as given and is not validated. The
 * text is not owned and must outlive the application. Its length is stored
 * apart, with ::lh_ui_web_app_set_ws_path_len.
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param ws_path The new path; with a length of 0 there is no websocket.
 */
lh_void
lh_ui_web_app_set_ws_path(lh_ui_web_app_t *self, const lh_str_ptr ws_path);

/**
 * @brief How long the websocket path is (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_usize_t
lh_ui_web_app_get_ws_path_len(const lh_ui_web_app_t *self);

/**
 * @brief Store how long the websocket path is (level 1).
 *
 * A level-1 setter: the length is stored as given.
 *
 * @param self        The application; checked by ::lh_assert_runtime_ref.
 * @param ws_path_len The new length.
 */
lh_void
lh_ui_web_app_set_ws_path_len(lh_ui_web_app_t *self, lh_usize_t ws_path_len);

/**
 * @brief The callback that draws a frame (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_app_frame_fn
lh_ui_web_app_get_frame_fn(const lh_ui_web_app_t *self);

/**
 * @brief Store the callback that draws a frame (level 1).
 *
 * A level-1 setter: the function pointer is stored as given.
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param frame_fn The new callback.
 */
lh_void
lh_ui_web_app_set_frame_fn(lh_ui_web_app_t *self, lh_ui_web_app_frame_fn frame_fn);

/**
 * @brief The callback that reads a message (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_app_message_fn
lh_ui_web_app_get_message_fn(const lh_ui_web_app_t *self);

/**
 * @brief Store the callback that reads a message (level 1).
 *
 * A level-1 setter: the function pointer is stored as given.
 *
 * @param self       The application; checked by ::lh_assert_runtime_ref.
 * @param message_fn The new callback.
 */
lh_void
lh_ui_web_app_set_message_fn(lh_ui_web_app_t *self, lh_ui_web_app_message_fn message_fn);

/**
 * @brief The callback that runs on a tick (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ui_web_app_tick_fn
lh_ui_web_app_get_tick_fn(const lh_ui_web_app_t *self);

/**
 * @brief Store the callback that runs on a tick (level 1).
 *
 * A level-1 setter: the function pointer is stored as given.
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param tick_fn The new callback.
 */
lh_void
lh_ui_web_app_set_tick_fn(lh_ui_web_app_t *self, lh_ui_web_app_tick_fn tick_fn);

/**
 * @brief The context every callback gets (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_ptr
lh_ui_web_app_get_ctx(const lh_ui_web_app_t *self);

/**
 * @brief Store the context every callback gets (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @param ctx  The new context.
 */
lh_void
lh_ui_web_app_set_ctx(lh_ui_web_app_t *self, lh_ptr ctx);

/**
 * @brief The frame counter (level 1).
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_uint_t
lh_ui_web_app_get_generation(const lh_ui_web_app_t *self);

/**
 * @brief Store the frame counter (level 1).
 *
 * A level-1 setter: the value is stored as given.
 *
 * @param self       The application; checked by ::lh_assert_runtime_ref.
 * @param generation The new counter.
 */
lh_void
lh_ui_web_app_set_generation(lh_ui_web_app_t *self, lh_uint_t generation);

/**
 * @brief Mark the content changed: the next frame is due for every client.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_ui_web_app_bump(lh_ui_web_app_t *self);

/**
 * @brief Whether @p req asks for the websocket.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @param request Parsed request; its target bytes remain caller-owned.
 *
 * @return ::lh_bool_true when the request target names the websocket path;
 *         ::lh_bool_false when no path is configured or it differs.
 */
lh_bool_t
lh_ui_web_app_is_websocket(const lh_ui_web_app_t *self, const lh_http_request_t *request);

/**
 * @brief Let the routes answer @p req.
 *
 * The first route that matches answers it. A route that does not answer is
 * not the end of it: the next route is tried.
 *
 * @param self   The application; checked by ::lh_assert_runtime_ref.
 * @param req    The request, as it came in.
 * @param len    Its length.
 * @param client The socket to answer on.
 *
 * @return ::lh_bool_true when a route answered.
 */
lh_bool_t
lh_ui_web_app_handle_request_is_handled(lh_ui_web_app_t *self, const lh_str_ptr req, lh_usize_t len,
                                        lh_os_net_socket_t *client);

/**
 * @brief The bytes of one frame for the websocket clients.
 *
 * @param self   The application; checked by ::lh_assert_runtime_ref.
 * @param glyphs The font masks the frame uses; the transport owns them.
 * @param bytes  Receives the frame.
 * @param len    Receives its length.
 */
lh_void
lh_ui_web_app_frame(lh_ui_web_app_t *self, lh_ui_canvas_web_glyphs_t *glyphs, const lh_str_ptr *bytes,
                    lh_usize_t *len);

/**
 * @brief Give one text message from a client to the application.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 * @param text The message.
 * @param len  Its length.
 */
lh_void
lh_ui_web_app_message(lh_ui_web_app_t *self, const lh_str_ptr text, lh_usize_t len);

/**
 * @brief Run the application's tick.
 *
 * @param self The application; checked by ::lh_assert_runtime_ref.
 */
lh_void
lh_ui_web_app_tick(lh_ui_web_app_t *self);

/**
 * @brief Start an application with @p routes.
 *
 * @param self    The application; checked by ::lh_assert_runtime_ref.
 * @param routes  The caller's route table; not owned, must outlive the
 *                application.
 * @param nroutes How many routes it holds.
 */
lh_void
lh_ui_web_app_init(lh_ui_web_app_t *self, const lh_ui_web_route_t *routes, lh_usize_t nroutes);

LH_COMPILER_EXTERN_C_END

#endif /* LH_UI_WEB_APP_H */
