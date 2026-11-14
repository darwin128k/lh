#include <gtest/gtest.h>

#include <lh/gl/scalar.h>

#include <type_traits>

TEST(gl_scalar, is_the_integer_screen_coordinate)
{
    EXPECT_TRUE((std::is_same<lh_gl_scalar_t, lh_math_scalar_t>::value));
    EXPECT_TRUE((std::is_same<lh_gl_scalar_t, lh_int_t>::value));
    const lh_gl_scalar_t value = lh_gl_scalar(3);

    EXPECT_EQ(value, 3);
}
