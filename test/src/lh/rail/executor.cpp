#include <gtest/gtest.h>

#include <lh/rail/executor.h>
#include <lh/rail/executor/initializer.h>
#include <lh/rail/task/initializer.h>
#include <lh/expect/death.h>

namespace
{

/* A test executor: remembers what it was handed, takes or refuses on demand. */
struct fake
{
    bool take = true;
    int calls = 0;
    lh_rail_task_t *last = nullptr;
};

lh_bool_t
fake_accept(lh_ptr context, lh_rail_task_t *task)
{
    auto *f = static_cast<fake *>(context);
    ++f->calls;
    f->last = task;
    return f->take ? lh_bool_true : lh_bool_false;
}

void
no_work(lh_ptr user, lh_ptr local)
{
    (void)user;
    (void)local;
}

TEST(rail_executor_initializer, holds_the_call_and_its_state)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    EXPECT_EQ(lh_rail_executor_get_accept_cb(&executor), fake_accept);
    EXPECT_EQ(lh_rail_executor_get_context(&executor), &f);
}

TEST(rail_executor_init, fills_both_fields)
{
    fake f;
    lh_rail_executor_t executor;
    lh_rail_executor_init(&executor, fake_accept, &f);
    EXPECT_EQ(lh_rail_executor_get_accept_cb(&executor), fake_accept);
    EXPECT_EQ(lh_rail_executor_get_context(&executor), &f);
    lh_rail_executor_set_context(&executor, lh_null);
    EXPECT_EQ(lh_rail_executor_get_context(&executor), nullptr);
    lh_rail_executor_set_accept_cb(&executor, lh_null);
    EXPECT_EQ(lh_rail_executor_get_accept_cb(&executor), nullptr);
}

TEST(rail_executor_try_accept, hands_over_with_the_context)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    EXPECT_TRUE(lh_rail_executor_try_accept(&executor, &task));
    EXPECT_EQ(f.last, &task);
    f.take = false;
    EXPECT_FALSE(lh_rail_executor_try_accept(&executor, &task));
}

TEST(rail_executor_try_submit, takes_a_place_and_names_the_mailbox)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 1U);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    EXPECT_TRUE(lh_rail_executor_try_submit(&executor, &task, &mailbox));
    EXPECT_EQ(f.last, &task);
    EXPECT_EQ(lh_rail_task_get_mailbox(&task), &mailbox);
    EXPECT_EQ(lh_rail_mailbox_get_used_count(&mailbox), 1U);
}

TEST(rail_executor_try_submit, full_mailbox_refuses_before_the_executor)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 0U);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    EXPECT_FALSE(lh_rail_executor_try_submit(&executor, &task, &mailbox));
    EXPECT_EQ(f.calls, 0);
    EXPECT_EQ(lh_rail_task_get_mailbox(&task), nullptr);
}

TEST(rail_executor_try_submit, refusal_gives_the_place_back)
{
    fake f;
    f.take = false;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 1U);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    EXPECT_FALSE(lh_rail_executor_try_submit(&executor, &task, &mailbox));
    EXPECT_EQ(f.calls, 1);
    EXPECT_EQ(lh_rail_mailbox_get_used_count(&mailbox), 0U);
}

TEST(rail_executor_submit, accepted_task_passes_the_check)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 1U);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    lh_rail_executor_submit(&executor, &task, &mailbox);
    EXPECT_EQ(f.last, &task);
}

TEST(rail_executor_death, refused_submit_fails_the_check)
{
    fake f;
    lh_rail_executor_t executor = lh_rail_executor_initializer(fake_accept, &f);
    lh_rail_mailbox_t mailbox;
    lh_rail_mailbox_init(&mailbox, 0U);
    lh_rail_task_t task = lh_rail_task_initializer(no_work, lh_null, lh_null, lh_null);
    LH_EXPECT_CHECK_DEATH(lh_rail_executor_submit(&executor, &task, &mailbox));
}

TEST(rail_executor_death, rejects_null_executor)
{
    LH_EXPECT_DEATH(lh_rail_executor_get_accept_cb(lh_null));
    LH_EXPECT_DEATH(lh_rail_executor_set_context(lh_null, lh_null));
}

} // namespace
