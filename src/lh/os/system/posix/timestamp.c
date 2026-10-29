/* `tm_gmtoff` is a BSD / glibc member of `struct tm`. */
#if !defined(_DEFAULT_SOURCE)
#    define _DEFAULT_SOURCE 1
#endif

#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/os/system/error/capture.h>

#include <errno.h>
#include <time.h>

lh_bool_t
lh_os_system_timestamp_try_get_now(lh_timestamp_t *out)
{
    lh_assert_runtime_ref(out);
    const time_t now = time(lh_null);
    if (now == lh_cast_static(time_t, -1))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_cast_static(lh_timestamp_t, now);
    return lh_bool_true;
}

lh_bool_t
lh_os_system_timestamp_try_get_zone_offset(lh_timestamp_t utc, lh_s32_t *out)
{
    const time_t moment = lh_cast_static(time_t, utc);
    struct tm local;

    lh_assert_runtime_ref(out);
    /* A 32-bit time_t cannot hold every moment. */
    if (lh_cast_static(lh_timestamp_t, moment) != utc)
    {
        errno = EOVERFLOW;
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    if (localtime_r(lh_addr_of(moment), lh_addr_of(local)) == lh_null)
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_cast_static(lh_s32_t, local.tm_gmtoff);
    return lh_bool_true;
}
