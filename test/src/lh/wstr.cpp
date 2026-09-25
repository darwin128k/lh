#include <gtest/gtest.h>

#include <lh/test/alloc_counter.h>
#include <lh/wstr.h>

#include <cwchar>

namespace
{

TEST(wstr_init, allocates_nothing_and_reads_empty)
{
    lh_test::alloc_counter counter;
    lh_wstr_t s;

    lh_wstr_init(&s);
    EXPECT_EQ(counter.count(), 0);
    EXPECT_TRUE(lh_wstr_is_empty(&s));
    EXPECT_EQ(std::wcscmp(lh_wstr_get_data(&s), L""), 0);
    lh_wstr_deinit(&s);
    EXPECT_EQ(counter.count(), 0);
}

TEST(wstr_bufferless, readers_and_resets_work_before_first_grow)
{
    lh_wstr_t s;
    lh_wstr_t other;

    lh_wstr_init(&s);
    lh_wstr_init(&other);
    lh_wstr_clear(&s);
    lh_wstr_truncate(&s, 0);
    lh_wstr_view_t view = lh_wstr_as_view(&s);
    EXPECT_TRUE(lh_wstr_view_is_empty(&view));
    lh_wstr_assign(&s, &other);
    lh_wstr_append_str(&s, &other);
    EXPECT_EQ(std::wcscmp(lh_wstr_get_data(&s), L""), 0);
    lh_wstr_deinit(&other);
    lh_wstr_deinit(&s);
}

TEST(wstr_append, grows_once_and_stays_terminated)
{
    lh_test::alloc_counter counter;
    lh_wstr_t s;

    lh_wstr_init(&s);
    lh_wstr_append(&s, L"\x0436\x0443\x043a", 3);
    EXPECT_EQ(counter.count(), 1);
    EXPECT_EQ(lh_wstr_get_size(&s), 3u);
    EXPECT_EQ(std::wcscmp(lh_wstr_get_data(&s), L"\x0436\x0443\x043a"), 0);
    lh_wstr_deinit(&s);
}

TEST(wstr_truncate, shrinks_and_stays_terminated)
{
    lh_wstr_t s;

    lh_wstr_init(&s);
    lh_wstr_append(&s, L"abcdef", 6);
    lh_wstr_truncate(&s, 2);
    EXPECT_EQ(lh_wstr_get_size(&s), 2u);
    EXPECT_EQ(std::wcscmp(lh_wstr_get_data(&s), L"ab"), 0);
    lh_wstr_deinit(&s);
}

} // namespace
