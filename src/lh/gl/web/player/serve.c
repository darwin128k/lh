/**
 * @file serve.c
 * @brief Implementation of `lh/gl/web/player.h`: what the player serves over HTTP: the page, its
 * revision, a frame.
 */

#include <lh/gl/web/player.h>

#include <lh/core/assert.h>
#include <lh/core/http/status.h>
#include <lh/core/attribute/unused.h>
#include <lh/core/str/format/text.h>
#include <lh/os/web/host.h>

lh_bool_t
lh_gl_web_player_serve_page(lh_ptr ctx, const lh_http_request_t *request,
                            const lh_http_header_t *headers, const lh_str_view_t *request_body,
                            lh_os_net_socket_t *client)
{
    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);

    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);
    lh_assert_runtime_ref(self);
    LH_ATTRIBUTE_UNUSED(
        lh_os_web_host_try_reply(client, request, lh_http_status_ok, "text/html; charset=utf-8",
                                 lh_ptr_rcast(const lh_char_t, lh_gl_web_player_get_page(self)),
                                 lh_gl_web_player_get_page_len(self)));
    return lh_bool_true;
}

lh_bool_t
lh_gl_web_player_serve_revision(lh_ptr ctx, const lh_http_request_t *request,
                                const lh_http_header_t *headers, const lh_str_view_t *request_body,
                                lh_os_net_socket_t *client)
{
    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);
    lh_char_t body[32];
    const lh_usize_t n = lh_str_ptr_format_text(
        body, sizeof(body), "%u\n",
        lh_cast_static(lh_uint_t, lh_os_web_app_get_generation(lh_gl_web_player_get_app(self))));

    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);
    lh_assert_runtime_ref(self);
    LH_ATTRIBUTE_UNUSED(lh_os_web_host_try_reply_text(client, request, lh_http_status_ok, body, n));
    return lh_bool_true;
}

lh_bool_t
lh_gl_web_player_serve_frame(lh_ptr ctx, const lh_http_request_t *request,
                             const lh_http_header_t *headers, const lh_str_view_t *request_body,
                             lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);

    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);
    const lh_str_ptr bytes = lh_null;
    lh_usize_t n = 0u;

    lh_assert_runtime_ref(self);
    if (lh_os_web_host_get_query_uint(request, lh_str_lit("rev=")) ==
        lh_os_web_app_get_generation(lh_gl_web_player_get_app(self)))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_web_host_try_reply_text(
            client, request, lh_http_status_not_modified, lh_str_lit("")));
        return lh_bool_true;
    }
    if (!lh_gl_web_player_draw(self, lh_null, lh_addr_of(bytes), lh_addr_of(n)))
    {
        LH_ATTRIBUTE_UNUSED(lh_os_web_host_try_reply_text(
            client, request, lh_http_status_internal_server_error, lh_str_lit("draw failed\n")));
        return lh_bool_true;
    }
    LH_ATTRIBUTE_UNUSED(lh_os_web_host_try_reply(client, request, lh_http_status_ok,
                                                 "application/json; charset=utf-8", bytes, n));
    return lh_bool_true;
}
