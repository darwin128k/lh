#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/str/list.h>
#include <lh/test/alloc_counter.h>

#include <cstring>
#include <string>

namespace
{

std::string
as_string(lh_str_view_t view)
{
    if (lh_str_view_is_empty(&view))
    {
        return std::string();
    }
    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

class str_list : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        lh_str_list_init(&list);
    }

    void
    TearDown() override
    {
        lh_str_list_deinit(&list);
    }

    lh_str_list_t list;
};

TEST_F(str_list, starts_empty_without_allocating)
{
    lh_str_list_t fresh;
    lh_test::alloc_counter counter;

    lh_str_list_init(&fresh);
    EXPECT_TRUE(lh_str_list_is_empty(&fresh));
    EXPECT_EQ(lh_str_list_get_size(&fresh), 0u);
    EXPECT_EQ(counter.count(), 0);
    lh_str_list_deinit(&fresh);
}

TEST_F(str_list, push_back_returns_indices_and_keeps_empty_elements)
{
    EXPECT_EQ(lh_str_list_push_back(&list, lh_str_view_make("one")), 0u);
    EXPECT_EQ(lh_str_list_push_back(&list, lh_str_view_make("")), 1u);
    EXPECT_EQ(lh_str_list_push_back(&list, lh_str_view_make("three")), 2u);

    ASSERT_EQ(lh_str_list_get_size(&list), 3u);
    EXPECT_EQ(as_string(lh_str_list_get(&list, 0)), "one");
    EXPECT_EQ(as_string(lh_str_list_get(&list, 1)), "");
    EXPECT_EQ(as_string(lh_str_list_get(&list, 2)), "three");
}

TEST_F(str_list, get_data_is_nul_terminated_per_element)
{
    lh_str_list_push_back(&list, lh_str_view_make("alpha"));
    lh_str_list_push_back(&list, lh_str_view_make(""));
    lh_str_list_push_back(&list, lh_str_view_make("beta"));

    EXPECT_STREQ(lh_str_list_get_data(&list, 0), "alpha");
    EXPECT_STREQ(lh_str_list_get_data(&list, 1), "");
    EXPECT_STREQ(lh_str_list_get_data(&list, 2), "beta");
}

TEST_F(str_list, push_back_str_copies_the_string)
{
    lh_str_t s;

    lh_str_init(&s);
    lh_str_append(&s, "copy", 4);
    lh_str_list_push_back_str(&list, &s);
    lh_str_clear(&s);
    lh_str_append(&s, "changed", 7);
    EXPECT_EQ(as_string(lh_str_list_get(&list, 0)), "copy");
    lh_str_deinit(&s);
}

TEST_F(str_list, growth_keeps_earlier_elements)
{
    int i;

    lh_str_list_push_back(&list, lh_str_view_make("first"));
    for (i = 0; i < 1000; ++i)
    {
        lh_str_list_push_back(&list, lh_str_view_make("grow"));
    }
    ASSERT_EQ(lh_str_list_get_size(&list), 1001u);
    EXPECT_STREQ(lh_str_list_get_data(&list, 0), "first");
    EXPECT_STREQ(lh_str_list_get_data(&list, 1000), "grow");
}

TEST_F(str_list, clear_keeps_buffers_for_reuse)
{
    lh_str_list_split_of(&list, lh_str_view_make("a/b/c/d"), "/", 1);
    lh_str_list_clear(&list);
    EXPECT_TRUE(lh_str_list_is_empty(&list));

    lh_test::alloc_counter counter;
    lh_str_list_split_of(&list, lh_str_view_make("w/x/y/z"), "/", 1);
    EXPECT_EQ(counter.count(), 0);
    EXPECT_EQ(as_string(lh_str_list_get(&list, 3)), "z");
}

TEST_F(str_list, join_glues_with_separator)
{
    lh_str_t out;

    lh_str_list_push_back(&list, lh_str_view_make("one"));
    lh_str_list_push_back(&list, lh_str_view_make(""));
    lh_str_list_push_back(&list, lh_str_view_make("three"));
    lh_str_init(&out);
    lh_str_list_join(&list, &out, ',');
    EXPECT_STREQ(lh_str_get_data(&out), "one,,three");
    lh_str_deinit(&out);
}

TEST_F(str_list, assign_copies_every_element)
{
    lh_str_list_t copy;

    lh_str_list_push_back(&list, lh_str_view_make("x"));
    lh_str_list_push_back(&list, lh_str_view_make("yy"));
    lh_str_list_init(&copy);
    lh_str_list_push_back(&copy, lh_str_view_make("old"));
    lh_str_list_assign(&copy, &list);
    ASSERT_EQ(lh_str_list_get_size(&copy), 2u);
    EXPECT_STREQ(lh_str_list_get_data(&copy, 1), "yy");
    lh_str_list_assign(&copy, &copy);
    EXPECT_EQ(lh_str_list_get_size(&copy), 2u);
    lh_str_list_deinit(&copy);
}

TEST_F(str_list, append_other_and_self)
{
    lh_str_list_t other;

    lh_str_list_push_back(&list, lh_str_view_make("a"));
    lh_str_list_init(&other);
    lh_str_list_push_back(&other, lh_str_view_make("b"));
    lh_str_list_push_back(&other, lh_str_view_make("c"));
    lh_str_list_append(&list, &other);
    ASSERT_EQ(lh_str_list_get_size(&list), 3u);
    EXPECT_STREQ(lh_str_list_get_data(&list, 2), "c");

    lh_str_list_append(&list, &list);
    ASSERT_EQ(lh_str_list_get_size(&list), 6u);
    EXPECT_STREQ(lh_str_list_get_data(&list, 3), "a");
    EXPECT_STREQ(lh_str_list_get_data(&list, 5), "c");
    lh_str_list_deinit(&other);
}

TEST_F(str_list, split_of_skips_empty_pieces)
{
    static const lh_char_t seps[] = {'/', '\\'};

    lh_str_list_split_of(&list, lh_str_view_make("//C:\\Users/x\\\\"), seps, 2);
    ASSERT_EQ(lh_str_list_get_size(&list), 3u);
    EXPECT_EQ(as_string(lh_str_list_get(&list, 0)), "C:");
    EXPECT_EQ(as_string(lh_str_list_get(&list, 1)), "Users");
    EXPECT_EQ(as_string(lh_str_list_get(&list, 2)), "x");
}

#if LH_TEST_EXPECT_DEATH_ENABLED
TEST_F(str_list, get_out_of_range_dies)
{
    lh_str_list_push_back(&list, lh_str_view_make("only"));
    LH_EXPECT_DEATH((void)lh_str_list_get(&list, 1));
    LH_EXPECT_DEATH((void)lh_str_list_get_data(&list, 1));
}
#endif

} // namespace
