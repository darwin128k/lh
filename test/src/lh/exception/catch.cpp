#include <gtest/gtest.h>

#include <lh/exception/catch.h>
#include <lh/exception/catch/initializer.h>
#include <lh/expect/death.h>

namespace
{

TEST(exception_catch_get_exception, returns_stored_exception)
{
    lh_exception_catch_t catch_frame = lh_exception_catch_initializer(42, "msg");
    lh_exception_t *exception = lh_exception_catch_get_exception(&catch_frame);

    ASSERT_NE(exception, nullptr);
    EXPECT_EQ(lh_exception_get_code(exception), 42);
    EXPECT_STREQ(lh_exception_get_desc(exception), "msg");
}

TEST(exception_catch_get_exception_as_const, returns_stored_exception)
{
    lh_exception_catch_t catch_frame = lh_exception_catch_initializer(42, "msg");
    const lh_exception_t *exception = lh_exception_catch_get_exception_as_const(&catch_frame);

    ASSERT_NE(exception, nullptr);
    EXPECT_EQ(lh_exception_get_code(exception), 42);
    EXPECT_STREQ(lh_exception_get_desc(exception), "msg");
}

TEST(exception_catch_get_error, returns_stored_exception_error)
{
    lh_exception_catch_t catch_frame = lh_exception_catch_initializer(42, "msg");
    lh_error_t *error = lh_exception_catch_get_error(&catch_frame);

    ASSERT_NE(error, nullptr);
    EXPECT_EQ(lh_error_get_code(error), 42);
    EXPECT_STREQ(lh_error_get_desc(error), "msg");
}

TEST(exception_catch_get_error_as_const, returns_stored_exception_error)
{
    lh_exception_catch_t catch_frame = lh_exception_catch_initializer(42, "msg");
    const lh_error_t *error = lh_exception_catch_get_error_as_const(&catch_frame);

    ASSERT_NE(error, nullptr);
    EXPECT_EQ(lh_error_get_code(error), 42);
    EXPECT_STREQ(lh_error_get_desc(error), "msg");
}

TEST(exception_catch_get_code, returns_stored_exception_code)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(7, "seven");

    EXPECT_EQ(lh_exception_catch_get_code(&catch_frame), 7);
}

TEST(exception_catch_get_desc, returns_stored_exception_desc)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(9, "nine");

    EXPECT_STREQ(lh_exception_catch_get_desc(&catch_frame), "nine");
}

TEST(exception_catch_get_desc_or, returns_desc_when_non_null)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(1, "desc");

    EXPECT_STREQ(lh_exception_catch_get_desc_or(&catch_frame, "fallback"), "desc");
}

TEST(exception_catch_get_desc_or, returns_fallback_when_desc_is_null)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(1, nullptr);

    EXPECT_STREQ(lh_exception_catch_get_desc_or(&catch_frame, "fallback"), "fallback");
}

TEST(exception_catch_has_code, returns_true_for_matching_code)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(11, "eleven");

    EXPECT_TRUE(lh_exception_catch_has_code(&catch_frame, 11));
}

TEST(exception_catch_has_code, returns_false_for_different_code)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(11, "eleven");

    EXPECT_FALSE(lh_exception_catch_has_code(&catch_frame, 12));
}

TEST(exception_catch_is_ok, returns_true_for_ok_code)
{
    const lh_exception_catch_t catch_frame =
        lh_exception_catch_initializer(lh_error_code_ok, nullptr);

    EXPECT_TRUE(lh_exception_catch_is_ok(&catch_frame));
}

TEST(exception_catch_is_failure, returns_true_for_non_ok_code)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(1, nullptr);

    EXPECT_TRUE(lh_exception_catch_is_failure(&catch_frame));
}

TEST(exception_catch_has_desc, returns_true_for_non_null_desc)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_initializer(1, "desc");

    EXPECT_TRUE(lh_exception_catch_has_desc(&catch_frame));
}

TEST(exception_catch_is_empty, returns_true_for_empty_exception)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();

    EXPECT_TRUE(lh_exception_catch_is_empty(&catch_frame));
}

TEST(exception_catch_equals, returns_true_for_same_embedded_exception)
{
    lh_error_desc_t desc = "same";
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, desc);
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(21, desc);

    EXPECT_TRUE(lh_exception_catch_equals(&lhs, &rhs));
}

TEST(exception_catch_equals, returns_false_for_different_embedded_exception)
{
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, "same");
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(22, "same");

    EXPECT_FALSE(lh_exception_catch_equals(&lhs, &rhs));
}

TEST(exception_catch_has_same_code, returns_true_for_same_code)
{
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, "lhs");
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(21, "rhs");

    EXPECT_TRUE(lh_exception_catch_has_same_code(&lhs, &rhs));
}

TEST(exception_catch_has_same_code, returns_false_for_different_code)
{
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, "same");
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(22, "same");

    EXPECT_FALSE(lh_exception_catch_has_same_code(&lhs, &rhs));
}

TEST(exception_catch_has_diff_code, returns_true_for_different_code)
{
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, "same");
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(22, "same");

    EXPECT_TRUE(lh_exception_catch_has_diff_code(&lhs, &rhs));
}

TEST(exception_catch_has_diff_code, returns_false_for_same_code)
{
    const lh_exception_catch_t lhs = lh_exception_catch_initializer(21, "lhs");
    const lh_exception_catch_t rhs = lh_exception_catch_initializer(21, "rhs");

    EXPECT_FALSE(lh_exception_catch_has_diff_code(&lhs, &rhs));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(exception_catch_death, get_exception_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_exception(nullptr));
}

TEST(exception_catch_death, get_exception_as_const_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_exception_as_const(nullptr));
}

TEST(exception_catch_death, get_error_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_error(nullptr));
}

TEST(exception_catch_death, get_error_as_const_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_error_as_const(nullptr));
}

TEST(exception_catch_death, get_code_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_code(nullptr));
}

TEST(exception_catch_death, get_desc_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_desc(nullptr));
}

TEST(exception_catch_death, get_desc_or_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_get_desc_or(nullptr, "fallback"));
}

TEST(exception_catch_death, has_code_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_has_code(nullptr, 1));
}

TEST(exception_catch_death, is_ok_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_is_ok(nullptr));
}

TEST(exception_catch_death, is_failure_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_is_failure(nullptr));
}

TEST(exception_catch_death, has_desc_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_has_desc(nullptr));
}

TEST(exception_catch_death, is_empty_null_self)
{
    LH_EXPECT_DEATH(lh_exception_catch_is_empty(nullptr));
}

TEST(exception_catch_death, equals_null_self)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_equals(nullptr, &catch_frame));
}

TEST(exception_catch_death, equals_null_other)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_equals(&catch_frame, nullptr));
}

TEST(exception_catch_death, has_same_code_null_self)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_has_same_code(nullptr, &catch_frame));
}

TEST(exception_catch_death, has_same_code_null_other)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_has_same_code(&catch_frame, nullptr));
}

TEST(exception_catch_death, has_diff_code_null_self)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_has_diff_code(nullptr, &catch_frame));
}

TEST(exception_catch_death, has_diff_code_null_other)
{
    const lh_exception_catch_t catch_frame = lh_exception_catch_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_catch_has_diff_code(&catch_frame, nullptr));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
