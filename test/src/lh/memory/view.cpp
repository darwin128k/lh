#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/memory/view.h>
#include <lh/memory/view/initializer.h>
#include <lh/offset/limits.h>
#include <lh/test/out.h>

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

TEST(memory_view_getters, return_stored_endpoints)
{
    const unsigned char buf[4] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 4));

    EXPECT_EQ(lh_memory_view_get_begin(&v), p(buf + 1));
    EXPECT_EQ(lh_memory_view_get_end(&v), p(buf + 4));
}

TEST(memory_view_classification, distinguishes_states)
{
    const unsigned char buf[4] = {};

    lh_memory_view_t empty = view(lh_null, lh_null);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&empty));
    EXPECT_FALSE(lh_memory_view_is_initialized(&empty));
    EXPECT_TRUE(lh_memory_view_is_empty(&empty));

    lh_memory_view_t unknown = view(p(buf), lh_null);
    EXPECT_EQ(lh_memory_view_get_direction(&unknown), lh_memory_view_direction_unknown);

    lh_memory_view_t forward = view(p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_is_initialized(&forward));
    EXPECT_EQ(lh_memory_view_get_direction(&forward), lh_memory_view_direction_forward);
    EXPECT_TRUE(lh_memory_view_is_forward(&forward));
    EXPECT_TRUE(lh_memory_view_is_valid(&forward));

    // [x, x) is the empty range - valid and forward, not a reversed/corrupted state.
    lh_memory_view_t equal = view(p(buf), p(buf));
    EXPECT_EQ(lh_memory_view_get_direction(&equal), lh_memory_view_direction_forward);
    EXPECT_TRUE(lh_memory_view_is_forward(&equal));
    EXPECT_TRUE(lh_memory_view_is_valid(&equal));
    EXPECT_TRUE(lh_memory_view_is_empty(&equal));

    lh_memory_view_t backward = view(p(buf + 4), p(buf));
    EXPECT_EQ(lh_memory_view_get_direction(&backward), lh_memory_view_direction_backward);
    EXPECT_TRUE(lh_memory_view_is_backward(&backward));
    EXPECT_FALSE(lh_memory_view_is_valid(&backward));
}

TEST(memory_view_validated_access, returns_endpoints_and_size)
{
    const unsigned char buf[8] = {};
    lh_memory_view_t v = view(p(buf + 1), p(buf + 5));

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

    EXPECT_TRUE(lh_memory_view_is_multiple_of(&left, 4));
    EXPECT_TRUE(lh_memory_view_is_multiple_of(&left, 8));
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

    lh_memory_view_t by_end;
    lh_memory_view_init(lh_addr_of(by_end), p(buf), p(buf + 4));
    EXPECT_TRUE(lh_memory_view_equals_of(&by_end, p(buf), p(buf + 4)));

    lh_memory_view_t by_size;
    lh_memory_view_init_by_size(lh_addr_of(by_size), p(buf + 1), 3);
    EXPECT_TRUE(lh_memory_view_equals_of(&by_size, p(buf + 1), p(buf + 4)));

    lh_memory_view_t empty;


    lh_memory_view_init_empty(lh_addr_of(empty));
    EXPECT_TRUE(lh_memory_view_equals_of(&empty, lh_null, lh_null));
}

TEST(memory_view_empty_range, reports_zero_size_and_no_containment)
{
    const unsigned char buf[4] = {1, 2, 3, 4};
    lh_memory_view_t empty = view(p(buf), p(buf));
    lh_memory_view_t other = view(p(buf), p(buf + 2));

    EXPECT_TRUE(lh_memory_view_is_valid(&empty));
    EXPECT_EQ(lh_memory_view_get_size(&empty), 0u);
    EXPECT_TRUE(lh_memory_view_is_empty(&empty));
    EXPECT_FALSE(lh_memory_view_is_valid_offset(&empty, 0));
    EXPECT_FALSE(lh_memory_view_contains_ptr(&empty, p(buf)));
    EXPECT_TRUE(lh_memory_view_contains_of(&empty, p(buf), p(buf)));
    EXPECT_FALSE(lh_memory_view_contains(&empty, &other));
    EXPECT_FALSE(lh_memory_view_overlaps(&empty, &other));
    EXPECT_FALSE(lh_memory_view_overlaps_of(&empty, p(buf), p(buf + 1)));
    EXPECT_TRUE(lh_memory_view_is_multiple_of(&empty, 4));
    EXPECT_EQ(lh_memory_view_find_range(&empty, p(buf), p(buf + 1)), lh_null);
    EXPECT_EQ(lh_memory_view_find_range(&other, p(buf + 1), p(buf + 1)), lh_null);

    lh_memory_view_t taken = lh_test::out(lh_memory_view_take_first, p(buf), p(buf), 0);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&taken));
    lh_memory_view_t dropped = lh_test::out(lh_memory_view_drop_first, &empty, 0);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&dropped));
}

TEST(memory_view_one_byte, shifts_end_offsets_by_one)
{
    alignas(16) const unsigned char buf[16] = {9, 8, 7};
    lh_memory_view_t byte = view(p(buf), p(buf + 1));

    EXPECT_EQ(lh_memory_view_get_size(&byte), 1u);
    EXPECT_EQ(lh_memory_view_get_ptr_from_end(&byte, 0), p(buf));
    EXPECT_EQ(lh_memory_view_get_offset_from_end(&byte, p(buf)), 0u);
    EXPECT_EQ(lh_memory_view_get_last_value(&byte), 9);
    EXPECT_TRUE(lh_memory_view_contains_ptr(&byte, p(buf)));
    EXPECT_FALSE(lh_memory_view_contains_ptr(&byte, p(buf + 1)));
    EXPECT_TRUE(lh_memory_view_is_begin_aligned(&byte, 16));
    EXPECT_FALSE(lh_memory_view_is_aligned(&byte, 16));

    lh_memory_view_t empty = view(p(buf), p(buf));
    EXPECT_TRUE(lh_memory_view_is_aligned(&empty, 16));
}

TEST(memory_view_between, keeps_empty_subrange)
{
    const unsigned char buf[4] = {};
    lh_memory_view_t outer = view(p(buf), p(buf + 4));
    lh_memory_view_t inner = lh_test::out(lh_memory_view_between, &outer, p(buf + 2), p(buf + 2));

    EXPECT_TRUE(lh_memory_view_equals_of(&inner, p(buf + 2), p(buf + 2)));
    EXPECT_TRUE(lh_memory_view_is_empty(&inner));
    EXPECT_FALSE(lh_memory_view_is_uninitialized(&inner));
}

TEST(memory_view_seek, returns_null_past_either_end)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_t v = view(p(buf), p(buf + 4));

    EXPECT_EQ(lh_memory_view_seek_ptr(&v, lh_null, -1), p(buf + 3));
    EXPECT_EQ(lh_memory_view_next_ptr(&v, p(buf + 3)), lh_null);
    EXPECT_EQ(lh_memory_view_prev_ptr(&v, p(buf)), lh_null);
}

TEST(memory_view_sub_range, takes_and_drops_bytes)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_t v = view(p(buf), p(buf + 6));

    lh_memory_view_t offset = lh_test::out(lh_memory_view_from_offset, &v, 2, 3);
    EXPECT_TRUE(lh_memory_view_equals_of(&offset, p(buf + 2), p(buf + 5)));

    lh_memory_view_t first = lh_test::out(lh_memory_view_from_begin, &v, 2);
    EXPECT_TRUE(lh_memory_view_equals_of(&first, p(buf), p(buf + 2)));

    lh_memory_view_t last = lh_test::out(lh_memory_view_from_end, &v, 2);
    EXPECT_TRUE(lh_memory_view_equals_of(&last, p(buf + 4), p(buf + 6)));

    lh_memory_view_t dropped = lh_test::out(lh_memory_view_drop_last, &v, 2);
    EXPECT_TRUE(lh_memory_view_equals_of(&dropped, p(buf), p(buf + 4)));

    lh_memory_view_t trimmed = lh_test::out(lh_memory_view_trim, &v, 1, 2);
    EXPECT_TRUE(lh_memory_view_equals_of(&trimmed, p(buf + 1), p(buf + 4)));
}

TEST(memory_view_drop_first, returns_uninitialized_when_all_removed)
{
    const unsigned char buf[3] = {10, 20, 30};
    lh_memory_view_t v = view(p(buf), p(buf + 3));

    lh_memory_view_t dropped = lh_test::out(lh_memory_view_drop_first, &v, 3);

    EXPECT_TRUE(lh_memory_view_equals_of(&dropped, lh_null, lh_null));
}

TEST(memory_view_uninitialized, safe_queries)
{
    lh_memory_view_t v = view(lh_null, lh_null);

    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
    EXPECT_FALSE(lh_memory_view_is_initialized(&v));
    EXPECT_FALSE(lh_memory_view_is_valid(&v));
    EXPECT_TRUE(lh_memory_view_is_empty(&v));
    EXPECT_EQ(lh_memory_view_get_direction(&v), lh_memory_view_direction_unknown);
    EXPECT_EQ(lh_memory_view_get_flags(&v), lh_memory_view_flags_uninitialized);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_view_get_begin_v, rejects_uninitialized_death)
{
    lh_memory_view_t v = view(lh_null, lh_null);

    LH_EXPECT_DEATH((void)lh_memory_view_get_begin_v(&v));
}

TEST(memory_view_make_v, rejects_invalid_range_death)
{
    const unsigned char buf[2] = {};

    LH_EXPECT_DEATH(([&] {
        lh_memory_view_t v;
        lh_memory_view_init(lh_addr_of(v), p(buf + 1), p(buf));
    }()));
}

TEST(memory_view_make_by_size, rejects_zero_size_death)
{
    const unsigned char buf[1] = {};

    LH_EXPECT_DEATH(([&] {
        lh_memory_view_t v;
        lh_memory_view_init_by_size(lh_addr_of(v), p(buf), 0);
    }()));
}

TEST(memory_view_make_by_size, rejects_null_begin_death)
{
    LH_EXPECT_DEATH(([&] {
        lh_memory_view_t v;
        lh_memory_view_init_by_size(lh_addr_of(v), lh_null, 1);
    }()));
}

TEST(memory_view_get_ptr_from_begin, rejects_offset_at_size_death)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_t v = view(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_view_get_ptr_from_begin(&v, 2));
}

TEST(memory_view_get_ptr, rejects_soffset_min_death)
{
    const unsigned char buf[2] = {10, 20};
    lh_memory_view_t v = view(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_view_get_ptr(&v, LH_SOFFSET_T_MIN));
}

TEST(memory_view_get_offset_from_begin, rejects_ptr_at_end_death)
{
    const unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_t v = view(p(buf), p(buf + 4));

    LH_EXPECT_DEATH((void)lh_memory_view_get_offset_from_begin(&v, p(buf + 4)));
}

TEST(memory_view_between, rejects_out_of_bounds_ptrs_death)
{
    const unsigned char buf[6] = {10, 20, 30, 40, 50, 60};
    lh_memory_view_t v = view(p(buf), p(buf + 4));

    LH_EXPECT_DEATH((void)lh_test::out(lh_memory_view_between, &v, p(buf + 1), p(buf + 5)));
}

TEST(memory_view_next_value, rejects_end_boundary_death)
{
    const unsigned char buf[2] = {1, 2};
    lh_memory_view_t v = view(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_view_next_value(&v, p(buf + 1)));
}

TEST(memory_view_empty_range, rejects_indexed_access_death)
{
    const unsigned char buf[1] = {1};
    lh_memory_view_t empty = view(p(buf), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_view_get_ptr_from_end(&empty, 0));
    LH_EXPECT_DEATH((void)lh_memory_view_get_offset_from_end(&empty, p(buf)));
    LH_EXPECT_DEATH((void)lh_test::out(lh_memory_view_take_first, p(buf), p(buf), 1));
}

TEST(memory_view_get_size, rejects_backward_death)
{
    const unsigned char buf[2] = {};
    lh_memory_view_t backward = view(p(buf + 1), p(buf));

    LH_EXPECT_DEATH((void)lh_memory_view_get_size(&backward));
}

TEST(memory_view_get_size, rejects_uninitialized_death)
{
    lh_memory_view_t v = view(lh_null, lh_null);

    LH_EXPECT_DEATH((void)lh_memory_view_get_size(&v));
}

TEST(memory_view_is_multiple_of, rejects_uninitialized_death)
{
    lh_memory_view_t v = view(lh_null, lh_null);

    LH_EXPECT_DEATH((void)lh_memory_view_is_multiple_of(&v, 1));
}

TEST(memory_view_is_multiple_of, rejects_zero_alignment_death)
{
    const unsigned char buf[4] = {};
    lh_memory_view_t v = view(p(buf), p(buf + 4));

    LH_EXPECT_DEATH((void)lh_memory_view_is_multiple_of(&v, 0));
}

TEST(memory_view_uninitialized, rejects_access_death)
{
    const unsigned char buf[2] = {1, 2};
    lh_memory_view_t v = view(lh_null, lh_null);
    lh_memory_view_t other = view(p(buf), p(buf + 2));

    LH_EXPECT_DEATH((void)lh_memory_view_get_end_v(&v));
    LH_EXPECT_DEATH((void)lh_memory_view_get_first_ptr(&v));
    LH_EXPECT_DEATH((void)lh_memory_view_get_last_ptr(&v));
    LH_EXPECT_DEATH((void)lh_memory_view_get_value(&v, 0));
    LH_EXPECT_DEATH((void)lh_memory_view_is_valid_offset(&v, 0));
    LH_EXPECT_DEATH((void)lh_memory_view_contains_ptr(&v, p(buf)));
    LH_EXPECT_DEATH((void)lh_test::out(lh_memory_view_from_offset, &v, 0, 1));
    LH_EXPECT_DEATH((void)lh_test::out(lh_memory_view_drop_first, &v, 0));
    LH_EXPECT_DEATH((void)lh_memory_view_find(&v, &other));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
