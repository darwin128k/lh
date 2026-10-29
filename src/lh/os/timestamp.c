#include <lh/os/timestamp.h>
#include <lh/os/system/timestamp.h>
#include <lh/assert.h>

lh_bool_t
lh_os_timestamp_try_get_now(lh_timestamp_t *out)
{
    return lh_os_system_timestamp_try_get_now(out);
}

lh_bool_t
lh_os_timestamp_try_get_zone_offset(lh_timestamp_t utc, lh_s32_t *out)
{
    return lh_os_system_timestamp_try_get_zone_offset(utc, out);
}

lh_bool_t
lh_os_timestamp_try_get_local(lh_timestamp_t utc, lh_timestamp_t *out)
{
    lh_s32_t offset;

    lh_assert_runtime_ref(out);
    if (!lh_os_timestamp_try_get_zone_offset(utc, lh_addr_of(offset)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_timestamp_add_seconds(utc, offset);
    return lh_bool_true;
}

lh_bool_t
lh_os_timestamp_try_get_utc(lh_timestamp_t local, lh_timestamp_t *out)
{
    lh_s32_t guess;
    lh_s32_t offset;

    lh_assert_runtime_ref(out);
    /* The offset at the wall clock read as UTC is off by at most one change; the
       offset at that first answer is the one in force there. */
    if (!lh_os_timestamp_try_get_zone_offset(local, lh_addr_of(guess)) ||
        !lh_os_timestamp_try_get_zone_offset(lh_timestamp_sub_seconds(local, guess),
                                             lh_addr_of(offset)))
    {
        return lh_bool_false;
    }
    lh_ptr_deref(out) = lh_timestamp_sub_seconds(local, offset);
    return lh_bool_true;
}

lh_bool_t
lh_os_timestamp_try_get_local_now(lh_timestamp_t *out)
{
    lh_timestamp_t now;

    return lh_os_timestamp_try_get_now(lh_addr_of(now)) &&
                   lh_os_timestamp_try_get_local(now, out)
               ? lh_bool_true
               : lh_bool_false;
}
