#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/bounds/allocated.h>
#include <lh/memory/bounds/allocated/initializer.h>
#include <lh/null.h>

#include <cstring>

namespace
{

TEST(memory_bounds_allocated_get_size, zero_for_uninitialized)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    EXPECT_EQ(lh_memory_bounds_allocated_get_size(&a), 0u);
}

TEST(memory_bounds_allocated_get_size, matches_resized_size)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 16);
    EXPECT_EQ(lh_memory_bounds_allocated_get_size(&a), 16u);
    lh_memory_bounds_allocated_clear(&a);
}

TEST(memory_bounds_allocated_resize, grows_from_empty_and_is_writable)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 8);

    ASSERT_EQ(lh_memory_bounds_allocated_get_size(&a), 8u);
    lh_ptr begin = lh_memory_bounds_get_begin(&a);
    ASSERT_NE(begin, nullptr);
    std::memset(begin, 0xAB, 8);

    lh_memory_bounds_allocated_clear(&a);
}

TEST(memory_bounds_allocated_resize, grows_and_preserves_prefix)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 4);
    auto *bytes = static_cast<unsigned char *>(lh_memory_bounds_get_begin(&a));
    bytes[0] = 1;
    bytes[1] = 2;
    bytes[2] = 3;
    bytes[3] = 4;

    lh_memory_bounds_allocated_resize(&a, 8);
    ASSERT_EQ(lh_memory_bounds_allocated_get_size(&a), 8u);
    auto *grown = static_cast<unsigned char *>(lh_memory_bounds_get_begin(&a));
    EXPECT_EQ(grown[0], 1u);
    EXPECT_EQ(grown[1], 2u);
    EXPECT_EQ(grown[2], 3u);
    EXPECT_EQ(grown[3], 4u);

    lh_memory_bounds_allocated_clear(&a);
}

TEST(memory_bounds_allocated_resize, zero_size_leaves_empty)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 8);
    lh_memory_bounds_allocated_resize(&a, 0);

    EXPECT_EQ(lh_memory_bounds_allocated_get_size(&a), 0u);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&a));
}

TEST(memory_bounds_allocated_clear, frees_and_resets_uninitialized_noop)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_clear(&a);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&a));

    lh_memory_bounds_allocated_resize(&a, 4);
    lh_memory_bounds_allocated_clear(&a);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&a));
}

TEST(memory_bounds_allocated_exchange, transfers_ownership_and_empties_other)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_t b = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&b, 6);
    lh_ptr b_begin = lh_memory_bounds_get_begin(&b);

    lh_memory_bounds_allocated_exchange(&a, &b);

    EXPECT_EQ(lh_memory_bounds_get_begin(&a), b_begin);
    EXPECT_EQ(lh_memory_bounds_allocated_get_size(&a), 6u);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&b));

    lh_memory_bounds_allocated_clear(&a);
}

TEST(memory_bounds_allocated_exchange, frees_previous_owner_of_self)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_t b = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 4);
    lh_memory_bounds_allocated_resize(&b, 6);
    lh_ptr b_begin = lh_memory_bounds_get_begin(&b);

    lh_memory_bounds_allocated_exchange(&a, &b);

    EXPECT_EQ(lh_memory_bounds_get_begin(&a), b_begin);
    lh_memory_bounds_allocated_clear(&a);
}

TEST(memory_bounds_allocated_exchange, same_object_just_frees)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_resize(&a, 4);

    lh_memory_bounds_allocated_exchange(&a, &a);

    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&a));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_bounds_allocated_death, get_size_null_self)
{
    LH_EXPECT_DEATH(lh_memory_bounds_allocated_get_size(nullptr));
}

TEST(memory_bounds_allocated_death, clear_null_self)
{
    LH_EXPECT_DEATH(lh_memory_bounds_allocated_clear(nullptr));
}

TEST(memory_bounds_allocated_death, resize_null_self)
{
    LH_EXPECT_DEATH(lh_memory_bounds_allocated_resize(nullptr, 4));
}

TEST(memory_bounds_allocated_death, exchange_invalid_other)
{
    lh_memory_bounds_allocated_t a = lh_memory_bounds_allocated_initializer();
    lh_memory_bounds_allocated_t b = lh_memory_bounds_allocated_initializer();
    LH_EXPECT_DEATH(lh_memory_bounds_allocated_exchange(&a, &b));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
