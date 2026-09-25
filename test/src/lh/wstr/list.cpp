#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/wstr/list.h>

#include <cwchar>
#include <string>

namespace
{

std::wstring
as_wstring(lh_wstr_view_t view)
{
    if (lh_wstr_view_is_empty(&view))
    {
        return std::wstring();
    }
    return std::wstring(lh_wstr_view_get_data(&view), lh_wstr_view_get_size(&view));
}

class wstr_list : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        lh_wstr_list_init(&list);
    }

    void
    TearDown() override
    {
        lh_wstr_list_deinit(&list);
    }

    lh_wstr_list_t list;
};

TEST_F(wstr_list, elements_are_wide_characters_not_bytes)
{
    lh_wstr_list_push_back(&list, lh_wstr_view_make(L"alpha"));
    lh_wstr_list_push_back(&list, lh_wstr_view_make(L""));
    lh_wstr_list_push_back(&list, lh_wstr_view_make(L"\x0436\x0443\x043a"));

    ASSERT_EQ(lh_wstr_list_get_size(&list), 3u);
    EXPECT_EQ(as_wstring(lh_wstr_list_get(&list, 0)), L"alpha");
    EXPECT_EQ(as_wstring(lh_wstr_list_get(&list, 1)), L"");
    EXPECT_EQ(as_wstring(lh_wstr_list_get(&list, 2)), L"\x0436\x0443\x043a");
    EXPECT_EQ(std::wcscmp(lh_wstr_list_get_data(&list, 2), L"\x0436\x0443\x043a"), 0);
    EXPECT_EQ(std::wcscmp(lh_wstr_list_get_data(&list, 1), L""), 0);
}

TEST_F(wstr_list, join_assign_append_self)
{
    lh_wstr_list_t copy;
    lh_wstr_t out;

    lh_wstr_list_push_back(&list, lh_wstr_view_make(L"a"));
    lh_wstr_list_push_back(&list, lh_wstr_view_make(L"bc"));
    lh_wstr_init(&out);
    lh_wstr_list_join(&list, &out, L'|');
    EXPECT_EQ(std::wcscmp(lh_wstr_get_data(&out), L"a|bc"), 0);

    lh_wstr_list_init(&copy);
    lh_wstr_list_assign(&copy, &list);
    lh_wstr_list_append(&copy, &copy);
    ASSERT_EQ(lh_wstr_list_get_size(&copy), 4u);
    EXPECT_EQ(std::wcscmp(lh_wstr_list_get_data(&copy, 3), L"bc"), 0);

    lh_wstr_list_deinit(&copy);
    lh_wstr_deinit(&out);
}

#if LH_TEST_EXPECT_DEATH_ENABLED
TEST_F(wstr_list, get_out_of_range_dies)
{
    LH_EXPECT_DEATH((void)lh_wstr_list_get(&list, 0));
}
#endif

} // namespace
