#include <lh/os/datetime.h>
#include <lh/assert.h>
#include <lh/cast/static.h>
#include <lh/compiler/os.h>
#include <lh/date.h>
#include <lh/null.h>
#include <lh/numeric/types.h>
#include <lh/os.h>
#include <lh/time.h>
#include <lh/util/addr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#else
#    include <time.h>
#endif

static void
lh_os_datetime_fill(lh_datetime_t *out, lh_date_year_t year, lh_date_month_t month,
                    lh_date_day_t day, lh_time_hour_t hour, lh_time_minute_t minute,
                    lh_uint_t second)
{
    lh_date_t date;
    lh_time_t tod;
    lh_time_second_t sec;

    sec = second > LH_TIME_SECOND_MAX ? LH_TIME_SECOND_MAX : lh_cast_static(lh_time_second_t, second);
    lh_date_set(lh_addr_of(date), year, month, day);
    lh_time_set(lh_addr_of(tod), hour, minute, sec);
    lh_datetime_set(out, lh_addr_of(date), lh_addr_of(tod));
}

lh_bool_t
lh_os_datetime_now(lh_datetime_t *out)
{
    lh_assert_runtime_ref(out);

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS
    {
        SYSTEMTIME local;

        GetLocalTime(lh_addr_of(local));
        lh_os_datetime_fill(out, lh_cast_static(lh_date_year_t, local.wYear),
                            lh_cast_static(lh_date_month_t, local.wMonth),
                            lh_cast_static(lh_date_day_t, local.wDay),
                            lh_cast_static(lh_time_hour_t, local.wHour),
                            lh_cast_static(lh_time_minute_t, local.wMinute),
                            lh_cast_static(lh_uint_t, local.wSecond));
        return lh_bool_true;
    }
#else
    {
        struct timespec ts;
        struct tm local;

        if (clock_gettime(CLOCK_REALTIME, lh_addr_of(ts)) != 0)
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        if (lh_null_eq(localtime_r(lh_addr_of(ts.tv_sec), lh_addr_of(local))))
        {
            lh_os_capture_last_error();
            return lh_bool_false;
        }
        lh_os_datetime_fill(out, lh_cast_static(lh_date_year_t, local.tm_year + 1900),
                            lh_cast_static(lh_date_month_t, local.tm_mon + 1),
                            lh_cast_static(lh_date_day_t, local.tm_mday),
                            lh_cast_static(lh_time_hour_t, local.tm_hour),
                            lh_cast_static(lh_time_minute_t, local.tm_min),
                            local.tm_sec < 0 ? 0U : lh_cast_static(lh_uint_t, local.tm_sec));
        return lh_bool_true;
    }
#endif
}
