#include <gtest/gtest.h>
#include <wchar.h>

#include <lh/werror.h>
#include <lh/werror/initializer.h>
#include <lh/wstr/view.h>
#include <lh/expect/death.h>


namespace
{

const wchar_t *
desc_wstr(lh_wstr_view_t desc)
{
    return lh_wstr_view_is_empty(&desc) ? nullptr : lh_wstr_view_get_data(&desc);
}

void
expect_desc_eq(lh_wstr_view_t desc, const wchar_t *expected)
{
    const wchar_t *text = desc_wstr(desc);

    ASSERT_NE(text, nullptr);
    EXPECT_EQ(wcscmp(text, expected), 0);
}

TEST(werror_set, roundtrip_code_and_desc)
{
    lh_werror_t err{};
    lh_werror_set(&err, 42, lh_wstr_view_lit(L"msg"));
    EXPECT_EQ(lh_werror_get_code(&err), 42);
    expect_desc_eq(lh_werror_get_desc(&err), L"msg");
}

TEST(werror_set, null_desc_roundtrip)
{
    lh_werror_t err{};
    lh_werror_set(&err, 3, lh_wstr_view_make(nullptr));
    EXPECT_EQ(lh_werror_get_code(&err), 3);
    EXPECT_EQ(desc_wstr(lh_werror_get_desc(&err)), nullptr);
}

TEST(werror_set_code, updates_code_and_keeps_desc)
{
    lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_lit(L"keep"));

    lh_werror_set_code(&err, 2);

    EXPECT_EQ(lh_werror_get_code(&err), 2);
    expect_desc_eq(lh_werror_get_desc(&err), L"keep");
}

TEST(werror_set_desc, updates_desc_and_keeps_code)
{
    lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_lit(L"old"));

    lh_werror_set_desc(&err, lh_wstr_view_lit(L"new"));

    EXPECT_EQ(lh_werror_get_code(&err), 1);
    expect_desc_eq(lh_werror_get_desc(&err), L"new");
}

TEST(werror_set_desc, accepts_null_desc)
{
    lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_lit(L"old"));

    lh_werror_set_desc(&err, lh_wstr_view_make(nullptr));

    EXPECT_EQ(lh_werror_get_code(&err), 1);
    EXPECT_EQ(desc_wstr(lh_werror_get_desc(&err)), nullptr);
}

TEST(werror_assign, copies_from_other)
{
    const lh_werror_t src = lh_werror_initializer(11, lh_wstr_view_lit(L"src"));
    lh_werror_t dst = lh_werror_initializer(0, lh_wstr_view_make(nullptr));
    lh_werror_assign(&dst, &src);
    EXPECT_EQ(lh_werror_get_code(&dst), 11);
    expect_desc_eq(lh_werror_get_desc(&dst), L"src");
}

TEST(werror_clear, resets_to_ok_and_null_desc)
{
    lh_werror_t err = lh_werror_initializer(5, lh_wstr_view_lit(L"gone"));
    lh_werror_clear(&err);
    EXPECT_EQ(lh_werror_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(desc_wstr(lh_werror_get_desc(&err)), nullptr);
}

TEST(werror_init, matches_set)
{
    lh_werror_t a{};
    lh_werror_t b{};
    lh_werror_init(&a, 8, lh_wstr_view_lit(L"eight"));
    lh_werror_set(&b, 8, lh_wstr_view_lit(L"eight"));
    EXPECT_EQ(lh_werror_get_code(&a), lh_werror_get_code(&b));
    EXPECT_STREQ(desc_wstr(lh_werror_get_desc(&a)), desc_wstr(lh_werror_get_desc(&b)));
}

TEST(werror_init_by_other, matches_assign)
{
    const lh_werror_t src = lh_werror_initializer(4, lh_wstr_view_lit(L"four"));
    lh_werror_t a{};
    lh_werror_t b{};
    lh_werror_init_by_other(&a, &src);
    lh_werror_assign(&b, &src);
    EXPECT_EQ(lh_werror_get_code(&a), lh_werror_get_code(&b));
    EXPECT_STREQ(desc_wstr(lh_werror_get_desc(&a)), desc_wstr(lh_werror_get_desc(&b)));
}

TEST(werror_init_by_empty, clears_like_clear)
{
    lh_werror_t a = lh_werror_initializer(2, lh_wstr_view_lit(L"two"));
    lh_werror_t b = lh_werror_initializer(2, lh_wstr_view_lit(L"two"));
    lh_werror_init_by_empty(&a);
    lh_werror_clear(&b);
    EXPECT_EQ(lh_werror_get_code(&a), lh_werror_get_code(&b));
    EXPECT_EQ(desc_wstr(lh_werror_get_desc(&a)), desc_wstr(lh_werror_get_desc(&b)));
}

TEST(werror_get_code_and_clear, returns_previous_then_ok)
{
    lh_werror_t err = lh_werror_initializer(100, lh_wstr_view_lit(L"z"));
    EXPECT_EQ(lh_werror_get_code_and_clear(&err), 100);
    EXPECT_EQ(lh_werror_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(desc_wstr(lh_werror_get_desc(&err)), nullptr);
}

TEST(werror_has_code, returns_true_for_matching_code)
{
    const lh_werror_t err = lh_werror_initializer(13, lh_wstr_view_lit(L"thirteen"));

    EXPECT_TRUE(lh_werror_has_code(&err, 13));
}

TEST(werror_has_code, returns_false_for_different_code)
{
    const lh_werror_t err = lh_werror_initializer(13, lh_wstr_view_lit(L"thirteen"));

    EXPECT_FALSE(lh_werror_has_code(&err, 14));
}

TEST(werror_get_desc_or, returns_desc_when_non_null)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_lit(L"desc"));

    EXPECT_STREQ(desc_wstr(lh_werror_get_desc_or(&err, lh_wstr_view_lit(L"fallback"))), L"desc");
}

TEST(werror_get_desc_or, returns_fallback_when_desc_is_null)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_STREQ(desc_wstr(lh_werror_get_desc_or(&err, lh_wstr_view_lit(L"fallback"))), L"fallback");
}

TEST(werror_get_desc_or, accepts_null_fallback)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_EQ(desc_wstr(lh_werror_get_desc_or(&err, lh_wstr_view_make(nullptr))), nullptr);
}

TEST(werror_is_ok, returns_true_for_ok_code)
{
    const lh_werror_t err = lh_werror_initializer(lh_error_code_ok, lh_wstr_view_make(nullptr));

    EXPECT_TRUE(lh_werror_is_ok(&err));
}

TEST(werror_is_ok, returns_false_for_non_ok_code)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_FALSE(lh_werror_is_ok(&err));
}

TEST(werror_is_failure, returns_true_for_non_ok_code)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_TRUE(lh_werror_is_failure(&err));
}

TEST(werror_is_failure, returns_false_for_ok_code)
{
    const lh_werror_t err = lh_werror_initializer(lh_error_code_ok, lh_wstr_view_make(nullptr));

    EXPECT_FALSE(lh_werror_is_failure(&err));
}

TEST(werror_has_desc, returns_true_for_non_null_desc)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_lit(L"desc"));

    EXPECT_TRUE(lh_werror_has_desc(&err));
}

TEST(werror_has_desc, returns_false_for_null_desc)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_FALSE(lh_werror_has_desc(&err));
}

TEST(werror_has_desc, empty_view_is_no_description)
{
    const lh_werror_t err = lh_werror_make_by_code(1);
    EXPECT_FALSE(lh_werror_has_desc(&err));
    EXPECT_TRUE(lh_werror_is_failure(&err));
}

TEST(werror_is_empty, returns_true_for_ok_code_without_desc)
{
    const lh_werror_t err = lh_werror_empty_initializer();

    EXPECT_TRUE(lh_werror_is_empty(&err));
}

TEST(werror_is_empty, returns_false_for_non_ok_code)
{
    const lh_werror_t err = lh_werror_initializer(1, lh_wstr_view_make(nullptr));

    EXPECT_FALSE(lh_werror_is_empty(&err));
}

TEST(werror_is_empty, returns_false_for_ok_code_with_desc)
{
    const lh_werror_t err = lh_werror_initializer(lh_error_code_ok, lh_wstr_view_lit(L"desc"));

    EXPECT_FALSE(lh_werror_is_empty(&err));
}

TEST(werror_equals, returns_true_for_same_code_and_desc_pointer)
{
    lh_wstr_view_t desc = lh_wstr_view_lit(L"same");
    const lh_werror_t lhs = lh_werror_initializer(21, desc);
    const lh_werror_t rhs = lh_werror_initializer(21, desc);

    EXPECT_TRUE(lh_werror_equals(&lhs, &rhs));
}

TEST(werror_equals, returns_true_for_equal_empty_errors)
{
    const lh_werror_t lhs = lh_werror_empty_initializer();
    const lh_werror_t rhs = lh_werror_empty_initializer();

    EXPECT_TRUE(lh_werror_equals(&lhs, &rhs));
}

TEST(werror_equals, returns_false_for_different_code)
{
    lh_wstr_view_t desc = lh_wstr_view_lit(L"same");
    const lh_werror_t lhs = lh_werror_initializer(21, desc);
    const lh_werror_t rhs = lh_werror_initializer(22, desc);

    EXPECT_FALSE(lh_werror_equals(&lhs, &rhs));
}

TEST(werror_equals, returns_false_for_different_desc_pointer)
{
    const wchar_t lhs_desc[] = L"same";
    const wchar_t rhs_desc[] = L"same";
    const lh_werror_t lhs = lh_werror_initializer(21, lh_wstr_view_make(lhs_desc));
    const lh_werror_t rhs = lh_werror_initializer(21, lh_wstr_view_make(rhs_desc));

    EXPECT_FALSE(lh_werror_equals(&lhs, &rhs));
}

TEST(werror_has_same_code, returns_true_for_same_code)
{
    const lh_werror_t lhs = lh_werror_initializer(21, lh_wstr_view_lit(L"lhs"));
    const lh_werror_t rhs = lh_werror_initializer(21, lh_wstr_view_lit(L"rhs"));

    EXPECT_TRUE(lh_werror_has_same_code(&lhs, &rhs));
}

TEST(werror_has_same_code, returns_false_for_different_code)
{
    const lh_werror_t lhs = lh_werror_initializer(21, lh_wstr_view_lit(L"same"));
    const lh_werror_t rhs = lh_werror_initializer(22, lh_wstr_view_lit(L"same"));

    EXPECT_FALSE(lh_werror_has_same_code(&lhs, &rhs));
}

TEST(werror_has_diff_code, returns_true_for_different_code)
{
    const lh_werror_t lhs = lh_werror_initializer(21, lh_wstr_view_lit(L"same"));
    const lh_werror_t rhs = lh_werror_initializer(22, lh_wstr_view_lit(L"same"));

    EXPECT_TRUE(lh_werror_has_diff_code(&lhs, &rhs));
}

TEST(werror_has_diff_code, returns_false_for_same_code)
{
    const lh_werror_t lhs = lh_werror_initializer(21, lh_wstr_view_lit(L"lhs"));
    const lh_werror_t rhs = lh_werror_initializer(21, lh_wstr_view_lit(L"rhs"));

    EXPECT_FALSE(lh_werror_has_diff_code(&lhs, &rhs));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(werror_death, assign_null_self)
{
    lh_werror_t other = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_assign(nullptr, &other));
}

TEST(werror_death, assign_null_other)
{
    lh_werror_t dst{};
    LH_EXPECT_DEATH(lh_werror_assign(&dst, nullptr));
}

TEST(werror_death, init_null_self)
{
    LH_EXPECT_DEATH(lh_werror_init(nullptr, 1, lh_wstr_view_make(nullptr)));
}

TEST(werror_death, init_by_other_null_self)
{
    lh_werror_t src = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_init_by_other(nullptr, &src));
}

TEST(werror_death, init_by_empty_null_self)
{
    LH_EXPECT_DEATH(lh_werror_init_by_empty(nullptr));
}

TEST(werror_death, set_null_self)
{
    LH_EXPECT_DEATH(lh_werror_set(nullptr, 0, lh_wstr_view_make(nullptr)));
}

TEST(werror_death, set_code_null_self)
{
    LH_EXPECT_DEATH(lh_werror_set_code(nullptr, 1));
}

TEST(werror_death, set_desc_null_self)
{
    LH_EXPECT_DEATH(lh_werror_set_desc(nullptr, lh_wstr_view_make(nullptr)));
}

TEST(werror_death, clear_null_self)
{
    LH_EXPECT_DEATH(lh_werror_clear(nullptr));
}

TEST(werror_death, get_code_null_self)
{
    LH_EXPECT_DEATH(lh_werror_get_code(nullptr));
}

TEST(werror_death, get_code_and_clear_null_self)
{
    LH_EXPECT_DEATH(lh_werror_get_code_and_clear(nullptr));
}

TEST(werror_death, get_desc_null_self)
{
    LH_EXPECT_DEATH(lh_werror_get_desc(nullptr));
}

TEST(werror_death, get_desc_or_null_self)
{
    LH_EXPECT_DEATH(lh_werror_get_desc_or(nullptr, lh_wstr_view_lit(L"fallback")));
}

TEST(werror_death, has_code_null_self)
{
    LH_EXPECT_DEATH(lh_werror_has_code(nullptr, 1));
}

TEST(werror_death, is_ok_null_self)
{
    LH_EXPECT_DEATH(lh_werror_is_ok(nullptr));
}

TEST(werror_death, is_failure_null_self)
{
    LH_EXPECT_DEATH(lh_werror_is_failure(nullptr));
}

TEST(werror_death, has_desc_null_self)
{
    LH_EXPECT_DEATH(lh_werror_has_desc(nullptr));
}

TEST(werror_death, is_empty_null_self)
{
    LH_EXPECT_DEATH(lh_werror_is_empty(nullptr));
}

TEST(werror_death, equals_null_self)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_equals(nullptr, &err));
}

TEST(werror_death, equals_null_other)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_equals(&err, nullptr));
}

TEST(werror_death, has_same_code_null_self)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_has_same_code(nullptr, &err));
}

TEST(werror_death, has_same_code_null_other)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_has_same_code(&err, nullptr));
}

TEST(werror_death, has_diff_code_null_self)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_has_diff_code(nullptr, &err));
}

TEST(werror_death, has_diff_code_null_other)
{
    const lh_werror_t err = lh_werror_empty_initializer();
    LH_EXPECT_DEATH(lh_werror_has_diff_code(&err, nullptr));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
