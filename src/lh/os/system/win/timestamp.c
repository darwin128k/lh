#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/os/system/win/filetime.h>

lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out)
{
    lh_os_system_win_filetime_t ft;

    lh_assert_runtime_ref(out);
    GetSystemTimeAsFileTime(lh_addr_of(ft));
    lh_ptr_deref(out) = lh_os_system_timestamp_from_filetime(lh_addr_of(ft));
    return lh_bool_true;
}
