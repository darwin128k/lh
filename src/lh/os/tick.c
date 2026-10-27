#include <lh/os/tick.h>
#include <lh/os/system/tick.h>

/**
 * @brief Read 64-bit monotonic microseconds.
 */
lh_u64_t
lh_os_tick_us(void)
{
    lh_u64_t value = 0;
    (void)lh_os_system_tick_us(&value);
    return value;
}

/**
 * @brief Read 64-bit monotonic milliseconds.
 */
lh_u64_t
lh_os_tick_ms64(void)
{
    return lh_os_tick_us() / 1000U;
}

/**
 * @brief Read wrapping 32-bit monotonic milliseconds.
 */
lh_tick_t
lh_os_tick_ms(void)
{
    return (lh_tick_t)lh_os_tick_ms64();
}
