#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/expect/death.h>
#include <lh/memory/typed.h>
#include <lh/null.h>

namespace {

TEST(memory_typed_basic, range_accessors_alias_embedded_range) {
    lh_memory_typed_t typed;
    unsigned char buffer[4];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 4),
                        1);

    EXPECT_EQ(lh_memory_typed_get_range(&typed), &typed.range);
    EXPECT_EQ(lh_memory_typed_get_crange(&typed), &typed.range);
}

TEST(memory_typed_basic, set_and_unpack_roundtrip_all_fields) {
    lh_memory_typed_t typed;
    unsigned char buffer[10];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 10),
                        2);

    lh_ptr begin = lh_null;
    lh_ptr end = lh_null;
    lh_usize_t type_size = 0;
    lh_memory_typed_unpack(&typed, &begin, &end, &type_size);

    EXPECT_EQ(begin, lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(end, lh_cast_static(lh_ptr, buffer + 10));
    EXPECT_EQ(type_size, 2u);
}

TEST(memory_typed_basic, pack_supports_optional_updates) {
    lh_memory_typed_t typed;
    unsigned char buffer[8];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 8),
                        2);

    lh_ptr new_begin = lh_cast_static(lh_ptr, buffer + 2);
    lh_memory_typed_pack(&typed, &new_begin, nullptr, nullptr);
    EXPECT_EQ(lh_memory_typed_get_begin(&typed), lh_cast_static(lh_ptr, buffer + 2));
    EXPECT_EQ(lh_memory_typed_get_end(&typed), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 2u);

    lh_usize_t new_type_size = 1;
    lh_memory_typed_pack(&typed, nullptr, nullptr, &new_type_size);
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 1u);
}

TEST(memory_typed_basic, pack_from_range_sets_bounds_and_type_size) {
    lh_memory_typed_t typed;
    lh_memory_typed_t baseline;
    unsigned char buffer[10];
    lh_memory_typed_set(&baseline, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 10),
                        2);

    lh_memory_range_t range;
    lh_ptr begin = lh_cast_static(lh_ptr, buffer + 2);
    lh_ptr end = lh_cast_static(lh_ptr, buffer + 8);
    lh_memory_range_pack(&range, &begin, &end);

    lh_memory_typed_assign(&typed, &baseline);
    lh_memory_typed_pack_from_range(&typed, &range, nullptr);
    EXPECT_EQ(lh_memory_typed_get_begin(&typed), lh_cast_static(lh_ptr, buffer + 2));
    EXPECT_EQ(lh_memory_typed_get_end(&typed), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 2u);

    lh_usize_t new_type_size = 3;
    lh_memory_typed_pack_from_range(&typed, &range, &new_type_size);
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 3u);
}

TEST(memory_typed_basic, pack_v_supports_optional_updates) {
    lh_memory_typed_t typed;
    unsigned char buffer[8];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 8),
                        2);

    lh_ptr new_begin = lh_cast_static(lh_ptr, buffer + 2);
    lh_memory_typed_pack_v(&typed, &new_begin, nullptr, nullptr);
    EXPECT_EQ(lh_memory_typed_get_begin(&typed), lh_cast_static(lh_ptr, buffer + 2));
    EXPECT_EQ(lh_memory_typed_get_end(&typed), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 2u);

    lh_usize_t new_type_size = 1;
    lh_memory_typed_pack_v(&typed, nullptr, nullptr, &new_type_size);
    EXPECT_EQ(lh_memory_typed_get_type_size(&typed), 1u);
}

TEST(memory_typed_basic, unpack_v_roundtrip_all_fields_for_valid_typed) {
    lh_memory_typed_t typed;
    unsigned char buffer[12];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        3);

    lh_ptr begin = lh_null;
    lh_ptr end = lh_null;
    lh_usize_t type_size = 0;
    lh_memory_typed_unpack_v(&typed, &begin, &end, &type_size);

    EXPECT_EQ(begin, lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(end, lh_cast_static(lh_ptr, buffer + 12));
    EXPECT_EQ(type_size, 3u);
}

TEST(memory_typed_basic, assign_and_init_by_size_copy_expected_state) {
    lh_memory_typed_t src;
    unsigned char buffer[12];
    lh_memory_typed_set_by_size(&src, lh_cast_static(lh_ptr, buffer), 12, 3);

    lh_memory_typed_t dst;
    lh_memory_typed_init(&dst, lh_null, lh_null, 1);
    lh_memory_typed_assign(&dst, &src);

    EXPECT_EQ(lh_memory_typed_get_begin(&dst), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&dst), lh_cast_static(lh_ptr, buffer + 12));
    EXPECT_EQ(lh_memory_typed_get_type_size(&dst), 3u);

    lh_memory_typed_t by_size;
    lh_memory_typed_init_by_size(&by_size, lh_cast_static(lh_ptr, buffer), 12, 3);
    EXPECT_EQ(lh_memory_typed_get_size(&by_size), 4u);
}

TEST(memory_typed_basic, clone_and_clone_v_copy_all_fields) {
    lh_memory_typed_t src;
    unsigned char buffer[12];
    lh_memory_typed_set(&src, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12), 3);

    const lh_memory_typed_t c1 = lh_memory_typed_clone(&src);
    EXPECT_EQ(lh_memory_typed_get_begin(&c1), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&c1), lh_cast_static(lh_ptr, buffer + 12));
    EXPECT_EQ(lh_memory_typed_get_type_size(&c1), 3u);

    const lh_memory_typed_t c2 = lh_memory_typed_clone_v(&src);
    EXPECT_EQ(lh_memory_typed_get_begin(&c2), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&c2), lh_cast_static(lh_ptr, buffer + 12));
    EXPECT_EQ(lh_memory_typed_get_type_size(&c2), 3u);
}

TEST(memory_typed_basic, dup_and_dup_v_copy_all_fields) {
    lh_memory_typed_t src;
    lh_memory_typed_t dst;
    lh_memory_typed_t dst_v;
    unsigned char buffer[10];
    lh_memory_typed_set(&src, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 10), 2);
    lh_memory_typed_init(&dst, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 2), 1);
    lh_memory_typed_init(&dst_v, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 2), 1);

    lh_memory_typed_dup(&src, &dst);
    EXPECT_EQ(lh_memory_typed_get_begin(&dst), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&dst), lh_cast_static(lh_ptr, buffer + 10));
    EXPECT_EQ(lh_memory_typed_get_type_size(&dst), 2u);

    lh_memory_typed_dup_v(&src, &dst_v);
    EXPECT_EQ(lh_memory_typed_get_begin(&dst_v), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&dst_v), lh_cast_static(lh_ptr, buffer + 10));
    EXPECT_EQ(lh_memory_typed_get_type_size(&dst_v), 2u);
}

TEST(memory_typed_basic, assign_v_copies_from_valid_source) {
    lh_memory_typed_t src;
    lh_memory_typed_t dst;
    unsigned char buffer[12];
    lh_memory_typed_set(&src, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12), 3);
    lh_memory_typed_init(&dst, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 3), 1);

    lh_memory_typed_assign_v(&dst, &src);
    EXPECT_EQ(lh_memory_typed_get_begin(&dst), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&dst), lh_cast_static(lh_ptr, buffer + 12));
    EXPECT_EQ(lh_memory_typed_get_type_size(&dst), 3u);
}

TEST(memory_typed_basic, make_by_empty_returns_empty_initializer_state) {
    const lh_memory_typed_t t = lh_memory_typed_make_by_empty(4);
    EXPECT_EQ(t.range.first, lh_null);
    EXPECT_EQ(t.range.second, lh_null);
    EXPECT_EQ(t.type_size, 4u);
}

TEST(memory_typed_basic, init_by_empty_resets_to_empty_initializer) {
    lh_memory_typed_t t;
    unsigned char buffer[6];
    lh_memory_typed_set(&t, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 6), 2);
    lh_memory_typed_init_by_empty(&t, 2);
    EXPECT_EQ(t.range.first, lh_null);
    EXPECT_EQ(t.range.second, lh_null);
    EXPECT_EQ(t.type_size, 2u);
}

TEST(memory_typed_basic, make_or_empty_returns_range_or_empty) {
    unsigned char buffer[6];
    const lh_memory_typed_t ok = lh_memory_typed_make_or_empty(lh_cast_static(lh_ptr, buffer),
                                                                lh_cast_static(lh_ptr, buffer + 6), 3);
    EXPECT_EQ(lh_memory_typed_get_begin(&ok), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&ok), lh_cast_static(lh_ptr, buffer + 6));
    EXPECT_EQ(lh_memory_typed_get_type_size(&ok), 3u);

    const lh_memory_typed_t bad =
        lh_memory_typed_make_or_empty(lh_cast_static(lh_ptr, buffer + 1), lh_cast_static(lh_ptr, buffer), 1);
    EXPECT_EQ(bad.range.first, lh_null);
    EXPECT_EQ(bad.range.second, lh_null);
}

TEST(memory_typed_basic, make_and_make_v_create_expected_typed) {
    unsigned char buffer[8];
    const lh_memory_typed_t made =
        lh_memory_typed_make(lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 8), 2);
    EXPECT_EQ(lh_memory_typed_get_begin(&made), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&made), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_type_size(&made), 2u);

    const lh_memory_typed_t made_v =
        lh_memory_typed_make_v(lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 8), 2);
    EXPECT_EQ(lh_memory_typed_get_begin(&made_v), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_end(&made_v), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_type_size(&made_v), 2u);
}

TEST(memory_typed_geometry, size_empty_and_index_validation) {
    lh_memory_typed_t typed;
    unsigned char buffer[9];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 9),
                        3);

    EXPECT_EQ(lh_memory_typed_get_size(&typed), 3u);
    EXPECT_FALSE(lh_memory_typed_is_empty(&typed));
    EXPECT_TRUE(lh_memory_typed_is_valid_index(&typed, 0));
    EXPECT_TRUE(lh_memory_typed_is_valid_index(&typed, 2));
    EXPECT_FALSE(lh_memory_typed_is_valid_index(&typed, 3));
}

TEST(memory_typed_geometry, is_sliceable_in_bounds_and_out_of_bounds) {
    lh_memory_typed_t typed;
    unsigned char buffer[12];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        3);

    EXPECT_TRUE(lh_memory_typed_is_sliceable(&typed, 0, 4));
    EXPECT_TRUE(lh_memory_typed_is_sliceable(&typed, 1, 2));
    EXPECT_FALSE(lh_memory_typed_is_sliceable(&typed, 3, 2));
}

TEST(memory_typed_geometry, slice_returns_expected_subspan) {
    lh_memory_typed_t typed;
    unsigned char buffer[12];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        3);

    const lh_memory_typed_t s = lh_memory_typed_slice(&typed, 1, 2);
    EXPECT_EQ(lh_memory_typed_get_begin(&s), lh_cast_static(lh_ptr, buffer + 3));
    EXPECT_EQ(lh_memory_typed_get_end(&s), lh_cast_static(lh_ptr, buffer + 9));
    EXPECT_EQ(lh_memory_typed_get_type_size(&s), 3u);
    EXPECT_EQ(lh_memory_typed_get_size(&s), 2u);
}

TEST(memory_typed_geometry, slice_or_empty_returns_subspan_or_empty) {
    lh_memory_typed_t typed;
    unsigned char buffer[12];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        3);

    const lh_memory_typed_t ok = lh_memory_typed_slice_or_empty(&typed, 1, 2);
    EXPECT_EQ(lh_memory_typed_get_begin(&ok), lh_cast_static(lh_ptr, buffer + 3));
    EXPECT_EQ(lh_memory_typed_get_end(&ok), lh_cast_static(lh_ptr, buffer + 9));

    const lh_memory_typed_t bad = lh_memory_typed_slice_or_empty(&typed, 3, 2);
    EXPECT_EQ(bad.range.first, lh_null);
    EXPECT_EQ(bad.range.second, lh_null);
}

TEST(memory_typed_geometry, is_valid_reflects_type_multiple_for_valid_range) {
    lh_memory_typed_t typed;
    unsigned char buffer[2];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 2),
                        1);
    EXPECT_TRUE(lh_memory_typed_is_valid(&typed));
    EXPECT_FALSE(lh_memory_typed_is_invalid(&typed));
}

TEST(memory_typed_access, ptr_access_front_back_and_dispatch) {
    lh_memory_typed_t typed;
    unsigned char buffer[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        4);

    EXPECT_EQ(lh_memory_typed_get_ptr_from_front(&typed, 0), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_ptr_from_front(&typed, 2), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_ptr_from_back(&typed, 0), lh_cast_static(lh_ptr, buffer + 8));
    EXPECT_EQ(lh_memory_typed_get_ptr_from_back(&typed, 2), lh_cast_static(lh_ptr, buffer));

    EXPECT_EQ(lh_memory_typed_get_ptr(&typed, 1, lh_bool_false),
              lh_cast_static(lh_ptr, buffer + 4));
    EXPECT_EQ(lh_memory_typed_get_ptr(&typed, 1, lh_bool_true), lh_cast_static(lh_ptr, buffer + 4));
}

TEST(memory_typed_access, value_access_front_back_and_extremes) {
    lh_memory_typed_t typed;
    unsigned char buffer[6] = {0x10, 0x11, 0x20, 0x21, 0x30, 0x31};
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 6),
                        2);

    EXPECT_EQ(lh_memory_typed_get_value_from_front(&typed, 0), 0x10);
    EXPECT_EQ(lh_memory_typed_get_value_from_front(&typed, 2), 0x30);
    EXPECT_EQ(lh_memory_typed_get_value_from_back(&typed, 0), 0x30);
    EXPECT_EQ(lh_memory_typed_get_value_from_back(&typed, 2), 0x10);
    EXPECT_EQ(lh_memory_typed_get_value(&typed, 1, lh_bool_false), 0x20);
    EXPECT_EQ(lh_memory_typed_get_value(&typed, 1, lh_bool_true), 0x20);

    EXPECT_EQ(lh_memory_typed_get_front_ptr(&typed), lh_cast_static(lh_ptr, buffer));
    EXPECT_EQ(lh_memory_typed_get_back_ptr(&typed), lh_cast_static(lh_ptr, buffer + 4));
    EXPECT_EQ(lh_memory_typed_get_front_value(&typed), 0x10);
    EXPECT_EQ(lh_memory_typed_get_back_value(&typed), 0x30);
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(memory_typed_death, null_self_in_get_range) {
    LH_EXPECT_DEATH(
        (void)lh_memory_typed_get_range(reinterpret_cast<lh_memory_typed_t *>(lh_null)));
}

TEST(memory_typed_death, null_self_in_get_crange) {
    LH_EXPECT_DEATH(
        (void)lh_memory_typed_get_crange(reinterpret_cast<const lh_memory_typed_t *>(lh_null)));
}

TEST(memory_typed_death, get_size_requires_multiple_of_type_size) {
    lh_memory_typed_t typed;
    unsigned char buffer[5];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 5),
                        2);
    LH_EXPECT_DEATH((void)lh_memory_typed_get_size(&typed));
}

TEST(memory_typed_death, is_valid_requires_valid_underlying_range) {
    lh_memory_typed_t typed;
    lh_memory_typed_init(&typed, lh_null, lh_null, 1);
    LH_EXPECT_DEATH((void)lh_memory_typed_is_valid(&typed));
}

TEST(memory_typed_death, is_invalid_requires_valid_underlying_range) {
    lh_memory_typed_t typed;
    lh_memory_typed_init(&typed, lh_null, lh_null, 1);
    LH_EXPECT_DEATH((void)lh_memory_typed_is_invalid(&typed));
}

TEST(memory_typed_death, get_ptr_from_front_rejects_out_of_range) {
    lh_memory_typed_t typed;
    unsigned char buffer[4];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 4),
                        2);
    LH_EXPECT_DEATH((void)lh_memory_typed_get_ptr_from_front(&typed, 2));
}

TEST(memory_typed_death, pack_v_rejects_partial_update_that_makes_range_invalid) {
    lh_memory_typed_t typed;
    unsigned char buffer[8];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 8),
                        1);

    lh_ptr bad_begin = lh_cast_static(lh_ptr, buffer + 9);
    LH_EXPECT_DEATH((void)lh_memory_typed_pack_v(&typed, &bad_begin, nullptr, nullptr));
}

TEST(memory_typed_death, assign_v_rejects_source_with_non_multiple_size) {
    lh_memory_typed_t src;
    lh_memory_typed_t dst;
    unsigned char buffer[5];
    lh_memory_typed_set(&src, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 5), 2);
    lh_memory_typed_set(&dst, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 4), 2);
    LH_EXPECT_DEATH((void)lh_memory_typed_assign_v(&dst, &src));
}

TEST(memory_typed_death, unpack_v_rejects_non_multiple_size) {
    lh_memory_typed_t typed;
    unsigned char buffer[5];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 5), 2);

    lh_ptr begin = lh_null;
    lh_ptr end = lh_null;
    lh_usize_t type_size = 0;
    LH_EXPECT_DEATH((void)lh_memory_typed_unpack_v(&typed, &begin, &end, &type_size));
}

TEST(memory_typed_death, clone_v_rejects_source_with_non_multiple_size) {
    lh_memory_typed_t typed;
    unsigned char buffer[5];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 5), 2);
    LH_EXPECT_DEATH((void)lh_memory_typed_clone_v(&typed));
}

TEST(memory_typed_death, dup_v_rejects_source_with_non_multiple_size) {
    lh_memory_typed_t src;
    lh_memory_typed_t dst;
    unsigned char buffer[5];
    lh_memory_typed_set(&src, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 5), 2);
    lh_memory_typed_set(&dst, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 4), 2);
    LH_EXPECT_DEATH((void)lh_memory_typed_dup_v(&src, &dst));
}

TEST(memory_typed_death, make_v_rejects_non_multiple_size) {
    unsigned char buffer[5];
    LH_EXPECT_DEATH((void)lh_memory_typed_make_v(lh_cast_static(lh_ptr, buffer),
                                                 lh_cast_static(lh_ptr, buffer + 5), 2));
}

TEST(memory_typed_death, slice_rejects_out_of_range_window) {
    lh_memory_typed_t typed;
    unsigned char buffer[12];
    lh_memory_typed_set(&typed, lh_cast_static(lh_ptr, buffer), lh_cast_static(lh_ptr, buffer + 12),
                        3);
    LH_EXPECT_DEATH((void)lh_memory_typed_slice(&typed, 3, 2));
}

#endif // LH_TEST_EXPECT_DEATH_ENABLED

} // namespace
