/**
 * @file tick.c
 * @brief Implementation of `lh/os/tick.h`.
 */

#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/os/tick.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#    include <lh/os/system/win/kernel32.h>
#    include <lh/util/addr.h>

lh_tick_t
lh_os_tick_ms(void)
{
    /* `GetTickCount` — Windows 95+. (`GetTickCount64` is Vista+.) */
    return lh_cast_static(lh_tick_t, GetTickCount());
}

lh_u64_t
lh_os_tick_us(void)
{
    static lh_s64_t frequency = 0;
    lh_os_system_win_large_integer_t counter;
    lh_os_system_win_large_integer_t freq;

    if (frequency == 0)
    {
        if (QueryPerformanceFrequency(lh_addr_of(freq)) == 0 || freq.QuadPart == 0)
        {
            return 0;
        }
        frequency = freq.QuadPart;
    }
    if (QueryPerformanceCounter(lh_addr_of(counter)) == 0)
    {
        return 0;
    }
    /* counter * 1e6 / frequency, in 64-bit arithmetic. */
    return lh_cast_static(lh_u64_t, (counter.QuadPart * 1000000LL) / frequency);
}

#elif LH_COMPILER_OS == LH_COMPILER_OS_LINUX || LH_COMPILER_OS == LH_COMPILER_OS_MAC

/* `<time.h>` first, and outside any branch that tests what it defines.
   CLOCK_MONOTONIC comes from it, and testing a macro whose header has not been
   read yet is always false — which is how this file shipped its `return 0` stub to
   every POSIX clock. Nothing above the clock can tell a clock that reads zero from
   a very fast one: every timer on it reported that it worked. */
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

lh_u64_t
lh_os_tick_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return lh_cast_static(lh_u64_t, (lh_ullong_t)ts.tv_sec * 1000000ull +
                                        (lh_ullong_t)ts.tv_nsec / 1000ull);
}

#else

/* Nothing to ask, and no header that would answer. A freestanding build that
   needs a clock brings its own. */
lh_tick_t
lh_os_tick_ms(void)
{
    return 0;
}

lh_u64_t
lh_os_tick_us(void)
{
    return 0;
}

#endif /* LH_COMPILER_OS */