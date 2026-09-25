#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/util/math.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* 100-ns intervals between the FILETIME epoch (1601-01-01) and the Unix
   epoch (1970-01-01) - a fixed, well-known constant. */
#define LH_OS_SYSTEM_TIMESTAMP_FILETIME_EPOCH_DIFF_100NS 116444736000000000ULL
#define LH_OS_SYSTEM_TIMESTAMP_100NS_PER_SECOND 10000000ULL

lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out)
{
    FILETIME ft;
    lh_u64_t ticks;

    lh_assert_runtime_ref(out);
    GetSystemTimeAsFileTime(&ft);
    ticks = lh_math_bit_or(lh_math_bit_shl(lh_cast_static(lh_u64_t, ft.dwHighDateTime), 32),
                           lh_cast_static(lh_u64_t, ft.dwLowDateTime));
    *out = lh_cast_static(lh_timestamp_t,
                          lh_math_div(lh_math_sub(ticks, LH_OS_SYSTEM_TIMESTAMP_FILETIME_EPOCH_DIFF_100NS),
                                     LH_OS_SYSTEM_TIMESTAMP_100NS_PER_SECOND));
    return lh_bool_true;
}
