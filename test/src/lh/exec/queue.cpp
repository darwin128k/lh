#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/exec/queue.h>
#include <lh/exec/task/initializer.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{

void
no_work(lh_ptr user, lh_ptr local)
{
    (void)user;
    (void)local;
}

/* A task that says who pushed it and in which order. */
struct item
{
    lh_exec_task_t task;
    int producer;
    int sequence;
};

item
make_item(int producer, int sequence)
{
    item it{lh_exec_task_initializer(no_work, lh_null, lh_null, lh_null), producer, sequence};
    return it;
}

TEST(exec_queue_init, starts_empty)
{
    lh_exec_queue_t queue;
    lh_exec_queue_init(&queue);
    EXPECT_EQ(lh_exec_queue_try_pop(&queue), nullptr);
    EXPECT_EQ(lh_exec_queue_get_front(&queue), lh_exec_queue_get_stub_mut(&queue));
}

TEST(exec_queue_try_pop, returns_tasks_first_in_first_out)
{
    lh_exec_queue_t queue;
    lh_exec_queue_init(&queue);
    item items[5] = {make_item(0, 0), make_item(0, 1), make_item(0, 2), make_item(0, 3),
                     make_item(0, 4)};
    for (auto &it : items)
    {
        lh_exec_queue_push(&queue, &it.task);
    }
    for (auto &it : items)
    {
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), &it.task);
    }
    EXPECT_EQ(lh_exec_queue_try_pop(&queue), nullptr);
}

/* The stub goes round and round: the queue drains and fills many times. */
TEST(exec_queue_try_pop, survives_draining_again_and_again)
{
    lh_exec_queue_t queue;
    lh_exec_queue_init(&queue);
    item a = make_item(0, 0);
    item b = make_item(0, 1);
    for (int round = 0; round < 100; ++round)
    {
        lh_exec_queue_push(&queue, &a.task);
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), &a.task);
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), nullptr);
        lh_exec_queue_push(&queue, &a.task);
        lh_exec_queue_push(&queue, &b.task);
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), &a.task);
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), &b.task);
        EXPECT_EQ(lh_exec_queue_try_pop(&queue), nullptr);
    }
}

/* Many producers, one consumer: every task comes out exactly once, and each
   producer's tasks come out in that producer's order. */
TEST(exec_queue_push, many_producers_one_consumer)
{
    constexpr int producers = 4;
    constexpr int per_producer = 20000;
    std::vector<item> items;
    items.reserve(producers * per_producer);
    for (int p = 0; p < producers; ++p)
    {
        for (int i = 0; i < per_producer; ++i)
        {
            items.push_back(make_item(p, i));
        }
    }
    lh_exec_queue_t queue;
    lh_exec_queue_init(&queue);
    std::vector<std::thread> threads;
    for (int p = 0; p < producers; ++p)
    {
        threads.emplace_back(
            [&, p]
            {
                for (int i = 0; i < per_producer; ++i)
                {
                    lh_exec_queue_push(&queue, &items[p * per_producer + i].task);
                }
            });
    }
    int next_of[producers] = {0, 0, 0, 0};
    int taken = 0;
    bool in_order = true;
    while (taken < producers * per_producer)
    {
        lh_exec_task_t *task = lh_exec_queue_try_pop(&queue);
        if (task == nullptr)
        {
            std::this_thread::yield();
            continue;
        }
        const item *it = reinterpret_cast<const item *>(task);
        in_order = in_order && it->sequence == next_of[it->producer];
        next_of[it->producer] = it->sequence + 1;
        ++taken;
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    EXPECT_TRUE(in_order);
    EXPECT_EQ(lh_exec_queue_try_pop(&queue), nullptr);
    for (int p = 0; p < producers; ++p)
    {
        EXPECT_EQ(next_of[p], per_producer);
    }
}

TEST(exec_queue_death, rejects_null_queue)
{
    LH_EXPECT_DEATH(lh_exec_queue_try_pop(lh_null));
    LH_EXPECT_DEATH(lh_exec_queue_get_front(lh_null));
}

} // namespace
