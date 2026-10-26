/** @file response.h @brief HTTP/1 status-line views and parsed header count. */
#ifndef LH_HTTP_RESPONSE_H
#define LH_HTTP_RESPONSE_H

#include <lh/str/view.h>
#include <lh/numeric/fixed/types.h>
#include <lh/size.h>
#include <lh/http/response/fields.h>

struct lh_http_response
{
    lh_http_response_fields(lh_str_view_t, lh_u16_t, lh_u8_t, lh_usize_t);
};
typedef struct lh_http_response lh_http_response_t;

#endif
