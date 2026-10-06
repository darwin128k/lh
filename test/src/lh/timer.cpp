#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/timer.h>
#include <lh/util/addr.h>

namespace
{

struct FireLog
{
    int count;
    lh_u8_t order[8];
};

lh_void
on_fire(lh_timer_t *self, lh_ptr context)
{
    auto *log = static_cast<FireLog *>(context);

    if (log == nullptr || log->count >= 8)
    {
        ADD_FAILURE() << "bad fire log";
        return;
    }
    log->order[static_cast<size_t>(log->count)] = lh_timer_get_priority(self);
    log->count++;
}

lh_void
on_nop(lh_timer_t * /*self*/, lh_ptr /*context*/)
{
}

TEST(timer_priority, compare_suite)
{
    lh_timer_group_t group{};
    lh_timer_t low{};
    lh_timer_t high{};

    lh_timer_group_init(lh_addr_of(group));
    lh_timer_init(lh_addr_of(low));
    lh_timer_init(lh_addr_of(high));

    lh_timer_start(lh_addr_of(group), lh_addr_of(low), 10, lh_bool_true, 1, on_nop, lh_null, 0);
    lh_timer_start(lh_addr_of(group), lh_addr_of(high), 10, lh_bool_true, 5, on_nop, lh_null, 0);

    EXPECT_EQ(lh_timer_get_priority(lh_addr_of(low)), 1);
    EXPECT_EQ(lh_timer_get_priority(lh_addr_of(high)), 5);
    EXPECT_EQ(lh_timer_priority_equals(lh_addr_of(low), lh_addr_of(high)), lh_bool_false);
    EXPECT_EQ(lh_timer_priority_is_less(lh_addr_of(low), lh_addr_of(high)), lh_bool_true);
    EXPECT_EQ(lh_timer_priority_is_greater(lh_addr_of(high), lh_addr_of(low)), lh_bool_true);
    EXPECT_EQ(lh_timer_priority_is_at_least(lh_addr_of(high), lh_addr_of(low)), lh_bool_true);
    EXPECT_EQ(lh_timer_priority_is_at_least(lh_addr_of(low), lh_addr_of(high)), lh_bool_false);
    EXPECT_LT(lh_timer_cmp_priority(lh_timer_get_node(lh_addr_of(high)),
                                    lh_timer_get_node(lh_addr_of(low)), lh_null),
              0);

    lh_timer_deinit(lh_addr_of(low));
    lh_timer_deinit(lh_addr_of(high));
    lh_timer_group_deinit(lh_addr_of(group));
}

TEST(timer_group, fires_higher_priority_first_when_due)
{
    lh_timer_group_t group{};
    lh_timer_t a{};
    lh_timer_t b{};
    lh_timer_t c{};
    FireLog log{};

    lh_timer_group_init(lh_addr_of(group));
    lh_timer_init(lh_addr_of(a));
    lh_timer_init(lh_addr_of(b));
    lh_timer_init(lh_addr_of(c));

    /* Insert low, then mid, then high — list must reorder by priority. */
    lh_timer_start(lh_addr_of(group), lh_addr_of(a), 10, lh_bool_true, 1, on_fire, lh_addr_of(log),
                   0);
    lh_timer_start(lh_addr_of(group), lh_addr_of(b), 10, lh_bool_true, 3, on_fire, lh_addr_of(log),
                   0);
    lh_timer_start(lh_addr_of(group), lh_addr_of(c), 10, lh_bool_true, 5, on_fire, lh_addr_of(log),
                   0);

    EXPECT_EQ(lh_timer_group_until_next(lh_addr_of(group), 0), 10U);
    lh_timer_group_handler(lh_addr_of(group), 10);
    ASSERT_EQ(log.count, 3);
    EXPECT_EQ(log.order[0], 5);
    EXPECT_EQ(log.order[1], 3);
    EXPECT_EQ(log.order[2], 1);

    lh_timer_deinit(lh_addr_of(a));
    lh_timer_deinit(lh_addr_of(b));
    lh_timer_deinit(lh_addr_of(c));
    lh_timer_group_deinit(lh_addr_of(group));
}

TEST(timer_group, one_shot_stops_after_fire)
{
    lh_timer_group_t group{};
    lh_timer_t once{};
    FireLog log{};

    lh_timer_group_init(lh_addr_of(group));
    lh_timer_init(lh_addr_of(once));
    lh_timer_start(lh_addr_of(group), lh_addr_of(once), 5, lh_bool_false, 0, on_fire,
                   lh_addr_of(log), 0);

    lh_timer_group_handler(lh_addr_of(group), 5);
    EXPECT_EQ(log.count, 1);
    EXPECT_EQ(lh_timer_is_running(lh_addr_of(once)), lh_bool_false);

    lh_timer_group_handler(lh_addr_of(group), 10);
    EXPECT_EQ(log.count, 1);

    lh_timer_deinit(lh_addr_of(once));
    lh_timer_group_deinit(lh_addr_of(group));
}

TEST(timer_group, pause_skips_due)
{
    lh_timer_group_t group{};
    lh_timer_t t{};
    FireLog log{};

    lh_timer_group_init(lh_addr_of(group));
    lh_timer_init(lh_addr_of(t));
    lh_timer_start(lh_addr_of(group), lh_addr_of(t), 5, lh_bool_true, 0, on_fire, lh_addr_of(log),
                   0);
    lh_timer_pause(lh_addr_of(t));
    EXPECT_EQ(lh_timer_is_running(lh_addr_of(t)), lh_bool_true);

    lh_timer_group_handler(lh_addr_of(group), 5);
    EXPECT_EQ(log.count, 0);

    lh_timer_resume(lh_addr_of(t), 5);
    lh_timer_group_handler(lh_addr_of(group), 10);
    EXPECT_EQ(log.count, 1);

    lh_timer_deinit(lh_addr_of(t));
    lh_timer_group_deinit(lh_addr_of(group));
}

TEST(timer_set_priority, relinks_while_running)
{
    lh_timer_group_t group{};
    lh_timer_t low{};
    lh_timer_t high{};
    FireLog log{};

    lh_timer_group_init(lh_addr_of(group));
    lh_timer_init(lh_addr_of(low));
    lh_timer_init(lh_addr_of(high));
    lh_timer_start(lh_addr_of(group), lh_addr_of(low), 10, lh_bool_true, 1, on_fire,
                   lh_addr_of(log), 0);
    lh_timer_start(lh_addr_of(group), lh_addr_of(high), 10, lh_bool_true, 2, on_fire,
                   lh_addr_of(log), 0);

    lh_timer_set_priority(lh_addr_of(group), lh_addr_of(low), 9);
    EXPECT_EQ(lh_timer_get_priority(lh_addr_of(low)), 9);

    lh_timer_group_handler(lh_addr_of(group), 10);
    ASSERT_EQ(log.count, 2);
    EXPECT_EQ(log.order[0], 9);
    EXPECT_EQ(log.order[1], 2);

    lh_timer_deinit(lh_addr_of(low));
    lh_timer_deinit(lh_addr_of(high));
    lh_timer_group_deinit(lh_addr_of(group));
}

} // namespace
