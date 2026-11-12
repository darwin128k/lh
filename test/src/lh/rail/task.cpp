#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/rail/task.h>
#include <lh/rail/task/initializer.h>

namespace
{

struct calls
{
    int work = 0;
    int complete = 0;
    int destroy = 0;
    lh_ptr local = nullptr;
};

void
record_work(lh_ptr user, lh_ptr local)
{
    auto *c = static_cast<calls *>(user);
    ++c->work;
    c->local = local;
}

void
record_complete(lh_ptr user)
{
    ++static_cast<calls *>(user)->complete;
}

void
record_destroy(lh_ptr user)
{
    ++static_cast<calls *>(user)->destroy;
}

TEST(rail_task_initializer, holds_the_calls_and_an_empty_link)
{
    calls c;
    lh_rail_task_t task =
        lh_rail_task_initializer(record_work, record_complete, record_destroy, &c);
    EXPECT_EQ(lh_rail_task_get_work_cb(&task), record_work);
    EXPECT_EQ(lh_rail_task_get_complete_cb(&task), record_complete);
    EXPECT_EQ(lh_rail_task_get_destroy_cb(&task), record_destroy);
    EXPECT_EQ(lh_rail_task_get_user(&task), &c);
    EXPECT_EQ(lh_rail_task_get_next_task(&task), nullptr);
    EXPECT_EQ(lh_rail_task_get_mailbox(&task), nullptr);
    EXPECT_FALSE(lh_rail_task_is_cancelled(&task));
}

TEST(rail_task_set, each_field_stands_alone)
{
    lh_rail_task_t task = lh_rail_task_initializer(record_work, lh_null, lh_null, lh_null);
    lh_rail_task_set_complete_cb(&task, record_complete);
    EXPECT_EQ(lh_rail_task_get_complete_cb(&task), record_complete);
    EXPECT_EQ(lh_rail_task_get_destroy_cb(&task), nullptr);
    lh_rail_task_set_destroy_cb(&task, record_destroy);
    EXPECT_EQ(lh_rail_task_get_destroy_cb(&task), record_destroy);
    calls c;
    lh_rail_task_set_user(&task, &c);
    EXPECT_EQ(lh_rail_task_get_user(&task), &c);
    lh_rail_task_set_work_cb(&task, lh_null);
    EXPECT_EQ(lh_rail_task_get_work_cb(&task), nullptr);
    auto *mailbox = reinterpret_cast<struct lh_rail_mailbox *>(&c);
    lh_rail_task_set_mailbox(&task, mailbox);
    EXPECT_EQ(lh_rail_task_get_mailbox_mut(&task), mailbox);
    EXPECT_EQ(lh_rail_task_get_mailbox(&task), mailbox);
    lh_rail_task_set_cancelled(&task, lh_bool_true);
    EXPECT_TRUE(lh_rail_task_is_cancelled(&task));
}

TEST(rail_task_init, clears_the_link)
{
    lh_rail_task_t other = lh_rail_task_initializer(record_work, lh_null, lh_null, lh_null);
    lh_rail_task_t task;
    lh_atomic_ptr_set_relaxed(lh_rail_task_get_next_mut(&task), &other);
    calls c;
    lh_rail_task_set_mailbox(&task, reinterpret_cast<struct lh_rail_mailbox *>(&c));
    lh_rail_task_set_cancelled(&task, lh_bool_true);
    lh_rail_task_init(&task, record_work, record_complete, record_destroy, &c);
    EXPECT_FALSE(lh_rail_task_is_cancelled(&task));
    EXPECT_EQ(lh_rail_task_get_next_task(&task), nullptr);
    EXPECT_EQ(lh_rail_task_get_mailbox(&task), nullptr);
    EXPECT_EQ(lh_rail_task_get_user(&task), &c);
}

TEST(rail_task_get_next_task, reads_the_link)
{
    lh_rail_task_t first = lh_rail_task_initializer(record_work, lh_null, lh_null, lh_null);
    lh_rail_task_t second = lh_rail_task_initializer(record_work, lh_null, lh_null, lh_null);
    lh_atomic_ptr_set_release(lh_rail_task_get_next_mut(&first), &second);
    EXPECT_EQ(lh_rail_task_get_next_task(&first), &second);
    EXPECT_EQ(lh_rail_task_get_next(&first), lh_rail_task_get_next_mut(&first));
}

TEST(rail_task_run, work_complete_and_destroy_receive_their_pointers)
{
    int local = 0;
    calls c;
    lh_rail_task_t task =
        lh_rail_task_initializer(record_work, record_complete, record_destroy, &c);
    lh_rail_task_run_work(&task, &local);
    EXPECT_EQ(c.work, 1);
    EXPECT_EQ(c.local, &local);
    lh_rail_task_run_complete(&task);
    EXPECT_EQ(c.complete, 1);
    lh_rail_task_run_destroy(&task);
    EXPECT_EQ(c.destroy, 1);
}

TEST(rail_task_run, absent_complete_and_destroy_do_nothing)
{
    calls c;
    lh_rail_task_t task = lh_rail_task_initializer(record_work, lh_null, lh_null, &c);
    lh_rail_task_run_complete(&task);
    lh_rail_task_run_destroy(&task);
    EXPECT_EQ(c.complete, 0);
    EXPECT_EQ(c.destroy, 0);
}

TEST(rail_task_death, rejects_null_task)
{
    LH_EXPECT_DEATH(lh_rail_task_get_work_cb(lh_null));
    LH_EXPECT_DEATH(lh_rail_task_set_user(lh_null, lh_null));
    LH_EXPECT_DEATH(lh_rail_task_run_complete(lh_null));
}

} // namespace
