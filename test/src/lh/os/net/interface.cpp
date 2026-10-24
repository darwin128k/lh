#include <gtest/gtest.h>

#include <lh/os/net/interface.h>

namespace
{

struct seen
{
    unsigned count;
    bool loopback;
};

void
note(lh_ptr user, const lh_net_ip4_t *ip)
{
    seen *const s = static_cast<seen *>(user);
    ++s->count;
    if (lh_net_ip4_is_loopback(ip))
    {
        s->loopback = true;
    }
}

TEST(os_net_interface, each_ip4_lists_the_loopback)
{
    seen s = {0U, false};

    ASSERT_TRUE(lh_os_net_interface_each_ip4(note, &s));
    EXPECT_GT(s.count, 0U);
    EXPECT_TRUE(s.loopback);
}

TEST(os_net_interface, each_ip4_needs_a_callback)
{
    EXPECT_FALSE(lh_os_net_interface_each_ip4(nullptr, nullptr));
}

} // namespace
