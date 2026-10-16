#include <gtest/gtest.h>

#include <lh/expect/death.h>
#include <lh/math/vec4.h>
#include <lh/util/addr.h>

#include <cmath>
#include <cstdint>
#include <cstring>

namespace
{

/* A row-major 3 x 4 matrix: element (r, c) is r * 10 + c. */
const lh_float_t matrix[] = {0.0f, 1.0f, 2.0f, 3.0f, 10.0f, 11.0f, 12.0f, 13.0f, 20.0f, 21.0f, 22.0f, 23.0f};

lh_math_slice_t
whole()
{
    lh_math_slice_t s;

    lh_math_slice_init(lh_addr_of(s), matrix, 12U);
    return s;
}

lh_math_view_t
view_of(lh_usize_t offset, lh_usize_t stride, lh_usize_t count)
{
    const lh_math_slice_t s = whole();
    lh_math_view_t v;

    lh_math_view_init(lh_addr_of(v), lh_addr_of(s), offset, stride, count);
    return v;
}

TEST(math_view, span_is_the_elements_between_the_first_and_the_last)
{
    EXPECT_EQ(lh_math_view_get_span(1U, 0U), 0U);
    EXPECT_EQ(lh_math_view_get_span(4U, 0U), 0U);
    EXPECT_EQ(lh_math_view_get_span(4U, 1U), 1U);
    EXPECT_EQ(lh_math_view_get_span(1U, 4U), 4U);
    EXPECT_EQ(lh_math_view_get_span(4U, 3U), 9U);
}

TEST(math_view, a_row_is_stride_one)
{
    const lh_math_view_t row = view_of(4U, 1U, 4U);

    EXPECT_EQ(lh_math_view_get_size(lh_addr_of(row)), 4U);
    EXPECT_FALSE(lh_math_view_is_empty(lh_addr_of(row)));
    for (lh_usize_t c = 0U; c < 4U; ++c)
    {
        EXPECT_EQ(lh_math_view_get_at(lh_addr_of(row), c), 10.0f + static_cast<lh_float_t>(c));
    }
}

TEST(math_view, a_column_is_stride_four_and_reaches_the_last_row)
{
    const lh_math_view_t column = view_of(3U, 4U, 3U);

    EXPECT_EQ(lh_math_view_get_size(lh_addr_of(column)), 3U);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(column), 0U), 3.0f);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(column), 1U), 13.0f);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(column), 2U), 23.0f);
}

TEST(math_view, init_by_slice_is_every_element)
{
    const lh_math_slice_t s = whole();
    lh_math_view_t v;

    lh_math_view_init_by_slice(lh_addr_of(v), lh_addr_of(s));
    EXPECT_EQ(lh_math_view_get_size(lh_addr_of(v)), 12U);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(v), 0U), 0.0f);
    EXPECT_EQ(lh_math_view_get_at(lh_addr_of(v), 11U), 23.0f);
}

TEST(math_view, empty_views)
{
    lh_math_slice_t empty;
    lh_math_view_t v;
    const lh_math_view_t none = view_of(12U, 3U, 0U);

    lh_math_slice_init_empty(lh_addr_of(empty));
    lh_math_view_init_by_slice(lh_addr_of(v), lh_addr_of(empty));
    EXPECT_TRUE(lh_math_view_is_empty(lh_addr_of(v)));
    EXPECT_EQ(lh_math_view_get_size(lh_addr_of(v)), 0U);
    EXPECT_TRUE(lh_math_view_is_empty(lh_addr_of(none)));
    EXPECT_EQ(lh_math_view_dot(lh_addr_of(v), lh_addr_of(none)), 0.0f);
    EXPECT_EQ(lh_math_view_length(lh_addr_of(v)), 0.0f);
}

TEST(math_view, dot_length_sq_and_length)
{
    const lh_math_view_t row = view_of(0U, 1U, 3U);     /* 0, 1, 2 */
    const lh_math_view_t column = view_of(1U, 4U, 3U);  /* 1, 11, 21 */
    const lh_float_t three_four[] = {3.0f, 4.0f};
    lh_math_slice_t s;
    lh_math_view_t v;

    EXPECT_EQ(lh_math_view_dot(lh_addr_of(row), lh_addr_of(column)), 0.0f * 1.0f + 1.0f * 11.0f + 2.0f * 21.0f);
    EXPECT_EQ(lh_math_view_length_sq(lh_addr_of(row)), 5.0f);
    lh_math_slice_init(lh_addr_of(s), three_four, 2U);
    lh_math_view_init_by_slice(lh_addr_of(v), lh_addr_of(s));
    EXPECT_EQ(lh_math_view_length_sq(lh_addr_of(v)), 25.0f);
    EXPECT_EQ(lh_math_view_length(lh_addr_of(v)), 5.0f);
}

/* The vectors measure through views now; the values must be the ones the
   hand-written sums gave, bit for bit -- the views add in the same order. */
std::uint32_t
bits(float f)
{
    std::uint32_t u;

    std::memcpy(&u, &f, sizeof u);
    return u;
}

TEST(math_view, vector_dot_and_length_are_the_hand_written_sums_bit_for_bit)
{
    const float values[] = {0.1f, -2.75f, 3.3333333f, 1e-20f, -0.0f, 7e10f, 0.5f, -1.25f, 123.456f, 1e-3f};

    for (float x : values)
    {
        for (float y : values)
        {
            for (float z : {0.1f, -0.0f, 9.75f})
            {
                const float w = x * 0.5f - z;
                lh_math_vec2_t a2;
                lh_math_vec2_t b2;
                lh_math_vec3_t a3;
                lh_math_vec3_t b3;
                lh_math_vec4_t a4;
                lh_math_vec4_t b4;

                lh_math_vec2_init(lh_addr_of(a2), x, y);
                lh_math_vec2_init(lh_addr_of(b2), y, z);
                lh_math_vec3_init(lh_addr_of(a3), x, y, z);
                lh_math_vec3_init(lh_addr_of(b3), z, x, y);
                lh_math_vec4_init(lh_addr_of(a4), x, y, z, w);
                lh_math_vec4_init(lh_addr_of(b4), w, z, y, x);

                EXPECT_EQ(bits(lh_math_vec2_dot(a2, b2)), bits(x * y + y * z));
                EXPECT_EQ(bits(lh_math_vec3_dot(a3, b3)), bits(x * z + y * x + z * y));
                EXPECT_EQ(bits(lh_math_vec4_dot(a4, b4)), bits(x * w + y * z + z * y + w * x));
                EXPECT_EQ(bits(lh_math_vec2_length_sq(a2)), bits(x * x + y * y));
                EXPECT_EQ(bits(lh_math_vec3_length_sq(a3)), bits(x * x + y * y + z * z));
                EXPECT_EQ(bits(lh_math_vec4_length_sq(a4)), bits(x * x + y * y + z * z + w * w));
                EXPECT_EQ(bits(lh_math_vec3_length(a3)), bits(std::sqrt(x * x + y * y + z * z)));
            }
        }
    }
}

#if LH_TEST_EXPECT_DEATH_ENABLED

TEST(math_view_death, get_at_past_the_end)
{
    const lh_math_view_t column = view_of(3U, 4U, 3U);

    LH_EXPECT_DEATH(lh_math_view_get_at(lh_addr_of(column), 3U));
}

TEST(math_view_death, a_view_past_its_slice_or_with_stride_zero)
{
    LH_EXPECT_DEATH(view_of(3U, 4U, 4U));
    LH_EXPECT_DEATH(view_of(0U, 0U, 2U));
}

TEST(math_view_death, dot_of_different_sizes)
{
    const lh_math_view_t a = view_of(0U, 1U, 2U);
    const lh_math_view_t b = view_of(0U, 1U, 3U);

    LH_EXPECT_DEATH(lh_math_view_dot(lh_addr_of(a), lh_addr_of(b)));
}

#endif

} // namespace
