#include <gtest/gtest.h>

#include <lh/vector.h>

namespace
{

struct IntVector
{
    lh_vector_t v{};

    IntVector()
    {
        lh_vector_init(&v, sizeof(int));
    }

    ~IntVector()
    {
        lh_memory_typed_allocated_clear(&v.typed);
    }

    void
    push(int value)
    {
        lh_vector_push_back(&v, &value);
    }

    int
    at(lh_uindex_t index) const
    {
        return *static_cast<const int *>(lh_vector_get_ptr(&v, index));
    }
};

TEST(vector_push_back, appends_from_empty_and_grows)
{
    IntVector iv;
    EXPECT_EQ(lh_vector_get_size(&iv.v), 0u);

    for (int i = 0; i < 100; ++i)
    {
        iv.push(i);
    }

    ASSERT_EQ(lh_vector_get_size(&iv.v), 100u);
    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

TEST(vector_push_back, fast_path_within_reserved_capacity_matches_growth_path)
{
    IntVector iv;
    lh_vector_reserve(&iv.v, 8);
    const lh_usize_t capacity_before = lh_vector_get_capacity(&iv.v);

    for (int i = 0; i < 8; ++i)
    {
        iv.push(i);
    }

    // No reallocation should have happened: capacity is unchanged, size matches pushes.
    EXPECT_EQ(lh_vector_get_capacity(&iv.v), capacity_before);
    ASSERT_EQ(lh_vector_get_size(&iv.v), 8u);
    for (int i = 0; i < 8; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

TEST(vector_push_back, crossing_reserved_capacity_falls_back_and_still_grows)
{
    IntVector iv;
    lh_vector_reserve(&iv.v, 4);

    for (int i = 0; i < 4; ++i)
    {
        iv.push(i); // fast path: room already reserved
    }
    iv.push(4); // must fall back to the growing path: capacity was exactly full

    ASSERT_EQ(lh_vector_get_size(&iv.v), 5u);
    EXPECT_GE(lh_vector_get_capacity(&iv.v), 5u);
    for (int i = 0; i <= 4; ++i)
    {
        EXPECT_EQ(iv.at(static_cast<lh_uindex_t>(i)), i);
    }
}

} // namespace
