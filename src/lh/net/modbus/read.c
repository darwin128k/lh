#include <lh/net/modbus/read.h>
#include <lh/assert.h>

lh_u16_t
lh_mb_read_get_start(const lh_mb_read_t *self)
{
    lh_assert_runtime_ref(self);
    return self->start;
}

lh_void
lh_mb_read_set_start(lh_mb_read_t *self, lh_u16_t start)
{
    lh_assert_runtime_ref(self);
    self->start = start;
}

lh_u16_t
lh_mb_read_get_count(const lh_mb_read_t *self)
{
    lh_assert_runtime_ref(self);
    return self->count;
}

lh_void
lh_mb_read_set_count(lh_mb_read_t *self, lh_u16_t count)
{
    lh_assert_runtime_ref(self);
    self->count = count;
}

lh_u16_t
lh_mb_read_get_first(const lh_mb_read_t *self)
{
    lh_assert_runtime_ref(self);
    return self->first;
}

lh_void
lh_mb_read_set_first(lh_mb_read_t *self, lh_u16_t first)
{
    lh_assert_runtime_ref(self);
    self->first = first;
}

lh_u16_t
lh_mb_read_get_last(const lh_mb_read_t *self)
{
    lh_assert_runtime_ref(self);
    return self->last;
}

lh_void
lh_mb_read_set_last(lh_mb_read_t *self, lh_u16_t last)
{
    lh_assert_runtime_ref(self);
    self->last = last;
}
