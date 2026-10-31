#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/os/system/error/capture.h>
#include <lh/os/system/win/filetime.h>

lh_bool_t
lh_os_system_timestamp_try_get_now(lh_timestamp_t *out)
{
    lh_os_system_win_filetime_t ft;

    lh_assert_runtime_ref(out);
    GetSystemTimeAsFileTime(lh_addr_of(ft));
    lh_ptr_deref(out) = lh_os_system_timestamp_from_filetime(lh_addr_of(ft));
    return lh_bool_true;
}

lh_bool_t
lh_os_system_timestamp_try_get_zone_offset(lh_timestamp_t utc, lh_s32_t *out)
{
    lh_os_system_win_filetime_t ft;

    lh_assert_runtime_ref(out);
    lh_os_system_win_systemtime_t universal;

    if (!lh_os_system_timestamp_to_filetime(utc, lh_addr_of(ft)) ||
        !FileTimeToSystemTime(lh_addr_of(ft), lh_addr_of(universal)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_os_system_win_systemtime_t local;

    if (!SystemTimeToTzSpecificLocalTime(lh_null, lh_addr_of(universal), lh_addr_of(local)) ||
        !SystemTimeToFileTime(lh_addr_of(local), lh_addr_of(ft)))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(out) =
        lh_cast_static(lh_s32_t, lh_math_sub(lh_os_system_timestamp_from_filetime(lh_addr_of(ft)),
                                             utc));
    return lh_bool_true;
}
