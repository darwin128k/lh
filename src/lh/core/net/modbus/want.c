#include <lh/core/net/modbus/want.h>
#include <lh/core/assert.h>

lh_u16_t
lh_mb_want_get_address(const lh_mb_want_t *self)
{
    lh_assert_runtime_ref(self);
    return self->address;
}

lh_void
lh_mb_want_set_address(lh_mb_want_t *self, lh_u16_t address)
{
    lh_assert_runtime_ref(self);
    self->address = address;
}

lh_u16_t
lh_mb_want_get_registers(const lh_mb_want_t *self)
{
    lh_assert_runtime_ref(self);
    return self->registers;
}

lh_void
lh_mb_want_set_registers(lh_mb_want_t *self, lh_u16_t registers)
{
    lh_assert_runtime_ref(self);
    self->registers = registers;
}

lh_u32_t
lh_mb_want_get_end(const lh_mb_want_t *self)
{
    return lh_cast_static(lh_u32_t, lh_mb_want_get_address(self)) +
           lh_math_max(lh_cast_static(lh_u32_t, lh_mb_want_get_registers(self)), 1U);
}
