/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_app_t.
 */

#ifndef LH_UI_WEB_APP_FIELDS_H
#define LH_UI_WEB_APP_FIELDS_H

/**
 * @def lh_ui_web_app_fields(route_type, stream_type, size_type, path_type, frame_type,
 * message_type, tick_type)
 * @brief What a web application serves: its routes, its websocket and the
 *        callbacks that feed it.
 *
 * - `routes` — the caller's route table (not owned);
 * - `nroutes` — how many routes it holds;
 * - `stream_routes` and `nstream_routes` — the borrowed streaming route table;
 * - `ws_path` — the path the websocket answers on;
 * - `ws_path_len` — its length;
 * - `frame_fn` — the bytes of one frame for the websocket clients;
 * - `message_fn` — what a text message from a client means;
 * - `tick_fn` — what the application does on a tick;
 * - `ctx` — passed to every callback;
 * - `generation` — the frame counter: the clients that saw it need no more.
 */
#define lh_ui_web_app_fields(route_type, stream_type, size_type, path_type, frame_type,            \
                             message_type, tick_type)                                              \
    route_type routes;                                                                             \
    size_type nroutes;                                                                             \
    stream_type stream_routes;                                                                     \
    size_type nstream_routes;                                                                      \
    path_type ws_path;                                                                             \
    size_type ws_path_len;                                                                         \
    frame_type frame_fn;                                                                           \
    message_type message_fn;                                                                       \
    tick_type tick_fn;                                                                             \
    lh_ptr ctx;                                                                                    \
    lh_uint_t generation

#endif /* LH_UI_WEB_APP_FIELDS_H */
