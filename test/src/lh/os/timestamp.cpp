#include <gtest/gtest.h>

#include <lh/os/timestamp.h>

#include <cstdlib>
#include <ctime>
#include <string>

namespace
{

/* 2020-01-01 00:00:00 UTC: any clock that works reads later than this. */
constexpr lh_timestamp_t g_since = 1577836800;

TEST(os_timestamp, now_reads_a_real_time)
{
    lh_timestamp_t now = 0;

    ASSERT_TRUE(lh_os_timestamp_try_get_now(&now));
    EXPECT_GT(now, g_since);
}

TEST(os_timestamp, the_zone_offset_is_a_real_one)
{
    lh_timestamp_t now = 0;
    lh_s32_t offset = 0;

    ASSERT_TRUE(lh_os_timestamp_try_get_now(&now));
    ASSERT_TRUE(lh_os_timestamp_try_get_zone_offset(now, &offset));
    /* UTC-12 .. UTC+14, in whole minutes. */
    EXPECT_GE(offset, -12 * 3600);
    EXPECT_LE(offset, 14 * 3600);
    EXPECT_EQ(offset % 60, 0);
}

TEST(os_timestamp, local_is_utc_plus_the_offset_and_back)
{
    const lh_timestamp_t utc = 1750000000; /* 2025-06-15, far from any change */
    lh_s32_t offset = 0;
    lh_timestamp_t local = 0;
    lh_timestamp_t back = 0;

    ASSERT_TRUE(lh_os_timestamp_try_get_zone_offset(utc, &offset));
    ASSERT_TRUE(lh_os_timestamp_try_get_local(utc, &local));
    EXPECT_EQ(local, utc + offset);
    ASSERT_TRUE(lh_os_timestamp_try_get_utc(local, &back));
    EXPECT_EQ(back, utc);
}

TEST(os_timestamp, local_now_is_now_in_the_local_zone)
{
    lh_timestamp_t local = 0;
    lh_timestamp_t now = 0;
    lh_s32_t offset = 0;

    ASSERT_TRUE(lh_os_timestamp_try_get_local_now(&local));
    ASSERT_TRUE(lh_os_timestamp_try_get_now(&now));
    ASSERT_TRUE(lh_os_timestamp_try_get_zone_offset(now, &offset));
    EXPECT_LE(local, now + offset);
    EXPECT_GE(local, now + offset - 2);
}

#if !defined(_WIN32)
/* POSIX reads the zone from TZ, so a fixed one pins the numbers. */
TEST(os_timestamp, a_fixed_zone_gives_its_offset)
{
    const char *saved = std::getenv("TZ");
    const std::string keep = saved != nullptr ? saved : "";
    lh_s32_t offset = 0;

    setenv("TZ", "XYZ-3", 1);
    tzset();
    EXPECT_TRUE(lh_os_timestamp_try_get_zone_offset(1750000000, &offset));
    EXPECT_EQ(offset, 3 * 3600);
    if (saved != nullptr)
    {
        setenv("TZ", keep.c_str(), 1);
    }
    else
    {
        unsetenv("TZ");
    }
    tzset();
}

TEST(os_timestamp, a_daylight_zone_changes_its_offset_through_the_year)
{
    const char *saved = std::getenv("TZ");
    const std::string keep = saved != nullptr ? saved : "";
    lh_s32_t winter = 0;
    lh_s32_t summer = 0;
    lh_timestamp_t utc = 0;

    /* Central Europe: UTC+1, UTC+2 from the last Sunday of March. */
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    EXPECT_TRUE(lh_os_timestamp_try_get_zone_offset(1736942400, &winter)); /* 2025-01-15 */
    EXPECT_TRUE(lh_os_timestamp_try_get_zone_offset(1752580800, &summer)); /* 2025-07-15 */
    EXPECT_EQ(winter, 3600);
    EXPECT_EQ(summer, 7200);
    /* 2025-03-30 02:30 local is skipped: it is moved forward to 03:30 CEST. */
    EXPECT_TRUE(lh_os_timestamp_try_get_utc(1743301800, &utc));
    EXPECT_EQ(utc, 1743301800 - 3600);
    /* 2025-10-26 02:30 local comes twice: the second time, 02:30 CET. */
    EXPECT_TRUE(lh_os_timestamp_try_get_utc(1761445800, &utc));
    EXPECT_EQ(utc, 1761445800 - 3600);
    if (saved != nullptr)
    {
        setenv("TZ", keep.c_str(), 1);
    }
    else
    {
        unsetenv("TZ");
    }
    tzset();
}
#endif

} // namespace
