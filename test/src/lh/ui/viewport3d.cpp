/**
 * @file viewport3d.cpp
 * @brief 3D is a viewport backend, not a canvas op.
 */

#include <gtest/gtest.h>

#include <lh/ui/viewport3d.h>

TEST(viewport3d, null_backend_is_not_a_web_op)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 320, 240);
    lh_ui_viewport3d_begin(lh_addr_of(lh_ui_viewport3d_backend_null), lh_null, lh_addr_of(rect));
    lh_ui_viewport3d_end(lh_addr_of(lh_ui_viewport3d_backend_null), lh_null);
    EXPECT_EQ(lh_ui_viewport3d_backend_null.begin == lh_null, false);
}
