/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_player_t.
 */

#ifndef LH_UI_WEB_PLAYER_FIELDS_H
#define LH_UI_WEB_PLAYER_FIELDS_H

/**
 * @def lh_ui_web_player_fields(view_type, uint_type, str_type, size_type, hooks_type, app_type, route_type, controller_type)
 * @brief The Canvas2D player: the view it shows, the buffers it draws into
 *        and the application it is.
 *
 * - `view` — the tree the player shows;
 * - `width`, `height` — the size of the canvas;
 * - `json` — the caller's scratch for one frame;
 * - `json_cap` — its size;
 * - `page` — the caller's buffer for the player document;
 * - `page_cap` — its size;
 * - `page_len` — how much of it the document takes;
 * - `hooks` — the application's hooks, as a window would call them;
 * - `routed` — the hooks the player works with: the caller's, or the
 *   player's own controller when the caller gave neither;
 * - `controller` — the player's own view controller;
 * - `owns_controller` — whether the controller is the player's;
 * - `app` — the application the host serves;
 * - `routes` — the player's routes.
 */
#define lh_ui_web_player_fields(view_type, uint_type, str_type, size_type, hooks_type, app_type,      \
                                route_type, controller_type, web_type, hooks_value_type)                \
    view_type view;                                                                                     \
    uint_type width;                                                                                    \
    uint_type height;                                                                                   \
    str_type json;                                                                                      \
    size_type json_cap;                                                                                 \
    lh_byte_ptr page;                                                                                   \
    size_type page_cap;                                                                                 \
    size_type page_len;                                                                                 \
    hooks_type hooks;                                                                                   \
    hooks_value_type routed;                                                                            \
    controller_type controller;                                                                         \
    lh_bool_t owns_controller;                                                                           \
    web_type web;                                                                                       \
    app_type app;                                                                                       \
    route_type routes[LH_UI_WEB_PLAYER_ROUTES]

#endif /* LH_UI_WEB_PLAYER_FIELDS_H */
