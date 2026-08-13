#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/view/slice.h>
#include <lh/null.h>

namespace
{

const lh_ptr
p(const unsigned char *ptr)
{
    return ptr;
}

lh_memory_view_slice_t
slice(const lh_ptr begin, const lh_ptr end)
{
    return {begin, end};
}

/* ── unpack ──────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_unpack, returns_const_endpoints)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    const lh_ptr begin = lh_null;
    const lh_ptr end = lh_null;

    lh_memory_view_slice_unpack(&s, &begin, &end);

    EXPECT_EQ(begin, p(buf));
    EXPECT_EQ(end, p(buf + 3));
}

/* ── flags / initialization ─────────────────────────────────────────────── */

TEST(memory_view_slice_get_flags, uninitialized_when_null)
{
    lh_memory_view_slice_t s = slice(lh_null, lh_null);
    EXPECT_EQ(lh_memory_view_slice_get_flags(&s), lh_memory_view_slice_flags_uninitialized);
}

TEST(memory_view_slice_get_flags, initialized_when_both_set)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));
    EXPECT_EQ(lh_memory_view_slice_get_flags(&s), lh_memory_view_slice_flags_initialized);
}

TEST(memory_view_slice_is_uninitialized, true_for_empty_slice)
{
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    EXPECT_TRUE(lh_memory_view_slice_is_uninitialized(&s));
}

TEST(memory_view_slice_is_initialized, true_for_valid_slice)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));
    EXPECT_TRUE(lh_memory_view_slice_is_initialized(&s));
}

/* ── direction / validity ───────────────────────────────────────────────── */

TEST(memory_view_slice_get_direction, forward_when_begin_le_end)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));
    EXPECT_EQ(lh_memory_view_slice_get_direction(&s), lh_memory_view_slice_direction_forward);
}

TEST(memory_view_slice_get_direction, unknown_when_uninitialized)
{
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    EXPECT_EQ(lh_memory_view_slice_get_direction(&s), lh_memory_view_slice_direction_unknown);
}

TEST(memory_view_slice_is_forward, true_for_valid_range)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));
    EXPECT_TRUE(lh_memory_view_slice_is_forward(&s));
}

TEST(memory_view_slice_is_backward, true_when_end_lt_begin)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf + 3), p(buf + 1));
    EXPECT_TRUE(lh_memory_view_slice_is_backward(&s));
}

TEST(memory_view_slice_is_valid, false_for_uninitialized)
{
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    EXPECT_FALSE(lh_memory_view_slice_is_valid(&s));
}

/* ── make ───────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_make_by_size, stores_closed_const_span)
{
    const unsigned char buf[4] = {10, 20, 30, 40};

    lh_memory_view_slice_t s = lh_memory_view_slice_make_by_size(p(buf), 4);

    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf), p(buf + 3)));
    EXPECT_EQ(lh_memory_view_slice_get_size(&s), 4u);
}

/* ── size / empty ───────────────────────────────────────────────────────── */

TEST(memory_view_slice_is_empty, true_for_uninitialized)
{
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    EXPECT_TRUE(lh_memory_view_slice_is_empty(&s));
}

TEST(memory_view_slice_is_empty, false_for_valid_slice)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));
    EXPECT_FALSE(lh_memory_view_slice_is_empty(&s));
}

TEST(memory_view_slice_is_valid_offset, true_within_bounds_false_at_size)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_is_valid_offset(&s, 0));
    EXPECT_TRUE(lh_memory_view_slice_is_valid_offset(&s, 3));
    EXPECT_FALSE(lh_memory_view_slice_is_valid_offset(&s, 4));
}

/* ── offsets ────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_get_offset_from_begin, returns_byte_distance)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_begin(&s, p(buf)), 0u);
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_begin(&s, p(buf + 2)), 2u);
}

TEST(memory_view_slice_get_offset_from_end, returns_byte_distance_from_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_end(&s, p(buf + 3)), 0u);
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_end(&s, p(buf + 1)), 2u);
}

TEST(memory_view_slice_get_offset_from_ptr, positive_offset)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_ptr(&s, p(buf + 1), 1), 2u);
}

TEST(memory_view_slice_get_offset_from_ptr, negative_offset)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_ptr(&s, p(buf + 2), -1), 1u);
}

TEST(memory_view_slice_get_offset_from_ptr, null_ptr_uses_by_offset)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_offset_from_ptr(&s, lh_null, 2), 2u);
}

/* ── ptr access ─────────────────────────────────────────────────────────── */

TEST(memory_view_slice_get_ptr_from_begin, returns_const_pointer)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    const lh_ptr ptr = lh_memory_view_slice_get_ptr_from_begin(&s, 2);

    EXPECT_EQ(ptr, p(buf + 2));
    EXPECT_EQ(lh_memory_view_slice_get_value_from_begin(&s, 2), 30);
}

TEST(memory_view_slice_seek_ptr, keeps_const_navigation)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    EXPECT_EQ(lh_memory_view_slice_next_ptr(&s, p(buf + 1)), p(buf + 2));
    EXPECT_EQ(lh_memory_view_slice_prev_ptr(&s, p(buf + 2)), p(buf + 1));
    EXPECT_EQ(lh_memory_view_slice_seek_ptr(&s, lh_null, -1), p(buf + 3));
}

TEST(memory_view_slice_seek_ptr, returns_null_on_out_of_range)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_next_ptr(&s, p(buf + 3)), lh_null);
    EXPECT_EQ(lh_memory_view_slice_prev_ptr(&s, p(buf)), lh_null);
}

/* ── contains ───────────────────────────────────────────────────────────── */

TEST(memory_view_slice_contains_ptr, true_for_begin_and_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_contains_ptr(&s, p(buf)));
    EXPECT_TRUE(lh_memory_view_slice_contains_ptr(&s, p(buf + 3)));
    EXPECT_FALSE(lh_memory_view_slice_contains_ptr(&s, p(buf + 4)));
}

TEST(memory_view_slice_contains_range, true_when_within_false_when_outside)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_contains_range(&s, p(buf + 1), p(buf + 2)));
    EXPECT_FALSE(lh_memory_view_slice_contains_range(&s, p(buf + 2), p(buf + 4)));
}

TEST(memory_view_slice_contains, true_when_other_is_subslice)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t outer = slice(p(buf), p(buf + 5));
    lh_memory_view_slice_t inner = slice(p(buf + 1), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_slice_contains(&outer, &inner));
    EXPECT_FALSE(lh_memory_view_slice_contains(&inner, &outer));
}

/* ── overlaps ───────────────────────────────────────────────────────────── */

TEST(memory_view_slice_overlaps_of, true_for_partial_overlap_false_when_disjoint)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_overlaps_of(&s, p(buf + 2), p(buf + 5)));
    EXPECT_FALSE(lh_memory_view_slice_overlaps_of(&s, p(buf + 4), p(buf + 5)));
}

TEST(memory_view_slice_overlaps, true_for_overlapping_slices)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t a = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_t b = slice(p(buf + 2), p(buf + 5));
    EXPECT_TRUE(lh_memory_view_slice_overlaps(&a, &b));
}

/* ── alignment / multiple ───────────────────────────────────────────────── */

TEST(memory_view_slice_is_multiple_of, true_when_size_divisible)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_is_multiple_of(&s, 4));
    EXPECT_TRUE(lh_memory_view_slice_is_multiple_of(&s, 2));
    EXPECT_FALSE(lh_memory_view_slice_is_multiple_of(&s, 3));
}

TEST(memory_view_slice_is_begin_aligned, true_when_begin_aligned)
{
    alignas(4) const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_is_begin_aligned(&s, 4));
}

TEST(memory_view_slice_is_aligned, true_when_both_ends_aligned_to_one)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_is_aligned(&s, 1));
}

/* ── equals ─────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_equals_of, true_for_same_range_false_for_different)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf), p(buf + 3)));
    EXPECT_FALSE(lh_memory_view_slice_equals_of(&s, p(buf), p(buf + 2)));
}

TEST(memory_view_slice_equals, true_for_equal_slices)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t a = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_t b = slice(p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_equals(&a, &b));
}

/* ── values ─────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_get_value_from_begin, returns_byte_at_offset)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_value_from_begin(&s, 0), 10);
    EXPECT_EQ(lh_memory_view_slice_get_value_from_begin(&s, 3), 40);
}

TEST(memory_view_slice_get_value_from_end, returns_byte_at_offset_from_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_value_from_end(&s, 0), 40);
    EXPECT_EQ(lh_memory_view_slice_get_value_from_end(&s, 3), 10);
}

TEST(memory_view_slice_get_value_by_offset, positive_and_negative)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_value(&s, 1), 20);
    EXPECT_EQ(lh_memory_view_slice_get_value(&s, -1), 40);
}

TEST(memory_view_slice_get_begin_value, returns_first_byte)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_first_value(&s), 10);
}

TEST(memory_view_slice_get_end_value, returns_last_byte)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_get_last_value(&s), 40);
}

TEST(memory_view_slice_seek_value, returns_value_at_ptr)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_seek_value(&s, p(buf + 2)), 30);
}

TEST(memory_view_slice_next_value, returns_value_at_next_ptr)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_next_value(&s, p(buf + 1)), 30);
}

TEST(memory_view_slice_prev_value, returns_value_at_prev_ptr)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    EXPECT_EQ(lh_memory_view_slice_prev_value(&s, p(buf + 2)), 20);
}

/* ── slicing ────────────────────────────────────────────────────────────── */

TEST(memory_view_slice_make_from_offset, returns_const_subslice)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t base = slice(p(buf), p(buf + 5));

    lh_memory_view_slice_t s = lh_memory_view_slice_make_from_offset(&base, 2, 3);

    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf + 2), p(buf + 4)));
}

TEST(memory_view_slice_make_from_begin, takes_n_from_start)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_t r = lh_memory_view_slice_make_from_begin(&s, 2);
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&r, p(buf), p(buf + 1)));
}

TEST(memory_view_slice_make_from_end, takes_n_from_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_t r = lh_memory_view_slice_make_from_end(&s, 2);
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&r, p(buf + 2), p(buf + 3)));
}

TEST(memory_view_slice_make_between, returns_subslice_between_ptrs)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 5));
    lh_memory_view_slice_t r = lh_memory_view_slice_make_between(&s, p(buf + 1), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&r, p(buf + 1), p(buf + 4)));
}

TEST(memory_view_slice_drop_first, returns_empty_when_all_removed)
{
    const unsigned char buf[3] = {10, 20, 30};
    lh_memory_view_slice_t base = slice(p(buf), p(buf + 2));

    lh_memory_view_slice_t s = lh_memory_view_slice_drop_first(&base, 3);

    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, lh_null, lh_null));
}

TEST(memory_view_slice_drop_last, removes_n_from_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_t r = lh_memory_view_slice_drop_last(&s, 2);
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&r, p(buf), p(buf + 1)));
}

TEST(memory_view_slice_trim, removes_from_both_sides)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 5));
    lh_memory_view_slice_t r = lh_memory_view_slice_trim(&s, 1, 2);
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&r, p(buf + 1), p(buf + 3)));
}

/* ── mutation ───────────────────────────────────────────────────────────── */

TEST(memory_view_slice_clear, resets_to_uninitialized)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_clear(&s);
    EXPECT_TRUE(lh_memory_view_slice_is_uninitialized(&s));
}

TEST(memory_view_slice_assign_v, copies_valid_slice)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    lh_memory_view_slice_t other = slice(p(buf), p(buf + 3));
    lh_memory_view_slice_assign_v(&s, &other);
    EXPECT_TRUE(lh_memory_view_slice_equals(&s, &other));
}

TEST(memory_view_slice_set_v, sets_by_range)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    lh_memory_view_slice_set_v(&s, p(buf), p(buf + 3));
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf), p(buf + 3)));
}

TEST(memory_view_slice_set_by_size, sets_by_begin_and_size)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = lh_memory_view_slice_make_empty();
    lh_memory_view_slice_set_by_size(&s, p(buf), 4);
    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf), p(buf + 3)));
}

TEST(memory_view_slice_init_by_other, copies_const_endpoints)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(lh_null, lh_null);
    lh_memory_view_slice_t other = slice(p(buf + 1), p(buf + 3));

    lh_memory_view_slice_init_by_other(&s, &other);

    EXPECT_TRUE(lh_memory_view_slice_equals(&s, &other));
}

/* ── death tests ────────────────────────────────────────────────────────── */

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_view_slice_make_v, rejects_invalid_range_death)
{
    const unsigned char buf[4] = {10, 20, 30, 40};

    LH_EXPECT_DEATH((void)lh_memory_view_slice_make_v(p(buf + 3), p(buf + 1)));
}

TEST(memory_view_slice_get_ptr_from_begin, rejects_out_of_range_offset_death)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 1));

    LH_EXPECT_DEATH((void)lh_memory_view_slice_get_ptr_from_begin(&s, 2));
}

TEST(memory_view_slice_get_offset_from_begin, rejects_out_of_range_ptr_death)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    LH_EXPECT_DEATH((void)lh_memory_view_slice_get_offset_from_begin(&s, p(buf + 4)));
}

TEST(memory_view_slice_is_multiple_of, rejects_zero_alignment_death)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    LH_EXPECT_DEATH((void)lh_memory_view_slice_is_multiple_of(&s, 0));
}

TEST(memory_view_slice_make_between, rejects_out_of_bounds_ptrs_death)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_slice_t s = slice(p(buf), p(buf + 3));

    LH_EXPECT_DEATH((void)lh_memory_view_slice_make_between(&s, p(buf + 1), p(buf + 5)));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
