#include <gtest/gtest.h>

#include <lh/value.h>

#include <cstring>

namespace
{

lh_str_view_t
view(const char *text)
{
    lh_str_view_t v;
    lh_str_view_init(&v, text);
    return v;
}

TEST(value_init, holds_nothing)
{
    lh_value_t v;
    lh_value_init(&v);

    EXPECT_TRUE(lh_value_is_none(&v));
    EXPECT_EQ(lh_value_get_kind(&v), lh_value_kind_none);
    const lh_value_t made = lh_value_make_none();
    EXPECT_TRUE(lh_value_is_none(&made));
}

TEST(value_make, each_kind_reads_back)
{
    const lh_value_t b = lh_value_make_bool(lh_bool_true);
    const lh_value_t i = lh_value_make_sint(-42);
    const lh_value_t u = lh_value_make_uint(42u);
    const lh_value_t s = lh_value_make_str(view("port"));
    int target = 0;
    const lh_value_t p = lh_value_make_ptr(&target);

    EXPECT_TRUE(lh_value_get_bool(&b));
    EXPECT_EQ(lh_value_get_sint(&i), -42);
    EXPECT_EQ(lh_value_get_uint(&u), 42u);
    const lh_str_view_t text = lh_value_get_str(&s);
    EXPECT_EQ(lh_str_view_get_size(&text), 4u);
    EXPECT_EQ(std::memcmp(lh_str_view_get_data(&text), "port", 4), 0);
    EXPECT_EQ(lh_value_get_ptr(&p), &target);

    EXPECT_TRUE(lh_value_has_kind(&u, lh_value_kind_uint));
    EXPECT_FALSE(lh_value_has_kind(&u, lh_value_kind_sint));
}

#if LH_REAL_ENABLED
TEST(value_make, real_reads_back)
{
    const lh_value_t r = lh_value_make_real(static_cast<lh_real_t>(0.25));
    EXPECT_EQ(lh_value_get_real(&r), static_cast<lh_real_t>(0.25));
}
#endif

TEST(value_get_or, falls_back_on_another_kind)
{
    const lh_value_t u = lh_value_make_uint(7u);
    const lh_value_t none = lh_value_make_none();

    EXPECT_EQ(lh_value_get_uint_or(&u, 1u), 7u);
    EXPECT_EQ(lh_value_get_uint_or(&none, 1u), 1u);
    EXPECT_EQ(lh_value_get_sint_or(&u, -1), -1); /* a uint is not a sint */
    EXPECT_FALSE(lh_value_get_bool_or(&u, lh_bool_false));
    EXPECT_EQ(lh_value_get_ptr_or(&u, nullptr), nullptr);
}

TEST(value_assign, copies_kind_and_content)
{
    lh_value_t a;
    const lh_value_t b = lh_value_make_str(view("x"));

    lh_value_init(&a);
    lh_value_assign(&a, &b);
    EXPECT_TRUE(lh_value_equals(&a, &b));
}

TEST(value_equals, same_kind_and_content_only)
{
    const lh_value_t u5 = lh_value_make_uint(5u);
    const lh_value_t u5b = lh_value_make_uint(5u);
    const lh_value_t u6 = lh_value_make_uint(6u);
    const lh_value_t i5 = lh_value_make_sint(5);
    const lh_value_t s1 = lh_value_make_str(view("abc"));
    const lh_value_t s2 = lh_value_make_str(view("abc"));
    const lh_value_t s3 = lh_value_make_str(view("ABC"));
    const lh_value_t empty1 = lh_value_make_str(view(""));
    const lh_value_t empty2 = lh_value_make_str(view(""));
    const lh_value_t none1 = lh_value_make_none();
    const lh_value_t none2 = lh_value_make_none();

    EXPECT_TRUE(lh_value_equals(&u5, &u5b));
    EXPECT_FALSE(lh_value_equals(&u5, &u6));
    EXPECT_FALSE(lh_value_equals(&u5, &i5)); /* different kinds */
    EXPECT_TRUE(lh_value_equals(&s1, &s2));
    EXPECT_FALSE(lh_value_equals(&s1, &s3)); /* case-sensitive */
    EXPECT_TRUE(lh_value_equals(&empty1, &empty2));
    EXPECT_FALSE(lh_value_equals(&empty1, &s1));
    EXPECT_TRUE(lh_value_equals(&none1, &none2));
    EXPECT_FALSE(lh_value_equals(&none1, &u5));
}

TEST(value_equals, bools_compare_as_truth)
{
    /* Any nonzero lh_bool_t is true: 2 and 1 are the same boolean. */
    const lh_value_t two = lh_value_make_bool(static_cast<lh_bool_t>(2));
    const lh_value_t one = lh_value_make_bool(lh_bool_true);
    const lh_value_t no = lh_value_make_bool(lh_bool_false);

    EXPECT_TRUE(lh_value_equals(&two, &one));
    EXPECT_FALSE(lh_value_equals(&two, &no));
    EXPECT_TRUE(lh_bool_eq(2, 1));
    EXPECT_FALSE(lh_bool_eq(0, 3));
    EXPECT_TRUE(lh_bool_eq(0, 0));
}

} // namespace
