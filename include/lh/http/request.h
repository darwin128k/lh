/** @file request.h @brief HTTP/1 request-line views and parsed header count. */
#ifndef LH_HTTP_REQUEST_H
#define LH_HTTP_REQUEST_H

#include <lh/str/view.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/http/request/fields.h>

struct lh_http_request
{
    lh_http_request_fields(lh_str_view_t, lh_u8_t, lh_usize_t);
};
typedef struct lh_http_request lh_http_request_t;

#endif
