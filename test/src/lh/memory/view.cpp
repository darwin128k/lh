#include <gtest/gtest.h>

#include <lh/cast.h>
#include <lh/expect/death.h>
#include <lh/memory/range/initializer.h>
#include <lh/memory/view.h>
#include <lh/null.h>
#include <lh/ptr.h>

namespace {

const lh_void *cv(lh_ptr p) {
    return lh_cast_static(const lh_void *, p);
}

TEST(memory_view_pack_unpack, partial_updates) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char low_mem;
    unsigned char high_mem;
    const lh_void *lo = cv(lh_cast_static(lh_ptr, &low_mem));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, &high_mem));
    if (lo > hi) {
        const lh_void *t = lo;
        lo = hi;
        hi = t;
    }

    lh_memory_view_pack(&v, &lo, nullptr);
    EXPECT_EQ(lh_memory_view_get_begin(&v), lo);
    EXPECT_EQ(lh_memory_view_get_end(&v), lh_null);

    lh_memory_view_pack(&v, nullptr, &hi);
    EXPECT_EQ(lh_memory_view_get_begin(&v), lo);
    EXPECT_EQ(lh_memory_view_get_end(&v), hi);
}

TEST(memory_view_init, getters_roundtrip) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 4));
    lh_memory_view_init(&v, b, e);
    EXPECT_EQ(lh_memory_view_get_begin(&v), b);
    EXPECT_EQ(lh_memory_view_get_end(&v), e);
}

TEST(memory_view_make, valid_pair_returns_expected_bounds) {
    unsigned char buf[4];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 4));
    lh_memory_view_t v = lh_memory_view_make(b, e);
    EXPECT_EQ(lh_memory_view_get_begin(&v), b);
    EXPECT_EQ(lh_memory_view_get_end(&v), e);
    EXPECT_TRUE(lh_memory_view_is_valid(&v));
}

TEST(memory_view_make_by_size, builds_expected_half_open_view) {
    unsigned char buf[5];
    lh_memory_view_t v = lh_memory_view_make_by_size(cv(lh_cast_static(lh_ptr, buf)), 5);
    EXPECT_EQ(lh_memory_view_get_begin(&v), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_end(&v), cv(lh_cast_static(lh_ptr, buf + 5)));
    EXPECT_EQ(lh_memory_view_get_size(&v), 5u);
}

TEST(memory_view_make_by_empty, returns_empty_initializer_state) {
    const lh_memory_view_t v = lh_memory_view_make_by_empty();
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
    EXPECT_EQ(lh_memory_view_get_begin(&v), cv(lh_null));
    EXPECT_EQ(lh_memory_view_get_end(&v), cv(lh_null));
}

TEST(memory_view_make, invalid_pair_is_allowed) {
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    lh_memory_view_t v = lh_memory_view_make(lo, hi);
    EXPECT_EQ(lh_memory_view_get_state(&v), lh_memory_range_state_reversed);
    EXPECT_TRUE(lh_memory_view_is_invalid(&v));
}

TEST(memory_view_state, uninitialized_empty_initializer) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    EXPECT_EQ(lh_memory_view_get_state(&v), lh_memory_range_state_uninitialized);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
    EXPECT_FALSE(lh_memory_view_is_empty(&v));
    EXPECT_FALSE(lh_memory_view_has_data(&v));
    EXPECT_FALSE(lh_memory_view_is_valid(&v));
    EXPECT_TRUE(lh_memory_view_is_invalid(&v));
}

TEST(memory_view_state, degenerate_equal_endpoints_is_empty) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char x = 0;
    const lh_void *p = cv(lh_cast_static(lh_ptr, &x));
    lh_memory_view_init(&v, p, p);
    EXPECT_EQ(lh_memory_view_get_state(&v), lh_memory_range_state_empty);
    EXPECT_TRUE(lh_memory_view_is_empty(&v));
    EXPECT_TRUE(lh_memory_view_is_valid(&v));
    EXPECT_EQ(lh_memory_view_get_size(&v), 0u);
}

TEST(memory_view_state, forward_span_has_data) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 8);
    EXPECT_EQ(lh_memory_view_get_state(&v), lh_memory_range_state_has_data);
    EXPECT_TRUE(lh_memory_view_has_data(&v));
    EXPECT_TRUE(lh_memory_view_is_valid(&v));
    EXPECT_EQ(lh_memory_view_get_size(&v), 8u);
    EXPECT_EQ(lh_memory_view_diff(&v), 8);
}

TEST(memory_view_state, reversed_bounds_invalid) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    lh_memory_view_init(&v, lo, hi);
    EXPECT_EQ(lh_memory_view_get_state(&v), lh_memory_range_state_reversed);
    EXPECT_TRUE(lh_memory_view_is_invalid(&v));
    EXPECT_LT(lh_memory_view_diff(&v), 0);
}

TEST(memory_view_state, only_begin_null) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    lh_memory_view_pack(&v, &b, nullptr);
    EXPECT_TRUE(lh_memory_view_is_invalid(&v));
    EXPECT_FALSE(lh_memory_view_is_uninitialized(&v));
}

TEST(memory_view_init_by_other, copies_bounds) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&a, cv(lh_cast_static(lh_ptr, buf)), 4);

    lh_memory_view_t b = lh_memory_range_empty_initializer();
    lh_memory_view_init_by_other(&b, &a);
    EXPECT_EQ(lh_memory_view_get_begin(&b), lh_memory_view_get_begin(&a));
    EXPECT_EQ(lh_memory_view_get_end(&b), lh_memory_view_get_end(&a));
}

TEST(memory_view_init_by_empty, resets_to_empty_initializer) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_set(&v, cv(lh_cast_static(lh_ptr, buf)), cv(lh_cast_static(lh_ptr, buf + 2)));
    lh_memory_view_init_by_empty(&v);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(memory_view_contains, half_open_endpoints) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 3));
    lh_memory_view_init(&v, b, e);
    EXPECT_TRUE(lh_memory_view_contains_ptr(&v, b));
    EXPECT_FALSE(lh_memory_view_contains_ptr(&v, e));
    EXPECT_TRUE(lh_memory_view_contains_ptr(&v, cv(lh_cast_static(lh_ptr, buf + 2))));
    EXPECT_TRUE(lh_memory_view_contains_ptr(&v, cv(lh_cast_static(lh_ptr, buf + 1))));
    EXPECT_FALSE(lh_memory_view_contains_ptr(&v, cv(lh_cast_static(lh_ptr, buf - 1))));
}

TEST(memory_view_contains_range, inner_within_outer) {
    lh_memory_view_t outer = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&outer, cv(lh_cast_static(lh_ptr, buf)), 8);

    const lh_void *i0 = cv(lh_cast_static(lh_ptr, buf + 2));
    const lh_void *i1 = cv(lh_cast_static(lh_ptr, buf + 5));
    EXPECT_TRUE(lh_memory_view_contains_range(&outer, i0, i1));

    const lh_void *o0 = cv(lh_cast_static(lh_ptr, buf - 1));
    EXPECT_FALSE(lh_memory_view_contains_range(&outer, o0, i1));
}

TEST(memory_view_contains, nested_view_object) {
    lh_memory_view_t outer = lh_memory_range_empty_initializer();
    lh_memory_view_t inner = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&outer, cv(lh_cast_static(lh_ptr, buf)), 8);
    lh_memory_view_init_by_size(&inner, cv(lh_cast_static(lh_ptr, buf + 2)), 3);
    EXPECT_TRUE(lh_memory_view_contains(&outer, &inner));
}

TEST(memory_view_equals, true_for_identical_bounds) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    const lh_void *begin = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *end = cv(lh_cast_static(lh_ptr, buf + 6));

    lh_memory_view_init(&a, begin, end);
    lh_memory_view_init(&b, begin, end);
    EXPECT_TRUE(lh_memory_view_equals(&a, &b));
}

TEST(memory_view_equals, false_for_different_bounds) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];

    lh_memory_view_init(&a, cv(lh_cast_static(lh_ptr, buf + 1)),
                        cv(lh_cast_static(lh_ptr, buf + 6)));
    lh_memory_view_init(&b, cv(lh_cast_static(lh_ptr, buf + 2)),
                        cv(lh_cast_static(lh_ptr, buf + 6)));
    EXPECT_FALSE(lh_memory_view_equals(&a, &b));
}

TEST(memory_view_equals_range, true_for_identical_bounds) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    const lh_void *begin = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *end = cv(lh_cast_static(lh_ptr, buf + 6));

    lh_memory_view_init(&v, begin, end);
    EXPECT_TRUE(lh_memory_view_equals_range(&v, begin, end));
}

TEST(memory_view_overlaps, touches_and_disjoint) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);

    const lh_void *a0 = cv(lh_cast_static(lh_ptr, buf + 2));
    const lh_void *a1 = cv(lh_cast_static(lh_ptr, buf + 5));
    EXPECT_TRUE(lh_memory_view_overlaps_range(&v, a0, a1));

    const lh_void *d0 = cv(lh_cast_static(lh_ptr, buf + 10));
    const lh_void *d1 = cv(lh_cast_static(lh_ptr, buf + 12));
    EXPECT_FALSE(lh_memory_view_overlaps_range(&v, d0, d1));
}

TEST(memory_view_overlaps, adjacent_half_open_disjoint) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);

    const lh_void *tail0 = cv(lh_cast_static(lh_ptr, buf + 4));
    const lh_void *tail1 = cv(lh_cast_static(lh_ptr, buf + 8));
    EXPECT_FALSE(lh_memory_view_overlaps_range(&v, tail0, tail1));
}

TEST(memory_view_at, front_back_and_offsets) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0xA, 0xB, 0xC, 0xD};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);

    EXPECT_EQ(lh_memory_view_get_front_ptr(&v), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_back_ptr(&v), cv(lh_cast_static(lh_ptr, buf + 3)));
    EXPECT_EQ(lh_memory_view_get_front_value(&v), 0xA);
    EXPECT_EQ(lh_memory_view_get_back_value(&v), 0xD);

    EXPECT_EQ(lh_memory_view_get_ptr_from_front(&v, 0), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_ptr_from_front(&v, 3), cv(lh_cast_static(lh_ptr, buf + 3)));
    EXPECT_EQ(lh_memory_view_get_ptr_from_back(&v, 0), cv(lh_cast_static(lh_ptr, buf + 3)));
    EXPECT_EQ(lh_memory_view_get_ptr_from_back(&v, 3), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_ptr(&v, 1, lh_bool_false), cv(lh_cast_static(lh_ptr, buf + 1)));
    EXPECT_EQ(lh_memory_view_get_ptr(&v, 1, lh_bool_true), cv(lh_cast_static(lh_ptr, buf + 2)));
    EXPECT_EQ(lh_memory_view_get_value_from_front(&v, 1), 0xB);
    EXPECT_EQ(lh_memory_view_get_value_from_back(&v, 1), 0xC);
    EXPECT_EQ(lh_memory_view_get_value(&v, 2, lh_bool_false), 0xC);
    EXPECT_EQ(lh_memory_view_get_value(&v, 2, lh_bool_true), 0xB);

    EXPECT_TRUE(lh_memory_view_is_valid_offset(&v, 0));
    EXPECT_TRUE(lh_memory_view_is_valid_offset(&v, 3));
    EXPECT_FALSE(lh_memory_view_is_valid_offset(&v, 4));
}

TEST(memory_view_next_ptr, returns_next_or_null_at_end) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);

    EXPECT_EQ(lh_memory_view_next_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[0]))),
              cv(lh_cast_static(lh_ptr, &buf[1])));
    EXPECT_EQ(lh_memory_view_next_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[2]))),
              cv(lh_cast_static(lh_ptr, &buf[3])));
    EXPECT_TRUE(lh_null_eq(lh_memory_view_next_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[3])))));
}

TEST(memory_view_next_ptr, returns_null_for_pointer_outside_view) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);
    EXPECT_TRUE(lh_null_eq(lh_memory_view_next_ptr(&v, cv(lh_cast_static(lh_ptr, buf - 1)))));
}

TEST(memory_view_prev_ptr, returns_prev_or_null_at_begin) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);

    EXPECT_EQ(lh_memory_view_prev_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[3]))),
              cv(lh_cast_static(lh_ptr, &buf[2])));
    EXPECT_EQ(lh_memory_view_prev_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[1]))),
              cv(lh_cast_static(lh_ptr, &buf[0])));
    EXPECT_TRUE(lh_null_eq(lh_memory_view_prev_ptr(&v, cv(lh_cast_static(lh_ptr, &buf[0])))));
}

TEST(memory_view_prev_ptr, returns_null_for_pointer_outside_view) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {0};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);
    EXPECT_TRUE(lh_null_eq(lh_memory_view_prev_ptr(&v, cv(lh_cast_static(lh_ptr, buf + 4)))));
}

TEST(memory_view_next_prev_value, returns_neighbor_values) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4] = {10, 20, 30, 40};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);
    EXPECT_EQ(lh_memory_view_next_value(&v, cv(lh_cast_static(lh_ptr, &buf[1]))), 30);
    EXPECT_EQ(lh_memory_view_prev_value(&v, cv(lh_cast_static(lh_ptr, &buf[2]))), 20);
}

TEST(memory_view_slice, returns_expected_subrange_from_front) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[6] = {};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 6);

    const lh_memory_view_t s = lh_memory_view_slice(&v, 0, 3);
    EXPECT_EQ(lh_memory_view_get_begin(&s), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_end(&s), cv(lh_cast_static(lh_ptr, buf + 3)));
    EXPECT_EQ(lh_memory_view_get_size(&s), 3u);
}

TEST(memory_view_slice, supports_non_zero_offset) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8] = {};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 8);

    const lh_memory_view_t s = lh_memory_view_slice(&v, 1, 3);
    EXPECT_EQ(lh_memory_view_get_begin(&s), cv(lh_cast_static(lh_ptr, buf + 1)));
    EXPECT_EQ(lh_memory_view_get_end(&s), cv(lh_cast_static(lh_ptr, buf + 4)));
    EXPECT_EQ(lh_memory_view_get_size(&s), 3u);
}

TEST(memory_view_slice_or_empty, returns_slice_or_empty) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8] = {};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 8);

    const lh_memory_view_t ok = lh_memory_view_slice_or_empty(&v, 1, 3);
    EXPECT_EQ(lh_memory_view_get_begin(&ok), cv(lh_cast_static(lh_ptr, buf + 1)));
    EXPECT_EQ(lh_memory_view_get_end(&ok), cv(lh_cast_static(lh_ptr, buf + 4)));

    const lh_memory_view_t bad = lh_memory_view_slice_or_empty(&v, 7, 2);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&bad));
}

TEST(memory_view_alignment, begin_and_full) {
    alignas(16) unsigned char block[32];
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, block)), 16);
    EXPECT_TRUE(lh_memory_view_is_begin_aligned(&v, 16));
    EXPECT_TRUE(lh_memory_view_is_aligned(&v, 16));
}

TEST(memory_view_multiple_of, size_divisible) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[12];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 12);
    EXPECT_TRUE(lh_memory_view_is_multiple_of(&v, 4));
    EXPECT_FALSE(lh_memory_view_is_multiple_of(&v, 5));
}

TEST(memory_view_clear, resets_to_empty_initializer) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 1);
    lh_memory_view_clear(&v);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(memory_view_assign_v, copies_valid) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_init_by_size(&a, cv(lh_cast_static(lh_ptr, buf)), 2);
    lh_memory_view_assign_v(&b, &a);
    EXPECT_EQ(lh_memory_view_get_begin(&b), lh_memory_view_get_begin(&a));
    EXPECT_EQ(lh_memory_view_get_end(&b), lh_memory_view_get_end(&a));
}

TEST(memory_view_init_v, valid_pair) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[3];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 2));
    lh_memory_view_init_v(&v, b, e);
    EXPECT_EQ(lh_memory_view_get_begin(&v), b);
    EXPECT_EQ(lh_memory_view_get_end(&v), e);
}

TEST(memory_view_init_by_size_or_clear, clears_on_invalid_begin) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 1);
    ASSERT_TRUE(lh_memory_view_has_data(&v));

    lh_memory_view_init_by_size_or_clear(&v, lh_null, 4);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(memory_view_swap_exchange, mutates_pair) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char x[1];
    unsigned char y[2];
    lh_memory_view_init_by_size(&a, cv(lh_cast_static(lh_ptr, x)), 1);
    lh_memory_view_init_by_size(&b, cv(lh_cast_static(lh_ptr, y)), 2);

    lh_memory_view_swap(&a, &b);
    EXPECT_EQ(lh_memory_view_get_begin(&a), cv(lh_cast_static(lh_ptr, y)));
    EXPECT_EQ(lh_memory_view_get_begin(&b), cv(lh_cast_static(lh_ptr, x)));

    lh_memory_view_exchange(&a, &b);
    EXPECT_TRUE(lh_memory_view_has_data(&a));
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&b));
}

TEST(memory_view_clone_dup, raw_copy_semantics) {
    lh_memory_view_t src = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&src, cv(lh_cast_static(lh_ptr, buf)), 4);

    const lh_memory_view_t c = lh_memory_view_clone(&src);
    EXPECT_EQ(lh_memory_view_get_begin(&c), lh_memory_view_get_begin(&src));
    EXPECT_EQ(lh_memory_view_get_end(&c), lh_memory_view_get_end(&src));

    lh_memory_view_t d = lh_memory_range_empty_initializer();
    lh_memory_view_dup(&src, &d);
    EXPECT_EQ(lh_memory_view_get_begin(&d), lh_memory_view_get_begin(&src));
    EXPECT_EQ(lh_memory_view_get_end(&d), lh_memory_view_get_end(&src));
}

TEST(memory_view_clone_dup_v, validated_copy_semantics) {
    lh_memory_view_t src = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&src, cv(lh_cast_static(lh_ptr, buf)), 4);

    const lh_memory_view_t c = lh_memory_view_clone_v(&src);
    EXPECT_EQ(lh_memory_view_get_begin(&c), lh_memory_view_get_begin(&src));
    EXPECT_EQ(lh_memory_view_get_end(&c), lh_memory_view_get_end(&src));

    lh_memory_view_t d = lh_memory_range_empty_initializer();
    lh_memory_view_dup_v(&src, &d);
    EXPECT_EQ(lh_memory_view_get_begin(&d), lh_memory_view_get_begin(&src));
    EXPECT_EQ(lh_memory_view_get_end(&d), lh_memory_view_get_end(&src));
}

TEST(memory_view_find_range, finds_first_occurrence) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, hay)), 5);
    unsigned char needle[] = {'a', 'b'};
    const lh_void *p = lh_memory_view_find_range(&v, cv(lh_cast_static(lh_ptr, needle)),
                                                 cv(lh_cast_static(lh_ptr, needle + 2)));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, cv(lh_cast_static(lh_ptr, &hay[0])));
}

TEST(memory_view_find_range, not_found_returns_null) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char hay[] = {1, 2, 3};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, hay)), 3);
    unsigned char needle[] = {9, 9};
    const lh_void *p = lh_memory_view_find_range(&v, cv(lh_cast_static(lh_ptr, needle)),
                                                 cv(lh_cast_static(lh_ptr, needle + 2)));
    EXPECT_TRUE(lh_null_eq(p));
}

TEST(memory_view_rfind_range, finds_last_occurrence) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'a', 'b', 'a', 'b', 'c'};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, hay)), 5);
    unsigned char needle[] = {'a', 'b'};
    const lh_void *p = lh_memory_view_rfind_range(&v, cv(lh_cast_static(lh_ptr, needle)),
                                                  cv(lh_cast_static(lh_ptr, needle + 2)));
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, cv(lh_cast_static(lh_ptr, &hay[2])));
}

TEST(memory_view_compare_range, equal_within_min_length_returns_null) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {1, 2, 3};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, lhs)), 3);
    unsigned char rhs[] = {1, 2};
    const lh_void *d = lh_memory_view_compare_range(&v, cv(lh_cast_static(lh_ptr, rhs)),
                                                    cv(lh_cast_static(lh_ptr, rhs + 2)));
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_view_compare_range, mismatch_returns_pointer_in_view) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {1, 9, 3};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, lhs)), 3);
    unsigned char rhs[] = {1, 2, 3};
    const lh_void *d = lh_memory_view_compare_range(&v, cv(lh_cast_static(lh_ptr, rhs)),
                                                    cv(lh_cast_static(lh_ptr, rhs + 3)));
    ASSERT_TRUE(lh_null_ne(d));
    EXPECT_EQ(d, cv(lh_cast_static(lh_ptr, &lhs[1])));
}

TEST(memory_view_rcompare_range, equal_suffix_returns_null) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {0, 0, 1, 2, 3, 4};
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, lhs)), 6);
    unsigned char rhs[] = {1, 2, 3, 4};
    const lh_void *d = lh_memory_view_rcompare_range(&v, cv(lh_cast_static(lh_ptr, rhs)),
                                                     cv(lh_cast_static(lh_ptr, rhs + 4)));
    EXPECT_TRUE(lh_null_eq(d));
}

TEST(memory_view_unpack, writes_both_endpoints) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[3];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 3));
    lh_memory_view_init(&v, b, e);

    const lh_void *out_b = lh_null;
    const lh_void *out_e = lh_null;
    lh_memory_view_unpack(&v, &out_b, &out_e);
    EXPECT_EQ(out_b, b);
    EXPECT_EQ(out_e, e);
}

TEST(memory_view_assign, copies_like_init_by_other) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[5];
    lh_memory_view_init_by_size(&a, cv(lh_cast_static(lh_ptr, buf)), 5);
    lh_memory_view_assign(&b, &a);
    EXPECT_EQ(lh_memory_view_get_begin(&b), lh_memory_view_get_begin(&a));
    EXPECT_EQ(lh_memory_view_get_end(&b), lh_memory_view_get_end(&a));
}

TEST(memory_view_init_by_size, sets_half_open_span) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);
    EXPECT_EQ(lh_memory_view_get_begin(&v), cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(lh_memory_view_get_end(&v), cv(lh_cast_static(lh_ptr, buf + 4)));
    EXPECT_EQ(lh_memory_view_get_size(&v), 4u);
}

TEST(memory_view_make_v, accepts_valid_ordered_pair) {
    unsigned char buf[3];
    const lh_void *b = cv(lh_cast_static(lh_ptr, buf));
    const lh_void *e = cv(lh_cast_static(lh_ptr, buf + 3));
    lh_memory_view_t v = lh_memory_view_make_v(b, e);
    EXPECT_EQ(lh_memory_view_get_begin(&v), b);
    EXPECT_EQ(lh_memory_view_get_end(&v), e);
    EXPECT_TRUE(lh_memory_view_is_valid(&v));
}

TEST(memory_view_make_or_empty, invalid_pair_returns_empty) {
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    const lh_memory_view_t v = lh_memory_view_make_or_empty(lo, hi);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(memory_view_unpack_v, reads_valid_view) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 2);
    const lh_void *o0 = lh_null;
    const lh_void *o1 = lh_null;
    lh_memory_view_unpack_v(&v, &o0, &o1);
    EXPECT_EQ(o0, cv(lh_cast_static(lh_ptr, buf)));
    EXPECT_EQ(o1, cv(lh_cast_static(lh_ptr, buf + 2)));
}

TEST(memory_view_is_sliceable, in_bounds_and_out_of_bounds) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 8);
    EXPECT_TRUE(lh_memory_view_is_sliceable(&v, 0, 8));
    EXPECT_TRUE(lh_memory_view_is_sliceable(&v, 1, 3));
    EXPECT_FALSE(lh_memory_view_is_sliceable(&v, 6, 3));
}

TEST(memory_view_overlaps, two_views_intersect_or_not) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&a, cv(lh_cast_static(lh_ptr, buf)), 4);
    lh_memory_view_init_by_size(&b, cv(lh_cast_static(lh_ptr, buf + 3)), 3);
    EXPECT_TRUE(lh_memory_view_overlaps(&a, &b));

    lh_memory_view_t c = lh_memory_range_empty_initializer();
    lh_memory_view_init_by_size(&c, cv(lh_cast_static(lh_ptr, buf + 4)), 4);
    EXPECT_FALSE(lh_memory_view_overlaps(&a, &c));
}

TEST(memory_view_find, finds_via_other_view) {
    lh_memory_view_t hay_v = lh_memory_range_empty_initializer();
    lh_memory_view_t needle_v = lh_memory_range_empty_initializer();
    unsigned char hay[] = {'x', 'y', 'z'};
    unsigned char needle[] = {'y', 'z'};
    lh_memory_view_init_by_size(&hay_v, cv(lh_cast_static(lh_ptr, hay)), 3);
    lh_memory_view_init_by_size(&needle_v, cv(lh_cast_static(lh_ptr, needle)), 2);
    const lh_void *p = lh_memory_view_find(&hay_v, &needle_v);
    ASSERT_TRUE(lh_null_ne(p));
    EXPECT_EQ(p, cv(lh_cast_static(lh_ptr, &hay[1])));
}

TEST(memory_view_compare, matches_compare_range) {
    lh_memory_view_t lhs_v = lh_memory_range_empty_initializer();
    lh_memory_view_t rhs_v = lh_memory_range_empty_initializer();
    unsigned char lhs[] = {5, 6, 7};
    unsigned char rhs[] = {5, 6};
    lh_memory_view_init_by_size(&lhs_v, cv(lh_cast_static(lh_ptr, lhs)), 3);
    lh_memory_view_init_by_size(&rhs_v, cv(lh_cast_static(lh_ptr, rhs)), 2);
    const lh_void *d = lh_memory_view_compare(&lhs_v, &rhs_v);
    EXPECT_TRUE(lh_null_eq(d));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_view_death, unpack_v_on_invalid) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    const lh_void *out0 = nullptr;
    const lh_void *out1 = nullptr;
    LH_EXPECT_DEATH(lh_memory_view_unpack_v(&v, &out0, &out1));
}

TEST(memory_view_death, get_size_on_invalid) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[1];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf));
    lh_memory_view_pack(&v, &lo, nullptr);
    LH_EXPECT_DEATH((void)lh_memory_view_get_size(&v));
}

TEST(memory_view_death, assign_v_invalid_other) {
    lh_memory_view_t a = lh_memory_range_empty_initializer();
    lh_memory_view_t b = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    lh_memory_view_init(&b, lo, hi);
    LH_EXPECT_DEATH(lh_memory_view_assign_v(&a, &b));
}

TEST(memory_view_death, init_v_invalid) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    LH_EXPECT_DEATH(lh_memory_view_init_v(&v, lo, hi));
}

TEST(memory_view_death, make_v_invalid) {
    unsigned char buf[2];
    const lh_void *lo = cv(lh_cast_static(lh_ptr, buf + 1));
    const lh_void *hi = cv(lh_cast_static(lh_ptr, buf));
    LH_EXPECT_DEATH((void)lh_memory_view_make_v(lo, hi));
}

TEST(memory_view_death, make_by_size_null_begin) {
    LH_EXPECT_DEATH((void)lh_memory_view_make_by_size(lh_null, 1));
}

TEST(memory_view_death, at_out_of_range) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 2);
    LH_EXPECT_DEATH((void)lh_memory_view_get_ptr_from_front(&v, 2));
}

TEST(memory_view_death, init_by_size_null_begin) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    LH_EXPECT_DEATH(lh_memory_view_init_by_size(&v, lh_null, 1));
}

TEST(memory_view_death, slice_out_of_range_by_offset_plus_size) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[8];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 8);
    LH_EXPECT_DEATH((void)lh_memory_view_slice(&v, 6, 3));
}

TEST(memory_view_death, slice_offset_out_of_range) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[4];
    lh_memory_view_init_by_size(&v, cv(lh_cast_static(lh_ptr, buf)), 4);
    LH_EXPECT_DEATH((void)lh_memory_view_slice(&v, 5, 0));
}

TEST(memory_view_death, clone_v_rejects_invalid_source) {
    lh_memory_view_t v = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_init(&v, cv(lh_cast_static(lh_ptr, buf + 1)), cv(lh_cast_static(lh_ptr, buf)));
    LH_EXPECT_DEATH((void)lh_memory_view_clone_v(&v));
}

TEST(memory_view_death, dup_v_rejects_invalid_source) {
    lh_memory_view_t src = lh_memory_range_empty_initializer();
    lh_memory_view_t dst = lh_memory_range_empty_initializer();
    unsigned char buf[2];
    lh_memory_view_init(&src, cv(lh_cast_static(lh_ptr, buf + 1)), cv(lh_cast_static(lh_ptr, buf)));
    lh_memory_view_init(&dst, cv(lh_cast_static(lh_ptr, buf)),
                        cv(lh_cast_static(lh_ptr, buf + 1)));
    LH_EXPECT_DEATH((void)lh_memory_view_dup_v(&src, &dst));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
