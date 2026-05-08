#include <gtest/gtest.h>

#include <lh/cast/static.h>
#include <lh/str/view.h>

namespace
{

TEST(str_view_make_by_empty, returns_uninitialized_view)
{
    const lh_str_view_t v = lh_str_make_by_empty();
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(str_view_make_or_empty, invalid_pair_returns_empty)
{
    lh_char_t buf[2];
    const lh_str_view_t v = lh_str_make_or_empty(lh_cast_static(const lh_str_ptr, buf + 1),
                                                 lh_cast_static(const lh_str_ptr, buf));
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&v));
}

TEST(str_view_slice_or_empty, returns_slice_or_empty)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));

    const lh_str_view_t ok = lh_str_slice_or_empty(&v, 1, 3);
    EXPECT_EQ(lh_str_view_get_begin(&ok), lh_cast_static(const lh_str_ptr, text + 1));
    EXPECT_EQ(lh_str_view_get_end(&ok), lh_cast_static(const lh_str_ptr, text + 4));

    const lh_str_view_t bad = lh_str_slice_or_empty(&v, 4, 4);
    EXPECT_TRUE(lh_memory_view_is_uninitialized(&bad));
}

TEST(str_view_clone_dup, raw_copy_semantics)
{
    lh_char_t text[] = "hello";
    lh_str_view_t src;
    lh_str_view_init(&src, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));

    const lh_str_view_t c = lh_str_view_clone(&src);
    EXPECT_EQ(lh_str_view_get_begin(&c), lh_str_view_get_begin(&src));
    EXPECT_EQ(lh_str_view_get_end(&c), lh_str_view_get_end(&src));

    lh_str_view_t d;
    lh_str_view_init_by_empty(&d);
    lh_str_view_dup(&src, &d);
    EXPECT_EQ(lh_str_view_get_begin(&d), lh_str_view_get_begin(&src));
    EXPECT_EQ(lh_str_view_get_end(&d), lh_str_view_get_end(&src));
}

TEST(str_view_clone_dup_v, validated_copy_semantics)
{
    lh_char_t text[] = "hello";
    lh_str_view_t src;
    lh_str_view_init(&src, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));

    const lh_str_view_t c = lh_str_view_clone_v(&src);
    EXPECT_EQ(lh_str_view_get_begin(&c), lh_str_view_get_begin(&src));
    EXPECT_EQ(lh_str_view_get_end(&c), lh_str_view_get_end(&src));

    lh_str_view_t d;
    lh_str_view_init_by_empty(&d);
    lh_str_view_dup_v(&src, &d);
    EXPECT_EQ(lh_str_view_get_begin(&d), lh_str_view_get_begin(&src));
    EXPECT_EQ(lh_str_view_get_end(&d), lh_str_view_get_end(&src));
}

TEST(str_view_unpack_v, reads_valid_view_bounds)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));

    const lh_str_ptr begin = nullptr;
    const lh_str_ptr end = nullptr;
    lh_str_view_unpack_v(&v, &begin, &end);

    EXPECT_EQ(begin, lh_cast_static(const lh_str_ptr, text));
    EXPECT_EQ(end, lh_cast_static(const lh_str_ptr, text + 5));
}

TEST(str_view_get_state, mirrors_memory_view_state)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));

    EXPECT_EQ(lh_str_view_get_state(&v), lh_memory_view_get_state(&v));
}

TEST(str_view_is_uninitialized, true_for_empty_initializer_state)
{
    lh_str_view_t v;
    lh_str_view_init_by_empty(&v);
    EXPECT_TRUE(lh_str_view_is_uninitialized(&v));
}

TEST(str_view_has_data, true_for_ordered_non_empty_view)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_TRUE(lh_str_view_has_data(&v));
}

TEST(str_view_is_valid, distinguishes_valid_and_uninitialized)
{
    lh_char_t text[] = "hello";
    lh_str_view_t valid;
    lh_str_view_init(&valid, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_TRUE(lh_str_view_is_valid(&valid));

    lh_str_view_t uninitialized;
    lh_str_view_init_by_empty(&uninitialized);
    EXPECT_FALSE(lh_str_view_is_valid(&uninitialized));
}

TEST(str_view_is_invalid, logical_negation_of_is_valid)
{
    lh_char_t text[] = "hello";
    lh_str_view_t valid;
    lh_str_view_init(&valid, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_FALSE(lh_str_view_is_invalid(&valid));

    lh_str_view_t uninitialized;
    lh_str_view_init_by_empty(&uninitialized);
    EXPECT_TRUE(lh_str_view_is_invalid(&uninitialized));
}

TEST(str_view_diff, returns_end_minus_begin_in_bytes)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_EQ(lh_str_view_diff(&v), 5);
}

TEST(str_view_is_begin_aligned, checks_begin_alignment)
{
    alignas(8) lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_TRUE(lh_str_view_is_begin_aligned(&v, alignof(lh_char_t)));
}

TEST(str_view_is_aligned, checks_both_bounds_alignment)
{
    alignas(8) lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 4));
    EXPECT_TRUE(lh_str_view_is_aligned(&v, alignof(lh_char_t)));
}

TEST(str_view_is_multiple_of, checks_size_divisibility)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_TRUE(lh_str_view_is_multiple_of(&v, 1));
    EXPECT_FALSE(lh_str_view_is_multiple_of(&v, 2));
}

TEST(str_view_is_valid_offset, validates_bounds)
{
    lh_char_t text[] = "hello";
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_TRUE(lh_str_view_is_valid_offset(&v, 0));
    EXPECT_TRUE(lh_str_view_is_valid_offset(&v, 4));
    EXPECT_FALSE(lh_str_view_is_valid_offset(&v, 5));
}

TEST(str_view_get_ptr_from_front, resolves_front_based_offset)
{
    lh_char_t text[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_EQ(lh_str_get_ptr_from_front(&v, 1), lh_cast_static(const lh_str_ptr, text + 1));
}

TEST(str_view_get_ptr_from_back, resolves_back_based_offset)
{
    lh_char_t text[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_EQ(lh_str_get_ptr_from_back(&v, 1), lh_cast_static(const lh_str_ptr, text + 3));
}

TEST(str_view_get_value_from_front, reads_front_based_character)
{
    lh_char_t text[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_EQ(lh_str_get_value_from_front(&v, 1), 'e');
}

TEST(str_view_get_value_from_back, reads_back_based_character)
{
    lh_char_t text[] = {'h', 'e', 'l', 'l', 'o'};
    lh_str_view_t v;
    lh_str_view_init(&v, lh_cast_static(const lh_str_ptr, text),
                     lh_cast_static(const lh_str_ptr, text + 5));
    EXPECT_EQ(lh_str_get_value_from_back(&v, 1), 'l');
}

} // namespace
