#include <gtest/gtest.h>

#include <lh/rail/inline.h>
#include <lh/rail/task/initializer.h>
#include <lh/expect/death.h>

#include <vector>

namespace
{

/* Records which call ran, in order, and what work received. */
struct job
{
    lh_rail_task_t task;
    std::vector<char> events;
    lh_ptr local = reinterpret_cast<lh_ptr>(1);
};

void
on_work(lh_ptr user, lh_ptr local)
{
    auto *j = static_cast<job *>(user);
    j->events.push_back('w');
    j->local = local;
}

void
on_complete(lh_ptr user)
{
    static_cast<job *>(user)->events.push_back('c');
}

void
on_destroy(lh_ptr user)
{
    static_cast<job *>(user)->events.push_back('d');
}

TEST(rail_inline_get_executor, is_one_shared_record)
{
    const lh_rail_executor_t *executor = lh_rail_inline_get_executor();
    ASSERT_NE(executor, nullptr);
    EXPECT_EQ(executor, lh_rail_inline_get_executor());
    EXPECT_EQ(lh_rail_executor_get_accept_cb(executor), lh_rail_inline_try_accept);
    EXPECT_EQ(lh_rail_executor_get_context(executor), nullptr);
}

/* Work runs at once on the submitter; complete waits for dispatch. */
TEST(rail_inline_submit, work_at_once_complete_at_dispatch)
{
    job j;
    lh_rail_task_init(&j.task, on_work, on_complete, on_destroy, &j);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 2U);
    ASSERT_TRUE(lh_rail_executor_try_submit(lh_rail_inline_get_executor(), &j.task, &mailbox));
    EXPECT_EQ(j.events, (std::vector<char>{'w'}));
    EXPECT_EQ(j.local, nullptr);
    EXPECT_EQ(lh_rail_mailbox_get_used_count(&mailbox), 1U);
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(j.events, (std::vector<char>{'w', 'c', 'd'}));
    EXPECT_EQ(lh_rail_mailbox_get_used_count(&mailbox), 0U);
}

TEST(rail_inline_submit, full_mailbox_runs_nothing)
{
    job j;
    lh_rail_task_init(&j.task, on_work, on_complete, on_destroy, &j);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 0U);
    EXPECT_FALSE(lh_rail_executor_try_submit(lh_rail_inline_get_executor(), &j.task, &mailbox));
    EXPECT_TRUE(j.events.empty());
}

/* A complete may submit a follow-up; it is delivered by the next dispatch. */
struct chain
{
    lh_rail_task_t first;
    lh_rail_task_t second;
    lh_rail_mailbox_t *mailbox;
    std::vector<char> events;
};

void
chain_work(lh_ptr user, lh_ptr local)
{
    (void)local;
    static_cast<chain *>(user)->events.push_back('w');
}

void
chain_complete_first(lh_ptr user)
{
    auto *c = static_cast<chain *>(user);
    c->events.push_back('1');
    lh_rail_executor_submit(lh_rail_inline_get_executor(), &c->second, c->mailbox);
}

void
chain_complete_second(lh_ptr user)
{
    static_cast<chain *>(user)->events.push_back('2');
}

TEST(rail_inline_submit, follow_up_from_complete)
{
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 2U);
    chain c;
    c.mailbox = &mailbox;
    lh_rail_task_init(&c.first, chain_work, chain_complete_first, lh_null, &c);
    lh_rail_task_init(&c.second, chain_work, chain_complete_second, lh_null, &c);
    lh_rail_executor_submit(lh_rail_inline_get_executor(), &c.first, &mailbox);
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(c.events, (std::vector<char>{'w', '1', 'w'}));
    EXPECT_EQ(lh_rail_mailbox_dispatch(&mailbox), 1U);
    EXPECT_EQ(c.events, (std::vector<char>{'w', '1', 'w', '2'}));
}

TEST(rail_inline_death, rejects_a_task_without_a_mailbox)
{
    job j;
    lh_rail_task_init(&j.task, on_work, lh_null, lh_null, &j);
    LH_EXPECT_DEATH(lh_rail_inline_try_accept(lh_null, &j.task));
}

} // namespace
