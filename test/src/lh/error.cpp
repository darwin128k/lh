#include <gtest/gtest.h>

#include <lh/error.h>
#include <lh/error/initializer.h>
#include <lh/expect/death.h>


namespace
{

const char *
desc_cstr(const char *desc)
{
    return (desc == nullptr || *desc == '\0') ? nullptr : desc;
}

TEST(error_set, roundtrip_code_and_desc)
{
    lh_error_t err{};
    lh_error_set(&err, 42, "msg");
    EXPECT_EQ(lh_error_get_code(&err), 42);
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&err)), "msg");
}

TEST(error_set, null_desc_roundtrip)
{
    lh_error_t err{};
    lh_error_set(&err, 3, nullptr);
    EXPECT_EQ(lh_error_get_code(&err), 3);
    EXPECT_EQ(desc_cstr(lh_error_get_desc(&err)), nullptr);
}

TEST(error_set_code, updates_code_and_keeps_desc)
{
    lh_error_t err = lh_error_initializer(1, "keep");

    lh_error_set_code(&err, 2);

    EXPECT_EQ(lh_error_get_code(&err), 2);
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&err)), "keep");
}

TEST(error_set_desc, updates_desc_and_keeps_code)
{
    lh_error_t err = lh_error_initializer(1, "old");

    lh_error_set_desc(&err, "new");

    EXPECT_EQ(lh_error_get_code(&err), 1);
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&err)), "new");
}

TEST(error_set_desc, accepts_null_desc)
{
    lh_error_t err = lh_error_initializer(1, "old");

    lh_error_set_desc(&err, nullptr);

    EXPECT_EQ(lh_error_get_code(&err), 1);
    EXPECT_EQ(desc_cstr(lh_error_get_desc(&err)), nullptr);
}

TEST(error_assign, copies_from_other)
{
    const lh_error_t src = lh_error_initializer(11, "src");
    lh_error_t dst = lh_error_initializer(0, nullptr);
    lh_error_assign(&dst, &src);
    EXPECT_EQ(lh_error_get_code(&dst), 11);
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&dst)), "src");
}

TEST(error_clear, resets_to_ok_and_null_desc)
{
    lh_error_t err = lh_error_initializer(5, "gone");
    lh_error_clear(&err);
    EXPECT_EQ(lh_error_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(desc_cstr(lh_error_get_desc(&err)), nullptr);
}

TEST(error_init, matches_set)
{
    lh_error_t a{};
    lh_error_t b{};
    lh_error_init(&a, 8, "eight");
    lh_error_set(&b, 8, "eight");
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&a)), desc_cstr(lh_error_get_desc(&b)));
}

TEST(error_init_by_other, matches_assign)
{
    const lh_error_t src = lh_error_initializer(4, "four");
    lh_error_t a{};
    lh_error_t b{};
    lh_error_init_by_other(&a, &src);
    lh_error_assign(&b, &src);
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_STREQ(desc_cstr(lh_error_get_desc(&a)), desc_cstr(lh_error_get_desc(&b)));
}

TEST(error_init_by_empty, clears_like_clear)
{
    lh_error_t a = lh_error_initializer(2, "two");
    lh_error_t b = lh_error_initializer(2, "two");
    lh_error_init_by_empty(&a);
    lh_error_clear(&b);
    EXPECT_EQ(lh_error_get_code(&a), lh_error_get_code(&b));
    EXPECT_EQ(desc_cstr(lh_error_get_desc(&a)), desc_cstr(lh_error_get_desc(&b)));
}

TEST(error_get_code_and_clear, returns_previous_then_ok)
{
    lh_error_t err = lh_error_initializer(100, "z");
    EXPECT_EQ(lh_error_get_code_and_clear(&err), 100);
    EXPECT_EQ(lh_error_get_code(&err), lh_error_code_ok);
    EXPECT_EQ(desc_cstr(lh_error_get_desc(&err)), nullptr);
}

TEST(error_has_code, returns_true_for_matching_code)
{
    const lh_error_t err = lh_error_initializer(13, "thirteen");

    EXPECT_TRUE(lh_error_has_code(&err, 13));
}

TEST(error_has_code, returns_false_for_different_code)
{
    const lh_error_t err = lh_error_initializer(13, "thirteen");

    EXPECT_FALSE(lh_error_has_code(&err, 14));
}

TEST(error_get_desc_or, returns_desc_when_non_null)
{
    const lh_error_t err = lh_error_initializer(1, "desc");

    EXPECT_STREQ(desc_cstr(lh_error_get_desc_or(&err, "fallback")), "desc");
}

TEST(error_get_desc_or, returns_fallback_when_desc_is_null)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_STREQ(desc_cstr(lh_error_get_desc_or(&err, "fallback")), "fallback");
}

TEST(error_get_desc_or, accepts_null_fallback)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_EQ(desc_cstr(lh_error_get_desc_or(&err, nullptr)), nullptr);
}

TEST(error_is_ok, returns_true_for_ok_code)
{
    const lh_error_t err = lh_error_initializer(lh_error_code_ok, nullptr);

    EXPECT_TRUE(lh_error_is_ok(&err));
}

TEST(error_is_ok, returns_false_for_non_ok_code)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_FALSE(lh_error_is_ok(&err));
}

TEST(error_is_failure, returns_true_for_non_ok_code)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_TRUE(lh_error_is_failure(&err));
}

TEST(error_is_failure, returns_false_for_ok_code)
{
    const lh_error_t err = lh_error_initializer(lh_error_code_ok, nullptr);

    EXPECT_FALSE(lh_error_is_failure(&err));
}

TEST(error_has_desc, returns_true_for_non_null_desc)
{
    const lh_error_t err = lh_error_initializer(1, "desc");

    EXPECT_TRUE(lh_error_has_desc(&err));
}

TEST(error_has_desc, returns_false_for_null_desc)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_FALSE(lh_error_has_desc(&err));
}

TEST(error_is_empty, returns_true_for_ok_code_without_desc)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);

    EXPECT_TRUE(lh_error_is_empty(&err));
}

TEST(error_is_empty, returns_false_for_non_ok_code)
{
    const lh_error_t err = lh_error_initializer(1, nullptr);

    EXPECT_FALSE(lh_error_is_empty(&err));
}

TEST(error_is_empty, returns_false_for_ok_code_with_desc)
{
    const lh_error_t err = lh_error_initializer(lh_error_code_ok, "desc");

    EXPECT_FALSE(lh_error_is_empty(&err));
}

TEST(error_equals, returns_true_for_same_code_and_desc_pointer)
{
    const char *desc = "same";
    const lh_error_t lhs = lh_error_initializer(21, desc);
    const lh_error_t rhs = lh_error_initializer(21, desc);

    EXPECT_TRUE(lh_error_equals(&lhs, &rhs));
}

TEST(error_equals, returns_true_for_equal_empty_errors)
{
    const lh_error_t lhs = lh_error_empty_initializer(nullptr);
    const lh_error_t rhs = lh_error_empty_initializer(nullptr);

    EXPECT_TRUE(lh_error_equals(&lhs, &rhs));
}

TEST(error_equals, returns_false_for_different_code)
{
    const char *desc = "same";
    const lh_error_t lhs = lh_error_initializer(21, desc);
    const lh_error_t rhs = lh_error_initializer(22, desc);

    EXPECT_FALSE(lh_error_equals(&lhs, &rhs));
}

TEST(error_equals, returns_false_for_different_desc_pointer)
{
    const char lhs_desc[] = "same";
    const char rhs_desc[] = "same";
    const lh_error_t lhs = lh_error_initializer(21, lhs_desc);
    const lh_error_t rhs = lh_error_initializer(21, rhs_desc);

    EXPECT_FALSE(lh_error_equals(&lhs, &rhs));
}

TEST(error_has_same_code, returns_true_for_same_code)
{
    const lh_error_t lhs = lh_error_initializer(21, "lhs");
    const lh_error_t rhs = lh_error_initializer(21, "rhs");

    EXPECT_TRUE(lh_error_has_same_code(&lhs, &rhs));
}

TEST(error_has_same_code, returns_false_for_different_code)
{
    const lh_error_t lhs = lh_error_initializer(21, "same");
    const lh_error_t rhs = lh_error_initializer(22, "same");

    EXPECT_FALSE(lh_error_has_same_code(&lhs, &rhs));
}

TEST(error_has_diff_code, returns_true_for_different_code)
{
    const lh_error_t lhs = lh_error_initializer(21, "same");
    const lh_error_t rhs = lh_error_initializer(22, "same");

    EXPECT_TRUE(lh_error_has_diff_code(&lhs, &rhs));
}

TEST(error_has_diff_code, returns_false_for_same_code)
{
    const lh_error_t lhs = lh_error_initializer(21, "lhs");
    const lh_error_t rhs = lh_error_initializer(21, "rhs");

    EXPECT_FALSE(lh_error_has_diff_code(&lhs, &rhs));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(error_death, assign_null_self)
{
    lh_error_t other = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_assign(nullptr, &other));
}

TEST(error_death, assign_null_other)
{
    lh_error_t dst{};
    LH_EXPECT_DEATH(lh_error_assign(&dst, nullptr));
}

TEST(error_death, init_null_self)
{
    LH_EXPECT_DEATH(lh_error_init(nullptr, 1, nullptr));
}

TEST(error_death, init_by_other_null_self)
{
    lh_error_t src = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_init_by_other(nullptr, &src));
}

TEST(error_death, init_by_empty_null_self)
{
    LH_EXPECT_DEATH(lh_error_init_by_empty(nullptr));
}

TEST(error_death, set_null_self)
{
    LH_EXPECT_DEATH(lh_error_set(nullptr, 0, nullptr));
}

TEST(error_death, set_code_null_self)
{
    LH_EXPECT_DEATH(lh_error_set_code(nullptr, 1));
}

TEST(error_death, set_desc_null_self)
{
    LH_EXPECT_DEATH(lh_error_set_desc(nullptr, nullptr));
}

TEST(error_death, clear_null_self)
{
    LH_EXPECT_DEATH(lh_error_clear(nullptr));
}

TEST(error_death, get_code_null_self)
{
    LH_EXPECT_DEATH(lh_error_get_code(nullptr));
}

TEST(error_death, get_code_and_clear_null_self)
{
    LH_EXPECT_DEATH(lh_error_get_code_and_clear(nullptr));
}

TEST(error_death, get_desc_null_self)
{
    LH_EXPECT_DEATH(lh_error_get_desc(nullptr));
}

TEST(error_death, get_desc_or_null_self)
{
    LH_EXPECT_DEATH(lh_error_get_desc_or(nullptr, "fallback"));
}

TEST(error_death, has_code_null_self)
{
    LH_EXPECT_DEATH(lh_error_has_code(nullptr, 1));
}

TEST(error_death, is_ok_null_self)
{
    LH_EXPECT_DEATH(lh_error_is_ok(nullptr));
}

TEST(error_death, is_failure_null_self)
{
    LH_EXPECT_DEATH(lh_error_is_failure(nullptr));
}

TEST(error_death, has_desc_null_self)
{
    LH_EXPECT_DEATH(lh_error_has_desc(nullptr));
}

TEST(error_death, is_empty_null_self)
{
    LH_EXPECT_DEATH(lh_error_is_empty(nullptr));
}

TEST(error_death, equals_null_self)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_equals(nullptr, &err));
}

TEST(error_death, equals_null_other)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_equals(&err, nullptr));
}

TEST(error_death, has_same_code_null_self)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_has_same_code(nullptr, &err));
}

TEST(error_death, has_same_code_null_other)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_has_same_code(&err, nullptr));
}

TEST(error_death, has_diff_code_null_self)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_has_diff_code(nullptr, &err));
}

TEST(error_death, has_diff_code_null_other)
{
    const lh_error_t err = lh_error_empty_initializer(nullptr);
    LH_EXPECT_DEATH(lh_error_has_diff_code(&err, nullptr));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
