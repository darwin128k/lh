/**
 * @file viewport3d.cpp
 * @brief 3D is a viewport backend, not a canvas op.
 */

#include <gtest/gtest.h>

#include <lh/gl/viewport3d.h>

TEST(viewport3d, null_backend_is_not_a_web_op)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 0, 0, 320, 240);
    lh_gl_viewport3d_begin(lh_addr_of(lh_gl_viewport3d_backend_null), lh_null, lh_addr_of(rect));
    lh_gl_viewport3d_end(lh_addr_of(lh_gl_viewport3d_backend_null), lh_null);
    EXPECT_EQ(lh_gl_viewport3d_backend_null.begin == lh_null, false);
}
