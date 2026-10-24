#include <gtest/gtest.h>

#include <cstring>

#include <lh/os/memory.h>

namespace
{

/* A page of our own, so changing its protection touches nothing else. */
alignas(4096) unsigned char g_page[4096];

TEST(os_memory, protect_changes_and_restores_access)
{
    ASSERT_TRUE(lh_os_memory_protect(g_page, sizeof g_page, lh_os_memory_access_read));
    EXPECT_EQ(g_page[0], 0U);
    ASSERT_TRUE(lh_os_memory_protect(g_page, sizeof g_page,
                                     lh_os_memory_access_read | lh_os_memory_access_write |
                                         lh_os_memory_access_execute));
    std::memset(g_page, 0x5A, 16);
    EXPECT_EQ(g_page[15], 0x5AU);
    ASSERT_TRUE(lh_os_memory_protect(g_page, sizeof g_page,
                                     lh_os_memory_access_read | lh_os_memory_access_write));
    g_page[0] = 0U;
}

TEST(os_memory, protect_widens_an_unaligned_range_to_its_pages)
{
    EXPECT_TRUE(lh_os_memory_protect(g_page + 3, 5U,
                                     lh_os_memory_access_read | lh_os_memory_access_write));
    g_page[3] = 1U;
    EXPECT_EQ(g_page[3], 1U);
}

TEST(os_memory, protect_rejects_an_empty_range)
{
    EXPECT_FALSE(lh_os_memory_protect(nullptr, 16U, lh_os_memory_access_read));
    EXPECT_FALSE(lh_os_memory_protect(g_page, 0U, lh_os_memory_access_read));
}

} // namespace
