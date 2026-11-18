/**
 * @file fields.h
 * @brief Macro for declaring the members of ::lh_gx_web_t.
 */

#ifndef LH_GX_WEB_FIELDS_H
#define LH_GX_WEB_FIELDS_H

/**
 * @def LH_GX_WEB_ROUTES
 * @brief How many routes the application answers: the page, `GET /`.
 */
#define LH_GX_WEB_ROUTES 1U

/**
 * @def lh_gx_web_fields(render_type, canvas_type, backend_type, writer_type, text_type,
 * pointer_type, route_type, app_type)
 * @brief A gx screen served to browsers, and everything that serves it.
 *
 * - `render` — the renderer of the screen; it draws through `canvas` with
 *   `backend` as the context;
 * - `canvas` — the web backend's table;
 * - `backend` — the web backend, writing through `writer`;
 * - `writer` — the JSON writer, appending to `frame`;
 * - `frame` — the text of the frame being sent (owned);
 * - `page` — the page every browser is given (owned);
 * - `pointer` — the pointer the browsers' lines drive;
 * - `routes` — the routes of `app`;
 * - `app` — what the host serves.
 *
 * The members point at each other, so the record must not move after
 * ::lh_gx_web_init.
 */
#define lh_gx_web_fields(render_type, canvas_type, backend_type, writer_type, text_type,           \
                         pointer_type, route_type, app_type)                                       \
    render_type render;                                                                            \
    canvas_type canvas;                                                                            \
    backend_type backend;                                                                          \
    writer_type writer;                                                                            \
    text_type frame;                                                                               \
    text_type page;                                                                                \
    pointer_type pointer;                                                                          \
    route_type routes[LH_GX_WEB_ROUTES];                                                           \
    app_type app

#endif /* LH_GX_WEB_FIELDS_H */
