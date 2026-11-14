#include <gtest/gtest.h>

#include <lh/gl/layer.h>
#include <lh/gl/surface.h>

namespace
{

lh_gl_rect_t
rect_of(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t w, lh_math_scalar_t h)
{
    lh_gl_rect_t rect;
    lh_gl_rect_init(lh_addr_of(rect), x, y, w, h);
    return rect;
}

TEST(gl_surface, dirty_clips_merges_and_collapses)
{
    lh_gl_surface_t surface;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 1000, 1000);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_rect_t area = rect_of(1100, 1100, 10, 10);
    lh_gl_surface_invalidate(lh_addr_of(surface), lh_addr_of(area));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    area = rect_of(-5, -5, 10, 10);
    lh_gl_surface_invalidate(lh_addr_of(surface), lh_addr_of(area));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    lh_gl_rect_t expected = rect_of(0, 0, 5, 5);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    area = rect_of(5, 0, 5, 5);
    lh_gl_surface_invalidate(lh_addr_of(surface), lh_addr_of(area));
    expected = rect_of(0, 0, 10, 5);
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    for (lh_uindex_t i = 0; i <= LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX; ++i)
    {
        area = rect_of(10 * i, 10 * i, 1, 1);
        lh_gl_surface_invalidate(lh_addr_of(surface), lh_addr_of(area));
    }
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    expected = rect_of(0, 0, 10 * LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX + 1,
                       10 * LH_LIBRARY_OPTION_GL_SURFACE_DIRTY_MAX + 1);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
}

TEST(gl_surface, attach_resize_and_detach)
{
    lh_gl_surface_t surface;
    lh_gl_layer_t root;
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 80, 60);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_init(lh_addr_of(root), rect_of(4, 5, 9, 9));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_rect_t expected = rect_of(0, 0, 80, 60);
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(root))),
                              lh_addr_of(expected)));
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_size_init(lh_addr_of(size), 30, 20);
    lh_gl_surface_resize(lh_addr_of(surface), size);
    expected = rect_of(0, 0, 30, 20);
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(root))),
                              lh_addr_of(expected)));
    ASSERT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_TRUE(
        lh_gl_rect_eq(lh_gl_surface_get_dirty(lh_addr_of(surface), 0), lh_addr_of(expected)));
    lh_gl_surface_detach(lh_addr_of(surface));
    EXPECT_EQ(lh_gl_layer_get_surface(lh_addr_of(root)), nullptr);
    EXPECT_TRUE(lh_gl_rect_eq(lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(root))),
                              lh_addr_of(expected)));
    lh_gl_layer_deinit(lh_addr_of(root));
}

} // namespace
