#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/expect/death.h>
#include <lh/memory/bounds/slice.h>
#include <lh/null.h>

namespace {

lh_ptr p(unsigned char *ptr) {
    return lh_cast_static(lh_ptr, ptr);
}

lh_memory_bounds_slice_t slice(lh_ptr begin, lh_ptr end) {
    return {begin, end};
}

TEST(memory_bounds_slice_unpack, skips_null_output_pointers) {
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    lh_ptr begin = lh_null;
    lh_memory_bounds_slice_unpack(&s, &begin, nullptr);
    EXPECT_EQ(begin, p(buf));

    lh_ptr end = lh_null;
    lh_memory_bounds_slice_unpack(&s, nullptr, &end);
    EXPECT_EQ(end, p(buf + 3));
}

TEST(memory_bounds_slice_getters, return_stored_bounds) {
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_get_begin(&s), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_end(&s), p(buf + 3));
}

TEST(memory_bounds_slice_flags, identify_initialized_endpoints) {
    unsigned char buf[2];

    lh_memory_bounds_slice_t none = slice(lh_null, lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&none), lh_memory_bounds_slice_flags_uninitialized);
    EXPECT_TRUE(lh_memory_bounds_slice_is_uninitialized(&none));
    EXPECT_FALSE(lh_memory_bounds_slice_is_initialized(&none));

    lh_memory_bounds_slice_t begin = slice(p(buf), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&begin),
              lh_memory_bounds_slice_flags_initialized_begin);

    lh_memory_bounds_slice_t end = slice(lh_null, p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&end), lh_memory_bounds_slice_flags_initialized_end);

    lh_memory_bounds_slice_t both = slice(p(buf), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&both), lh_memory_bounds_slice_flags_initialized);
    EXPECT_TRUE(lh_memory_bounds_slice_is_initialized(&both));
}

TEST(memory_bounds_slice_direction, distinguishes_forward_backward_and_unknown) {
    unsigned char buf[4];

    lh_memory_bounds_slice_t unknown = slice(p(buf), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&unknown),
              lh_memory_bounds_slice_direction_unknown);

    lh_memory_bounds_slice_t forward = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&forward),
              lh_memory_bounds_slice_direction_forward);
    EXPECT_TRUE(lh_memory_bounds_slice_is_forward_direction(&forward));
    EXPECT_TRUE(lh_memory_bounds_slice_is_valid(&forward));

    lh_memory_bounds_slice_t backward = slice(p(buf + 3), p(buf));
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&backward),
              lh_memory_bounds_slice_direction_backward);
    EXPECT_TRUE(lh_memory_bounds_slice_is_backward_direction(&backward));
    EXPECT_FALSE(lh_memory_bounds_slice_is_valid(&backward));
}

TEST(memory_bounds_slice_size, closed_range_includes_both_endpoints) {
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_begin_v(&s), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_end_v(&s), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_size(&s), 4u);
    EXPECT_FALSE(lh_memory_bounds_slice_is_empty(&s));
}

TEST(memory_bounds_slice_empty, uninitialized_slice_is_empty) {
    lh_memory_bounds_slice_t s = slice(lh_null, lh_null);

    EXPECT_TRUE(lh_memory_bounds_slice_is_empty(&s));
}

TEST(memory_bounds_slice_contains_ptr, closed_range_contains_both_endpoints) {
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 1)));
    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 4)));
    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 3)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_ptr(&s, p(buf)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 5)));
}

TEST(memory_bounds_slice_contains, accepts_inner_ranges) {
    unsigned char buf[8];
    lh_memory_bounds_slice_t outer = slice(p(buf + 1), p(buf + 6));
    lh_memory_bounds_slice_t inner = slice(p(buf + 2), p(buf + 4));

    EXPECT_TRUE(lh_memory_bounds_slice_contains_of(&outer, p(buf + 2), p(buf + 4)));
    EXPECT_TRUE(lh_memory_bounds_slice_contains(&outer, &inner));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_of(&outer, p(buf), p(buf + 4)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_of(&outer, p(buf + 2), p(buf + 7)));
}

TEST(memory_bounds_slice_offset, validates_offsets_within_closed_size) {
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_TRUE(lh_memory_bounds_slice_is_valid_offset(&s, 0));
    EXPECT_TRUE(lh_memory_bounds_slice_is_valid_offset(&s, 3));
    EXPECT_FALSE(lh_memory_bounds_slice_is_valid_offset(&s, 4));
}

TEST(memory_bounds_slice_ptr_access, supports_offsets_from_both_ends) {
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_begin(&s, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_begin(&s, 3), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_end(&s, 0), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_end(&s, 1), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr(&s, 2), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr(&s, -1), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr(&s, -2), p(buf + 3));
}

TEST(memory_bounds_slice_value_access, reads_bytes_from_requested_offsets) {
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_begin(&s, 0), 20);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_begin(&s, 3), 50);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_end(&s, 0), 50);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_end(&s, 1), 40);
    EXPECT_EQ(lh_memory_bounds_slice_get_value(&s, 1), 30);
    EXPECT_EQ(lh_memory_bounds_slice_get_value(&s, -1), 50);
    EXPECT_EQ(lh_memory_bounds_slice_get_begin_value(&s), 20);
    EXPECT_EQ(lh_memory_bounds_slice_get_end_value(&s), 50);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_bounds_slice_death, unpack_null_self) {
    lh_ptr begin = lh_null;
    LH_EXPECT_DEATH(lh_memory_bounds_slice_unpack(nullptr, &begin, nullptr));
}

TEST(memory_bounds_slice_death, validated_unpack_rejects_backward_range) {
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf));
    lh_ptr begin = lh_null;

    LH_EXPECT_DEATH(lh_memory_bounds_slice_unpack_v(&s, &begin, nullptr));
}

TEST(memory_bounds_slice_death, size_rejects_uninitialized_range) {
    lh_memory_bounds_slice_t s = slice(lh_null, lh_null);

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_size(&s));
}

TEST(memory_bounds_slice_death, ptr_access_rejects_out_of_range_offset) {
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_ptr_from_begin(&s, 2));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
