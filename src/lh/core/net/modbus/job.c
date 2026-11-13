#include <lh/core/net/modbus/job.h>

#include <lh/core/assert/runtime.h>
#include <lh/core/memory/std.h>

lh_u32_t
lh_mb_job_get_owner(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->owner;
}

lh_void
lh_mb_job_set_owner(lh_mb_job_t *self, lh_u32_t owner)
{
    lh_assert_runtime_ref(self);
    self->owner = owner;
}

lh_u32_t
lh_mb_job_get_ticket(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->ticket;
}

lh_void
lh_mb_job_set_ticket(lh_mb_job_t *self, lh_u32_t ticket)
{
    lh_assert_runtime_ref(self);
    self->ticket = ticket;
}

lh_u8_t
lh_mb_job_get_unit(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->unit;
}

lh_void
lh_mb_job_set_unit(lh_mb_job_t *self, lh_u8_t unit)
{
    lh_assert_runtime_ref(self);
    self->unit = unit;
}

lh_u8_t
lh_mb_job_get_fc(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->fc;
}

lh_void
lh_mb_job_set_fc(lh_mb_job_t *self, lh_u8_t fc)
{
    lh_assert_runtime_ref(self);
    self->fc = fc;
}

lh_u16_t
lh_mb_job_get_address(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->address;
}

lh_void
lh_mb_job_set_address(lh_mb_job_t *self, lh_u16_t address)
{
    lh_assert_runtime_ref(self);
    self->address = address;
}

lh_u16_t
lh_mb_job_get_count(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_mb_job_set_count(lh_mb_job_t *self, lh_u16_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

const lh_u16_t *
lh_mb_job_get_values(const lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->values;
}

lh_u16_t *
lh_mb_job_get_values_mut(lh_mb_job_t *self)
{
    lh_assert_runtime_ref(self);
    return self->values;
}

lh_bool_t
lh_mb_job_is_write(const lh_mb_job_t *self)
{
    const lh_u8_t fc = lh_mb_job_get_fc(self);

    return lh_cast_static(lh_bool_t, (fc == LH_MB_FC_WRITE_ONE || fc == LH_MB_FC_WRITE_MANY));
}

lh_void
lh_mb_job_init_read(lh_mb_job_t *self, lh_u8_t unit, lh_u8_t fc, lh_u16_t address, lh_u16_t count)
{
    lh_mb_job_set_owner(self, 0);
    lh_mb_job_set_ticket(self, 0);
    lh_mb_job_set_unit(self, unit);
    lh_mb_job_set_fc(self, fc);
    lh_mb_job_set_address(self, address);
    lh_mb_job_set_count(self, count);
}

lh_void
lh_mb_job_init_write(lh_mb_job_t *self, lh_u8_t unit, lh_u8_t fc, lh_u16_t address,
                     const lh_u16_t *values, lh_u16_t count)
{
    lh_assert_runtime_ref(values);
    const lh_u16_t kept = lh_math_min(count, LH_MB_WRITE_MAX);

    lh_mb_job_init_read(self, unit, fc, address, kept);
    lh_memory_std_copy(lh_mb_job_get_values_mut(self), values,
                       lh_cast_static(lh_usize_t, kept * sizeof(lh_u16_t)));
}
