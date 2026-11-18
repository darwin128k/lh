/**
 * @file finish.c
 * @brief Implementation of `lh/os/web/host/finish.h`.
 */

#include <lh/os/web/host/finish.h>

lh_bool_t
lh_os_web_host_finish_stream(lh_os_web_app_t *app, const lh_os_web_app_stream_route_t *route,
                             const lh_http_request_t *request, const lh_http_header_t *headers,
                             lh_os_net_socket_t *client, lh_bool_t committed)
{
    if (committed)
    {
        lh_os_web_app_stream_route_get_commit(route)(lh_os_web_app_get_ctx(app), request, headers,
                                                     client);
    }
    else
    {
        lh_os_web_app_stream_route_get_abort(route)(lh_os_web_app_get_ctx(app), request, headers,
                                                    client);
    }
    return committed;
}
