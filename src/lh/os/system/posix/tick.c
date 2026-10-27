#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif
#include <lh/os/system/tick.h>
#include <lh/os/system/error/capture.h>
#include <errno.h>
#include <time.h>

lh_bool_t
lh_os_system_tick_us(lh_u64_t *out)
{
    if (!out)
    {
        errno = EINVAL;
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    *out = (lh_u64_t)now.tv_sec * 1000000U + (lh_u64_t)now.tv_nsec / 1000U;
    return lh_bool_true;
}
lh_bool_t
lh_os_system_sleep_ms(lh_u32_t milliseconds)
{
    struct timespec delay = {milliseconds / 1000U, (long)(milliseconds % 1000U) * 1000000L};
    while (nanosleep(&delay, &delay))
    {
        if (errno == EINTR)
        {
            continue;
        }
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    return lh_bool_true;
}
