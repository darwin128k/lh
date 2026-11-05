#include <gtest/gtest.h>

#include <lh/rail/base/initializer.h>
#include <lh/rail/runtime/base.h>

namespace
{

TEST(rail_runtime_base, both_reads_name_the_one_slot)
{
    EXPECT_EQ(lh_rail_runtime_base_get(), lh_rail_runtime_base_get_mut());
}

TEST(rail_runtime_base, the_host_fills_and_empties_it)
{
    lh_rail_base_t saved = lh_rail_base_empty_initializer();
    lh_rail_base_assign(&saved, lh_rail_runtime_base_get());

    const lh_rail_base_t empty = lh_rail_base_empty_initializer();
    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), &empty);
    EXPECT_FALSE(lh_rail_base_has_clock(lh_rail_runtime_base_get()));
    EXPECT_FALSE(lh_rail_base_has_thread(lh_rail_runtime_base_get()));

    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), &saved);
    EXPECT_EQ(lh_rail_base_get_now_cb(lh_rail_runtime_base_get()),
              lh_rail_base_get_now_cb(&saved));
}

} // namespace
