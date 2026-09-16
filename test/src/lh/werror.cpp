#include <gtest/gtest.h>

#include <lh/werror.h>
#include <lh/werror/initializer.h>
#include <lh/wstr/view.h>

TEST(werror_set, roundtrip_code_and_desc)
{
    lh_werror_t err{};
    lh_werror_set(&err, 42, lh_wstr_view_lit(L"msg"));
    EXPECT_EQ(lh_werror_get_code(&err), 42);
    EXPECT_FALSE(lh_wstr_view_is_empty(&err.desc));
    EXPECT_EQ(lh_wstr_view_get_size(&err.desc), 3u);
    EXPECT_EQ(lh_wstr_view_get_data(&err.desc)[0], L'm');
}

TEST(werror_has_desc, empty_view_is_no_description)
{
    const lh_werror_t err = lh_werror_make_by_code(1);
    EXPECT_FALSE(lh_werror_has_desc(&err));
    EXPECT_TRUE(lh_werror_is_failure(&err));
}
