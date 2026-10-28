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

/* -- Bricks ------------------------------------------------------------------ */

TEST_F(renderer3d, camera_is_orthonormal_and_usable)
{
    EXPECT_TRUE(lh_ui_renderer3d_camera_is_usable(&camera));
    EXPECT_NEAR(lh_math_vec3_length(camera.forward), 1, 1e-6f);
    EXPECT_NEAR(lh_math_vec3_dot(camera.forward, camera.up), 0, 1e-6f);
    EXPECT_NEAR(lh_math_vec3_dot(camera.forward, camera.right), 0, 1e-6f);
    lh_ui_renderer3d_camera_t broken = camera;
    broken.far_plane = broken.near_plane;
    EXPECT_FALSE(lh_ui_renderer3d_camera_is_usable(&broken));
}

TEST_F(renderer3d, to_view_measures_from_the_eye_along_the_axes)
{
    lh_ui_renderer3d_camera_t moved;
    ASSERT_TRUE(lh_ui_renderer3d_camera_init(&moved, v(1, 2, 3), v(0, 0, 1), v(0, 1, 0), 1, .1f,
                                             100));
    lh_math_vec3_t view;
    lh_ui_renderer3d_camera_to_view(&moved, v(1, 2, 8), &view);
    EXPECT_NEAR(lh_math_vec3_get_x(&view), 0, 1e-6f);
    EXPECT_NEAR(lh_math_vec3_get_y(&view), 0, 1e-6f);
    EXPECT_NEAR(lh_math_vec3_get_z(&view), 5, 1e-6f);
    lh_ui_renderer3d_camera_to_view(&moved, v(1, 4, 3), &view);
    EXPECT_NEAR(lh_math_vec3_get_y(&view), 2, 1e-6f);
}

TEST_F(renderer3d, plane_distance_is_positive_inside_the_frustum)
{
    const lh_math_vec3_t inside = v(0, 0, 5);
    for (int plane = 0; plane < lh_ui_renderer3d_plane_count; plane++)
    {
        EXPECT_GT(lh_ui_renderer3d_get_plane_distance(&renderer, inside,
                                                      (lh_ui_renderer3d_plane_t)plane),
                  0)
            << plane;
    }
    EXPECT_LT(lh_ui_renderer3d_get_plane_distance(&renderer, v(0, 0, .05f),
                                                  lh_ui_renderer3d_plane_near),
              0);
    EXPECT_LT(lh_ui_renderer3d_get_plane_distance(&renderer, v(0, 0, 200),
                                                  lh_ui_renderer3d_plane_far),
              0);
    EXPECT_LT(lh_ui_renderer3d_get_plane_distance(&renderer, v(100, 0, 1),
                                                  lh_ui_renderer3d_plane_right),
              0);
}

TEST_F(renderer3d, clip_plane_cuts_at_the_crossing_and_reports_overflow)
{
    const lh_math_vec3_t in[3] = {v(0, 0, -1), v(1, 0, 1), v(-1, 0, 1)};
    lh_math_vec3_t out[4];
    const lh_s32_t n =
        lh_ui_renderer3d_clip_plane(&renderer, lh_ui_renderer3d_plane_near, in, 3, out, 4);
    ASSERT_EQ(n, 4);
    for (lh_s32_t i = 0; i < n; i++)
    {
        EXPECT_GE(lh_math_vec3_get_z(&out[i]), .1f - 1e-6f) << i;
    }
    EXPECT_EQ(lh_ui_renderer3d_clip_plane(&renderer, lh_ui_renderer3d_plane_near, in, 3, out, 3),
              -1);
    const lh_math_vec3_t behind[3] = {v(0, 0, -1), v(1, 0, -1), v(0, 1, -1)};
    EXPECT_EQ(
        lh_ui_renderer3d_clip_plane(&renderer, lh_ui_renderer3d_plane_near, behind, 3, out, 4), 0);
}

TEST_F(renderer3d, project_puts_the_view_axis_at_the_centre)
{
    lh_math_vec3_t p;
    lh_ui_renderer3d_project(&renderer, v(0, 0, 4), &p);
    EXPECT_FLOAT_EQ(lh_math_vec3_get_x(&p), 32);
    EXPECT_FLOAT_EQ(lh_math_vec3_get_y(&p), 32);
    EXPECT_FLOAT_EQ(lh_math_vec3_get_z(&p), .25f);
    lh_ui_renderer3d_project(&renderer, v(0, 1, 4), &p);
    EXPECT_LT(lh_math_vec3_get_y(&p), 32); /* up is towards row 0 */
}

TEST(renderer3d_get_edge, sign_tells_the_side)
{
    const lh_math_vec3_t a = v(0, 0, 0);
    const lh_math_vec3_t b = v(10, 0, 0);
    EXPECT_EQ(lh_ui_renderer3d_get_edge(a, b, 5, 0), 0);
    EXPECT_LT(lh_ui_renderer3d_get_edge(a, b, 5, 1), 0);
    EXPECT_GT(lh_ui_renderer3d_get_edge(a, b, 5, -1), 0);
}

TEST_F(renderer3d, put_fragment_keeps_the_nearest)
{
    EXPECT_TRUE(lh_ui_renderer3d_put_fragment(&renderer, 3, 4, 5, 0xff112233u));
    EXPECT_EQ(bits[4 * 64 + 3], 0xff112233u);
    EXPECT_FALSE(lh_ui_renderer3d_put_fragment(&renderer, 3, 4, 6, 0xff445566u));
    EXPECT_EQ(bits[4 * 64 + 3], 0xff112233u);
    EXPECT_TRUE(lh_ui_renderer3d_put_fragment(&renderer, 3, 4, 2, 0xff778899u));
    EXPECT_EQ(depth[4 * 64 + 3], 2);
}

TEST_F(renderer3d, raster_fills_pixel_centres_and_skips_degenerate)
{
    lh_ui_renderer3d_raster(&renderer, v(10, 10, .5f), v(20, 10, .5f), v(10, 20, .5f), 7);
    EXPECT_EQ(bits[12 * 64 + 12], 7u);
    EXPECT_EQ(bits[30 * 64 + 30], 0u);
    EXPECT_EQ(lh_ui_renderer3d_get_triangles(&renderer), 1u);
    lh_ui_renderer3d_raster(&renderer, v(1, 1, .5f), v(2, 2, .5f), v(3, 3, .5f), 9);
    EXPECT_EQ(lh_ui_renderer3d_get_triangles(&renderer), 1u);
}

TEST_F(renderer3d, clear_resets_colour_and_depth)
{
    lh_ui_renderer3d_put_fragment(&renderer, 0, 0, 1, 5);
    ASSERT_TRUE(lh_ui_renderer3d_clear(&renderer, 0xff000001u));
    EXPECT_EQ(bits[0], 0xff000001u);
    EXPECT_GT(depth[0], 1e30f);
}
