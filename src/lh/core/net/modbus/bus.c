#include <lh/core/net/modbus/bus.h>

#include <lh/core/assert/runtime.h>

const lh_mb_link_t *
lh_mb_bus_get_link(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

lh_mb_link_t *
lh_mb_bus_get_link_mut(lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->link);
}

const lh_mb_job_t *
lh_mb_bus_get_job(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->job);
}

lh_mb_job_t *
lh_mb_bus_get_job_mut(lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return lh_addr_of(self->job);
}

lh_bool_t
lh_mb_bus_is_busy(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->busy;
}

lh_void
lh_mb_bus_set_busy(lh_mb_bus_t *self, lh_bool_t busy)
{
    lh_assert_runtime_ref(self);
    self->busy = busy;
}

lh_u8_t
lh_mb_bus_get_retries(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->retries;
}

lh_void
lh_mb_bus_set_retries(lh_mb_bus_t *self, lh_u8_t retries)
{
    lh_assert_runtime_ref(self);
    self->retries = retries;
}

lh_u8_t
lh_mb_bus_get_attempt(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->attempt;
}

lh_void
lh_mb_bus_set_attempt(lh_mb_bus_t *self, lh_u8_t attempt)
{
    lh_assert_runtime_ref(self);
    self->attempt = attempt;
}

lh_u32_t
lh_mb_bus_get_timeout_us(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->timeout_us;
}

lh_void
lh_mb_bus_set_timeout_us(lh_mb_bus_t *self, lh_u32_t timeout_us)
{
    lh_assert_runtime_ref(self);
    self->timeout_us = timeout_us;
}

lh_u32_t
lh_mb_bus_get_resent(const lh_mb_bus_t *self)
{
    lh_assert_runtime_ref(self);
    return self->resent;
}

lh_void
lh_mb_bus_set_resent(lh_mb_bus_t *self, lh_u32_t resent)
{
    lh_assert_runtime_ref(self);
    self->resent = resent;
}

lh_bool_t
lh_mb_bus_is_retryable(lh_mb_status_t reason)
{
    return lh_cast_static(lh_bool_t,
                          (reason != lh_mb_status_exception && reason != lh_mb_status_lost &&
                           reason != lh_mb_status_ok));
}

lh_void
lh_mb_bus_init(lh_mb_bus_t *self, const lh_io_stream_t *stream, lh_mb_framing_t framing,
               lh_mb_clock_cb now_us, lh_ptr clock, lh_u32_t timeout_us)
{
    /* Unit 1 until the first job names its own. */
    lh_mb_link_init(lh_mb_bus_get_link_mut(self), stream, framing, 1, now_us, clock);
    lh_mb_job_init_read(lh_mb_bus_get_job_mut(self), 1, LH_MB_FC_READ_HOLDING, 0, 1);
    lh_mb_bus_set_busy(self, lh_bool_false);
    lh_mb_bus_set_retries(self, 0);
    lh_mb_bus_set_attempt(self, 0);
    lh_mb_bus_set_timeout_us(self, timeout_us);
    lh_mb_bus_set_resent(self, 0);
}

lh_bool_t
lh_mb_bus_try_send(lh_mb_bus_t *self)
{
    lh_mb_link_t *link = lh_mb_bus_get_link_mut(self);
    const lh_mb_job_t *job = lh_mb_bus_get_job(self);
    const lh_u8_t fc = lh_mb_job_get_fc(job);

    lh_mb_link_set_unit(link, lh_mb_job_get_unit(job));
    if (fc == LH_MB_FC_READ_HOLDING || fc == LH_MB_FC_READ_INPUT)
    {
        lh_mb_link_set_fc(link, fc);
        return lh_mb_link_ask(link, lh_mb_job_get_address(job), lh_mb_job_get_count(job));
    }
    if (fc == LH_MB_FC_WRITE_ONE)
    {
        return lh_mb_link_write_one(link, lh_mb_job_get_address(job), lh_mb_job_get_values(job)[0]);
    }
    if (fc == LH_MB_FC_WRITE_MANY && lh_mb_job_get_count(job) <= LH_MB_WRITE_MAX)
    {
        return lh_mb_link_write(link, lh_mb_job_get_address(job), lh_mb_job_get_values(job),
                                lh_mb_job_get_count(job));
    }
    return lh_bool_false;
}

lh_bool_t
lh_mb_bus_try_start(lh_mb_bus_t *self, const lh_mb_job_t *job)
{
    lh_assert_runtime_ref(job);
    if (lh_mb_bus_is_busy(self))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(lh_mb_bus_get_job_mut(self)) = lh_ptr_deref(job);
    lh_mb_bus_set_attempt(self, 0);
    if (!lh_mb_bus_try_send(self))
    {
        return lh_bool_false;
    }
    lh_mb_bus_set_busy(self, lh_bool_true);
    return lh_bool_true;
}

lh_mb_link_state_t
lh_mb_bus_poll(lh_mb_bus_t *self)
{
    if (!lh_mb_bus_is_busy(self))
    {
        return lh_mb_link_idle;
    }
    lh_mb_link_t *link = lh_mb_bus_get_link_mut(self);
    const lh_mb_link_state_t state = lh_mb_link_poll(link, lh_mb_bus_get_timeout_us(self));

    if (state != lh_mb_link_failed || !lh_mb_bus_is_retryable(lh_mb_link_get_reason(link)) ||
        lh_mb_bus_get_attempt(self) >= lh_mb_bus_get_retries(self))
    {
        return state;
    }
    lh_mb_bus_set_attempt(self, lh_cast_static(lh_u8_t, lh_mb_bus_get_attempt(self) + 1U));
    lh_mb_bus_set_resent(self, lh_mb_bus_get_resent(self) + 1U);
    /* Sent again: the job goes on waiting. Not sent: the line failed under it. */
    return lh_mb_bus_try_send(self) ? lh_mb_link_waiting : lh_mb_link_failed;
}

lh_void
lh_mb_bus_release(lh_mb_bus_t *self)
{
    lh_mb_link_release(lh_mb_bus_get_link_mut(self));
    lh_mb_bus_set_busy(self, lh_bool_false);
}
