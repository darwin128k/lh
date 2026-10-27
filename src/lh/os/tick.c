#include <lh/os/tick.h>
#include <lh/os/system/tick.h>

lh_u64_t
lh_os_tick_us(void)
{
    lh_u64_t value = 0;
    (void)lh_os_system_tick_us(&value);
    return value;
}

lh_u64_t
lh_os_tick_ms64(void)
{
    return lh_os_tick_us() / 1000U;
}

lh_tick_t
lh_os_tick_ms(void)
{
    return (lh_tick_t)lh_os_tick_ms64();
}
