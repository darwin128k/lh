#include <gtest/gtest.h>

#include <lh/runtime/exception/catch/stack.h>

#include <vector>

namespace
{

class runtime_exception_catch_stack : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        ASSERT_TRUE(lh_runtime_exception_catch_stack_is_begin())
            << "stack must start each test fully unwound";
    }

    void
    TearDown() override
    {
        while (!lh_runtime_exception_catch_stack_is_begin())
        {
            lh_runtime_exception_catch_stack_prev();
        }
    }
};

TEST_F(runtime_exception_catch_stack, begin_and_end_span_exactly_capacity_slots)
{
    lh_usize_t capacity = lh_runtime_exception_catch_stack_get_capacity();

    EXPECT_EQ(lh_runtime_exception_catch_stack_get_end() - lh_runtime_exception_catch_stack_get_begin(),
              static_cast<std::ptrdiff_t>(capacity));
}

TEST_F(runtime_exception_catch_stack, push_then_prev_round_trips_to_begin)
{
    lh_exception_catch_t frame;

    EXPECT_FALSE(lh_runtime_exception_catch_stack_is_end());
    EXPECT_EQ(lh_runtime_exception_catch_stack_push(&frame), &frame);
    EXPECT_FALSE(lh_runtime_exception_catch_stack_is_begin());

    EXPECT_EQ(lh_runtime_exception_catch_stack_prev(), &frame);
    EXPECT_TRUE(lh_runtime_exception_catch_stack_is_begin());
}

TEST_F(runtime_exception_catch_stack, prev_at_begin_stays_at_begin_instead_of_reading_out_of_bounds)
{
    EXPECT_EQ(lh_runtime_exception_catch_stack_prev(), nullptr);
    EXPECT_TRUE(lh_runtime_exception_catch_stack_is_begin());
}

TEST_F(runtime_exception_catch_stack, push_beyond_capacity_fails_instead_of_writing_out_of_bounds)
{
    lh_usize_t capacity = lh_runtime_exception_catch_stack_get_capacity();
    std::vector<lh_exception_catch_t> frames(capacity);

    for (lh_usize_t i = 0; i < capacity; ++i)
    {
        EXPECT_EQ(lh_runtime_exception_catch_stack_push(&frames[i]), &frames[i]);
    }
    EXPECT_TRUE(lh_runtime_exception_catch_stack_is_end());

    lh_exception_catch_t overflow_frame;
    EXPECT_EQ(lh_runtime_exception_catch_stack_push(&overflow_frame), nullptr);
    EXPECT_TRUE(lh_runtime_exception_catch_stack_is_end());
}

TEST_F(runtime_exception_catch_stack, filling_the_last_slot_does_not_read_past_the_array)
{
    lh_usize_t capacity = lh_runtime_exception_catch_stack_get_capacity();
    std::vector<lh_exception_catch_t> frames(capacity - 1);

    for (lh_usize_t i = 0; i < capacity - 1; ++i)
    {
        lh_runtime_exception_catch_stack_push(&frames[i]);
    }
    ASSERT_FALSE(lh_runtime_exception_catch_stack_is_end());

    lh_exception_catch_t last_frame;
    EXPECT_EQ(lh_runtime_exception_catch_stack_push(&last_frame), &last_frame);
    EXPECT_TRUE(lh_runtime_exception_catch_stack_is_end());
}

} // namespace
