#include <gtest/gtest.h>

#include <lh/rail/base/initializer.h>
#include <lh/rail/context/initializer.h>
#include <lh/rail/runtime/base.h>
#include <lh/rail/thread.h>
#include <lh/expect/death.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace
{

/* Shared record of what happened, and where. */
struct trace
{
    std::thread::id open_on;
    std::thread::id close_on;
    lh_ptr closed_local = nullptr;
    int opens = 0;
    int closes = 0;
};

int g_local_value = 0;

lh_ptr
trace_open(lh_ptr user)
{
    auto *t = static_cast<trace *>(user);
    t->open_on = std::this_thread::get_id();
    ++t->opens;
    return &g_local_value;
}

void
trace_close(lh_ptr user, lh_ptr local)
{
    auto *t = static_cast<trace *>(user);
    t->close_on = std::this_thread::get_id();
    t->closed_local = local;
    ++t->closes;
}

/* One task: optionally waits on a gate, records order and thread. */
struct job
{
    lh_rail_task_t task;
    int id = 0;
    std::atomic<bool> *gate = nullptr;
    std::atomic<bool> started{false};
    std::vector<int> *order = nullptr;
    std::thread::id ran_on;
    lh_ptr local = nullptr;
    int completes = 0;
    int destroys = 0;
};

void
job_work(lh_ptr user, lh_ptr local)
{
    auto *j = static_cast<job *>(user);
    j->started = true;
    if (j->gate != nullptr)
    {
        while (!j->gate->load())
        {
            std::this_thread::yield();
        }
    }
    j->ran_on = std::this_thread::get_id();
    j->local = local;
    if (j->order != nullptr)
    {
        j->order->push_back(j->id); /* only the executor's thread writes it */
    }
}

void
job_complete(lh_ptr user)
{
    ++static_cast<job *>(user)->completes;
}

void
job_destroy(lh_ptr user)
{
    ++static_cast<job *>(user)->destroys;
}

void
make(job &j, int id)
{
    j.id = id;
    lh_rail_task_init(&j.task, job_work, job_complete, job_destroy, &j);
}

/* Dispatch until @p count tasks were delivered (or a generous time ran out). */
void
dispatch_all(lh_rail_mailbox_t *mailbox, unsigned count)
{
    unsigned delivered = 0U;
    auto until = std::chrono::steady_clock::now() + std::chrono::seconds(20);
    while (delivered < count && std::chrono::steady_clock::now() < until)
    {
        delivered += lh_rail_mailbox_dispatch(mailbox);
        std::this_thread::yield();
    }
    ASSERT_EQ(delivered, count);
}

TEST(rail_thread_try_start, context_opens_and_closes_on_the_thread)
{
    trace t;
    lh_rail_context_t context = lh_rail_context_initializer(trace_open, trace_close, &t);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 4U);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    job j;
    make(j, 1);
    ASSERT_TRUE(
        lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor), &j.task, &mailbox));
    dispatch_all(&mailbox, 1U);
    EXPECT_TRUE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_drain, 10000U));
    lh_rail_thread_destroy(&executor);
    EXPECT_EQ(t.opens, 1);
    EXPECT_EQ(t.closes, 1);
    EXPECT_NE(t.open_on, std::this_thread::get_id());
    EXPECT_EQ(j.ran_on, t.open_on);
    EXPECT_EQ(t.close_on, t.open_on);
    EXPECT_EQ(j.local, &g_local_value);
    EXPECT_EQ(t.closed_local, &g_local_value);
    EXPECT_EQ(j.completes, 1);
    EXPECT_EQ(j.destroys, 1);
}

TEST(rail_thread_submit, tasks_run_in_submission_order)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    constexpr int count = 200;
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, count);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    std::vector<int> order;
    std::vector<job> jobs(count);
    for (int i = 0; i < count; ++i)
    {
        make(jobs[i], i);
        jobs[i].order = &order;
        ASSERT_TRUE(lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor),
                                                &jobs[i].task, &mailbox));
    }
    dispatch_all(&mailbox, count);
    lh_rail_thread_destroy(&executor);
    ASSERT_EQ(order.size(), static_cast<size_t>(count));
    for (int i = 0; i < count; ++i)
    {
        EXPECT_EQ(order[i], i);
        EXPECT_EQ(jobs[i].completes, 1);
        EXPECT_EQ(jobs[i].destroys, 1);
        EXPECT_EQ(jobs[i].local, nullptr);
    }
}

/* Drain: everything accepted runs, then the thread ends; new tasks are refused. */
TEST(rail_thread_try_stop, drain_runs_every_accepted_task)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    constexpr int count = 50;
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, count + 1);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    std::vector<job> jobs(count);
    for (int i = 0; i < count; ++i)
    {
        make(jobs[i], i);
        ASSERT_TRUE(lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor),
                                                &jobs[i].task, &mailbox));
    }
    EXPECT_TRUE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_drain, 10000U));
    job late;
    make(late, 99);
    EXPECT_FALSE(
        lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor), &late.task, &mailbox));
    EXPECT_EQ(lh_rail_mailbox_get_used_count(&mailbox), static_cast<unsigned>(count));
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), static_cast<unsigned>(count));
    lh_rail_thread_destroy(&executor);
    for (auto &j : jobs)
    {
        EXPECT_EQ(j.completes, 1);
        EXPECT_EQ(j.destroys, 1);
    }
    EXPECT_EQ(late.destroys, 0);
}

/* Cancel: the running task finishes, the rest get destroy only — at dispatch. */
TEST(rail_thread_try_stop, cancel_drops_what_has_not_started)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    constexpr int count = 10;
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, count);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    std::atomic<bool> gate{false};
    std::vector<job> jobs(count);
    for (int i = 0; i < count; ++i)
    {
        make(jobs[i], i);
    }
    jobs[0].gate = &gate;
    for (auto &j : jobs)
    {
        ASSERT_TRUE(
            lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor), &j.task, &mailbox));
    }
    while (!jobs[0].started)
    {
        std::this_thread::yield();
    }
    /* The first task is stuck: the stop cannot end yet. */
    EXPECT_FALSE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_cancel, 0U));
    EXPECT_FALSE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_cancel, 30U));
    gate = true;
    EXPECT_TRUE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_cancel, 10000U));
    /* Nothing was delivered behind the owner's back. */
    for (auto &j : jobs)
    {
        EXPECT_EQ(j.completes + j.destroys, 0);
    }
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), static_cast<unsigned>(count));
    lh_rail_thread_destroy(&executor);
    EXPECT_EQ(jobs[0].completes, 1);
    for (int i = 1; i < count; ++i)
    {
        EXPECT_FALSE(jobs[i].started);
        EXPECT_EQ(jobs[i].completes, 0);
    }
    for (auto &j : jobs)
    {
        EXPECT_EQ(j.destroys, 1);
    }
}

/* A drain may become a cancel. */
TEST(rail_thread_try_stop, drain_turns_into_cancel)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 2U);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    std::atomic<bool> gate{false};
    job first;
    job second;
    make(first, 1);
    make(second, 2);
    first.gate = &gate;
    ASSERT_TRUE(
        lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor), &first.task, &mailbox));
    ASSERT_TRUE(lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor), &second.task,
                                            &mailbox));
    while (!first.started)
    {
        std::this_thread::yield();
    }
    EXPECT_FALSE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_drain, 0U));
    EXPECT_FALSE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_cancel, 0U));
    gate = true;
    EXPECT_TRUE(lh_rail_thread_try_stop(&executor, lh_rail_stop_kind_drain, 10000U));
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), 2U);
    lh_rail_thread_destroy(&executor);
    EXPECT_EQ(first.completes, 1);
    EXPECT_EQ(second.completes, 0);
    EXPECT_EQ(second.destroys, 1);
}

/* Several submitters at once into one thread executor. */
TEST(rail_thread_submit, from_many_threads)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    constexpr int submitters = 4;
    constexpr int per_submitter = 2000;
    constexpr int count = submitters * per_submitter;
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, count);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    std::vector<job> jobs(count);
    for (int i = 0; i < count; ++i)
    {
        make(jobs[i], i);
    }
    std::atomic<int> refused{0};
    std::vector<std::thread> threads;
    for (int s = 0; s < submitters; ++s)
    {
        threads.emplace_back(
            [&, s]
            {
                for (int i = 0; i < per_submitter; ++i)
                {
                    if (!lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor),
                                                     &jobs[s * per_submitter + i].task, &mailbox))
                    {
                        ++refused;
                    }
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    EXPECT_EQ(refused.load(), 0);
    dispatch_all(&mailbox, count);
    lh_rail_thread_destroy(&executor);
    for (auto &j : jobs)
    {
        EXPECT_EQ(j.completes, 1);
        EXPECT_EQ(j.destroys, 1);
    }
}

/* Destroy without a stop drains first. */
TEST(rail_thread_destroy, drains_without_a_stop)
{
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 3U);
    lh_rail_thread_t executor;
    ASSERT_TRUE(lh_rail_thread_try_start(&executor, &context));
    job jobs[3];
    for (int i = 0; i < 3; ++i)
    {
        make(jobs[i], i);
        ASSERT_TRUE(lh_rail_executor_try_submit(lh_rail_thread_get_executor(&executor),
                                                &jobs[i].task, &mailbox));
    }
    lh_rail_thread_destroy(&executor);
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), 3U);
    for (auto &j : jobs)
    {
        EXPECT_EQ(j.completes, 1);
    }
}

TEST(rail_thread_try_start, refuses_on_a_base_without_threads)
{
    lh_rail_base_t saved = lh_rail_base_empty_initializer();
    lh_rail_base_assign(&saved, lh_rail_runtime_base_get());
    lh_rail_base_set_thread_start_cb(lh_rail_runtime_base_get_mut(), lh_null);

    lh_rail_thread_t thread;
    lh_rail_context_t context = lh_rail_context_initializer(lh_null, lh_null, lh_null);
    EXPECT_FALSE(lh_rail_thread_try_start(&thread, &context));

    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), &saved);
}

TEST(rail_thread_death, rejects_null_thread)
{
    LH_EXPECT_DEATH(lh_rail_thread_get_executor(lh_null));
    LH_EXPECT_DEATH(lh_rail_thread_try_stop(lh_null, lh_rail_stop_kind_drain, 0U));
}

} // namespace
