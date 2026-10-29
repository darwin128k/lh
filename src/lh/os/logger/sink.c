/**
 * @file sink.c
 * @brief Accessors of ::lh_os_logger_sink_t.
 */

#include <lh/os/logger/sink.h>
#include <lh/assert/runtime.h>

lh_os_logger_sink_emit_cb
lh_os_logger_sink_get_emit_fn(const lh_os_logger_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->emit;
}

lh_void
lh_os_logger_sink_set_emit_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_emit_cb emit)
{
    lh_assert_runtime_ref(self);
    self->emit = emit;
}

lh_os_logger_sink_flush_cb
lh_os_logger_sink_get_flush_fn(const lh_os_logger_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->flush;
}

lh_void
lh_os_logger_sink_set_flush_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_flush_cb flush)
{
    lh_assert_runtime_ref(self);
    self->flush = flush;
}

lh_os_logger_sink_close_cb
lh_os_logger_sink_get_close_fn(const lh_os_logger_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->close;
}

lh_void
lh_os_logger_sink_set_close_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_close_cb close)
{
    lh_assert_runtime_ref(self);
    self->close = close;
}

lh_os_logger_sink_dropped_cb
lh_os_logger_sink_get_dropped_fn(const lh_os_logger_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->dropped;
}

lh_void
lh_os_logger_sink_set_dropped_fn(lh_os_logger_sink_t *self, lh_os_logger_sink_dropped_cb dropped)
{
    lh_assert_runtime_ref(self);
    self->dropped = dropped;
}

lh_ptr
lh_os_logger_sink_get_user(const lh_os_logger_sink_t *self)
{
    lh_assert_runtime_ref(self);
    return self->user;
}

lh_void
lh_os_logger_sink_set_user(lh_os_logger_sink_t *self, lh_ptr user)
{
    lh_assert_runtime_ref(self);
    self->user = user;
}
