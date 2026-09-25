#include <lh/os/system/timestamp.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/null.h>
#include <lh/os/system/error/capture.h>

#include <time.h>

lh_bool_t
lh_os_system_timestamp_now(lh_timestamp_t *out)
{
    time_t now;

    lh_assert_runtime_ref(out);
    now = time(lh_null);
    if (now == lh_cast_static(time_t, -1))
    {
        lh_os_system_error_capture();
        return lh_bool_false;
    }
    *out = lh_cast_static(lh_timestamp_t, now);
    return lh_bool_true;
}
