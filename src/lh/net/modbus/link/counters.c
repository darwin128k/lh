#include <lh/net/modbus/link/counters.h>
#include <lh/assert.h>

lh_u32_t
lh_mb_link_counters_get_sent(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->sent;
}

lh_void
lh_mb_link_counters_set_sent(lh_mb_link_counters_t *self, lh_u32_t sent)
{
    lh_assert_runtime_ref(self);
    self->sent = sent;
}

lh_u32_t
lh_mb_link_counters_get_done(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->done;
}

lh_void
lh_mb_link_counters_set_done(lh_mb_link_counters_t *self, lh_u32_t done)
{
    lh_assert_runtime_ref(self);
    self->done = done;
}

lh_u32_t
lh_mb_link_counters_get_failed(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->failed;
}

lh_void
lh_mb_link_counters_set_failed(lh_mb_link_counters_t *self, lh_u32_t failed)
{
    lh_assert_runtime_ref(self);
    self->failed = failed;
}

lh_u32_t
lh_mb_link_counters_get_refused(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->refused;
}

lh_void
lh_mb_link_counters_set_refused(lh_mb_link_counters_t *self, lh_u32_t refused)
{
    lh_assert_runtime_ref(self);
    self->refused = refused;
}

lh_u32_t
lh_mb_link_counters_get_bytes(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->bytes;
}

lh_void
lh_mb_link_counters_set_bytes(lh_mb_link_counters_t *self, lh_u32_t bytes)
{
    lh_assert_runtime_ref(self);
    self->bytes = bytes;
}

lh_u64_t
lh_mb_link_counters_get_us_busy(const lh_mb_link_counters_t *self)
{
    lh_assert_runtime_ref(self);
    return self->us_busy;
}

lh_void
lh_mb_link_counters_set_us_busy(lh_mb_link_counters_t *self, lh_u64_t us_busy)
{
    lh_assert_runtime_ref(self);
    self->us_busy = us_busy;
}

lh_void
lh_mb_link_counters_count_sent(lh_mb_link_counters_t *self)
{
    lh_mb_link_counters_set_sent(self, lh_mb_link_counters_get_sent(self) + 1U);
}

lh_void
lh_mb_link_counters_count_done(lh_mb_link_counters_t *self)
{
    lh_mb_link_counters_set_done(self, lh_mb_link_counters_get_done(self) + 1U);
}

lh_void
lh_mb_link_counters_count_failed(lh_mb_link_counters_t *self)
{
    lh_mb_link_counters_set_failed(self, lh_mb_link_counters_get_failed(self) + 1U);
}

lh_void
lh_mb_link_counters_count_refused(lh_mb_link_counters_t *self)
{
    lh_mb_link_counters_set_refused(self, lh_mb_link_counters_get_refused(self) + 1U);
}

lh_void
lh_mb_link_counters_add_bytes(lh_mb_link_counters_t *self, lh_u32_t bytes)
{
    lh_mb_link_counters_set_bytes(self, lh_mb_link_counters_get_bytes(self) + bytes);
}

lh_void
lh_mb_link_counters_add_us_busy(lh_mb_link_counters_t *self, lh_u64_t us)
{
    lh_mb_link_counters_set_us_busy(self, lh_mb_link_counters_get_us_busy(self) + us);
}
