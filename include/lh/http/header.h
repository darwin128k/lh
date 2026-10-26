/** @file header.h @brief Non-owning HTTP field views; populated by the parser. */
#ifndef LH_HTTP_HEADER_H
#define LH_HTTP_HEADER_H

#include <lh/str/view.h>
#include <lh/http/header/fields.h>

struct lh_http_header
{
    lh_http_header_fields(lh_str_view_t);
};
typedef struct lh_http_header lh_http_header_t;

#endif
