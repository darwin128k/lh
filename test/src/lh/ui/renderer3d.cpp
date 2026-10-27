#include <gtest/gtest.h>
#include <lh/ui/renderer3d.h>
#include <algorithm>
#include <cmath>

namespace
{
lh_math_vec3_t
v(float x, float y, float z)
{
    lh_math_vec3_t value;
    lh_math_vec3_init(&value, x, y, z);
    return value;
}
class renderer3d : public testing::Test
{
  protected:
    lh_u32_t bits[64 * 64]{};
    lh_math_fscalar_t depth[64 * 64]{};
    lh_ui_pixmap_t pixmap;
    lh_ui_renderer3d_camera_t camera;
    lh_ui_renderer3d_t renderer;
    void
    SetUp() override
    {
        lh_ui_pixmap_init(&pixmap, reinterpret_cast<lh_byte_ptr>(bits), 64, 64, 64 * 4,
                          lh_ui_pixmap_format_argb8888);
        ASSERT_TRUE(
            lh_ui_renderer3d_camera_init(&camera, v(0, 0, 0), v(0, 0, 1), v(0, 1, 0), 1, .1, 100));
        ASSERT_TRUE(lh_ui_renderer3d_begin(&renderer, &pixmap, depth, 64 * 64, &camera, 0));
    }
};
} // namespace
TEST_F(renderer3d, rejects_degenerate_camera_and_small_depth_buffer)
{
    EXPECT_FALSE(
        lh_ui_renderer3d_camera_init(&camera, v(0, 0, 0), v(0, 0, 0), v(0, 1, 0), 1, .1, 100));
    EXPECT_FALSE(lh_ui_renderer3d_begin(&renderer, &pixmap, depth, 2, &camera, 0));
}
TEST_F(renderer3d, nearer_triangles_win_the_depth_test)
{
    lh_ui_renderer3d_triangle(&renderer, v(-1, -1, 2), v(1, -1, 2), v(0, 1, 2), 0xff00ff00u);
    ASSERT_EQ(bits[32 * 64 + 32], 0xff00ff00u);
    lh_ui_renderer3d_triangle(&renderer, v(-2, -2, 4), v(2, -2, 4), v(0, 2, 4), 0xffff0000u);
    EXPECT_EQ(bits[32 * 64 + 32], 0xff00ff00u);
    lh_ui_renderer3d_triangle(&renderer, v(-.5f, -.5f, 1), v(.5f, -.5f, 1), v(0, .5f, 1),
                              0xff0000ffu);
    EXPECT_EQ(bits[32 * 64 + 32], 0xff0000ffu);
    lh_ui_renderer3d_triangle(&renderer, v(NAN, 0, 1), v(0, 1, 1), v(1, 1, 1), 0);
    EXPECT_EQ(bits[32 * 64 + 32], 0xff0000ffu);
}
TEST_F(renderer3d, rejects_behind_camera_and_clips_the_frustum)
{
    lh_ui_renderer3d_triangle(&renderer, v(-1, -1, -2), v(1, -1, -2), v(0, 1, -2), 1);
    for (auto pixel : bits)
    {
        EXPECT_EQ(pixel, 0u);
    }
    lh_ui_renderer3d_triangle(&renderer, v(-10, -10, .01f), v(10, -10, 2), v(0, 10, 2),
                              0xffabcdefu);
    EXPECT_GT(
        std::count_if(std::begin(bits), std::end(bits), [](auto pixel) { return pixel != 0; }), 0);
    EXPECT_GT(lh_ui_renderer3d_get_triangles(&renderer), 0u);
}
