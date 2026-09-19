#include <gtest/gtest.h>

#include <lh/str.h>

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

} // namespace
