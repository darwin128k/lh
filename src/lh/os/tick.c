/**
 * @file tick.c
 * @brief Implementation of `lh/os/tick.h`.
 */

#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/os/tick.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#    include <lh/os/system/win/kernel32.h>

lh_tick_t
lh_os_tick_ms(void)
{
    /* `GetTickCount` — Windows 95+. (`GetTickCount64` is Vista+.) */
    return lh_cast_static(lh_tick_t, GetTickCount());
}

#elif defined(CLOCK_MONOTONIC)

#    include <lh/numeric/types.h>
#    include <time.h>

lh_tick_t
lh_os_tick_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return lh_cast_static(lh_tick_t, (lh_ullong_t)ts.tv_sec * 1000ull +
                                         (lh_ullong_t)ts.tv_nsec / 1000000ull);
}

#else

lh_tick_t
lh_os_tick_ms(void)
{
    return 0;
}

#endif
