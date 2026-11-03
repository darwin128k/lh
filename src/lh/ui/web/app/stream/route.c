/**
 * @file route.c
 * @brief Streaming web route accessors and matching.
 */

#include <lh/ui/web/app/stream/route.h>
#include <lh/assert.h>

const lh_ui_web_route_t *
lh_ui_web_stream_route_get_match(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->match;
}

lh_void
lh_ui_web_stream_route_set_match(lh_ui_web_stream_route_t *self, const lh_ui_web_route_t *value)
{
    lh_assert_runtime_ref(self);
    self->match = value;
}

lh_ui_web_stream_route_cb
lh_ui_web_stream_route_get_handler(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->handler;
}

lh_void
lh_ui_web_stream_route_set_handler(lh_ui_web_stream_route_t *self, lh_ui_web_stream_route_cb value)
{
    lh_assert_runtime_ref(self);
    self->handler = value;
}

lh_ui_web_stream_route_finish_cb
lh_ui_web_stream_route_get_commit(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->commit;
}

lh_void
lh_ui_web_stream_route_set_commit(lh_ui_web_stream_route_t *self,
                                  lh_ui_web_stream_route_finish_cb value)
{
    lh_assert_runtime_ref(self);
    self->commit = value;
}

lh_ui_web_stream_route_finish_cb
lh_ui_web_stream_route_get_abort(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->abort;
}

lh_void
lh_ui_web_stream_route_set_abort(lh_ui_web_stream_route_t *self,
                                 lh_ui_web_stream_route_finish_cb value)
{
    lh_assert_runtime_ref(self);
    self->abort = value;
}

lh_usize_t
lh_ui_web_stream_route_get_body_limit(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->body_limit;
}

lh_void
lh_ui_web_stream_route_set_body_limit(lh_ui_web_stream_route_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->body_limit = value;
}

lh_usize_t
lh_ui_web_stream_route_get_wire_limit(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->wire_limit;
}

lh_void
lh_ui_web_stream_route_set_wire_limit(lh_ui_web_stream_route_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->wire_limit = value;
}

lh_usize_t
lh_ui_web_stream_route_get_metadata_limit(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->metadata_limit;
}

lh_void
lh_ui_web_stream_route_set_metadata_limit(lh_ui_web_stream_route_t *self, lh_usize_t value)
{
    lh_assert_runtime_ref(self);
    self->metadata_limit = value;
}

lh_u32_t
lh_ui_web_stream_route_get_timeout_ms(const lh_ui_web_stream_route_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timeout_ms;
}

lh_void
lh_ui_web_stream_route_set_timeout_ms(lh_ui_web_stream_route_t *self, lh_u32_t value)
{
    lh_assert_runtime_ref(self);
    self->timeout_ms = value;
}

lh_void
lh_ui_web_stream_route_init(lh_ui_web_stream_route_t *self, const lh_ui_web_route_t *match,
                            lh_ui_web_stream_route_cb handler,
                            lh_ui_web_stream_route_finish_cb commit,
                            lh_ui_web_stream_route_finish_cb abort, lh_usize_t body_limit,
                            lh_usize_t metadata_limit, lh_u32_t timeout_ms)
{
    lh_ui_web_stream_route_set_match(self, match);
    lh_ui_web_stream_route_set_handler(self, handler);
    lh_ui_web_stream_route_set_commit(self, commit);
    lh_ui_web_stream_route_set_abort(self, abort);
    lh_ui_web_stream_route_set_body_limit(self, body_limit);
    lh_ui_web_stream_route_set_wire_limit(self, LH_USIZE_T_MAX);
    lh_ui_web_stream_route_set_metadata_limit(self, metadata_limit);
    lh_ui_web_stream_route_set_timeout_ms(self, timeout_ms);
}

lh_bool_t
lh_ui_web_stream_route_is_match(const lh_ui_web_stream_route_t *self,
                                const lh_http_request_t *request)
{
    return lh_null_ne(lh_ui_web_stream_route_get_handler(self)) &&
           lh_null_ne(lh_ui_web_stream_route_get_commit(self)) &&
           lh_null_ne(lh_ui_web_stream_route_get_abort(self)) &&
           lh_ui_web_route_is_match(lh_ui_web_stream_route_get_match(self), request);
}
