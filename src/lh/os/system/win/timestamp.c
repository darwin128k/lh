#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/os/system/win/filetime.h>
#include <lh/os/system/win/kernel32.h>

lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out)
{
    lh_os_system_win_filetime_t ft;

    lh_assert_runtime_ref(out);
    GetSystemTimeAsFileTime(&ft);
    *out = lh_os_system_timestamp_from_filetime(&ft);
    return lh_bool_true;
}
