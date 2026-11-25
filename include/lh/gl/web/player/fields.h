/**
 * @file fields.h
 * @brief Member fields of ::lh_gl_web_player_t.
 */

#ifndef LH_GL_WEB_PLAYER_FIELDS_H
#define LH_GL_WEB_PLAYER_FIELDS_H

/**
 * @def lh_gl_web_player_fields(view_type, uint_type, str_type, size_type, hooks_type, app_type,
 * route_type, recognizer_type, web_type, hooks_value_type, glyphs_type)
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
 * - `routed` — the hooks the player works with: the caller's, with the
 *   player's own recognizer when the caller gave neither an input nor a
 *   recognizer;
 * - `recognizer` — the player's own recognizer, in front of its view;
 * - `app` — the application the host serves;
 * - `routes` — the player's routes;
 * - `glyphs` — the glyph masks the websocket clients already hold, so a
 *   frame sends each one once; emptied when the host asks for a frame that
 *   stands on its own.
 */
#define lh_gl_web_player_fields(view_type, uint_type, str_type, size_type, hooks_type, app_type,   \
                                route_type, recognizer_type, web_type, hooks_value_type,           \
                                glyphs_type)                                                       \
    view_type view;                                                                                \
    uint_type width;                                                                               \
    uint_type height;                                                                              \
    str_type json;                                                                                 \
    size_type json_cap;                                                                            \
    lh_byte_ptr page;                                                                              \
    size_type page_cap;                                                                            \
    size_type page_len;                                                                            \
    hooks_type hooks;                                                                              \
    hooks_value_type routed;                                                                       \
    recognizer_type recognizer;                                                                    \
    web_type web;                                                                                  \
    app_type app;                                                                                  \
    route_type routes[LH_GL_WEB_PLAYER_ROUTES];                                                    \
    glyphs_type glyphs

#endif /* LH_GL_WEB_PLAYER_FIELDS_H */
