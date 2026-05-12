#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/expect/death.h>
#include <lh/memory/bounds.h>
#include <lh/memory/bounds/initializer.h>
#include <lh/null.h>

namespace
{

lh_ptr
p(unsigned char *ptr)
{
    return lh_cast_static(lh_ptr, ptr);
}

lh_memory_bounds_t
bounds(lh_ptr begin, lh_ptr end)
{
    return {begin, end};
}

TEST(memory_bounds_unpack, skips_null_output_pointers)
{
    unsigned char buf[4];
    lh_memory_bounds_t b = bounds(p(buf), p(buf + 4));

    lh_ptr begin = lh_null;
    lh_memory_bounds_unpack(&b, &begin, nullptr);
    EXPECT_EQ(begin, p(buf));

    lh_ptr end = lh_null;
    lh_memory_bounds_unpack(&b, nullptr, &end);
    EXPECT_EQ(end, p(buf + 4));
}

TEST(memory_bounds_getters, return_stored_endpoints)
{
    unsigned char buf[4];
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_get_begin(&b), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_get_end(&b), p(buf + 4));
}

TEST(memory_bounds_make_slice, copies_half_open_bounds_as_closed_slice)
{
    unsigned char buf[8];
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 5));

    lh_memory_bounds_slice_t s = lh_memory_bounds_make_slice(&b);

    EXPECT_TRUE(lh_memory_bounds_slice_equals_of(&s, p(buf + 1), p(buf + 4)));
}

TEST(memory_bounds_make_slice, copies_uninitialized_bounds_as_empty_slice)
{
    lh_memory_bounds_t b = bounds(lh_null, lh_null);

    lh_memory_bounds_slice_t s = lh_memory_bounds_make_slice(&b);

    EXPECT_TRUE(lh_memory_bounds_slice_equals_of(&s, lh_null, lh_null));
}

TEST(memory_bounds_classification, distinguishes_states)
{
    unsigned char buf[4];

    lh_memory_bounds_t empty = bounds(lh_null, lh_null);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&empty));
    EXPECT_FALSE(lh_memory_bounds_is_initialized(&empty));
    EXPECT_TRUE(lh_memory_bounds_is_empty(&empty));
    EXPECT_TRUE(lh_memory_bounds_is_invalid(&empty));

    lh_memory_bounds_t unknown = bounds(p(buf), lh_null);
    EXPECT_EQ(lh_memory_bounds_get_direction(&unknown), lh_memory_view_slice_direction_unknown);

    lh_memory_bounds_t forward = bounds(p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_bounds_is_initialized(&forward));
    EXPECT_EQ(lh_memory_bounds_get_direction(&forward), lh_memory_view_slice_direction_forward);
    EXPECT_TRUE(lh_memory_bounds_is_forward(&forward));
    EXPECT_TRUE(lh_memory_bounds_is_valid(&forward));
    EXPECT_FALSE(lh_memory_bounds_is_invalid(&forward));

    lh_memory_bounds_t equal = bounds(p(buf), p(buf));
    EXPECT_EQ(lh_memory_bounds_get_direction(&equal), lh_memory_view_slice_direction_backward);
    EXPECT_TRUE(lh_memory_bounds_is_backward(&equal));
    EXPECT_FALSE(lh_memory_bounds_is_valid(&equal));
}

TEST(memory_bounds_validated_access, returns_endpoints_and_size)
{
    unsigned char buf[8];
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 5));

    lh_ptr begin = lh_null;
    lh_ptr end = lh_null;
    lh_memory_bounds_unpack_v(&b, &begin, &end);

    EXPECT_EQ(begin, p(buf + 1));
    EXPECT_EQ(end, p(buf + 5));
    EXPECT_EQ(lh_memory_bounds_get_begin_v(&b), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_get_end_v(&b), p(buf + 5));
    EXPECT_EQ(lh_memory_bounds_get_size(&b), 4u);
}

TEST(memory_bounds_offsets, validate_half_open_offsets)
{
    unsigned char buf[6];
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 5));

    EXPECT_TRUE(lh_memory_bounds_is_valid_offset(&b, 0));
    EXPECT_TRUE(lh_memory_bounds_is_valid_offset(&b, 3));
    EXPECT_FALSE(lh_memory_bounds_is_valid_offset(&b, 4));

    EXPECT_EQ(lh_memory_bounds_get_offset_from_begin(&b, p(buf + 1)), 0u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_begin(&b, p(buf + 4)), 3u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_end(&b, p(buf + 4)), 0u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_end(&b, p(buf + 1)), 3u);
}

TEST(memory_bounds_contains, uses_half_open_ranges)
{
    unsigned char buf[8];
    lh_memory_bounds_t outer = bounds(p(buf + 1), p(buf + 6));
    lh_memory_bounds_t inner = bounds(p(buf + 2), p(buf + 5));

    EXPECT_TRUE(lh_memory_bounds_contains_ptr(&outer, p(buf + 1)));
    EXPECT_TRUE(lh_memory_bounds_contains_ptr(&outer, p(buf + 5)));
    EXPECT_FALSE(lh_memory_bounds_contains_ptr(&outer, p(buf + 6)));

    EXPECT_TRUE(lh_memory_bounds_contains_of(&outer, p(buf + 2), p(buf + 5)));
    EXPECT_FALSE(lh_memory_bounds_contains_of(&outer, p(buf), p(buf + 5)));
    EXPECT_TRUE(lh_memory_bounds_contains(&outer, &inner));
}

TEST(memory_bounds_pointer_access, supports_offsets_from_both_ends)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_bounds_get_ptr_from_begin(&b, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_get_ptr_from_begin(&b, 3), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_get_ptr_from_end(&b, 0), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_get_ptr_from_end(&b, 1), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_get_ptr_by_offset(&b, 2), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_get_ptr_by_offset(&b, -1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_get_value_from_begin(&b, 0), 20);
    EXPECT_EQ(lh_memory_bounds_get_value_from_end(&b, 0), 50);
    EXPECT_EQ(lh_memory_bounds_get_value_by_offset(&b, -2), 40);
    EXPECT_EQ(lh_memory_bounds_get_first_value(&b), 20);
    EXPECT_EQ(lh_memory_bounds_get_last_value(&b), 50);
}

TEST(memory_bounds_set_value, writes_from_begin_offset)
{
    unsigned char buf[4] = {1, 2, 3, 4};
    lh_memory_bounds_t b = bounds(p(buf), p(buf + 4));

    lh_memory_bounds_set_value(&b, 2, 99);

    EXPECT_EQ(buf[2], 99);
}

TEST(memory_bounds_seek, supports_absolute_and_relative_offsets)
{
    unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_bounds_t b = bounds(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_bounds_get_offset_from_ptr(&b, lh_null, 0), 0u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_ptr(&b, lh_null, -1), 3u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_ptr(&b, p(buf + 2), 2), 3u);
    EXPECT_EQ(lh_memory_bounds_get_offset_from_ptr(&b, p(buf + 3), -2), 0u);

    EXPECT_EQ(lh_memory_bounds_seek_ptr(&b, lh_null, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_seek_ptr(&b, p(buf + 2), 2), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_seek_ptr(&b, p(buf + 4), 1), lh_null);
    EXPECT_EQ(lh_memory_bounds_next_ptr(&b, p(buf + 2)), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_prev_ptr(&b, p(buf + 2)), p(buf + 1));

    EXPECT_EQ(lh_memory_bounds_seek_value(&b, p(buf + 2)), 30);
    EXPECT_EQ(lh_memory_bounds_next_value(&b, p(buf + 2)), 40);
    EXPECT_EQ(lh_memory_bounds_prev_value(&b, p(buf + 2)), 20);
}

TEST(memory_bounds_overlap_alignment_and_equality, reports_expected_values)
{
    alignas(16) unsigned char buf[32];
    lh_memory_bounds_t left = bounds(p(buf), p(buf + 8));
    lh_memory_bounds_t touching = bounds(p(buf + 8), p(buf + 12));
    lh_memory_bounds_t overlapping = bounds(p(buf + 7), p(buf + 12));

    EXPECT_TRUE(lh_memory_bounds_overlaps_of(&left, p(buf + 7), p(buf + 9)));
    EXPECT_FALSE(lh_memory_bounds_overlaps_range(&left, p(buf + 8), p(buf + 12)));
    EXPECT_FALSE(lh_memory_bounds_overlaps(&left, &touching));
    EXPECT_TRUE(lh_memory_bounds_overlaps_v(&left, &overlapping));

    EXPECT_TRUE(lh_memory_bounds_multiple_of(&left, 4));
    EXPECT_TRUE(lh_memory_bounds_is_multiple_of(&left, 8));
    EXPECT_TRUE(lh_memory_bounds_aligned_is_begin_aligned(&left, 16));
    EXPECT_TRUE(lh_memory_bounds_is_begin_aligned(&left, 16));
    EXPECT_FALSE(lh_memory_bounds_is_aligned(&left, 16));

    EXPECT_TRUE(lh_memory_bounds_equals_of(&left, p(buf), p(buf + 8)));
    EXPECT_TRUE(lh_memory_bounds_equals_range(&left, p(buf), p(buf + 8)));
    EXPECT_FALSE(lh_memory_bounds_equals(&left, &overlapping));
}

TEST(memory_bounds_raw_copy_and_move, delegate_to_raw_operations)
{
    unsigned char dst[4] = {};
    unsigned char src[] = {1, 2, 3, 4, 5};
    lh_memory_bounds_t dst_bounds = bounds(p(dst), p(dst + 4));
    lh_memory_bounds_t src_bounds = bounds(p(src), p(src + 5));

    EXPECT_EQ(lh_memory_bounds_copy_range(&dst_bounds, p(src), p(src + 5)), p(dst + 4));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[3], 4);

    unsigned char moved[4] = {};
    lh_memory_bounds_t moved_bounds = bounds(p(moved), p(moved + 4));
    EXPECT_EQ(lh_memory_bounds_move(&moved_bounds, &src_bounds), p(moved + 4));
    EXPECT_EQ(moved[0], 1);
    EXPECT_EQ(moved[3], 4);

    unsigned char moved_range[2] = {};
    lh_memory_bounds_t moved_range_bounds = bounds(p(moved_range), p(moved_range + 2));
    EXPECT_EQ(lh_memory_bounds_move_range(&moved_range_bounds, p(src + 1), p(src + 4)),
              p(moved_range + 2));
    EXPECT_EQ(moved_range[0], 2);
    EXPECT_EQ(moved_range[1], 3);
}

TEST(memory_bounds_raw_search_and_compare, delegate_to_raw_operations)
{
    unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    unsigned char needle[] = {'a', 'b'};
    unsigned char mismatch[] = {'a', 'x'};
    lh_memory_bounds_t hay_bounds = bounds(p(hay), p(hay + 5));
    lh_memory_bounds_t needle_bounds = bounds(p(needle), p(needle + 2));
    lh_memory_bounds_t mismatch_bounds = bounds(p(mismatch), p(mismatch + 2));

    EXPECT_EQ(lh_memory_bounds_find_range(&hay_bounds, p(needle), p(needle + 2)), p(hay));
    EXPECT_EQ(lh_memory_bounds_find(&hay_bounds, &needle_bounds), p(hay));
    EXPECT_EQ(lh_memory_bounds_rfind_range(&hay_bounds, p(needle), p(needle + 2)), p(hay + 2));
    EXPECT_EQ(lh_memory_bounds_rfind(&hay_bounds, &needle_bounds), p(hay + 2));

    EXPECT_EQ(lh_memory_bounds_compare_range(&hay_bounds, p(needle), p(needle + 2)), lh_null);
    EXPECT_EQ(lh_memory_bounds_compare(&hay_bounds, &mismatch_bounds), p(hay + 1));
    EXPECT_EQ(lh_memory_bounds_rcompare_range(&hay_bounds, p(needle), p(needle + 2)), p(hay + 4));
    EXPECT_EQ(lh_memory_bounds_rcompare(&hay_bounds, &needle_bounds), p(hay + 4));
}

TEST(memory_bounds_raw_fill, fills_with_value_and_pattern)
{
    unsigned char dst[5] = {};
    unsigned char pattern[] = {1, 2};
    lh_memory_bounds_t dst_bounds = bounds(p(dst), p(dst + 5));
    lh_memory_bounds_t pattern_bounds = bounds(p(pattern), p(pattern + 2));

    EXPECT_EQ(lh_memory_bounds_fill(&dst_bounds, 0x7E), p(dst + 5));
    EXPECT_EQ(dst[0], 0x7E);
    EXPECT_EQ(dst[4], 0x7E);

    EXPECT_EQ(lh_memory_bounds_fill_pattern_range(&dst_bounds, p(pattern), p(pattern + 2)),
              p(dst + 5));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 1);

    EXPECT_EQ(lh_memory_bounds_fill_pattern(&dst_bounds, &pattern_bounds), p(dst + 5));
    EXPECT_EQ(dst[3], 2);
    EXPECT_EQ(dst[4], 1);
}

TEST(memory_bounds_mutation, clears_assigns_sets_and_initializes)
{
    unsigned char buf[8];
    lh_memory_bounds_t b = bounds(lh_null, lh_null);
    lh_memory_bounds_t other = bounds(p(buf + 1), p(buf + 5));

    lh_memory_bounds_assign_v(&b, &other);
    EXPECT_TRUE(lh_memory_bounds_equals(&b, &other));

    lh_memory_bounds_clear(&b);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, lh_null, lh_null));

    lh_memory_bounds_set_v(&b, p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, p(buf), p(buf + 4)));

    lh_memory_bounds_set_by_size(&b, p(buf + 2), 3);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, p(buf + 2), p(buf + 5)));

    lh_memory_bounds_init(&b, p(buf), p(buf + 2));
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, p(buf), p(buf + 2)));

    lh_memory_bounds_init_by_size(&b, p(buf + 1), 2);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, p(buf + 1), p(buf + 3)));

    lh_memory_bounds_init_empty(&b);
    EXPECT_TRUE(lh_memory_bounds_is_uninitialized(&b));

    lh_memory_bounds_init_by_other(&b, &other);
    EXPECT_TRUE(lh_memory_bounds_equals(&b, &other));
}

TEST(memory_bounds_make, returns_validated_values)
{
    unsigned char buf[8];

    lh_memory_bounds_t by_end = lh_memory_bounds_make_v(p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_bounds_equals_of(&by_end, p(buf), p(buf + 4)));

    lh_memory_bounds_t by_size = lh_memory_bounds_make_by_size(p(buf + 1), 3);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&by_size, p(buf + 1), p(buf + 4)));

    lh_memory_bounds_t empty = lh_memory_bounds_make_empty();
    EXPECT_TRUE(lh_memory_bounds_equals_of(&empty, lh_null, lh_null));
}

TEST(memory_bounds_swap, swaps_and_clears)
{
    unsigned char buf[8];
    lh_memory_bounds_t a = bounds(p(buf), p(buf + 2));
    lh_memory_bounds_t b = bounds(p(buf + 4), p(buf + 8));

    lh_memory_bounds_swap_v(&a, &b);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&a, p(buf + 4), p(buf + 8)));
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, p(buf), p(buf + 2)));

    lh_memory_bounds_swap_and_clear(&a, &b);
    EXPECT_TRUE(lh_memory_bounds_equals_of(&a, p(buf), p(buf + 2)));
    EXPECT_TRUE(lh_memory_bounds_equals_of(&b, lh_null, lh_null));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_bounds_unpack_v, rejects_uninitialized_death)
{
    lh_memory_bounds_t b = bounds(lh_null, lh_null);
    lh_ptr begin = lh_null;

    LH_EXPECT_DEATH(lh_memory_bounds_unpack_v(&b, &begin, nullptr));
}

TEST(memory_bounds_make_v, rejects_invalid_range_death)
{
    unsigned char buf[2];

    LH_EXPECT_DEATH((void)lh_memory_bounds_make_v(p(buf + 1), p(buf)));
}

TEST(memory_bounds_make_by_size, rejects_zero_size_death)
{
    unsigned char buf[1];

    LH_EXPECT_DEATH((void)lh_memory_bounds_make_by_size(p(buf), 0));
}

TEST(memory_bounds_make_by_size, rejects_null_begin_death)
{
    LH_EXPECT_DEATH((void)lh_memory_bounds_make_by_size(lh_null, 1));
}

TEST(memory_bounds_make_slice, rejects_invalid_bounds_death)
{
    unsigned char buf[2];
    lh_memory_bounds_t b = bounds(p(buf), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_bounds_make_slice(&b));
}

TEST(memory_bounds_next_value, rejects_end_boundary_death)
{
    unsigned char buf[2] = {1, 2};
    lh_memory_bounds_t b = bounds(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_next_value(&b, p(buf + 1)));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
