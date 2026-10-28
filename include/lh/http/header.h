/**
 * @file header.h
 * @brief Non-owning HTTP field views; populated by the parser.
 */

#ifndef LH_HTTP_HEADER_H
#define LH_HTTP_HEADER_H

#include <lh/str/view.h>
#include <lh/http/header/fields.h>

/**
 * @struct lh_http_header
 * @brief One parsed HTTP field: its name and its value.
 *
 * Both views point into the caller's input buffer and expire with it.
 */
struct lh_http_header
{
    /** Members: ::lh_http_header_fields. */
    lh_http_header_fields(lh_str_view_t);
};
/** @copybrief lh_http_header */
typedef struct lh_http_header lh_http_header_t;

#endif /* LH_HTTP_HEADER_H */
