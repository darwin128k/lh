#include <lh/core/net/modbus/server/counters.h>
#include <lh/core/assert.h>

lh_u32_t
lh_mb_server_counters_get_received(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->received;
}

lh_void
lh_mb_server_counters_set_received(lh_mb_server_counters_t *self, lh_u32_t received)
{
    lh_assert_runtime_ref(self);
    self->received = received;
}

lh_u32_t
lh_mb_server_counters_get_answered(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->answered;
}

lh_void
lh_mb_server_counters_set_answered(lh_mb_server_counters_t *self, lh_u32_t answered)
{
    lh_assert_runtime_ref(self);
    self->answered = answered;
}

lh_u32_t
lh_mb_server_counters_get_refused(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->refused;
}

lh_void
lh_mb_server_counters_set_refused(lh_mb_server_counters_t *self, lh_u32_t refused)
{
    lh_assert_runtime_ref(self);
    self->refused = refused;
}

lh_u32_t
lh_mb_server_counters_get_broadcast(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->broadcast;
}

lh_void
lh_mb_server_counters_set_broadcast(lh_mb_server_counters_t *self, lh_u32_t broadcast)
{
    lh_assert_runtime_ref(self);
    self->broadcast = broadcast;
}

lh_u32_t
lh_mb_server_counters_get_ignored(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ignored;
}

lh_void
lh_mb_server_counters_set_ignored(lh_mb_server_counters_t *self, lh_u32_t ignored)
{
    lh_assert_runtime_ref(self);
    self->ignored = ignored;
}

lh_u32_t
lh_mb_server_counters_get_broken(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->broken;
}

lh_void
lh_mb_server_counters_set_broken(lh_mb_server_counters_t *self, lh_u32_t broken)
{
    lh_assert_runtime_ref(self);
    self->broken = broken;
}

lh_u32_t
lh_mb_server_counters_get_bytes(const lh_mb_server_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bytes;
}

lh_void
lh_mb_server_counters_set_bytes(lh_mb_server_counters_t *self, lh_u32_t bytes)
{
    lh_assert_runtime_ref(self);
    self->bytes = bytes;
}

lh_void
lh_mb_server_counters_count_received(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_received(self, lh_mb_server_counters_get_received(self) + 1U);
}

lh_void
lh_mb_server_counters_count_answered(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_answered(self, lh_mb_server_counters_get_answered(self) + 1U);
}

lh_void
lh_mb_server_counters_count_refused(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_refused(self, lh_mb_server_counters_get_refused(self) + 1U);
}

lh_void
lh_mb_server_counters_count_broadcast(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_broadcast(self, lh_mb_server_counters_get_broadcast(self) + 1U);
}

lh_void
lh_mb_server_counters_count_ignored(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_ignored(self, lh_mb_server_counters_get_ignored(self) + 1U);
}

lh_void
lh_mb_server_counters_count_broken(lh_mb_server_counters_t *self)
{
    lh_mb_server_counters_set_broken(self, lh_mb_server_counters_get_broken(self) + 1U);
}

lh_void
lh_mb_server_counters_add_bytes(lh_mb_server_counters_t *self, lh_u32_t bytes)
{
    lh_mb_server_counters_set_bytes(self, lh_mb_server_counters_get_bytes(self) + bytes);
}
