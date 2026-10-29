#include <gtest/gtest.h>

#include <lh/attribute/unused.h>
#include <lh/os/thread/group.h>

#include <atomic>

namespace
{

/* A thread that runs until told to stop, and one that waits for another to end. */
struct job
{
    std::atomic<bool> stop{false};
    std::atomic<bool> running{false};
    std::atomic<int> wakes{0};
    job *after = nullptr;
};

lh_void
run(lh_ptr user)
{
    auto *self = static_cast<job *>(user);

    self->running.store(true);
    while (!self->stop.load())
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    /* Ends only once the other one has been told to stop too. */
    while (self->after != nullptr && !self->after->stop.load())
    {
        LH_ATTRIBUTE_UNUSED(lh_os_thread_sleep(1U));
    }
    self->running.store(false);
}

lh_void
wake(lh_ptr user)
{
    auto *self = static_cast<job *>(user);

    self->wakes.fetch_add(1);
    self->stop.store(true);
}

} // namespace

TEST(os_thread_group, stop_wakes_joins_and_frees_one_thread)
{
    lh_os_thread_group_t group;
    job a;

    lh_os_thread_group_init(&group);
    lh_os_thread_group_member_t *member =
        lh_os_thread_group_try_start(&group, run, wake, &a, -1, 0U);
    ASSERT_NE(member, nullptr);
    EXPECT_EQ(lh_os_thread_group_get_count(&group), 1U);
    EXPECT_TRUE(lh_os_thread_group_stop(&group, member));
    EXPECT_FALSE(a.running.load());
    EXPECT_EQ(a.wakes.load(), 1);
    EXPECT_EQ(lh_os_thread_group_get_count(&group), 0U);
    lh_os_thread_group_deinit(&group);
}

TEST(os_thread_group, stop_all_wakes_every_thread_before_joining)
{
    lh_os_thread_group_t group;
    job a;
    job b;

    /* a waits for b's stop: joining a before waking b would hang. */
    a.after = &b;
    lh_os_thread_group_init(&group);
    ASSERT_NE(lh_os_thread_group_try_start(&group, run, wake, &a, -1, 0U), nullptr);
    ASSERT_NE(lh_os_thread_group_try_start(&group, run, wake, &b, -1, 0U), nullptr);
    lh_os_thread_group_stop_all(&group);
    EXPECT_FALSE(a.running.load());
    EXPECT_FALSE(b.running.load());
    EXPECT_EQ(a.wakes.load(), 1);
    EXPECT_EQ(b.wakes.load(), 1);
    /* Closed: nothing more starts. */
    EXPECT_EQ(lh_os_thread_group_try_start(&group, run, wake, &a, -1, 0U), nullptr);
    lh_os_thread_group_deinit(&group);
    EXPECT_EQ(a.wakes.load(), 1);
}

TEST(os_thread_group, bad_arguments_and_strangers_are_refused)
{
    lh_os_thread_group_t group;
    lh_os_thread_group_t other;
    job a;

    lh_os_thread_group_init(&group);
    lh_os_thread_group_init(&other);
    EXPECT_EQ(lh_os_thread_group_try_start(&group, nullptr, wake, &a, -1, 0U), nullptr);
    EXPECT_EQ(lh_os_thread_group_try_start(&group, run, nullptr, &a, -1, 0U), nullptr);
    lh_os_thread_group_member_t *member =
        lh_os_thread_group_try_start(&group, run, wake, &a, -1, 0U);
    ASSERT_NE(member, nullptr);
    EXPECT_FALSE(lh_os_thread_group_stop(&other, member));
    EXPECT_FALSE(lh_os_thread_group_stop(&group, nullptr));
    lh_os_thread_group_deinit(&other);
    lh_os_thread_group_deinit(&group);
    EXPECT_EQ(a.wakes.load(), 1);
}
