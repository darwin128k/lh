#include <gtest/gtest.h>

#include <lh/memory/typed.h>

namespace
{

struct IntTyped
{
    int buf[8]{};
    lh_memory_typed_t typed{};

    IntTyped()
    {
        lh_memory_typed_init(&typed, buf, buf + 8, sizeof(int));
    }
};

TEST(memory_typed_set_value, writes_value_at_index)
{
    IntTyped t;
    int value = 42;
    lh_memory_typed_set_value(&t.typed, 3, &value);
    EXPECT_EQ(t.buf[3], 42);
}

TEST(memory_typed_set_value, does_not_touch_neighbors)
{
    IntTyped t;
    for (int i = 0; i < 8; ++i)
    {
        t.buf[i] = -1;
    }
    int value = 7;
    lh_memory_typed_set_value(&t.typed, 4, &value);
    for (int i = 0; i < 8; ++i)
    {
        if (i == 4)
        {
            EXPECT_EQ(t.buf[i], 7);
        }
        else
        {
            EXPECT_EQ(t.buf[i], -1);
        }
    }
}

TEST(memory_typed_get_value_into, reads_value_at_index)
{
    IntTyped t;
    t.buf[5] = 99;
    int out = 0;
    lh_memory_typed_get_value_into(&t.typed, 5, &out);
    EXPECT_EQ(out, 99);
}

TEST(memory_typed_get_value_into, roundtrips_with_set_value)
{
    IntTyped t;
    int in = 12345;
    lh_memory_typed_set_value(&t.typed, 2, &in);
    int out = 0;
    lh_memory_typed_get_value_into(&t.typed, 2, &out);
    EXPECT_EQ(out, in);
}

TEST(memory_typed_swap_values, swaps_two_elements)
{
    IntTyped t;
    t.buf[0] = 1;
    t.buf[1] = 2;
    int scratch;
    lh_memory_typed_swap_values(&t.typed, 0, 1, &scratch);
    EXPECT_EQ(t.buf[0], 2);
    EXPECT_EQ(t.buf[1], 1);
}

TEST(memory_typed_swap_values, swap_with_self_is_no_op)
{
    IntTyped t;
    t.buf[3] = 55;
    int scratch;
    lh_memory_typed_swap_values(&t.typed, 3, 3, &scratch);
    EXPECT_EQ(t.buf[3], 55);
}

TEST(memory_typed_swap_values, does_not_touch_other_elements)
{
    IntTyped t;
    for (int i = 0; i < 8; ++i)
    {
        t.buf[i] = i;
    }
    int scratch;
    lh_memory_typed_swap_values(&t.typed, 2, 5, &scratch);
    for (int i = 0; i < 8; ++i)
    {
        if (i == 2)
        {
            EXPECT_EQ(t.buf[i], 5);
        }
        else if (i == 5)
        {
            EXPECT_EQ(t.buf[i], 2);
        }
        else
        {
            EXPECT_EQ(t.buf[i], i);
        }
    }
}

TEST(memory_typed_value_equals, true_for_matching_value)
{
    IntTyped t;
    t.buf[1] = 77;
    int candidate = 77;
    EXPECT_TRUE(lh_memory_typed_value_equals(&t.typed, 1, &candidate));
}

TEST(memory_typed_value_equals, false_for_different_value)
{
    IntTyped t;
    t.buf[1] = 77;
    int candidate = 78;
    EXPECT_FALSE(lh_memory_typed_value_equals(&t.typed, 1, &candidate));
}

} // namespace
