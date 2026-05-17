#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/expect/death.h>
#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/null.h>

namespace
{

const lh_ptr
p(const unsigned char *ptr)
{
    return lh_cast_static(const lh_ptr, ptr);
}

lh_memory_view_t
view(const lh_ptr begin, const lh_ptr end)
{
    return {begin, end};
}

TEST(memory_view_unpack, skips_null_output_pointers)
{
    const unsigned char buf[4] = {};
    lh_memory_view_t v = view(p(buf), p(buf + 4));

    const lh_ptr begin = lh_null;
    lh_memory_view_unpack(&v, &begin, nullptr);
    EXPECT_EQ(begin, p(buf));

    const lh_ptr end = lh_null;
    lh_memory_view_unpack(&v, nullptr, &end);
    EXPECT_EQ(end, p(buf + 4));
}

TEST(memory_view_getters, return_stored_endpoints)
{
    const unsigned char buf[4] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_view_get_begin(&v), p(buf + 1));
    EXPECT_EQ(lh_memory_view_get_end(&v), p(buf + 4));
}

TEST(memory_view_make_slice, copies_half_open_view_as_closed_slice)
{
    const unsigned char buf[8] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

    lh_memory_view_slice_t s = lh_memory_view_make_slice(&v);

    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, p(buf + 1), p(buf + 4)));
}

TEST(memory_view_make_slice, copies_uninitialized_view_as_empty_slice)
{
    lh_memory_view_t v = view(lh_null, lh_null);

    lh_memory_view_slice_t s = lh_memory_view_make_slice(&v);

    EXPECT_TRUE(lh_memory_view_slice_equals_of(&s, lh_null, lh_null));
}

TEST(memory_view_classification, distinguishes_states)
{
    const unsigned char buf[4] = {};

    lh_memory_view_t empty = view(lh_null, lh_null);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&empty));
    EXPECT_FALSE(lh_memory_view_is_initialized(&empty));
    EXPECT_TRUE(lh_memory_view_is_empty(&empty));

    lh_memory_view_t unknown = view(p(buf), lh_null);
    EXPECT_EQ(lh_memory_view_get_direction(&unknown), lh_memory_view_slice_direction_unknown);

    lh_memory_view_t forward = view(p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_is_initialized(&forward));
    EXPECT_EQ(lh_memory_view_get_direction(&forward), lh_memory_view_slice_direction_forward);
    EXPECT_TRUE(lh_memory_view_is_forward(&forward));
    EXPECT_TRUE(lh_memory_view_is_valid(&forward));

    lh_memory_view_t equal = view(p(buf), p(buf));
    EXPECT_EQ(lh_memory_view_get_direction(&equal), lh_memory_view_slice_direction_backward);
    EXPECT_TRUE(lh_memory_view_is_backward(&equal));
    EXPECT_FALSE(lh_memory_view_is_valid(&equal));
}

TEST(memory_view_validated_access, returns_endpoints_and_size)
{
    const unsigned char buf[8] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

    const lh_ptr begin = lh_null;
    const lh_ptr end = lh_null;
    lh_memory_view_unpack_v(&v, &begin, &end);

    EXPECT_EQ(begin, p(buf + 1));
    EXPECT_EQ(end, p(buf + 5));
    EXPECT_EQ(lh_memory_view_get_begin_v(&v), p(buf + 1));
    EXPECT_EQ(lh_memory_view_get_end_v(&v), p(buf + 5));
    EXPECT_EQ(lh_memory_view_get_size(&v), 4u);
}

TEST(memory_view_offsets, validate_half_open_offsets)
{
    const unsigned char buf[6] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

    EXPECT_TRUE(lh_memory_view_is_valid_offset(&v, 0));
    EXPECT_TRUE(lh_memory_view_is_valid_offset(&v, 3));
    EXPECT_FALSE(lh_memory_view_is_valid_offset(&v, 4));

    EXPECT_EQ(lh_memory_view_get_offset_from_begin(&v, p(buf + 1)), 0u);
    EXPECT_EQ(lh_memory_view_get_offset_from_begin(&v, p(buf + 4)), 3u);
    EXPECT_EQ(lh_memory_view_get_offset_from_end(&v, p(buf + 4)), 0u);
    EXPECT_EQ(lh_memory_view_get_offset_from_end(&v, p(buf + 1)), 3u);
}

TEST(memory_view_contains, uses_half_open_ranges)
{
    const unsigned char buf[8] = {};
    lh_memory_view_t outer = view(p(buf + 1), p(buf + 6));
    lh_memory_view_t inner = view(p(buf + 2), p(buf + 5));

    EXPECT_TRUE(lh_memory_view_contains_ptr(&outer, p(buf + 1)));
    EXPECT_TRUE(lh_memory_view_contains_ptr(&outer, p(buf + 5)));
    EXPECT_FALSE(lh_memory_view_contains_ptr(&outer, p(buf + 6)));

    EXPECT_TRUE(lh_memory_view_contains_of(&outer, p(buf + 2), p(buf + 5)));
    EXPECT_FALSE(lh_memory_view_contains_of(&outer, p(buf), p(buf + 5)));
    EXPECT_TRUE(lh_memory_view_contains(&outer, &inner));
}

TEST(memory_view_pointer_access, supports_offsets_from_both_ends)
{
    const unsigned char buf[5] = {10, 20, 30, 40, 50};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_view_get_ptr_from_begin(&v, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_view_get_ptr_from_begin(&v, 3), p(buf + 4));
    EXPECT_EQ(lh_memory_view_get_ptr_from_end(&v, 0), p(buf + 4));
    EXPECT_EQ(lh_memory_view_get_ptr_from_end(&v, 1), p(buf + 3));
    EXPECT_EQ(lh_memory_view_get_ptr(&v, 2), p(buf + 3));
    EXPECT_EQ(lh_memory_view_get_ptr(&v, -1), p(buf + 4));

    EXPECT_EQ(lh_memory_view_get_value_from_begin(&v, 0), 20);
    EXPECT_EQ(lh_memory_view_get_value_from_end(&v, 0), 50);
    EXPECT_EQ(lh_memory_view_get_value(&v, -2), 40);
    EXPECT_EQ(lh_memory_view_get_first_value(&v), 20);
    EXPECT_EQ(lh_memory_view_get_last_value(&v), 50);
}

TEST(memory_view_seek, supports_absolute_and_relative_offsets)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

    EXPECT_EQ(lh_memory_view_get_offset_from_ptr(&v, lh_null, 0), 0u);
    EXPECT_EQ(lh_memory_view_get_offset_from_ptr(&v, lh_null, -1), 3u);
    EXPECT_EQ(lh_memory_view_get_offset_from_ptr(&v, p(buf + 2), 2), 3u);
    EXPECT_EQ(lh_memory_view_get_offset_from_ptr(&v, p(buf + 3), -2), 0u);

    EXPECT_EQ(lh_memory_view_seek_ptr(&v, lh_null, 0), p(buf + 1));
    EXPECT_EQ(lh_memory_view_seek_ptr(&v, p(buf + 2), 2), p(buf + 4));
    EXPECT_EQ(lh_memory_view_seek_ptr(&v, p(buf + 4), 1), lh_null);
    EXPECT_EQ(lh_memory_view_next_ptr(&v, p(buf + 2)), p(buf + 3));
    EXPECT_EQ(lh_memory_view_prev_ptr(&v, p(buf + 2)), p(buf + 1));

    EXPECT_EQ(lh_memory_view_seek_value(&v, p(buf + 2)), 30);
    EXPECT_EQ(lh_memory_view_next_value(&v, p(buf + 2)), 40);
    EXPECT_EQ(lh_memory_view_prev_value(&v, p(buf + 2)), 20);
}

TEST(memory_view_overlap_alignment_and_equality, reports_expected_values)
{
    alignas(16) const unsigned char buf[32] = {};
    lh_memory_view_t left = view(p(buf), p(buf + 8));
    lh_memory_view_t touching = view(p(buf + 8), p(buf + 12));
    lh_memory_view_t overlapping = view(p(buf + 7), p(buf + 12));

    EXPECT_TRUE(lh_memory_view_overlaps_of(&left, p(buf + 7), p(buf + 9)));
    EXPECT_FALSE(lh_memory_view_overlaps_range(&left, p(buf + 8), p(buf + 12)));
    EXPECT_FALSE(lh_memory_view_overlaps(&left, &touching));
    EXPECT_TRUE(lh_memory_view_overlaps_v(&left, &overlapping));

    EXPECT_TRUE(lh_memory_view_multiple_of(&left, 4));
    EXPECT_TRUE(lh_memory_view_is_multiple_of(&left, 8));
    EXPECT_TRUE(lh_memory_view_aligned_is_begin_aligned(&left, 16));
    EXPECT_TRUE(lh_memory_view_is_begin_aligned(&left, 16));
    EXPECT_FALSE(lh_memory_view_is_aligned(&left, 16));

    EXPECT_TRUE(lh_memory_view_equals_of(&left, p(buf), p(buf + 8)));
    EXPECT_TRUE(lh_memory_view_equals_range(&left, p(buf), p(buf + 8)));
    EXPECT_FALSE(lh_memory_view_equals(&left, &overlapping));
}

TEST(memory_view_raw_search_and_compare, delegate_to_raw_operations)
{
    const unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    const unsigned char needle[] = {'a', 'b'};
    const unsigned char mismatch[] = {'a', 'x'};
    lh_memory_view_t hay_view = view(p(hay), p(hay + 5));
    lh_memory_view_t needle_view = view(p(needle), p(needle + 2));
    lh_memory_view_t mismatch_view = view(p(mismatch), p(mismatch + 2));

    EXPECT_EQ(lh_memory_view_find_range(&hay_view, p(needle), p(needle + 2)), p(hay));
    EXPECT_EQ(lh_memory_view_find(&hay_view, &needle_view), p(hay));
    EXPECT_EQ(lh_memory_view_rfind_range(&hay_view, p(needle), p(needle + 2)), p(hay + 2));
    EXPECT_EQ(lh_memory_view_rfind(&hay_view, &needle_view), p(hay + 2));

    EXPECT_EQ(lh_memory_view_compare_range(&hay_view, p(needle), p(needle + 2)), lh_null);
    EXPECT_EQ(lh_memory_view_compare(&hay_view, &mismatch_view), p(hay + 1));
    EXPECT_EQ(lh_memory_view_rcompare_range(&hay_view, p(needle), p(needle + 2)), p(hay + 4));
    EXPECT_EQ(lh_memory_view_rcompare(&hay_view, &needle_view), p(hay + 4));
}

TEST(memory_view_mutation, clears_assigns_sets_and_initializes)
{
    const unsigned char buf[8] = {};
    lh_memory_view_t v = view(lh_null, lh_null);
    lh_memory_view_t other = view(p(buf + 1), p(buf + 5));

    lh_memory_view_assign_v(&v, &other);
    EXPECT_TRUE(lh_memory_view_equals(&v, &other));

    lh_memory_view_clear(&v);
    EXPECT_TRUE(lh_memory_view_equals_of(&v, lh_null, lh_null));

    lh_memory_view_set_v(&v, p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_equals_of(&v, p(buf), p(buf + 4)));

    lh_memory_view_set_by_size(&v, p(buf + 2), 3);
    EXPECT_TRUE(lh_memory_view_equals_of(&v, p(buf + 2), p(buf + 5)));

    lh_memory_view_init(&v, p(buf), p(buf + 2));
    EXPECT_TRUE(lh_memory_view_equals_of(&v, p(buf), p(buf + 2)));

    lh_memory_view_init_by_size(&v, p(buf + 1), 2);
    EXPECT_TRUE(lh_memory_view_equals_of(&v, p(buf + 1), p(buf + 3)));

    lh_memory_view_init_empty(&v);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));

    lh_memory_view_init_by_other(&v, &other);
    EXPECT_TRUE(lh_memory_view_equals(&v, &other));
}

TEST(memory_view_make, returns_validated_values)
{
    const unsigned char buf[8] = {};

    lh_memory_view_t by_end = lh_memory_view_make_v(p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_equals_of(&by_end, p(buf), p(buf + 4)));

    lh_memory_view_t by_size = lh_memory_view_make_by_size(p(buf + 1), 3);
    EXPECT_TRUE(lh_memory_view_equals_of(&by_size, p(buf + 1), p(buf + 4)));

    lh_memory_view_t empty = lh_memory_view_make_empty();
    EXPECT_TRUE(lh_memory_view_equals_of(&empty, lh_null, lh_null));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_view_unpack_v, rejects_uninitialized_death)
{
    lh_memory_view_t v = view(lh_null, lh_null);
    const lh_ptr begin = lh_null;

    LH_EXPECT_DEATH(lh_memory_view_unpack_v(&v, &begin, nullptr));
}

TEST(memory_view_make_v, rejects_invalid_range_death)
{
    const unsigned char buf[2] = {};

    LH_EXPECT_DEATH((void)lh_memory_view_make_v(p(buf + 1), p(buf)));
}

TEST(memory_view_make_by_size, rejects_zero_size_death)
{
    const unsigned char buf[1] = {};

    LH_EXPECT_DEATH((void)lh_memory_view_make_by_size(p(buf), 0));
}

TEST(memory_view_make_by_size, rejects_null_begin_death)
{
    LH_EXPECT_DEATH((void)lh_memory_view_make_by_size(lh_null, 1));
}

TEST(memory_view_make_slice, rejects_invalid_view_death)
{
    const unsigned char buf[2] = {};
    lh_memory_view_t v = view(p(buf), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_view_make_slice(&v));
}

TEST(memory_view_next_value, rejects_end_boundary_death)
{
    const unsigned char buf[2] = {1, 2};
    lh_memory_view_t v = view(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_view_next_value(&v, p(buf + 1)));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
