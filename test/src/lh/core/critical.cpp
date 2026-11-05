#include <gtest/gtest.h>

#include <lh/core/critical.h>
#include <lh/core/critical/initializer.h>
#include <lh/expect/death.h>

namespace
{

/* A fake section: counts how deep it is and hands out the depth as the token,
   as a Cortex-M one hands out the PRIMASK it found. */
int g_depth = 0;
int g_enters = 0;
lh_usize_t g_last_leave = 99u;

lh_usize_t
fake_enter(void)
{
    ++g_enters;
    return static_cast<lh_usize_t>(g_depth++);
}

lh_void
fake_leave(lh_usize_t state)
{
    --g_depth;
    g_last_leave = state;
}

TEST(critical_empty_initializer, is_empty_and_does_nothing)
{
    const lh_critical_t critical = lh_critical_empty_initializer();
    EXPECT_TRUE(lh_critical_is_empty(&critical));
    EXPECT_EQ(lh_critical_enter(&critical), 0u);
    lh_critical_leave(&critical, 5u);
}

TEST(critical_initializer, holds_both_calls)
{
    const lh_critical_t critical = lh_critical_initializer(fake_enter, fake_leave);
    EXPECT_EQ(lh_critical_get_enter_cb(&critical), fake_enter);
    EXPECT_EQ(lh_critical_get_leave_cb(&critical), fake_leave);
    EXPECT_FALSE(lh_critical_is_empty(&critical));
}

TEST(critical_set, one_missing_call_makes_it_empty)
{
    lh_critical_t critical = lh_critical_initializer(fake_enter, fake_leave);
    lh_critical_set_leave_cb(&critical, lh_null);
    EXPECT_TRUE(lh_critical_is_empty(&critical));
    g_enters = 0;
    EXPECT_EQ(lh_critical_enter(&critical), 0u);
    EXPECT_EQ(g_enters, 0);
    lh_critical_set_leave_cb(&critical, fake_leave);
    lh_critical_set_enter_cb(&critical, lh_null);
    EXPECT_TRUE(lh_critical_is_empty(&critical));
}

TEST(critical_enter, nests_and_each_leave_takes_its_own_token)
{
    const lh_critical_t critical = lh_critical_initializer(fake_enter, fake_leave);
    g_depth = 0;
    const lh_usize_t outer = lh_critical_enter(&critical);
    const lh_usize_t inner = lh_critical_enter(&critical);
    EXPECT_EQ(g_depth, 2);
    lh_critical_leave(&critical, inner);
    EXPECT_EQ(g_last_leave, inner);
    lh_critical_leave(&critical, outer);
    EXPECT_EQ(g_last_leave, outer);
    EXPECT_EQ(g_depth, 0);
}

TEST(critical_assign, copies_both_calls)
{
    const lh_critical_t full = lh_critical_initializer(fake_enter, fake_leave);
    lh_critical_t critical = lh_critical_empty_initializer();
    lh_critical_assign(&critical, &full);
    EXPECT_EQ(lh_critical_get_enter_cb(&critical), fake_enter);
    EXPECT_EQ(lh_critical_get_leave_cb(&critical), fake_leave);
}

TEST(critical_death, rejects_null_section)
{
    LH_EXPECT_DEATH(lh_critical_get_enter_cb(lh_null));
    LH_EXPECT_DEATH(lh_critical_enter(lh_null));
}

} // namespace
