#include <lh/os/clock.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <time.h>
#endif

lh_u64_t
lh_os_clock_ms(void)
{
#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    return GetTickCount64();
#else
    {
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC, &ts);
        return lh_cast_static(lh_u64_t, ts.tv_sec) * 1000ULL +
               lh_cast_static(lh_u64_t, ts.tv_nsec) / 1000000ULL;
    }
#endif
}
