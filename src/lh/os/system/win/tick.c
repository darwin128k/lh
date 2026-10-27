#include <lh/os/system/tick.h>
#include <lh/os/system/error/capture.h>
#include <windows.h>

lh_bool_t
lh_os_system_tick_us(lh_u64_t *out)
{
    if (!out)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    LARGE_INTEGER counter, frequency;
    if (!QueryPerformanceCounter(&counter) || !QueryPerformanceFrequency(&frequency))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    if (counter.QuadPart < 0 || frequency.QuadPart <= 0)
    {
        SetLastError(ERROR_INVALID_DATA);
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    /* Divide first, avoiding the old uptime * 1e6 overflow. No shared cache. */
    *out =
        (lh_u64_t)(counter.QuadPart / frequency.QuadPart) * 1000000U +
        (lh_u64_t)(counter.QuadPart % frequency.QuadPart) * 1000000U / (lh_u64_t)frequency.QuadPart;
    return lh_bool_true;
}
lh_bool_t
lh_os_system_sleep_ms(lh_u32_t milliseconds)
{
    /* MAXDWORD means INFINITE to Sleep, not a finite interval. */
    if (milliseconds == MAXDWORD)
    {
        Sleep(MAXDWORD - 1);
        Sleep(1);
    }
    else
    {
        Sleep(milliseconds);
    }
    return lh_bool_true;
}
