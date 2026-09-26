#include <lh/os/system/timestamp.h>
#include <lh/assert.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "filetime.h"

lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out)
{
    FILETIME ft;

    lh_assert_runtime_ref(out);
    GetSystemTimeAsFileTime(&ft);
    *out = lh_os_system_timestamp_from_filetime(&ft);
    return lh_bool_true;
}
