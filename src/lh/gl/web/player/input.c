/**
 * @file input.c
 * @brief Implementation of `lh/gl/web/player.h`: the input: one event down the chain, and the lines
 * that carry them.
 */

#include <lh/gl/web/player.h>

#include <lh/core/assert.h>
#include <lh/core/http/status.h>
#include <lh/core/attribute/unused.h>
#include <lh/os/web/host.h>
#include <lh/gl/view/sink.h>
#include <lh/os/tick.h>
#include <lh/core/str/parse/text.h>
#include <lh/core/util/return.h>

lh_input_recognizer_t *
lh_gl_web_player_get_routed_recognizer(const lh_gl_web_player_t *self)
{
    return lh_os_web_host_hooks_get_recognizer(lh_gl_web_player_get_routed(self));
}

lh_input_sink_cb
lh_gl_web_player_get_sink(const lh_gl_web_player_t *self)
{
    lh_input_sink_cb input = lh_os_web_host_hooks_get_input_fn(lh_gl_web_player_get_routed(self));

    return lh_null_eq(lh_ptr_rcast(lh_void, input)) ? lh_gl_view_sink : input;
}

lh_ptr
lh_gl_web_player_get_sink_context(const lh_gl_web_player_t *self)
{
    const lh_os_web_host_hooks_t *hooks = lh_gl_web_player_get_routed(self);

    return lh_null_eq(lh_ptr_rcast(lh_void, lh_os_web_host_hooks_get_input_fn(hooks)))
               ? lh_ptr_rcast(lh_void, lh_gl_web_player_get_view(self))
               : lh_os_web_host_hooks_get_ctx(hooks);
}

lh_bool_t
lh_gl_web_player_input(lh_gl_web_player_t *self, const lh_input_event_t *event)
{
    lh_input_sink_cb sink = lh_gl_web_player_get_sink(self);
    lh_ptr context = lh_gl_web_player_get_sink_context(self);
    lh_input_recognizer_t *recognizer = lh_gl_web_player_get_routed_recognizer(self);

    if (lh_null_eq(recognizer))
    {
        sink(context, event);
    }
    else
    {
        lh_input_recognizer_feed(recognizer, event, sink, context);
    }
    /* The caller's hook does what it does out of sight; the player's own view says. */
    return lh_type_cast(lh_bool_t, sink != lh_gl_view_sink ||
                                       lh_gl_view_has_damage(lh_gl_web_player_get_view(self)));
}

lh_bool_t
lh_gl_web_player_pointer(lh_gl_web_player_t *self, lh_input_kind_t kind, lh_sint_t x, lh_sint_t y)
{
    const lh_bool_t up = lh_type_cast(lh_bool_t, kind == lh_input_kind_pointer_up);
    const lh_input_button_t button =
        kind == lh_input_kind_pointer_move ? lh_input_button_none : lh_input_button_primary;

    lh_input_event_t event;
    lh_input_event_init_pointer(lh_addr_of(event), kind, lh_input_device_kind_mouse, 0U, button,
                                lh_input_coord_from_int(x), lh_input_coord_from_int(y),
                                lh_os_tick_ms());
    lh_input_event_set_buttons(lh_addr_of(event),
                               up ? 0U : lh_input_button_to_mask(lh_input_button_primary));
    return lh_gl_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_gl_web_player_wheel(lh_gl_web_player_t *self, lh_sint_t x, lh_sint_t y, lh_sint_t dx,
                       lh_sint_t dy)
{
    lh_input_event_t event;

    lh_input_event_init_wheel(lh_addr_of(event), lh_input_coord_from_int(x),
                              lh_input_coord_from_int(y),
                              lh_type_cast(lh_s32_t, dx) * LH_INPUT_WHEEL_NOTCH,
                              lh_type_cast(lh_s32_t, dy) * LH_INPUT_WHEEL_NOTCH, lh_os_tick_ms());
    return lh_gl_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_gl_web_player_key(lh_gl_web_player_t *self, lh_sint_t k, lh_bool_t pressed)
{
    if (k < lh_cast_static(lh_sint_t, lh_key_other) || k > lh_cast_static(lh_sint_t, lh_key_alt))
    {
        k = lh_cast_static(lh_sint_t, lh_key_other);
    }
    lh_input_event_t event;
    lh_input_event_init_key(lh_addr_of(event), lh_type_cast(lh_key_t, k), pressed, lh_bool_false,
                            0U, lh_os_tick_ms());
    return lh_gl_web_player_input(self, lh_addr_of(event));
}

lh_bool_t
lh_gl_web_player_text(lh_gl_web_player_t *self, lh_u32_t code)
{
    lh_return_if(code == 0U, lh_bool_false);
    lh_input_event_t event;
    lh_input_event_init_text(lh_addr_of(event), code, lh_os_tick_ms());
    return lh_gl_web_player_input(self, lh_addr_of(event));
}

lh_void
lh_gl_web_player_event_line(lh_gl_web_player_t *self, const lh_str_ptr line, lh_usize_t len)
{
    lh_sint_t x = 0;
    lh_sint_t y = 0;
    lh_sint_t dx = 0;
    lh_sint_t dy = 0;
    lh_sint_t k = 0;
    lh_sint_t pressed = 0;
    lh_uint_t code = 0u;

    if (lh_str_ptr_parse_text(line, len, "p %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_down,
                                                     lh_cast_static(lh_sint_t, x),
                                                     lh_cast_static(lh_sint_t, y)));
    }
    else if (lh_str_ptr_parse_text(line, len, "r %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_up,
                                                     lh_cast_static(lh_sint_t, x),
                                                     lh_cast_static(lh_sint_t, y)));
    }
    else if (lh_str_ptr_parse_text(line, len, "w %d %d %d %d", lh_addr_of(x), lh_addr_of(y),
                                   lh_addr_of(dx), lh_addr_of(dy)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(
            lh_gl_web_player_wheel(self, lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y),
                                   lh_cast_static(lh_sint_t, dx), lh_cast_static(lh_sint_t, dy)));
    }
    else if (lh_str_ptr_parse_text(line, len, "m %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* A drag sends one every frame; one that changed nothing is not worth a draw. */
        if (!lh_gl_web_player_pointer(self, lh_input_kind_pointer_move,
                                      lh_cast_static(lh_sint_t, x), lh_cast_static(lh_sint_t, y)))
        {
            return;
        }
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d %d", lh_addr_of(k), lh_addr_of(pressed)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_key(self, lh_cast_static(lh_sint_t, k),
                                                 (pressed != 0) ? lh_bool_true : lh_bool_false));
    }
    else if (lh_str_ptr_parse_text(line, len, "k %d", lh_addr_of(k)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_key(self, lh_cast_static(lh_sint_t, k), lh_bool_true));
    }
    else if (lh_str_ptr_parse_text(line, len, "t %u", lh_addr_of(code)) != 0u)
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_text(self, lh_cast_static(lh_u32_t, code)));
    }
    else if (lh_str_ptr_parse_text(line, len, "z %d %d", lh_addr_of(x), lh_addr_of(y)) != 0u)
    {
        /* Always bump: an unchanged size still needs a frame for a new tab, or the
           player keeps the design 980×620 it got before `z` arrived. */
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_resize(self, lh_cast_static(lh_sint_t, x),
                                                    lh_cast_static(lh_sint_t, y)));
    }
    else
    {
        return;
    }
    lh_os_web_app_bump(lh_gl_web_player_get_app(self));
}

lh_bool_t
lh_gl_web_player_serve_input(lh_ptr ctx, const lh_http_request_t *request,
                             const lh_http_header_t *headers, const lh_str_view_t *request_body,
                             lh_os_net_socket_t *client)
{
    LH_ATTRIBUTE_UNUSED(headers);
    LH_ATTRIBUTE_UNUSED(request_body);

    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);
    const lh_sint_t x = lh_os_web_host_get_query_int(request, lh_str_lit("x="));
    const lh_sint_t y = lh_os_web_host_get_query_int(request, lh_str_lit("y="));

    lh_assert_runtime_ref(self);
    if (lh_os_web_host_is_route(request, lh_str_lit("/ui/click")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_down, x, y));
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_up, x, y));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/press")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_down, x, y));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/release")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_up, x, y));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/move")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_pointer(self, lh_input_kind_pointer_move, x, y));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/wheel")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_wheel(
            self, x, y, lh_os_web_host_get_query_int(request, lh_str_lit("dx=")),
            lh_os_web_host_get_query_int(request, lh_str_lit("dy="))));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/key")))
    {
        /* `p=` is the direction; without it the request is a key going down, as before. */
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_key(
            self, lh_os_web_host_get_query_int(request, lh_str_lit("k=")),
            lh_os_web_host_get_query_flag(request, lh_str_lit("p="), lh_bool_true)));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/text")))
    {
        LH_ATTRIBUTE_UNUSED(lh_gl_web_player_text(
            self,
            lh_cast_static(lh_u32_t, lh_os_web_host_get_query_uint(request, lh_str_lit("c=")))));
    }
    else if (lh_os_web_host_is_route(request, lh_str_lit("/ui/size")))
    {
        /* Always handled: an unchanged size is still a valid no-op, not a 404. */
        LH_ATTRIBUTE_UNUSED(
            lh_gl_web_player_resize(self, lh_os_web_host_get_query_int(request, lh_str_lit("w=")),
                                    lh_os_web_host_get_query_int(request, lh_str_lit("h="))));
    }
    else
    {
        return lh_bool_false;
    }
    lh_os_web_app_bump(lh_gl_web_player_get_app(self));
    LH_ATTRIBUTE_UNUSED(
        lh_os_web_host_try_reply_text(client, request, lh_http_status_ok, lh_str_lit("ok\n")));
    return lh_bool_true;
}

lh_void
lh_gl_web_player_message(lh_ptr ctx, const lh_str_ptr text, lh_usize_t len)
{
    lh_gl_web_player_t *self = lh_ptr_rcast(lh_gl_web_player_t, ctx);

    lh_gl_web_player_event_line(self, text, len);
}
