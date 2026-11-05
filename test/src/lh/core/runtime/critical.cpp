#include <gtest/gtest.h>

#include <lh/core/critical/initializer.h>
#include <lh/core/runtime/critical.h>

namespace
{

int g_enters = 0;
int g_leaves = 0;

lh_usize_t
count_enter(void)
{
    ++g_enters;
    return 7u;
}

lh_void
count_leave(lh_usize_t state)
{
    EXPECT_EQ(state, 7u);
    ++g_leaves;
}

TEST(runtime_critical, both_reads_name_the_one_slot)
{
    EXPECT_EQ(lh_runtime_critical_get(), lh_runtime_critical_get_mut());
}

TEST(runtime_critical, enter_and_leave_go_through_the_slot)
{
    lh_critical_t saved = lh_critical_empty_initializer();
    lh_critical_assign(&saved, lh_runtime_critical_get());

    const lh_critical_t counting = lh_critical_initializer(count_enter, count_leave);
    lh_critical_assign(lh_runtime_critical_get_mut(), &counting);
    g_enters = 0;
    g_leaves = 0;
    lh_runtime_critical_leave(lh_runtime_critical_enter());
    EXPECT_EQ(g_enters, 1);
    EXPECT_EQ(g_leaves, 1);

    lh_critical_assign(lh_runtime_critical_get_mut(), &saved);
}

} // namespace
