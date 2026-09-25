#include <lh/os/timestamp.h>
#include <lh/os/system/timestamp.h>

lh_bool_t
lh_os_timestamp_now(lh_timestamp_t *out)
{
    return lh_os_system_timestamp_now(out);
}
