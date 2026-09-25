#include <gtest/gtest.h>

#include <lh/str.h>
#include <lh/test/alloc_counter.h>

#include <cstring>

namespace
{

TEST(str_init, starts_empty_and_terminated)
{
    lh_str_t s;
    lh_str_init(&s);

    EXPECT_TRUE(lh_str_is_empty(&s));
    EXPECT_EQ(lh_str_get_size(&s), 0u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);

    lh_str_deinit(&s);
}

TEST(str_push_back, appends_chars_and_stays_terminated)
{
    lh_str_t s;
    lh_str_init(&s);

    lh_str_push_back(&s, 'h');
    lh_str_push_back(&s, 'i');

    ASSERT_EQ(lh_str_get_size(&s), 2u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "hi"), 0);

    lh_str_deinit(&s);
}

TEST(str_append, appends_text_and_stays_terminated)
{
    lh_str_t s;
    lh_str_init(&s);

    lh_str_append(&s, "hi", 2);
    lh_str_append(&s, "!!!", 3);

    ASSERT_EQ(lh_str_get_size(&s), 5u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "hi!!!"), 0);

    lh_str_deinit(&s);
}

TEST(str_push_back, survives_many_reallocations_still_terminated)
{
    lh_str_t s;
    lh_str_init(&s);

    for (int i = 0; i < 1000; ++i)
    {
        lh_str_push_back(&s, 'x');
    }

    ASSERT_EQ(lh_str_get_size(&s), 1000u);
    EXPECT_EQ(lh_str_get_data(&s)[1000], '\0');

    lh_str_deinit(&s);
}

TEST(str_clear, empties_but_keeps_buffer_terminated)
{
    lh_str_t s;
    lh_str_init(&s);
    lh_str_append(&s, "hello", 5);

    lh_str_clear(&s);

    EXPECT_TRUE(lh_str_is_empty(&s));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);

    lh_str_deinit(&s);
}

TEST(str_as_view, reflects_current_contents)
{
    lh_str_t s;
    lh_str_init(&s);
    lh_str_append(&s, "hello", 5);

    lh_str_view_t v = lh_str_as_view(&s);

    ASSERT_EQ(lh_str_view_get_size(&v), 5u);
    EXPECT_TRUE(lh_str_view_equals(&v, &v, lh_bool_false));
    EXPECT_EQ(std::strncmp(lh_str_view_get_data(&v), "hello", 5), 0);

    lh_str_deinit(&s);
}

TEST(str_truncate, shrinks_and_stays_terminated)
{
    lh_str_t s;
    lh_str_init(&s);
    lh_str_append(&s, "hello", 5);

    lh_str_truncate(&s, 2);

    ASSERT_EQ(lh_str_get_size(&s), 2u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "he"), 0);

    lh_str_truncate(&s, 0);

    EXPECT_TRUE(lh_str_is_empty(&s));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);

    lh_str_deinit(&s);
}

TEST(str_append_view, appends_view_and_skips_empty)
{
    lh_str_t s;
    lh_str_view_t empty;

    lh_str_init(&s);
    lh_str_view_init_empty(&empty);
    lh_str_append_view(&s, empty);
    EXPECT_TRUE(lh_str_is_empty(&s));

    lh_str_append_view(&s, lh_str_view_make("ab"));
    lh_str_append_view(&s, lh_str_view_make("cd"));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "abcd"), 0);

    lh_str_deinit(&s);
}

TEST(str_append_str, appends_other_string)
{
    lh_str_t a;
    lh_str_t b;

    lh_str_init(&a);
    lh_str_init(&b);
    lh_str_append(&a, "xy", 2);
    lh_str_append_str(&b, &a);
    lh_str_append_str(&b, &a);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&b), "xyxy"), 0);

    lh_str_append_str(&a, &a);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&a), "xyxy"), 0);

    lh_str_deinit(&a);
    lh_str_deinit(&b);
}

TEST(str_assign, replaces_contents)
{
    lh_str_t a;
    lh_str_t b;

    lh_str_init(&a);
    lh_str_init(&b);
    lh_str_append(&a, "src", 3);
    lh_str_append(&b, "dst", 3);
    lh_str_assign(&b, &a);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&b), "src"), 0);
    lh_str_assign(&a, &a);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&a), "src"), 0);

    lh_str_assign_view(&b, lh_str_view_make("view"));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&b), "view"), 0);

    lh_str_deinit(&a);
    lh_str_deinit(&b);
}

TEST(str_join, glues_views_with_sep)
{
    lh_str_t s;
    lh_str_view_t parts[3];

    lh_str_init(&s);
    parts[0] = lh_str_view_make("a");
    parts[1] = lh_str_view_make("b");
    parts[2] = lh_str_view_make("c");
    lh_str_join(&s, parts, 3, '/');
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "a/b/c"), 0);

    parts[1] = lh_str_view_make("");
    lh_str_join(&s, parts, 3, '/');
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "a//c"), 0);

    lh_str_join(&s, parts, 0, '/');
    EXPECT_TRUE(lh_str_is_empty(&s));

    lh_str_deinit(&s);
}

TEST(str_format, writes_into_self)
{
    lh_str_t s;

    lh_str_init(&s);
    lh_str_append(&s, "old", 3);
    EXPECT_EQ(lh_str_format(&s, "%s:%u", "id", 7U), 4u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "id:7"), 0);
    EXPECT_EQ(lh_str_format(&s, "%q", 1), 0u);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "id:7"), 0);

    lh_str_deinit(&s);
}

TEST(str_init, allocates_nothing)
{
    lh_test::alloc_counter counter;
    lh_str_t s;

    lh_str_init(&s);
    EXPECT_EQ(counter.count(), 0);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);
    lh_str_deinit(&s);
    EXPECT_EQ(counter.count(), 0);
}

TEST(str_bufferless, every_reader_and_reset_works_before_first_grow)
{
    lh_test::alloc_counter counter;
    lh_str_t s;
    lh_str_t other;

    lh_str_init(&s);
    lh_str_init(&other);

    lh_str_clear(&s);
    lh_str_truncate(&s, 0);
    lh_str_view_t view = lh_str_as_view(&s);
    EXPECT_TRUE(lh_str_view_is_empty(&view));
    lh_str_append_str(&s, &other);
    lh_str_assign(&s, &other);
    lh_str_append(&s, "", 0);
    EXPECT_TRUE(lh_str_is_empty(&s));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);
    EXPECT_EQ(counter.count(), 0);

    lh_str_deinit(&other);
    lh_str_deinit(&s);
}

TEST(str_assign, from_empty_string_empties_target)
{
    lh_str_t s;
    lh_str_t empty;

    lh_str_init(&s);
    lh_str_init(&empty);
    lh_str_append(&s, "abc", 3);
    lh_str_assign(&s, &empty);
    EXPECT_TRUE(lh_str_is_empty(&s));
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);

    lh_str_deinit(&empty);
    lh_str_deinit(&s);
}

TEST(str_append, first_append_grows_once_for_text_and_terminator)
{
    lh_test::alloc_counter counter;
    lh_str_t s;

    lh_str_init(&s);
    lh_str_append(&s, "segment", 7);
    EXPECT_EQ(counter.count(), 1);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "segment"), 0);
    lh_str_deinit(&s);
}

TEST(str_reserve, then_appends_within_it_do_not_allocate)
{
    lh_test::alloc_counter counter;
    lh_str_t s;
    int i;

    lh_str_init(&s);
    lh_str_reserve(&s, 64);
    EXPECT_EQ(counter.count(), 1);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), ""), 0);
    for (i = 0; i < 64; ++i)
    {
        lh_str_push_back(&s, 'x');
    }
    EXPECT_EQ(counter.count(), 1);
    EXPECT_EQ(lh_str_get_size(&s), 64u);
    EXPECT_EQ(lh_str_get_data(&s)[64], '\0');
    lh_str_deinit(&s);
}

TEST(str_clear, keeps_the_buffer_for_reuse)
{
    lh_test::alloc_counter counter;
    lh_str_t s;

    lh_str_init(&s);
    lh_str_append(&s, "abcdef", 6);
    lh_str_clear(&s);
    lh_str_append(&s, "xyz", 3);
    EXPECT_EQ(counter.count(), 1);
    EXPECT_EQ(std::strcmp(lh_str_get_data(&s), "xyz"), 0);
    lh_str_deinit(&s);
}

} // namespace
