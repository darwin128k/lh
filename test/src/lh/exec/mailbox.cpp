#include <gtest/gtest.h>

#include <lh/exec/mailbox.h>
#include <lh/exec/task/initializer.h>
#include <lh/expect/death.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{

/* Records the order of complete / destroy calls across tasks. */
struct log
{
    std::vector<int> events;
};

struct item
{
    lh_exec_task_t task;
    log *out;
    int id;
};

void
no_work(lh_ptr user, lh_ptr local)
{
    (void)user;
    (void)local;
}

void
on_complete(lh_ptr user)
{
    auto *it = static_cast<item *>(user);
    it->out->events.push_back(it->id);
}

void
on_destroy(lh_ptr user)
{
    auto *it = static_cast<item *>(user);
    it->out->events.push_back(-it->id);
}

void
make(item &it, log &out, int id)
{
    it.out = &out;
    it.id = id;
    lh_exec_task_init(&it.task, no_work, on_complete, on_destroy, &it);
}

TEST(exec_mailbox_init, starts_empty_with_its_capacity)
{
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 3U);
    EXPECT_EQ(lh_exec_mailbox_get_capacity(&mailbox), 3U);
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
    EXPECT_EQ(lh_exec_mailbox_try_take(&mailbox), nullptr);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 0U);
}

TEST(exec_mailbox_try_reserve, refuses_past_the_capacity)
{
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 2U);
    EXPECT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    EXPECT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    EXPECT_FALSE(lh_exec_mailbox_try_reserve(&mailbox));
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 2U);
    lh_exec_mailbox_release(&mailbox);
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 1U);
    EXPECT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
}

TEST(exec_mailbox_try_reserve, zero_capacity_refuses_everything)
{
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 0U);
    EXPECT_FALSE(lh_exec_mailbox_try_reserve(&mailbox));
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
}

/* Many threads fight for a few places: never more than the capacity taken. */
TEST(exec_mailbox_try_reserve, exact_under_contention)
{
    constexpr unsigned capacity = 5U;
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, capacity);
    std::atomic<unsigned> held{0U};
    std::atomic<bool> passed{false};
    std::vector<std::thread> threads;
    for (int t = 0; t < 4; ++t)
    {
        threads.emplace_back(
            [&]
            {
                for (int i = 0; i < 20000; ++i)
                {
                    if (!lh_exec_mailbox_try_reserve(&mailbox))
                    {
                        continue;
                    }
                    if (held.fetch_add(1U) + 1U > capacity)
                    {
                        passed = true;
                    }
                    held.fetch_sub(1U);
                    lh_exec_mailbox_release(&mailbox);
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    EXPECT_FALSE(passed);
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
}

TEST(exec_mailbox_dispatch, delivers_complete_then_destroy_in_post_order)
{
    log out;
    item a;
    item b;
    make(a, out, 1);
    make(b, out, 2);
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 4U);
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    lh_exec_mailbox_post(&mailbox, &a.task);
    lh_exec_mailbox_post(&mailbox, &b.task);
    EXPECT_TRUE(out.events.empty()); /* nothing behind the owner's back */
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 2U);
    EXPECT_EQ(out.events, (std::vector<int>{1, -1, 2, -2}));
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 0U);
}

TEST(exec_mailbox_deliver, cancelled_task_gets_destroy_only)
{
    log out;
    item a;
    make(a, out, 7);
    lh_exec_task_set_cancelled(&a.task, lh_bool_true);
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 1U);
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    lh_exec_mailbox_post(&mailbox, &a.task);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(out.events, (std::vector<int>{-7}));
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
}

TEST(exec_mailbox_dispatch, stops_when_none_is_ready)
{
    log out;
    item a;
    make(a, out, 1);
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 4U);
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox)); /* a task still running */
    lh_exec_mailbox_post(&mailbox, &a.task);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 1U);
}

/* A complete that posts again waits for the next dispatch. */
struct again
{
    lh_exec_task_t task;
    lh_exec_mailbox_t *mailbox;
    int completes;
};

void
post_again(lh_ptr user)
{
    auto *a = static_cast<again *>(user);
    ++a->completes;
    if (lh_exec_mailbox_try_reserve(a->mailbox))
    {
        lh_exec_mailbox_post(a->mailbox, &a->task);
    }
}

TEST(exec_mailbox_dispatch, bounded_by_the_places_taken_when_it_began)
{
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, 4U);
    again a;
    a.mailbox = &mailbox;
    a.completes = 0;
    lh_exec_task_init(&a.task, no_work, post_again, lh_null, &a);
    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
    lh_exec_mailbox_post(&mailbox, &a.task);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(a.completes, 1);
    EXPECT_EQ(lh_exec_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(a.completes, 2);
}

/* Posts from several threads all reach the owner once. */
TEST(exec_mailbox_post, from_many_threads)
{
    constexpr int threads_count = 4;
    constexpr int per_thread = 5000;
    log out;
    std::vector<item> items(threads_count * per_thread);
    for (int i = 0; i < threads_count * per_thread; ++i)
    {
        make(items[i], out, i + 1);
        lh_exec_task_set_destroy_cb(&items[i].task, lh_null);
    }
    lh_exec_mailbox_t mailbox;
    lh_exec_mailbox_init(&mailbox, threads_count * per_thread);
    std::vector<std::thread> threads;
    for (int t = 0; t < threads_count; ++t)
    {
        threads.emplace_back(
            [&, t]
            {
                for (int i = 0; i < per_thread; ++i)
                {
                    ASSERT_TRUE(lh_exec_mailbox_try_reserve(&mailbox));
                    lh_exec_mailbox_post(&mailbox, &items[t * per_thread + i].task);
                }
            });
    }
    unsigned delivered = 0U;
    while (delivered < static_cast<unsigned>(threads_count * per_thread))
    {
        delivered += lh_exec_mailbox_dispatch(&mailbox);
        std::this_thread::yield();
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    std::vector<bool> seen(threads_count * per_thread + 1, false);
    bool twice = false;
    for (int id : out.events)
    {
        twice = twice || seen[id];
        seen[id] = true;
    }
    EXPECT_FALSE(twice);
    EXPECT_EQ(out.events.size(), static_cast<size_t>(threads_count * per_thread));
    EXPECT_EQ(lh_exec_mailbox_get_used_count(&mailbox), 0U);
}

TEST(exec_mailbox_death, rejects_null_mailbox)
{
    LH_EXPECT_DEATH(lh_exec_mailbox_try_reserve(lh_null));
    LH_EXPECT_DEATH(lh_exec_mailbox_dispatch(lh_null));
    LH_EXPECT_DEATH(lh_exec_mailbox_get_used_count(lh_null));
}

} // namespace
