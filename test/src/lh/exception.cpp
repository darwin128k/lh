#include <gtest/gtest.h>

#include <lh/exception.h>
#include <lh/exception/initializer.h>
#include <lh/expect/death.h>


namespace
{

const char *
desc_cstr(lh_error_desc_t desc)
{
    return lh_str_view_is_empty(&desc) ? nullptr : lh_str_view_get_data(&desc);
}

TEST(exception_get_error, returns_embedded_error)
{
    lh_exception_t exception = lh_exception_initializer(42, lh_str_view_make("msg"));
    lh_error_t *error = lh_exception_get_error(&exception);

    ASSERT_NE(error, nullptr);
    EXPECT_EQ(error->code, 42);
    EXPECT_STREQ(desc_cstr(error->desc), "msg");
}

TEST(exception_get_error_as_const, returns_embedded_error)
{
    lh_exception_t exception = lh_exception_initializer(42, lh_str_view_make("msg"));
    const lh_error_t *error = lh_exception_get_error_as_const(&exception);

    ASSERT_NE(error, nullptr);
    EXPECT_EQ(error->code, 42);
    EXPECT_STREQ(desc_cstr(error->desc), "msg");
}

#ifndef NDEBUG
TEST(exception_get_origin, returns_embedded_origin)
{
    lh_exception_t exception = lh_exception_empty_initializer();
    lh_exception_set(&exception, 1, lh_str_view_make("desc"), "ts", "file.c", "func", 42);

    lh_exception_origin_t *origin = lh_exception_get_origin(&exception);

    ASSERT_NE(origin, nullptr);
    EXPECT_STREQ(origin->filename, "file.c");
    EXPECT_STREQ(origin->function, "func");
    EXPECT_EQ(origin->line, 42u);
}

TEST(exception_get_origin_as_const, returns_embedded_origin)
{
    lh_exception_t exception = lh_exception_empty_initializer();
    lh_exception_set(&exception, 1, lh_str_view_make("desc"), "ts", "file.c", "func", 42);

    const lh_exception_origin_t *origin = lh_exception_get_origin_as_const(&exception);

    ASSERT_NE(origin, nullptr);
    EXPECT_STREQ(origin->filename, "file.c");
    EXPECT_STREQ(origin->function, "func");
    EXPECT_EQ(origin->line, 42u);
}
#endif

TEST(exception_get_code, returns_embedded_error_code)
{
    const lh_exception_t exception = lh_exception_initializer(7, lh_str_view_make("seven"));

    EXPECT_EQ(lh_exception_get_code(&exception), 7);
}

TEST(exception_get_desc, returns_embedded_error_desc)
{
    const lh_exception_t exception = lh_exception_initializer(9, lh_str_view_make("nine"));

    EXPECT_STREQ(desc_cstr(lh_exception_get_desc(&exception)), "nine");
}

TEST(exception_set_code, updates_code_and_keeps_desc)
{
    lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make("keep"));

    lh_exception_set_code(&exception, 2);

    EXPECT_EQ(lh_exception_get_code(&exception), 2);
    EXPECT_STREQ(desc_cstr(lh_exception_get_desc(&exception)), "keep");
}

TEST(exception_set_desc, updates_desc_and_keeps_code)
{
    lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make("old"));

    lh_exception_set_desc(&exception, lh_str_view_make("new"));

    EXPECT_EQ(lh_exception_get_code(&exception), 1);
    EXPECT_STREQ(desc_cstr(lh_exception_get_desc(&exception)), "new");
}

TEST(exception_set_desc, accepts_null_desc)
{
    lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make("old"));

    lh_exception_set_desc(&exception, lh_str_view_make(nullptr));

    EXPECT_EQ(lh_exception_get_code(&exception), 1);
    EXPECT_EQ(desc_cstr(lh_exception_get_desc(&exception)), nullptr);
}

TEST(exception_get_desc_or, returns_desc_when_non_null)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make("desc"));

    EXPECT_STREQ(desc_cstr(lh_exception_get_desc_or(&exception, lh_str_view_make("fallback"))), "desc");
}

TEST(exception_get_desc_or, returns_fallback_when_desc_is_null)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make(nullptr));

    EXPECT_STREQ(desc_cstr(lh_exception_get_desc_or(&exception, lh_str_view_make("fallback"))), "fallback");
}

TEST(exception_has_code, returns_true_for_matching_code)
{
    const lh_exception_t exception = lh_exception_initializer(11, lh_str_view_make("eleven"));

    EXPECT_TRUE(lh_exception_has_code(&exception, 11));
}

TEST(exception_has_code, returns_false_for_different_code)
{
    const lh_exception_t exception = lh_exception_initializer(11, lh_str_view_make("eleven"));

    EXPECT_FALSE(lh_exception_has_code(&exception, 12));
}

TEST(exception_is_ok, returns_true_for_ok_code)
{
    const lh_exception_t exception = lh_exception_initializer(lh_error_code_ok, lh_str_view_make(nullptr));

    EXPECT_TRUE(lh_exception_is_ok(&exception));
}

TEST(exception_is_ok, returns_false_for_non_ok_code)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make(nullptr));

    EXPECT_FALSE(lh_exception_is_ok(&exception));
}

TEST(exception_is_failure, returns_true_for_non_ok_code)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make(nullptr));

    EXPECT_TRUE(lh_exception_is_failure(&exception));
}

TEST(exception_is_failure, returns_false_for_ok_code)
{
    const lh_exception_t exception = lh_exception_initializer(lh_error_code_ok, lh_str_view_make(nullptr));

    EXPECT_FALSE(lh_exception_is_failure(&exception));
}

TEST(exception_has_desc, returns_true_for_non_null_desc)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make("desc"));

    EXPECT_TRUE(lh_exception_has_desc(&exception));
}

TEST(exception_has_desc, returns_false_for_null_desc)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make(nullptr));

    EXPECT_FALSE(lh_exception_has_desc(&exception));
}

TEST(exception_is_empty, returns_true_for_ok_code_without_desc)
{
    const lh_exception_t exception = lh_exception_empty_initializer();

    EXPECT_TRUE(lh_exception_is_empty(&exception));
}

TEST(exception_is_empty, returns_false_for_non_ok_code)
{
    const lh_exception_t exception = lh_exception_initializer(1, lh_str_view_make(nullptr));

    EXPECT_FALSE(lh_exception_is_empty(&exception));
}

TEST(exception_is_empty, returns_false_for_ok_code_with_desc)
{
    const lh_exception_t exception = lh_exception_initializer(lh_error_code_ok, lh_str_view_make("desc"));

    EXPECT_FALSE(lh_exception_is_empty(&exception));
}

TEST(exception_equals, returns_true_for_same_embedded_error)
{
    lh_error_desc_t desc = lh_str_view_make("same");
    const lh_exception_t lhs = lh_exception_initializer(21, desc);
    const lh_exception_t rhs = lh_exception_initializer(21, desc);

    EXPECT_TRUE(lh_exception_equals(&lhs, &rhs));
}

TEST(exception_equals, returns_false_for_different_code)
{
    lh_error_desc_t desc = lh_str_view_make("same");
    const lh_exception_t lhs = lh_exception_initializer(21, desc);
    const lh_exception_t rhs = lh_exception_initializer(22, desc);

    EXPECT_FALSE(lh_exception_equals(&lhs, &rhs));
}

TEST(exception_equals, returns_false_for_different_desc_pointer)
{
    const char lhs_desc[] = "same";
    const char rhs_desc[] = "same";
    const lh_exception_t lhs = lh_exception_initializer(21, lh_str_view_make(lhs_desc));
    const lh_exception_t rhs = lh_exception_initializer(21, lh_str_view_make(rhs_desc));

    EXPECT_FALSE(lh_exception_equals(&lhs, &rhs));
}

TEST(exception_has_same_code, returns_true_for_same_code)
{
    const lh_exception_t lhs = lh_exception_initializer(21, lh_str_view_make("lhs"));
    const lh_exception_t rhs = lh_exception_initializer(21, lh_str_view_make("rhs"));

    EXPECT_TRUE(lh_exception_has_same_code(&lhs, &rhs));
}

TEST(exception_has_same_code, returns_false_for_different_code)
{
    const lh_exception_t lhs = lh_exception_initializer(21, lh_str_view_make("same"));
    const lh_exception_t rhs = lh_exception_initializer(22, lh_str_view_make("same"));

    EXPECT_FALSE(lh_exception_has_same_code(&lhs, &rhs));
}

TEST(exception_has_diff_code, returns_true_for_different_code)
{
    const lh_exception_t lhs = lh_exception_initializer(21, lh_str_view_make("same"));
    const lh_exception_t rhs = lh_exception_initializer(22, lh_str_view_make("same"));

    EXPECT_TRUE(lh_exception_has_diff_code(&lhs, &rhs));
}

TEST(exception_has_diff_code, returns_false_for_same_code)
{
    const lh_exception_t lhs = lh_exception_initializer(21, lh_str_view_make("lhs"));
    const lh_exception_t rhs = lh_exception_initializer(21, lh_str_view_make("rhs"));

    EXPECT_FALSE(lh_exception_has_diff_code(&lhs, &rhs));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(exception_death, get_error_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_error(nullptr));
}

TEST(exception_death, get_error_as_const_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_error_as_const(nullptr));
}

#ifndef NDEBUG
TEST(exception_death, get_origin_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_origin(nullptr));
}

TEST(exception_death, get_origin_as_const_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_origin_as_const(nullptr));
}
#endif

TEST(exception_death, set_code_null_self)
{
    LH_EXPECT_DEATH(lh_exception_set_code(nullptr, 1));
}

TEST(exception_death, set_desc_null_self)
{
    LH_EXPECT_DEATH(lh_exception_set_desc(nullptr, lh_str_view_make(nullptr)));
}

TEST(exception_death, get_code_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_code(nullptr));
}

TEST(exception_death, get_desc_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_desc(nullptr));
}

TEST(exception_death, get_desc_or_null_self)
{
    LH_EXPECT_DEATH(lh_exception_get_desc_or(nullptr, lh_str_view_make("fallback")));
}

TEST(exception_death, has_code_null_self)
{
    LH_EXPECT_DEATH(lh_exception_has_code(nullptr, 1));
}

TEST(exception_death, is_ok_null_self)
{
    LH_EXPECT_DEATH(lh_exception_is_ok(nullptr));
}

TEST(exception_death, is_failure_null_self)
{
    LH_EXPECT_DEATH(lh_exception_is_failure(nullptr));
}

TEST(exception_death, has_desc_null_self)
{
    LH_EXPECT_DEATH(lh_exception_has_desc(nullptr));
}

TEST(exception_death, is_empty_null_self)
{
    LH_EXPECT_DEATH(lh_exception_is_empty(nullptr));
}

TEST(exception_death, equals_null_self)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_equals(nullptr, &exception));
}

TEST(exception_death, equals_null_other)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_equals(&exception, nullptr));
}

TEST(exception_death, has_same_code_null_self)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_has_same_code(nullptr, &exception));
}

TEST(exception_death, has_same_code_null_other)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_has_same_code(&exception, nullptr));
}

TEST(exception_death, has_diff_code_null_self)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_has_diff_code(nullptr, &exception));
}

TEST(exception_death, has_diff_code_null_other)
{
    const lh_exception_t exception = lh_exception_empty_initializer();
    LH_EXPECT_DEATH(lh_exception_has_diff_code(&exception, nullptr));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
