/**
 * @file tick.c
 * @brief Implementation of `lh/os/tick.h`.
 */

#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/os/tick.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <windows.h>

lh_tick_t
lh_os_tick_ms(void)
{
    return lh_cast_static(lh_tick_t, GetTickCount64());
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

#    include <lh/null.h>
#    include <lh/numeric/types.h>
#    include <time.h>

lh_tick_t
lh_os_tick_ms(void)
{
    /* Fallback: coarse wall clock in ms (wraps like lh_tick_t). */
    return lh_cast_static(lh_tick_t, (lh_ullong_t)time(lh_null) * 1000ull);
}

#endif
