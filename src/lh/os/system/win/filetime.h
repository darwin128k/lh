/**
 * @file filetime.h
 * @brief Backend-private: Win32 `FILETIME` -> ::lh_timestamp_t.
 *
 * Shared by `win/timestamp.c` (the clock) and `win/fs/stat.c` (file times)
 * so both read a `FILETIME` the same way. Not installed, not part of the
 * API.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_FILETIME_H
#define LH_SRC_OS_SYSTEM_WIN_FILETIME_H

#include <lh/attribute/static.h>
#include <lh/os/system/win/kernel32.h>
#include <lh/timestamp.h>
#include <lh/bit/half.h>
#include <lh/math.h>

/** 100-ns intervals between the FILETIME epoch (1601-01-01) and the Unix
   epoch (1970-01-01) - a fixed, well-known constant. */
#define LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH 116444736000000000LL

/** FILETIME ticks per second (100 ns each). */
#define LH_OS_SYSTEM_WIN_FILETIME_HZ 10000000LL

LH_COMPILER_EXTERN_C_BEGIN

/** Seconds since the Unix epoch of @p time. A FILETIME before 1970 gives a
   negative timestamp, floored to whole seconds; every valid FILETIME
   (below 2^63 ticks) fits the signed arithmetic. */
LH_ATTRIBUTE_STATIC
lh_timestamp_t
lh_os_system_timestamp_from_filetime(const lh_os_system_win_filetime_t *time)
{
    const lh_u64_t ticks = lh_bit_make_u64(lh_cast_static(lh_u32_t, time->dwHighDateTime),
                                           lh_cast_static(lh_u32_t, time->dwLowDateTime));

    return lh_math_floor_div(
        lh_math_sub(lh_cast_static(lh_s64_t, ticks), LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH),
        LH_OS_SYSTEM_WIN_FILETIME_HZ);
}

/** The `FILETIME` of @p self, the inverse of ::lh_os_system_timestamp_from_filetime.
   False, with @p out untouched, for a moment before 1601-01-01. */
LH_ATTRIBUTE_STATIC
lh_bool_t
lh_os_system_timestamp_to_filetime(lh_timestamp_t self, lh_os_system_win_filetime_t *out)
{
    const lh_s64_t seconds_before = lh_math_floor_div(-LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH,
                                                      LH_OS_SYSTEM_WIN_FILETIME_HZ);
    lh_u64_t ticks;

    if (self < seconds_before)
    {
        return lh_bool_false;
    }
    ticks = lh_cast_static(lh_u64_t, lh_math_add(lh_math_mul(self, LH_OS_SYSTEM_WIN_FILETIME_HZ),
                                                 LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH));
    out->dwLowDateTime = lh_cast_static(lh_os_system_win_dword_t, ticks);
    out->dwHighDateTime = lh_cast_static(lh_os_system_win_dword_t, ticks >> 32);
    return lh_bool_true;
}

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_FILETIME_H */
