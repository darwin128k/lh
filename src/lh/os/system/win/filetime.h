/**
 * @file filetime.h
 * @brief Backend-private: Win32 `FILETIME` -> ::lh_timestamp_t.
 *
 * Shared by `win/timestamp.c` (the clock) and `win/fs/stat.c` (file times)
 * so both read a `FILETIME` the same way. Not installed, not part of the
 * API. The includer must already have `<windows.h>` in scope.
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_FILETIME_H
#define LH_SRC_OS_SYSTEM_WIN_FILETIME_H

#include <lh/attribute/static.h>
#include <lh/cast/static.h>
#include <lh/numeric/fixed/types.h>
#include <lh/timestamp.h>
#include <lh/util/bit/half.h>
#include <lh/util/math.h>
#include <lh/util/math/floor.h>

/* 100-ns intervals between the FILETIME epoch (1601-01-01) and the Unix
   epoch (1970-01-01) - a fixed, well-known constant. */
#define LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH 116444736000000000LL

/* FILETIME ticks per second (100 ns each). */
#define LH_OS_SYSTEM_WIN_FILETIME_HZ 10000000LL

/* Seconds since the Unix epoch of @p time. A FILETIME before 1970 gives a
   negative timestamp, floored to whole seconds; every valid FILETIME
   (below 2^63 ticks) fits the signed arithmetic. */
LH_ATTRIBUTE_STATIC
lh_timestamp_t
lh_os_system_timestamp_from_filetime(const FILETIME *time)
{
    const lh_u64_t ticks = lh_bit_make_u64(lh_cast_static(lh_u32_t, time->dwHighDateTime),
                                           lh_cast_static(lh_u32_t, time->dwLowDateTime));

    return lh_math_floor_div(
        lh_math_sub(lh_cast_static(lh_s64_t, ticks), LH_OS_SYSTEM_WIN_FILETIME_UNIX_EPOCH),
        LH_OS_SYSTEM_WIN_FILETIME_HZ);
}

#endif /* LH_SRC_OS_SYSTEM_WIN_FILETIME_H */
