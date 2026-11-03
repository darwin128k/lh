/**
 * @file fields.h
 * @brief Fields of one streaming web route.
 */

#ifndef LH_UI_WEB_APP_STREAM_ROUTE_FIELDS_H
#define LH_UI_WEB_APP_STREAM_ROUTE_FIELDS_H

/** @brief Borrowed path matcher and handler, with content and metadata limits. */
#define lh_ui_web_stream_route_fields(route_type, handler_type, finish_type, size_type)            \
    route_type match;                                                                              \
    handler_type handler;                                                                          \
    finish_type commit;                                                                            \
    finish_type abort;                                                                             \
    size_type body_limit;                                                                          \
    size_type metadata_limit;                                                                      \
    lh_u32_t timeout_ms

#endif /* LH_UI_WEB_APP_STREAM_ROUTE_FIELDS_H */
