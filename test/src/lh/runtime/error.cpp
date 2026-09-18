#include <gtest/gtest.h>

#include <lh/runtime/error.h>
#include <lh/runtime/error/initializer.h>
#include <lh/expect/death.h>


namespace
{

const char *
desc_cstr(lh_str_view_t desc)
{
    return lh_str_view_is_empty(&desc) ? nullptr : lh_str_view_get_data(&desc);
}

TEST(runtime_error_set, roundtrip_code_and_desc)
{
    lh_runtime_error_t err{};
    lh_runtime_error_set(&err, lh_runtime_error_code_interrupt, lh_str_view_lit("msg"));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_interrupt);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&err)), "msg");
}

TEST(runtime_error_set, null_desc_roundtrip)
{
    lh_runtime_error_t err{};
    lh_runtime_error_set(&err, lh_runtime_error_code_null_pointer, lh_str_view_make(nullptr));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_null_pointer);
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&err)), nullptr);
}

TEST(runtime_error_set_code, updates_code_and_keeps_desc)
{
    lh_runtime_error_t err = lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("keep"));
    lh_runtime_error_set_code(&err, lh_runtime_error_code_null_pointer);
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_null_pointer);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&err)), "keep");
}

TEST(runtime_error_set_desc, updates_desc_and_keeps_code)
{
    lh_runtime_error_t err = lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("old"));
    lh_runtime_error_set_desc(&err, lh_str_view_lit("new"));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_interrupt);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&err)), "new");
}

TEST(runtime_error_set_desc, accepts_null_desc)
{
    lh_runtime_error_t err = lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("old"));
    lh_runtime_error_set_desc(&err, lh_str_view_make(nullptr));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_interrupt);
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&err)), nullptr);
}

TEST(runtime_error_get_desc_or, returns_desc_when_non_null)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("desc"));
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc_or(&err, lh_str_view_lit("fallback"))), "desc");
}

TEST(runtime_error_get_desc_or, returns_fallback_when_desc_is_null)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc_or(&err, lh_str_view_lit("fallback"))), "fallback");
}

TEST(runtime_error_is_ok, returns_true_for_ok_code)
{
    const lh_runtime_error_t err = lh_runtime_error_empty_initializer();
    EXPECT_TRUE(lh_runtime_error_is_ok(&err));
}

TEST(runtime_error_is_ok, returns_false_for_non_ok_code)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_FALSE(lh_runtime_error_is_ok(&err));
}

TEST(runtime_error_is_failure, returns_true_for_non_ok_code)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_null_pointer, lh_str_view_make(nullptr));
    EXPECT_TRUE(lh_runtime_error_is_failure(&err));
}

TEST(runtime_error_is_failure, returns_false_for_ok_code)
{
    const lh_runtime_error_t err = lh_runtime_error_empty_initializer();
    EXPECT_FALSE(lh_runtime_error_is_failure(&err));
}

TEST(runtime_error_has_code, returns_true_for_matching_code)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_TRUE(lh_runtime_error_has_code(&err, lh_runtime_error_code_interrupt));
}

TEST(runtime_error_has_code, returns_false_for_different_code)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_FALSE(lh_runtime_error_has_code(&err, lh_runtime_error_code_null_pointer));
}

TEST(runtime_error_has_desc, returns_true_for_non_null_desc)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("desc"));
    EXPECT_TRUE(lh_runtime_error_has_desc(&err));
}

TEST(runtime_error_has_desc, returns_false_for_null_desc)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_FALSE(lh_runtime_error_has_desc(&err));
}

TEST(runtime_error_is_empty, returns_true_for_ok_code_without_desc)
{
    const lh_runtime_error_t err = lh_runtime_error_empty_initializer();
    EXPECT_TRUE(lh_runtime_error_is_empty(&err));
}

TEST(runtime_error_is_empty, returns_false_for_non_ok_code)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    EXPECT_FALSE(lh_runtime_error_is_empty(&err));
}

TEST(runtime_error_is_empty, returns_false_for_ok_code_with_desc)
{
    const lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_ok, lh_str_view_lit("desc"));
    EXPECT_FALSE(lh_runtime_error_is_empty(&err));
}

TEST(runtime_error_equals, returns_true_for_same_code_and_desc_pointer)
{
    lh_str_view_t desc = lh_str_view_lit("same");
    const lh_runtime_error_t lhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, desc);
    const lh_runtime_error_t rhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, desc);
    EXPECT_TRUE(lh_runtime_error_equals(&lhs, &rhs));
}

TEST(runtime_error_equals, returns_false_for_different_code)
{
    lh_str_view_t desc = lh_str_view_lit("same");
    const lh_runtime_error_t lhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, desc);
    const lh_runtime_error_t rhs =
        lh_runtime_error_initializer(lh_runtime_error_code_null_pointer, desc);
    EXPECT_FALSE(lh_runtime_error_equals(&lhs, &rhs));
}

TEST(runtime_error_has_same_code, returns_true_for_same_code)
{
    const lh_runtime_error_t lhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("lhs"));
    const lh_runtime_error_t rhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("rhs"));
    EXPECT_TRUE(lh_runtime_error_has_same_code(&lhs, &rhs));
}

TEST(runtime_error_has_diff_code, returns_true_for_different_code)
{
    const lh_runtime_error_t lhs =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_make(nullptr));
    const lh_runtime_error_t rhs =
        lh_runtime_error_initializer(lh_runtime_error_code_null_pointer, lh_str_view_make(nullptr));
    EXPECT_TRUE(lh_runtime_error_has_diff_code(&lhs, &rhs));
}

TEST(runtime_error_assign, copies_from_other)
{
    const lh_runtime_error_t src =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("src"));
    lh_runtime_error_t dst = lh_runtime_error_empty_initializer();
    lh_runtime_error_assign(&dst, &src);
    EXPECT_EQ(lh_runtime_error_get_code(&dst), lh_runtime_error_code_interrupt);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&dst)), "src");
}

TEST(runtime_error_clear, resets_to_ok_and_null_desc)
{
    lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("gone"));
    lh_runtime_error_clear(&err);
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_ok);
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&err)), nullptr);
}

TEST(runtime_error_get_code_and_clear, returns_previous_then_ok)
{
    lh_runtime_error_t err =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("z"));
    EXPECT_EQ(lh_runtime_error_get_code_and_clear(&err), lh_runtime_error_code_interrupt);
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_ok);
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&err)), nullptr);
}

TEST(runtime_error_make, constructs_with_code_and_desc)
{
    const lh_runtime_error_t err =
        lh_runtime_error_make(lh_runtime_error_code_interrupt, lh_str_view_lit("msg"));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_interrupt);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&err)), "msg");
}

TEST(runtime_error_make_by_code, constructs_with_null_desc)
{
    const lh_runtime_error_t err =
        lh_runtime_error_make_by_code(lh_runtime_error_code_null_pointer);
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_null_pointer);
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&err)), nullptr);
}

TEST(runtime_error_make_by_desc, constructs_with_interrupt_code)
{
    const lh_runtime_error_t err = lh_runtime_error_make_by_desc(lh_str_view_lit("operation failed"));
    EXPECT_EQ(lh_runtime_error_get_code(&err), lh_runtime_error_code_interrupt);
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&err)), "operation failed");
}

TEST(runtime_error_init, matches_make)
{
    lh_runtime_error_t a{};
    lh_runtime_error_init(&a, lh_runtime_error_code_interrupt, lh_str_view_lit("eight"));
    const lh_runtime_error_t b = lh_runtime_error_make(lh_runtime_error_code_interrupt, lh_str_view_lit("eight"));
    EXPECT_EQ(lh_runtime_error_get_code(&a), lh_runtime_error_get_code(&b));
    EXPECT_STREQ(desc_cstr(lh_runtime_error_get_desc(&a)), desc_cstr(lh_runtime_error_get_desc(&b)));
}

TEST(runtime_error_init_by_empty, clears_like_clear)
{
    lh_runtime_error_t a =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("two"));
    lh_runtime_error_t b =
        lh_runtime_error_initializer(lh_runtime_error_code_interrupt, lh_str_view_lit("two"));
    lh_runtime_error_init_by_empty(&a);
    lh_runtime_error_clear(&b);
    EXPECT_EQ(lh_runtime_error_get_code(&a), lh_runtime_error_get_code(&b));
    EXPECT_EQ(desc_cstr(lh_runtime_error_get_desc(&a)), desc_cstr(lh_runtime_error_get_desc(&b)));
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(runtime_error_death, set_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_set(nullptr, lh_runtime_error_code_interrupt, lh_str_view_make(nullptr)));
}

TEST(runtime_error_death, set_code_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_set_code(nullptr, lh_runtime_error_code_interrupt));
}

TEST(runtime_error_death, set_desc_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_set_desc(nullptr, lh_str_view_make(nullptr)));
}

TEST(runtime_error_death, get_code_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_get_code(nullptr));
}

TEST(runtime_error_death, get_desc_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_get_desc(nullptr));
}

TEST(runtime_error_death, get_desc_or_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_get_desc_or(nullptr, lh_str_view_lit("fallback")));
}

TEST(runtime_error_death, has_code_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_has_code(nullptr, lh_runtime_error_code_interrupt));
}

TEST(runtime_error_death, is_ok_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_is_ok(nullptr));
}

TEST(runtime_error_death, is_failure_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_is_failure(nullptr));
}

TEST(runtime_error_death, clear_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_clear(nullptr));
}

TEST(runtime_error_death, assign_null_self)
{
    lh_runtime_error_t other = lh_runtime_error_empty_initializer();
    LH_EXPECT_DEATH(lh_runtime_error_assign(nullptr, &other));
}

TEST(runtime_error_death, assign_null_other)
{
    lh_runtime_error_t dst{};
    LH_EXPECT_DEATH(lh_runtime_error_assign(&dst, nullptr));
}

TEST(runtime_error_death, init_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_init(nullptr, lh_runtime_error_code_interrupt, lh_str_view_make(nullptr)));
}

TEST(runtime_error_death, get_code_and_clear_null_self)
{
    LH_EXPECT_DEATH(lh_runtime_error_get_code_and_clear(nullptr));
}

#endif /* LH_TEST_EXPECT_DEATH_ENABLED */

} // namespace
