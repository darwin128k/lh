#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/expect/death.h>
#include <lh/memory/bounds/slice.h>
#include <lh/null.h>

namespace
{

lh_ptr
p(unsigned char *ptr)
{
    return lh_cast_static(lh_ptr, ptr);
}

lh_memory_bounds_slice_t
slice(lh_ptr begin, lh_ptr end)
{
    return {begin, end};
}

TEST(memory_bounds_slice_unpack, skips_null_output_pointers)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    lh_ptr begin = lh_null;
    lh_memory_bounds_slice_unpack(&s, &begin, nullptr);
    EXPECT_EQ(begin, p(buf));

    lh_ptr end = lh_null;
    lh_memory_bounds_slice_unpack(&s, nullptr, &end);
    EXPECT_EQ(end, p(buf + 3));
}

TEST(memory_bounds_slice_get_begin, returns_stored_begin)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_get_begin(&s), p(buf + 1));
}

TEST(memory_bounds_slice_get_end, returns_stored_end)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_get_end(&s), p(buf + 3));
}

TEST(memory_bounds_slice_get_flags, identifies_initialized_endpoints)
{
    unsigned char buf[2];

    lh_memory_bounds_slice_t none = slice(lh_null, lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&none), lh_memory_bounds_slice_flags_uninitialized);

    lh_memory_bounds_slice_t begin = slice(p(buf), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&begin),
              lh_memory_bounds_slice_flags_initialized_begin);

    lh_memory_bounds_slice_t end = slice(lh_null, p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&end), lh_memory_bounds_slice_flags_initialized_end);

    lh_memory_bounds_slice_t both = slice(p(buf), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_flags(&both), lh_memory_bounds_slice_flags_initialized);
}

TEST(memory_bounds_slice_is_uninitialized, returns_true_for_empty_endpoints)
{
    lh_memory_bounds_slice_t s = slice(lh_null, lh_null);

    EXPECT_TRUE(lh_memory_bounds_slice_is_uninitialized(&s));
}

TEST(memory_bounds_slice_is_initialized, returns_true_for_both_endpoints)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t both = slice(p(buf), p(buf + 1));

    EXPECT_TRUE(lh_memory_bounds_slice_is_initialized(&both));
}

TEST(memory_bounds_slice_get_direction, distinguishes_forward_backward_and_unknown)
{
    unsigned char buf[4];

    lh_memory_bounds_slice_t unknown = slice(p(buf), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&unknown),
              lh_memory_bounds_slice_direction_unknown);

    lh_memory_bounds_slice_t forward = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&forward),
              lh_memory_bounds_slice_direction_forward);

    lh_memory_bounds_slice_t backward = slice(p(buf + 3), p(buf));
    EXPECT_EQ(lh_memory_bounds_slice_get_direction(&backward),
              lh_memory_bounds_slice_direction_backward);
}

TEST(memory_bounds_slice_is_forward, returns_true_for_forward_slice)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t forward = slice(p(buf), p(buf + 3));

    EXPECT_TRUE(lh_memory_bounds_slice_is_forward(&forward));
}

TEST(memory_bounds_slice_is_backward, returns_true_for_backward_slice)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t backward = slice(p(buf + 3), p(buf));

    EXPECT_TRUE(lh_memory_bounds_slice_is_backward(&backward));
}

TEST(memory_bounds_slice_is_valid, returns_true_for_forward_slice)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t forward = slice(p(buf), p(buf + 3));
    lh_memory_bounds_slice_t backward = slice(p(buf + 3), p(buf));

    EXPECT_TRUE(lh_memory_bounds_slice_is_valid(&forward));
    EXPECT_FALSE(lh_memory_bounds_slice_is_valid(&backward));
}

TEST(memory_bounds_slice_get_size, closed_range_includes_both_endpoints)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_size(&s), 4u);
}

TEST(memory_bounds_slice_get_begin_v, returns_validated_begin)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_begin_v(&s), p(buf + 1));
}

TEST(memory_bounds_slice_get_end_v, returns_validated_end)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_end_v(&s), p(buf + 4));
}

TEST(memory_bounds_slice_is_empty, uninitialized_slice_is_empty)
{
    lh_memory_bounds_slice_t s = slice(lh_null, lh_null);

    EXPECT_TRUE(lh_memory_bounds_slice_is_empty(&s));
}

TEST(memory_bounds_slice_multiple_of, checks_size_divisibility)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_TRUE(lh_memory_bounds_slice_multiple_of(&s, 4));
    EXPECT_FALSE(lh_memory_bounds_slice_multiple_of(&s, 3));
}

TEST(memory_bounds_slice_aligned_is_begin_aligned, checks_begin_alignment)
{
    alignas(16) unsigned char buf[32];
    lh_memory_bounds_slice_t aligned = slice(p(buf), p(buf + 16));
    lh_memory_bounds_slice_t unaligned = slice(p(buf + 1), p(buf + 16));

    EXPECT_TRUE(lh_memory_bounds_slice_aligned_is_begin_aligned(&aligned, 16));
    EXPECT_FALSE(lh_memory_bounds_slice_aligned_is_begin_aligned(&unaligned, 16));
}

TEST(memory_bounds_slice_is_aligned, checks_both_endpoints)
{
    alignas(16) unsigned char buf[32];
    lh_memory_bounds_slice_t aligned = slice(p(buf), p(buf + 16));
    lh_memory_bounds_slice_t unaligned_end = slice(p(buf), p(buf + 15));

    EXPECT_TRUE(lh_memory_bounds_slice_is_aligned(&aligned, 16));
    EXPECT_FALSE(lh_memory_bounds_slice_is_aligned(&unaligned_end, 16));
}

TEST(memory_bounds_slice_contains_ptr, closed_range_contains_both_endpoints)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 1)));
    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 4)));
    EXPECT_TRUE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 3)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_ptr(&s, p(buf)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_ptr(&s, p(buf + 5)));
}

TEST(memory_bounds_slice_contains_range, accepts_inner_ranges)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t outer = slice(p(buf + 1), p(buf + 6));

    EXPECT_TRUE(lh_memory_bounds_slice_contains_range(&outer, p(buf + 2), p(buf + 4)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_range(&outer, p(buf), p(buf + 4)));
    EXPECT_FALSE(lh_memory_bounds_slice_contains_range(&outer, p(buf + 2), p(buf + 7)));
}

TEST(memory_bounds_slice_contains, accepts_inner_slice)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t outer = slice(p(buf + 1), p(buf + 6));
    lh_memory_bounds_slice_t inner = slice(p(buf + 2), p(buf + 4));

    EXPECT_TRUE(lh_memory_bounds_slice_contains(&outer, &inner));
}

TEST(memory_bounds_slice_overlaps_of, accepts_overlapping_ranges)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t s = slice(p(buf + 2), p(buf + 5));

    EXPECT_TRUE(lh_memory_bounds_slice_overlaps_of(&s, p(buf + 1), p(buf + 2)));
    EXPECT_TRUE(lh_memory_bounds_slice_overlaps_of(&s, p(buf + 4), p(buf + 6)));
    EXPECT_TRUE(lh_memory_bounds_slice_overlaps_of(&s, p(buf + 3), p(buf + 4)));
    EXPECT_FALSE(lh_memory_bounds_slice_overlaps_of(&s, p(buf), p(buf + 1)));
    EXPECT_FALSE(lh_memory_bounds_slice_overlaps_of(&s, p(buf + 6), p(buf + 7)));
}

TEST(memory_bounds_slice_overlaps, accepts_overlapping_slice)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t left = slice(p(buf + 1), p(buf + 3));
    lh_memory_bounds_slice_t middle = slice(p(buf + 3), p(buf + 5));
    lh_memory_bounds_slice_t right = slice(p(buf + 4), p(buf + 6));

    EXPECT_TRUE(lh_memory_bounds_slice_overlaps(&left, &middle));
    EXPECT_FALSE(lh_memory_bounds_slice_overlaps(&left, &right));
}

TEST(memory_bounds_slice_overlaps_v, accepts_valid_overlapping_slice)
{
    unsigned char buf[8];
    lh_memory_bounds_slice_t left = slice(p(buf + 1), p(buf + 3));
    lh_memory_bounds_slice_t middle = slice(p(buf + 3), p(buf + 5));
    lh_memory_bounds_slice_t right = slice(p(buf + 4), p(buf + 6));

    EXPECT_TRUE(lh_memory_bounds_slice_overlaps_v(&left, &middle));
    EXPECT_FALSE(lh_memory_bounds_slice_overlaps_v(&left, &right));
}

TEST(memory_bounds_slice_is_valid_offset, validates_offsets_within_closed_size)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_TRUE(lh_memory_bounds_slice_is_valid_offset(&s, 0));
    EXPECT_TRUE(lh_memory_bounds_slice_is_valid_offset(&s, 3));
    EXPECT_FALSE(lh_memory_bounds_slice_is_valid_offset(&s, 4));
}

TEST(memory_bounds_slice_get_offset_from_begin, returns_offset_from_begin)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_begin(&s, p(buf + 1)), 0u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_begin(&s, p(buf + 3)), 2u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_begin(&s, p(buf + 4)), 3u);
}

TEST(memory_bounds_slice_get_offset_from_end, returns_offset_from_end)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_end(&s, p(buf + 4)), 0u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_end(&s, p(buf + 3)), 1u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_end(&s, p(buf + 1)), 3u);
}

TEST(memory_bounds_slice_get_ptr_from_begin, returns_ptr_from_begin_offset)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_begin(&s, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_begin(&s, 3), p(buf + 4));
}

TEST(memory_bounds_slice_get_ptr_from_end, returns_ptr_from_end_offset)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_end(&s, 0), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_from_end(&s, 1), p(buf + 3));
}

TEST(memory_bounds_slice_get_ptr_by_offset, supports_signed_offsets_from_both_ends)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_by_offset(&s, 2), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_by_offset(&s, -1), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_get_ptr_by_offset(&s, -2), p(buf + 3));
}

TEST(memory_bounds_slice_get_offset_from_ptr, null_pointer_uses_absolute_signed_offset)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, lh_null, 0), 0u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, lh_null, 2), 2u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, lh_null, -1), 3u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, lh_null, -4), 0u);
}

TEST(memory_bounds_slice_get_offset_from_ptr, shifts_relative_to_pointer_inside_slice)
{
    unsigned char buf[6];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, p(buf + 3), 0), 2u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, p(buf + 3), 2), 4u);
    EXPECT_EQ(lh_memory_bounds_slice_get_offset_from_ptr(&s, p(buf + 3), -2), 0u);
}

TEST(memory_bounds_slice_seek_ptr, null_pointer_uses_absolute_signed_offset)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, lh_null, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, lh_null, 2), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, lh_null, -1), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, lh_null, -4), p(buf + 1));
}

TEST(memory_bounds_slice_seek_ptr, seeks_relative_to_pointer_inside_slice)
{
    unsigned char buf[6];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), 0), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), 1), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), 2), p(buf + 5));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), -1), p(buf + 2));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), -2), p(buf + 1));
}

TEST(memory_bounds_slice_seek_ptr, supports_boundary_relative_moves)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 1), 3), p(buf + 4));
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 4), -3), p(buf + 1));
}

TEST(memory_bounds_slice_seek_ptr, returns_null_after_boundary_shift)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), 1), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 1), -1), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 2), 3), lh_null);
    EXPECT_EQ(lh_memory_bounds_slice_seek_ptr(&s, p(buf + 2), -2), lh_null);
}

TEST(memory_bounds_slice_next_ptr, moves_by_one_byte)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_next_ptr(&s, p(buf + 1)), p(buf + 2));
    EXPECT_EQ(lh_memory_bounds_slice_next_ptr(&s, p(buf + 3)), p(buf + 4));
}

TEST(memory_bounds_slice_prev_ptr, moves_by_one_byte)
{
    unsigned char buf[5];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_prev_ptr(&s, p(buf + 4)), p(buf + 3));
    EXPECT_EQ(lh_memory_bounds_slice_prev_ptr(&s, p(buf + 2)), p(buf + 1));
}

TEST(memory_bounds_slice_next_ptr, null_pointer_uses_absolute_shift)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_next_ptr(&s, lh_null), p(buf + 1));
}

TEST(memory_bounds_slice_next_ptr, returns_null_from_end_pointer)
{
    unsigned char buf[3];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));

    EXPECT_EQ(lh_memory_bounds_slice_next_ptr(&s, p(buf + 2)), lh_null);
}

TEST(memory_bounds_slice_prev_ptr, null_pointer_uses_absolute_shift)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_prev_ptr(&s, lh_null), p(buf + 3));
}

TEST(memory_bounds_slice_prev_ptr, returns_null_from_begin_pointer)
{
    unsigned char buf[3];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));

    EXPECT_EQ(lh_memory_bounds_slice_prev_ptr(&s, p(buf)), lh_null);
}

TEST(memory_bounds_slice_get_value_from_begin, reads_byte_from_begin_offset)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_begin(&s, 0), 20);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_begin(&s, 3), 50);
}

TEST(memory_bounds_slice_get_value_from_end, reads_byte_from_end_offset)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_end(&s, 0), 50);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_from_end(&s, 1), 40);
}

TEST(memory_bounds_slice_get_value_by_offset, reads_byte_by_signed_offset)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_value_by_offset(&s, 1), 30);
    EXPECT_EQ(lh_memory_bounds_slice_get_value_by_offset(&s, -1), 50);
}

TEST(memory_bounds_slice_get_begin_value, reads_begin_value)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_begin_value(&s), 20);
}

TEST(memory_bounds_slice_get_end_value, reads_end_value)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_bounds_slice_get_end_value(&s), 50);
}

TEST(memory_bounds_slice_seek_value, reads_value_at_current_pointer)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_seek_value(&s, p(buf + 2)), 30);
}

TEST(memory_bounds_slice_seek_value, null_pointer_reads_begin_value)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_seek_value(&s, lh_null), 20);
}

TEST(memory_bounds_slice_next_value, reads_next_value)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_next_value(&s, p(buf + 1)), 30);
    EXPECT_EQ(lh_memory_bounds_slice_next_value(&s, p(buf + 2)), 40);
}

TEST(memory_bounds_slice_prev_value, reads_prev_value)
{
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    EXPECT_EQ(lh_memory_bounds_slice_prev_value(&s, p(buf + 3)), 30);
    EXPECT_EQ(lh_memory_bounds_slice_prev_value(&s, p(buf + 2)), 20);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_bounds_slice_unpack, null_self_death)
{
    lh_ptr begin = lh_null;
    LH_EXPECT_DEATH(lh_memory_bounds_slice_unpack(nullptr, &begin, nullptr));
}

TEST(memory_bounds_slice_unpack_v, rejects_backward_range_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf));
    lh_ptr begin = lh_null;

    LH_EXPECT_DEATH(lh_memory_bounds_slice_unpack_v(&s, &begin, nullptr));
}

TEST(memory_bounds_slice_get_size, rejects_uninitialized_range_death)
{
    lh_memory_bounds_slice_t s = slice(lh_null, lh_null);

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_size(&s));
}

TEST(memory_bounds_slice_multiple_of, rejects_zero_alignment_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_multiple_of(&s, 0));
}

TEST(memory_bounds_slice_aligned_is_begin_aligned, rejects_non_power_of_two_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_aligned_is_begin_aligned(&s, 3));
}

TEST(memory_bounds_slice_aligned_is_begin_aligned, rejects_invalid_range_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_aligned_is_begin_aligned(&s, 1));
}

TEST(memory_bounds_slice_is_aligned, rejects_non_power_of_two_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_is_aligned(&s, 3));
}

TEST(memory_bounds_slice_is_aligned, rejects_invalid_range_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_is_aligned(&s, 1));
}

TEST(memory_bounds_slice_get_ptr_from_begin, rejects_out_of_range_offset_death)
{
    unsigned char buf[2];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_ptr_from_begin(&s, 2));
}

TEST(memory_bounds_slice_get_offset_from_begin, rejects_pointer_before_slice_death)
{
    unsigned char buf[3];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_offset_from_begin(&s, p(buf)));
}

TEST(memory_bounds_slice_get_offset_from_end, rejects_pointer_after_slice_death)
{
    unsigned char buf[3];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_get_offset_from_end(&s, p(buf + 2)));
}

TEST(memory_bounds_slice_seek_ptr, rejects_pointer_before_slice_death)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 3));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_seek_ptr(&s, p(buf), 0));
}

TEST(memory_bounds_slice_seek_ptr, rejects_pointer_after_slice_death)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_seek_ptr(&s, p(buf + 3), 0));
}

TEST(memory_bounds_slice_overlaps_v, rejects_invalid_other_death)
{
    unsigned char buf[4];
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));
    lh_memory_bounds_slice_t other = slice(p(buf + 3), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_overlaps_v(&s, &other));
}

TEST(memory_bounds_slice_seek_value, rejects_pointer_before_slice_death)
{
    unsigned char buf[3] = {10, 20, 30};
    lh_memory_bounds_slice_t s = slice(p(buf + 1), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_seek_value(&s, p(buf)));
}

TEST(memory_bounds_slice_next_value, rejects_end_boundary_death)
{
    unsigned char buf[3] = {10, 20, 30};
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_next_value(&s, p(buf + 2)));
}

TEST(memory_bounds_slice_prev_value, rejects_begin_boundary_death)
{
    unsigned char buf[3] = {10, 20, 30};
    lh_memory_bounds_slice_t s = slice(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_bounds_slice_prev_value(&s, p(buf)));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
