/**
 * @file fields.h
 * @brief Member fields of ::lh_ui_web_route_t.
 */

#ifndef LH_UI_WEB_APP_ROUTE_FIELDS_H
#define LH_UI_WEB_APP_ROUTE_FIELDS_H

/**
 * @def lh_ui_web_route_fields(method_type, size_type, path_type, handler_type)
 * @brief One route of an application: the method and the path it answers, and
 *        what answers it.
 *
 * - `method` — the HTTP method ("GET", "POST"); empty for any method;
 * - `method_len` — its length;
 * - `path` — the path ("/ui.json");
 * - `path_len` — its length;
 * - `handler` — what answers it.
 */
#define lh_ui_web_route_fields(method_type, size_type, path_type, handler_type)                         \
    method_type method;                                                                                 \
    size_type method_len;                                                                               \
    path_type path;                                                                                     \
    size_type path_len;                                                                                 \
    handler_type handler

#endif /* LH_UI_WEB_APP_ROUTE_FIELDS_H */
