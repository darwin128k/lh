#include <gtest/gtest.h>

#include <lh/cast.h>
#include <lh/expect/death.h>
#include <lh/memory/range.h>
#include <lh/memory/range/initializer.h>
#include <lh/null.h>

namespace {

TEST(memory_range_pack_unpack, partial_updates) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char low_mem;
    unsigned char high_mem;
    lh_ptr lo = lh_cast_static(lh_ptr, &low_mem);
    lh_ptr hi = lh_cast_static(lh_ptr, &high_mem);
    if (lo > hi) {
        lh_ptr t = lo;
        lo = hi;
        hi = t;
    }

    lh_memory_range_pack(&r, &lo, nullptr);
    EXPECT_EQ(lh_memory_range_get_begin(&r), lo);
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_null);

    lh_memory_range_pack(&r, nullptr, &hi);
    EXPECT_EQ(lh_memory_range_get_begin(&r), lo);
    EXPECT_EQ(lh_memory_range_get_end(&r), hi);
}

TEST(memory_range_set, getters_roundtrip) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 4);
    lh_memory_range_set(&r, b, e);
    EXPECT_EQ(lh_memory_range_get_begin(&r), b);
    EXPECT_EQ(lh_memory_range_get_end(&r), e);
}

TEST(memory_range_make, valid_pair_returns_expected_bounds) {
    unsigned char buf[4];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 4);
    lh_memory_range_t r = lh_memory_range_make(b, e);
    EXPECT_EQ(lh_memory_range_get_begin(&r), b);
    EXPECT_EQ(lh_memory_range_get_end(&r), e);
    EXPECT_TRUE(lh_memory_range_is_valid(&r));
}

TEST(memory_range_make_by_size, builds_expected_half_open_range) {
    unsigned char buf[5];
    lh_memory_range_t r = lh_memory_range_make_by_size(lh_cast_static(lh_ptr, buf), 5);
    EXPECT_EQ(lh_memory_range_get_begin(&r), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_cast_static(lh_ptr, buf + 5));
    EXPECT_EQ(lh_memory_range_get_size(&r), 5u);
}

TEST(memory_range_make_by_empty, returns_empty_initializer_state) {
    const lh_memory_range_t r = lh_memory_range_make_by_empty();
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
    EXPECT_EQ(lh_memory_range_get_begin(&r), lh_null);
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_null);
}

TEST(memory_range_make, invalid_pair_is_allowed) {
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    lh_memory_range_t r = lh_memory_range_make(lo, hi);
    EXPECT_EQ(lh_memory_range_get_state(&r), lh_memory_range_state_reversed);
    EXPECT_TRUE(lh_memory_range_is_invalid(&r));
}

TEST(memory_range_state, uninitialized_empty_initializer) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    EXPECT_EQ(lh_memory_range_get_state(&r), lh_memory_range_state_uninitialized);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
    EXPECT_FALSE(lh_memory_range_is_empty(&r));
    EXPECT_FALSE(lh_memory_range_has_data(&r));
    EXPECT_FALSE(lh_memory_range_is_valid(&r));
    EXPECT_TRUE(lh_memory_range_is_invalid(&r));
}

TEST(memory_range_state, degenerate_equal_endpoints_is_empty) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char x = 0;
    lh_ptr p = lh_cast_static(lh_ptr, &x);
    lh_memory_range_set(&r, p, p);
    EXPECT_EQ(lh_memory_range_get_state(&r), lh_memory_range_state_empty);
    EXPECT_TRUE(lh_memory_range_is_empty(&r));
    EXPECT_TRUE(lh_memory_range_is_valid(&r));
    EXPECT_EQ(lh_memory_range_get_size(&r), 0u);
}

TEST(memory_range_state, forward_span_has_data) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 8);
    EXPECT_EQ(lh_memory_range_get_state(&r), lh_memory_range_state_has_data);
    EXPECT_TRUE(lh_memory_range_has_data(&r));
    EXPECT_TRUE(lh_memory_range_is_valid(&r));
    EXPECT_EQ(lh_memory_range_get_size(&r), 8u);
    EXPECT_EQ(lh_memory_range_diff(&r), 8);
}

TEST(memory_range_state, reversed_bounds_invalid) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    lh_memory_range_set(&r, lo, hi);
    EXPECT_EQ(lh_memory_range_get_state(&r), lh_memory_range_state_reversed);
    EXPECT_TRUE(lh_memory_range_is_invalid(&r));
    EXPECT_LT(lh_memory_range_diff(&r), 0);
}

TEST(memory_range_state, only_begin_null) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_memory_range_pack(&r, &b, nullptr);
    EXPECT_TRUE(lh_memory_range_is_invalid(&r));
    EXPECT_FALSE(lh_memory_range_is_uninitialized(&r));
}

TEST(memory_range_init_by_other, copies_bounds) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_init_by_size(&a, lh_cast_static(lh_ptr, buf), 4);

    lh_memory_range_t b = lh_memory_range_empty_initializer();
    lh_memory_range_init_by_other(&b, &a);
    EXPECT_EQ(lh_memory_range_get_begin(&b), lh_memory_range_get_begin(&a));
    EXPECT_EQ(lh_memory_range_get_end(&b), lh_memory_range_get_end(&a));
}

TEST(memory_range_init_by_empty, resets_to_empty_initializer) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 2));
    lh_memory_range_init_by_empty(&r);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
}

TEST(memory_range_contains, half_open_endpoints) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 3);
    lh_memory_range_set(&r, b, e);
    EXPECT_TRUE(lh_memory_range_contains_ptr(&r, b));
    EXPECT_FALSE(lh_memory_range_contains_ptr(&r, e));
    EXPECT_TRUE(lh_memory_range_contains_ptr(&r, lh_cast_static(lh_ptr, buf + 2)));
    EXPECT_TRUE(lh_memory_range_contains_ptr(&r, lh_cast_static(lh_ptr, buf + 1)));
    EXPECT_FALSE(lh_memory_range_contains_ptr(&r, lh_cast_static(lh_ptr, buf - 1)));
}

TEST(memory_range_contains_range, inner_within_outer) {
    lh_memory_range_t outer = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&outer, lh_cast_static(lh_ptr, buf), 8);

    lh_ptr i0 = lh_cast_static(lh_ptr, buf + 2);
    lh_ptr i1 = lh_cast_static(lh_ptr, buf + 5);
    EXPECT_TRUE(lh_memory_range_contains_range(&outer, i0, i1));

    lh_ptr o0 = lh_cast_static(lh_ptr, buf - 1);
    EXPECT_FALSE(lh_memory_range_contains_range(&outer, o0, i1));
}

TEST(memory_range_contains, nested_range_object) {
    lh_memory_range_t outer = lh_memory_range_empty_initializer();
    lh_memory_range_t inner = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&outer, lh_cast_static(lh_ptr, buf), 8);
    lh_memory_range_init_by_size(&inner, lh_cast_static(lh_ptr, buf + 2), 3);
    EXPECT_TRUE(lh_memory_range_contains(&outer, &inner));
}

TEST(memory_range_equals, true_for_identical_bounds) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_ptr begin = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr end = lh_cast_static(lh_ptr, buf + 6);

    lh_memory_range_set(&a, begin, end);
    lh_memory_range_set(&b, begin, end);
    EXPECT_TRUE(lh_memory_range_equals(&a, &b));
}

TEST(memory_range_equals, false_for_different_bounds) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];

    lh_memory_range_set(&a, lh_cast_static(lh_ptr, buf + 1), lh_cast_static(lh_ptr, buf + 6));
    lh_memory_range_set(&b, lh_cast_static(lh_ptr, buf + 2), lh_cast_static(lh_ptr, buf + 6));
    EXPECT_FALSE(lh_memory_range_equals(&a, &b));
}

TEST(memory_range_equals_range, true_for_identical_bounds) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_ptr begin = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr end = lh_cast_static(lh_ptr, buf + 6);

    lh_memory_range_set(&r, begin, end);
    EXPECT_TRUE(lh_memory_range_equals_range(&r, begin, end));
}

TEST(memory_range_equals_range, false_for_different_bounds) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];

    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf + 1), lh_cast_static(lh_ptr, buf + 6));
    EXPECT_FALSE(lh_memory_range_equals_range(&r, lh_cast_static(lh_ptr, buf + 2),
                                              lh_cast_static(lh_ptr, buf + 6)));
}

TEST(memory_range_overlaps, touches_and_disjoint) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    lh_ptr a0 = lh_cast_static(lh_ptr, buf + 2);
    lh_ptr a1 = lh_cast_static(lh_ptr, buf + 5);
    EXPECT_TRUE(lh_memory_range_overlaps_range(&r, a0, a1));

    lh_ptr d0 = lh_cast_static(lh_ptr, buf + 10);
    lh_ptr d1 = lh_cast_static(lh_ptr, buf + 12);
    EXPECT_FALSE(lh_memory_range_overlaps_range(&r, d0, d1));
}

TEST(memory_range_overlaps, adjacent_half_open_disjoint) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    lh_ptr tail0 = lh_cast_static(lh_ptr, buf + 4);
    lh_ptr tail1 = lh_cast_static(lh_ptr, buf + 8);
    EXPECT_FALSE(lh_memory_range_overlaps_range(&r, tail0, tail1));
}

TEST(memory_range_at, front_back_and_offsets) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0xA, 0xB, 0xC, 0xD};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    EXPECT_EQ(lh_memory_range_get_front_ptr(&r), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_back_ptr(&r), lh_cast_static(lh_ptr, buf + 3));
    EXPECT_EQ(lh_memory_range_get_front_value(&r), 0xA);
    EXPECT_EQ(lh_memory_range_get_back_value(&r), 0xD);

    EXPECT_EQ(lh_memory_range_get_ptr_from_front(&r, 0), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_ptr_from_front(&r, 3), lh_cast_static(lh_ptr, buf + 3));
    EXPECT_EQ(lh_memory_range_get_ptr_from_back(&r, 0), lh_cast_static(lh_ptr, buf + 3));
    EXPECT_EQ(lh_memory_range_get_ptr_from_back(&r, 3), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_ptr(&r, 1, lh_bool_false), lh_cast_static(lh_ptr, buf + 1));
    EXPECT_EQ(lh_memory_range_get_ptr(&r, 1, lh_bool_true), lh_cast_static(lh_ptr, buf + 2));
    EXPECT_EQ(lh_memory_range_get_value_from_front(&r, 1), 0xB);
    EXPECT_EQ(lh_memory_range_get_value_from_back(&r, 1), 0xC);
    EXPECT_EQ(lh_memory_range_get_value(&r, 2, lh_bool_false), 0xC);
    EXPECT_EQ(lh_memory_range_get_value(&r, 2, lh_bool_true), 0xB);

    EXPECT_TRUE(lh_memory_range_is_valid_offset(&r, 0));
    EXPECT_TRUE(lh_memory_range_is_valid_offset(&r, 3));
    EXPECT_FALSE(lh_memory_range_is_valid_offset(&r, 4));
}

TEST(memory_range_next_ptr, returns_next_or_null_at_end) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    EXPECT_EQ(lh_memory_range_next_ptr(&r, lh_cast_static(lh_ptr, &buf[0])),
              lh_cast_static(lh_ptr, &buf[1]));
    EXPECT_EQ(lh_memory_range_next_ptr(&r, lh_cast_static(lh_ptr, &buf[2])),
              lh_cast_static(lh_ptr, &buf[3]));
    EXPECT_TRUE(lh_null_eq(lh_memory_range_next_ptr(&r, lh_cast_static(lh_ptr, &buf[3]))));
}

TEST(memory_range_next_ptr, returns_null_for_pointer_outside_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    EXPECT_TRUE(lh_null_eq(lh_memory_range_next_ptr(&r, lh_cast_static(lh_ptr, buf - 1))));
}

TEST(memory_range_prev_ptr, returns_prev_or_null_at_begin) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    EXPECT_EQ(lh_memory_range_prev_ptr(&r, lh_cast_static(lh_ptr, &buf[3])),
              lh_cast_static(lh_ptr, &buf[2]));
    EXPECT_EQ(lh_memory_range_prev_ptr(&r, lh_cast_static(lh_ptr, &buf[1])),
              lh_cast_static(lh_ptr, &buf[0]));
    EXPECT_TRUE(lh_null_eq(lh_memory_range_prev_ptr(&r, lh_cast_static(lh_ptr, &buf[0]))));
}

TEST(memory_range_prev_ptr, returns_null_for_pointer_outside_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    EXPECT_TRUE(lh_null_eq(lh_memory_range_prev_ptr(&r, lh_cast_static(lh_ptr, buf + 4))));
}

TEST(memory_range_next_prev_value, returns_neighbor_values) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    EXPECT_EQ(lh_memory_range_next_value(&r, lh_cast_static(lh_ptr, &buf[1])), 30);
    EXPECT_EQ(lh_memory_range_prev_value(&r, lh_cast_static(lh_ptr, &buf[2])), 20);
}

TEST(memory_range_slice, returns_expected_subrange_from_front) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[6] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 6);

    const lh_memory_range_t s = lh_memory_range_slice(&r, 0, 3);
    EXPECT_EQ(lh_memory_range_get_begin(&s), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_end(&s), lh_cast_static(lh_ptr, buf + 3));
    EXPECT_EQ(lh_memory_range_get_size(&s), 3u);
}

TEST(memory_range_slice, supports_non_zero_offset) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 8);

    const lh_memory_range_t s = lh_memory_range_slice(&r, 1, 3);
    EXPECT_EQ(lh_memory_range_get_begin(&s), lh_cast_static(lh_ptr, buf + 1));
    EXPECT_EQ(lh_memory_range_get_end(&s), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_EQ(lh_memory_range_get_size(&s), 3u);
}

TEST(memory_range_slice, supports_end_at_range_end) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);

    const lh_memory_range_t s = lh_memory_range_slice(&r, 0, 4);
    EXPECT_EQ(lh_memory_range_get_begin(&s), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_end(&s), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_EQ(lh_memory_range_get_size(&s), 4u);
}

TEST(memory_range_set_value, writes_byte) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    lh_memory_range_set_value(&r, 1, 0xEE, lh_bool_false);
    EXPECT_EQ(buf[1], 0xEE);
    lh_memory_range_set_value(&r, 0, 0xFF, lh_bool_true);
    EXPECT_EQ(buf[3], 0xFF);
}

TEST(memory_range_alignment, begin_and_full) {
    alignas(16) unsigned char block[32];
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, block), 16);
    EXPECT_TRUE(lh_memory_range_is_begin_aligned(&r, 16));
    EXPECT_TRUE(lh_memory_range_is_aligned(&r, 16));
}

TEST(memory_range_multiple_of, size_divisible) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[12];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 12);
    EXPECT_TRUE(lh_memory_range_is_multiple_of(&r, 4));
    EXPECT_FALSE(lh_memory_range_is_multiple_of(&r, 5));
}

TEST(memory_range_clear, resets_to_empty_initializer) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 1);
    lh_memory_range_clear(&r);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
}

TEST(memory_range_assign_v, copies_valid) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_init_by_size(&a, lh_cast_static(lh_ptr, buf), 2);
    lh_memory_range_assign_v(&b, &a);
    EXPECT_EQ(lh_memory_range_get_begin(&b), lh_memory_range_get_begin(&a));
    EXPECT_EQ(lh_memory_range_get_end(&b), lh_memory_range_get_end(&a));
}

TEST(memory_range_set_v, valid_pair) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[3];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 2);
    lh_memory_range_set_v(&r, b, e);
    EXPECT_EQ(lh_memory_range_get_begin(&r), b);
    EXPECT_EQ(lh_memory_range_get_end(&r), e);
}

TEST(memory_range_set_by_size_or_clear, clears_on_invalid_begin) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 1);
    ASSERT_TRUE(lh_memory_range_has_data(&r));

    lh_memory_range_set_by_size_or_clear(&r, lh_null, 4);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
}

TEST(memory_range_swap_exchange, mutates_pair) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char x[1];
    unsigned char y[2];
    lh_memory_range_init_by_size(&a, lh_cast_static(lh_ptr, x), 1);
    lh_memory_range_init_by_size(&b, lh_cast_static(lh_ptr, y), 2);

    lh_memory_range_swap(&a, &b);
    EXPECT_EQ(lh_memory_range_get_begin(&a), lh_cast_static(lh_ptr, y));
    EXPECT_EQ(lh_memory_range_get_begin(&b), lh_cast_static(lh_ptr, x));

    lh_memory_range_exchange(&a, &b);
    EXPECT_TRUE(lh_memory_range_has_data(&a));
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&b));
}

TEST(memory_range_copy_range, truncates_when_destination_smaller) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char dst[2] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, dst), 2);
    unsigned char src[] = {1, 2, 3, 4};
    lh_ptr end = lh_memory_range_copy_range(&r, lh_cast_static(lh_ptr, src),
                                            lh_cast_static(lh_ptr, src + 4));
    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 2));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
}

TEST(memory_range_copy_range, source_smaller_writes_partial) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char dst[4] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, dst), 4);
    unsigned char src[] = {9, 8};
    lh_ptr end = lh_memory_range_copy_range(&r, lh_cast_static(lh_ptr, src),
                                            lh_cast_static(lh_ptr, src + 2));
    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 2));
    EXPECT_EQ(dst[0], 9);
    EXPECT_EQ(dst[1], 8);
    EXPECT_EQ(dst[2], 0);
}

TEST(memory_range_move_range, disjoint_source_into_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char dst[4] = {};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, dst), 4);
    unsigned char src[] = {7, 6, 5, 4};
    lh_ptr end = lh_memory_range_move_range(&r, lh_cast_static(lh_ptr, src),
                                            lh_cast_static(lh_ptr, src + 4));
    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 4));
    EXPECT_EQ(dst[0], 7);
    EXPECT_EQ(dst[3], 4);
}

TEST(memory_range_find_range, finds_first_occurrence) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, hay), 5);
    unsigned char needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_range_find_range(&r, lh_cast_static(lh_ptr, needle),
                                                lh_cast_static(lh_ptr, needle + 2));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lh_cast_static(const lh_ptr, &hay[0]));
}

TEST(memory_range_find_range, not_found_returns_null) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char hay[] = {1, 2, 3};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, hay), 3);
    unsigned char needle[] = {9, 9};
    const lh_ptr p = lh_memory_range_find_range(&r, lh_cast_static(lh_ptr, needle),
                                                lh_cast_static(lh_ptr, needle + 2));
    EXPECT_TRUE(lh_null_eq(p));
}

TEST(memory_range_rfind_range, finds_last_occurrence) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, hay), 5);
    unsigned char needle[] = {'a', 'b'};
    const lh_ptr p = lh_memory_range_rfind_range(&r, lh_cast_static(lh_ptr, needle),
                                                 lh_cast_static(lh_ptr, needle + 2));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lh_cast_static(const lh_ptr, &hay[2]));
}

TEST(memory_range_compare_range, equal_within_min_length_returns_null) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {1, 2, 3};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, lhs), 3);
    unsigned char rhs[] = {1, 2};
    const lh_ptr d = lh_memory_range_compare_range(&r, lh_cast_static(lh_ptr, rhs),
                                                   lh_cast_static(lh_ptr, rhs + 2));
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_range_compare_range, mismatch_returns_pointer_in_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {1, 9, 3};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, lhs), 3);
    unsigned char rhs[] = {1, 2, 3};
    const lh_ptr d = lh_memory_range_compare_range(&r, lh_cast_static(lh_ptr, rhs),
                                                   lh_cast_static(lh_ptr, rhs + 3));
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, lh_cast_static(const lh_ptr, &lhs[1]));
}

TEST(memory_range_rcompare_range, equal_suffix_returns_null) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {0, 0, 1, 2, 3, 4};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, lhs), 6);
    unsigned char rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_range_rcompare_range(&r, lh_cast_static(lh_ptr, rhs),
                                                    lh_cast_static(lh_ptr, rhs + 4));
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_range_rcompare_range, suffix_mismatch_returns_pointer_in_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {1, 2, 3, 9};
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, lhs), 4);
    unsigned char rhs[] = {1, 2, 3, 4};
    const lh_ptr d = lh_memory_range_rcompare_range(&r, lh_cast_static(lh_ptr, rhs),
                                                    lh_cast_static(lh_ptr, rhs + 4));
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, lh_cast_static(const lh_ptr, &lhs[3]));
}

TEST(memory_range_unpack, writes_both_endpoints) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[3];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 3);
    lh_memory_range_set(&r, b, e);

    lh_ptr out_b = lh_null;
    lh_ptr out_e = lh_null;
    lh_memory_range_unpack(&r, &out_b, &out_e);
    EXPECT_EQ(out_b, b);
    EXPECT_EQ(out_e, e);
}

TEST(memory_range_init, same_as_set_for_valid_bounds) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 2);
    lh_memory_range_init(&r, lh_cast_static(lh_void *, b), lh_cast_static(lh_void *, e));
    EXPECT_EQ(lh_memory_range_get_begin(&r), b);
    EXPECT_EQ(lh_memory_range_get_end(&r), e);
    EXPECT_TRUE(lh_memory_range_has_data(&r));
}

TEST(memory_range_assign, copies_like_init_by_other) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[5];
    lh_memory_range_init_by_size(&a, lh_cast_static(lh_ptr, buf), 5);
    lh_memory_range_assign(&b, &a);
    EXPECT_EQ(lh_memory_range_get_begin(&b), lh_memory_range_get_begin(&a));
    EXPECT_EQ(lh_memory_range_get_end(&b), lh_memory_range_get_end(&a));
}

TEST(memory_range_set_by_size, sets_half_open_span) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_set_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    EXPECT_EQ(lh_memory_range_get_begin(&r), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_EQ(lh_memory_range_get_size(&r), 4u);
}

TEST(memory_range_make_v, accepts_valid_ordered_pair) {
    unsigned char buf[3];
    lh_ptr b = lh_cast_static(lh_ptr, buf);
    lh_ptr e = lh_cast_static(lh_ptr, buf + 3);
    lh_memory_range_t r = lh_memory_range_make_v(b, e);
    EXPECT_EQ(lh_memory_range_get_begin(&r), b);
    EXPECT_EQ(lh_memory_range_get_end(&r), e);
    EXPECT_TRUE(lh_memory_range_is_valid(&r));
}

TEST(memory_range_make_or_empty, invalid_pair_returns_empty) {
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    const lh_memory_range_t r = lh_memory_range_make_or_empty(lo, hi);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&r));
}

TEST(memory_range_make_or_empty, valid_pair_returns_range) {
    unsigned char buf[4];
    const lh_memory_range_t r =
        lh_memory_range_make_or_empty(lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_EQ(lh_memory_range_get_begin(&r), lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_TRUE(lh_memory_range_is_valid(&r));
}

TEST(memory_range_unpack_v, reads_valid_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 2);
    lh_ptr o0 = lh_null;
    lh_ptr o1 = lh_null;
    lh_memory_range_unpack_v(&r, &o0, &o1);
    EXPECT_EQ(o0, lh_cast_static(lh_ptr, buf));
    EXPECT_EQ(o1, lh_cast_static(lh_ptr, buf + 2));
}

TEST(memory_range_is_sliceable, in_bounds_and_out_of_bounds) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 8);
    EXPECT_TRUE(lh_memory_range_is_sliceable(&r, 0, 8));
    EXPECT_TRUE(lh_memory_range_is_sliceable(&r, 1, 3));
    EXPECT_FALSE(lh_memory_range_is_sliceable(&r, 6, 3));
}

TEST(memory_range_slice_or_empty, returns_slice_or_empty) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 8);

    const lh_memory_range_t s_ok = lh_memory_range_slice_or_empty(&r, 1, 3);
    EXPECT_EQ(lh_memory_range_get_begin(&s_ok), lh_cast_static(lh_ptr, buf + 1));
    EXPECT_EQ(lh_memory_range_get_end(&s_ok), lh_cast_static(lh_ptr, buf + 4));

    const lh_memory_range_t s_bad = lh_memory_range_slice_or_empty(&r, 6, 3);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&s_bad));
}

TEST(memory_range_slice_or_empty, invalid_self_returns_empty) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf + 1), lh_cast_static(lh_ptr, buf));
    const lh_memory_range_t s = lh_memory_range_slice_or_empty(&r, 0, 1);
    EXPECT_TRUE(lh_memory_range_is_uninitialized(&s));
}

TEST(memory_range_is_valid_offset, checks_bounds) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 4));
    EXPECT_TRUE(lh_memory_range_is_valid_offset(&r, 0));
    EXPECT_TRUE(lh_memory_range_is_valid_offset(&r, 3));
    EXPECT_FALSE(lh_memory_range_is_valid_offset(&r, 4));
}

TEST(memory_range_clone_dup, raw_copy_semantics) {
    lh_memory_range_t src = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_set(&src, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 4));

    const lh_memory_range_t c = lh_memory_range_clone(&src);
    EXPECT_EQ(lh_memory_range_get_begin(&c), lh_memory_range_get_begin(&src));
    EXPECT_EQ(lh_memory_range_get_end(&c), lh_memory_range_get_end(&src));

    lh_memory_range_t d = lh_memory_range_empty_initializer();
    lh_memory_range_dup(&src, &d);
    EXPECT_EQ(lh_memory_range_get_begin(&d), lh_memory_range_get_begin(&src));
    EXPECT_EQ(lh_memory_range_get_end(&d), lh_memory_range_get_end(&src));
}

TEST(memory_range_clone_dup_v, validated_copy_semantics) {
    lh_memory_range_t src = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_set(&src, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 4));

    const lh_memory_range_t c = lh_memory_range_clone_v(&src);
    EXPECT_EQ(lh_memory_range_get_begin(&c), lh_memory_range_get_begin(&src));
    EXPECT_EQ(lh_memory_range_get_end(&c), lh_memory_range_get_end(&src));

    lh_memory_range_t d = lh_memory_range_empty_initializer();
    lh_memory_range_dup_v(&src, &d);
    EXPECT_EQ(lh_memory_range_get_begin(&d), lh_memory_range_get_begin(&src));
    EXPECT_EQ(lh_memory_range_get_end(&d), lh_memory_range_get_end(&src));
}

TEST(memory_range_pack_v, partial_update_preserves_validity) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 8));

    lh_ptr new_begin = lh_cast_static(lh_ptr, buf + 2);
    lh_memory_range_pack_v(&r, &new_begin, nullptr);
    EXPECT_EQ(lh_memory_range_get_begin(&r), lh_cast_static(lh_ptr, buf + 2));
    EXPECT_EQ(lh_memory_range_get_end(&r), lh_cast_static(lh_ptr, buf + 8));
}

TEST(memory_range_overlaps, two_ranges_intersect_or_not) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&a, lh_cast_static(lh_ptr, buf), 4);
    lh_memory_range_init_by_size(&b, lh_cast_static(lh_ptr, buf + 3), 3);
    EXPECT_TRUE(lh_memory_range_overlaps(&a, &b));

    lh_memory_range_t c = lh_memory_range_empty_initializer();
    lh_memory_range_init_by_size(&c, lh_cast_static(lh_ptr, buf + 4), 4);
    EXPECT_FALSE(lh_memory_range_overlaps(&a, &c));
}

TEST(memory_range_copy, delegates_to_copy_range) {
    lh_memory_range_t dst_r = lh_memory_range_empty_initializer();
    lh_memory_range_t src_r = lh_memory_range_empty_initializer();
    unsigned char dst[2] = {};
    unsigned char src[] = {0x11, 0x22, 0x33};
    lh_memory_range_init_by_size(&dst_r, lh_cast_static(lh_ptr, dst), 2);
    lh_memory_range_init_by_size(&src_r, lh_cast_static(lh_ptr, src), 3);
    lh_ptr end = lh_memory_range_copy(&dst_r, &src_r);
    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 2));
    EXPECT_EQ(dst[0], 0x11);
    EXPECT_EQ(dst[1], 0x22);
}

TEST(memory_range_move, delegates_to_move_range) {
    lh_memory_range_t dst_r = lh_memory_range_empty_initializer();
    lh_memory_range_t src_r = lh_memory_range_empty_initializer();
    unsigned char dst[3] = {};
    unsigned char src[] = {3, 2, 1};
    lh_memory_range_init_by_size(&dst_r, lh_cast_static(lh_ptr, dst), 3);
    lh_memory_range_init_by_size(&src_r, lh_cast_static(lh_ptr, src), 3);
    lh_ptr end = lh_memory_range_move(&dst_r, &src_r);
    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 3));
    EXPECT_EQ(dst[0], 3);
    EXPECT_EQ(dst[2], 1);
}

TEST(memory_range_fill, fills_destination_with_value) {
    lh_memory_range_t dst_r = lh_memory_range_empty_initializer();
    unsigned char dst[5] = {1, 2, 3, 4, 5};

    lh_memory_range_init_by_size(&dst_r, lh_cast_static(lh_ptr, dst), 5);
    lh_ptr end = lh_memory_range_fill(&dst_r, 0x7E);

    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 5));
    EXPECT_EQ(dst[0], 0x7E);
    EXPECT_EQ(dst[1], 0x7E);
    EXPECT_EQ(dst[2], 0x7E);
    EXPECT_EQ(dst[3], 0x7E);
    EXPECT_EQ(dst[4], 0x7E);
}

TEST(memory_range_fill_pattern_range, repeats_pattern_into_destination) {
    lh_memory_range_t dst_r = lh_memory_range_empty_initializer();
    unsigned char dst[7] = {};
    unsigned char pattern[] = {0xAB, 0xCD, 0xEF};

    lh_memory_range_init_by_size(&dst_r, lh_cast_static(lh_ptr, dst), 7);
    lh_ptr end = lh_memory_range_fill_pattern_range(&dst_r, lh_cast_static(lh_ptr, pattern),
                                                    lh_cast_static(lh_ptr, pattern + 3));

    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 7));
    EXPECT_EQ(dst[0], 0xAB);
    EXPECT_EQ(dst[1], 0xCD);
    EXPECT_EQ(dst[2], 0xEF);
    EXPECT_EQ(dst[3], 0xAB);
    EXPECT_EQ(dst[4], 0xCD);
    EXPECT_EQ(dst[5], 0xEF);
    EXPECT_EQ(dst[6], 0xAB);
}

TEST(memory_range_fill_pattern, delegates_to_fill_pattern_range) {
    lh_memory_range_t dst_r = lh_memory_range_empty_initializer();
    lh_memory_range_t pattern_r = lh_memory_range_empty_initializer();
    unsigned char dst[5] = {};
    unsigned char pattern[] = {1, 2};

    lh_memory_range_init_by_size(&dst_r, lh_cast_static(lh_ptr, dst), 5);
    lh_memory_range_init_by_size(&pattern_r, lh_cast_static(lh_ptr, pattern), 2);
    lh_ptr end = lh_memory_range_fill_pattern(&dst_r, &pattern_r);

    EXPECT_EQ(end, lh_cast_static(lh_ptr, dst + 5));
    EXPECT_EQ(dst[0], 1);
    EXPECT_EQ(dst[1], 2);
    EXPECT_EQ(dst[2], 1);
    EXPECT_EQ(dst[3], 2);
    EXPECT_EQ(dst[4], 1);
}

TEST(memory_range_find, finds_via_other_range) {
    lh_memory_range_t hay_r = lh_memory_range_empty_initializer();
    lh_memory_range_t needle_r = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'x', 'y', 'z'};
    unsigned char needle[] = {'y', 'z'};
    lh_memory_range_init_by_size(&hay_r, lh_cast_static(lh_ptr, hay), 3);
    lh_memory_range_init_by_size(&needle_r, lh_cast_static(lh_ptr, needle), 2);
    const lh_ptr p = lh_memory_range_find(&hay_r, &needle_r);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lh_cast_static(const lh_ptr, &hay[1]));
}

TEST(memory_range_rfind, finds_last_via_other_range) {
    lh_memory_range_t hay_r = lh_memory_range_empty_initializer();
    lh_memory_range_t needle_r = lh_memory_range_empty_initializer();
    unsigned char hay[] = {1, 2, 1, 2, 9};
    unsigned char needle[] = {1, 2};
    lh_memory_range_init_by_size(&hay_r, lh_cast_static(lh_ptr, hay), 5);
    lh_memory_range_init_by_size(&needle_r, lh_cast_static(lh_ptr, needle), 2);
    const lh_ptr p = lh_memory_range_rfind(&hay_r, &needle_r);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, lh_cast_static(const lh_ptr, &hay[2]));
}

TEST(memory_range_compare, matches_compare_range) {
    lh_memory_range_t lhs_r = lh_memory_range_empty_initializer();
    lh_memory_range_t rhs_r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {5, 6, 7};
    unsigned char rhs[] = {5, 6};
    lh_memory_range_init_by_size(&lhs_r, lh_cast_static(lh_ptr, lhs), 3);
    lh_memory_range_init_by_size(&rhs_r, lh_cast_static(lh_ptr, rhs), 2);
    const lh_ptr d = lh_memory_range_compare(&lhs_r, &rhs_r);
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_range_rcompare, matches_rcompare_range) {
    lh_memory_range_t lhs_r = lh_memory_range_empty_initializer();
    lh_memory_range_t rhs_r = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {0, 0, 3, 4};
    unsigned char rhs[] = {3, 4};
    lh_memory_range_init_by_size(&lhs_r, lh_cast_static(lh_ptr, lhs), 4);
    lh_memory_range_init_by_size(&rhs_r, lh_cast_static(lh_ptr, rhs), 2);
    const lh_ptr d = lh_memory_range_rcompare(&lhs_r, &rhs_r);
    EXPECT_TRUE(lh_null_eq(d));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_range_death, unpack_v_on_invalid) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    lh_ptr out0 = nullptr;
    lh_ptr out1 = nullptr;
    LH_EXPECT_DEATH(lh_memory_range_unpack_v(&r, &out0, &out1));
}

TEST(memory_range_death, get_size_on_invalid) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_ptr lo = lh_cast_static(lh_ptr, buf);
    lh_memory_range_pack(&r, &lo, nullptr);
    LH_EXPECT_DEATH((void)lh_memory_range_get_size(&r));
}

TEST(memory_range_death, assign_v_invalid_other) {
    lh_memory_range_t a = lh_memory_range_empty_initializer();
    lh_memory_range_t b = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    lh_memory_range_set(&b, lo, hi);
    LH_EXPECT_DEATH(lh_memory_range_assign_v(&a, &b));
}

TEST(memory_range_death, set_v_invalid) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    LH_EXPECT_DEATH(lh_memory_range_set_v(&r, lo, hi));
}

TEST(memory_range_death, make_v_invalid) {
    unsigned char buf[2];
    lh_ptr lo = lh_cast_static(lh_ptr, buf + 1);
    lh_ptr hi = lh_cast_static(lh_ptr, buf);
    LH_EXPECT_DEATH((void)lh_memory_range_make_v(lo, hi));
}

TEST(memory_range_death, make_by_size_null_begin) {
    LH_EXPECT_DEATH((void)lh_memory_range_make_by_size(lh_null, 1));
}

TEST(memory_range_death, at_out_of_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 2);
    LH_EXPECT_DEATH((void)lh_memory_range_get_ptr_from_front(&r, 2));
}

TEST(memory_range_death, set_by_size_null_begin) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    LH_EXPECT_DEATH(lh_memory_range_set_by_size(&r, lh_null, 1));
}

TEST(memory_range_death, copy_range_invalid_self) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char src[] = {1};
    LH_EXPECT_DEATH(lh_memory_range_copy_range(&r, lh_cast_static(lh_ptr, src),
                                               lh_cast_static(lh_ptr, src + 1)));
}

TEST(memory_range_death, slice_out_of_range_by_offset_plus_size) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 8);
    LH_EXPECT_DEATH((void)lh_memory_range_slice(&r, 6, 3));
}

TEST(memory_range_death, slice_offset_out_of_range) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_range_init_by_size(&r, lh_cast_static(lh_ptr, buf), 4);
    LH_EXPECT_DEATH((void)lh_memory_range_slice(&r, 5, 0));
}

TEST(memory_range_death, pack_v_rejects_invalid_partial_update) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 8));
    lh_ptr bad_begin = lh_cast_static(lh_ptr, buf + 9);
    LH_EXPECT_DEATH((void)lh_memory_range_pack_v(&r, &bad_begin, nullptr));
}

TEST(memory_range_death, clone_v_rejects_invalid_source) {
    lh_memory_range_t r = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_set(&r, lh_cast_static(lh_ptr, buf + 1), lh_cast_static(lh_ptr, buf));
    LH_EXPECT_DEATH((void)lh_memory_range_clone_v(&r));
}

TEST(memory_range_death, dup_v_rejects_invalid_source) {
    lh_memory_range_t src = lh_memory_range_empty_initializer();
    lh_memory_range_t dst = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_range_set(&src, lh_cast_static(lh_ptr, buf + 1), lh_cast_static(lh_ptr, buf));
    lh_memory_range_set(&dst, lh_cast_static(lh_ptr, buf), lh_cast_static(lh_ptr, buf + 1));
    LH_EXPECT_DEATH((void)lh_memory_range_dup_v(&src, &dst));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
