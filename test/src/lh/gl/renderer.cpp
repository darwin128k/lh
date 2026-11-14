#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/layer.h>
#include <lh/gl/renderer.h>
#include <lh/gl/surface.h>

namespace
{

const lh_gl_entity_t *drawn[8];
lh_gl_rect_t clips[8];
lh_usize_t drawn_count;

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t w, lh_gl_scalar_t h)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, w, h);
    return rect;
}

lh_void
record_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    const lh_usize_t index = drawn_count++;

    drawn[index] = entity;
    clips[index] = lh_ptr_deref(lh_gl_canvas_get_clip(canvas));
}

TEST(gl_renderer, render_is_deferred_and_slices_the_dirty_rect)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_surface_t surface;
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 100, 100);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_t root;

    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 1, 1));
    lh_gl_entity_t child;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 50, 100, 10), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_canvas_t canvas;
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 1, 1));
    lh_gl_renderer_t renderer;

    lh_gl_renderer_init(lh_addr_of(renderer), lh_addr_of(surface), lh_addr_of(canvas), 40);
    drawn_count = 0;
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    EXPECT_EQ(drawn_count, 0U);
    lh_gl_renderer_render(lh_addr_of(renderer));
    ASSERT_EQ(drawn_count, 1U);
    EXPECT_EQ(drawn[0], lh_addr_of(child));
    lh_gl_rect_t visible = rect_of(0, 50, 100, 10);

    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(clips[0]), lh_addr_of(visible)));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_renderer_render(lh_addr_of(renderer));
    EXPECT_EQ(drawn_count, 1U);
    lh_gl_layer_deinit(lh_addr_of(root));
}

TEST(gl_renderer, band_zero_draws_the_dirty_rect_once)
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_surface_t surface;
    lh_gl_size_t size;

    lh_gl_size_init(lh_addr_of(size), 80, 60);
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_layer_t root;

    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 1, 1));
    lh_gl_entity_t child;

    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 80, 60), lh_addr_of(cls));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(root));
    lh_gl_canvas_t canvas;
    lh_gl_point_t origin;

    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 1, 1));
    lh_gl_renderer_t renderer;

    lh_gl_renderer_init(lh_addr_of(renderer), lh_addr_of(surface), lh_addr_of(canvas), 0);
    drawn_count = 0;
    lh_gl_renderer_render(lh_addr_of(renderer));
    ASSERT_EQ(drawn_count, 1U);
    lh_gl_rect_t whole = rect_of(0, 0, 80, 60);

    EXPECT_TRUE(lh_gl_rect_eq(lh_addr_of(clips[0]), lh_addr_of(whole)));
    lh_gl_layer_deinit(lh_addr_of(root));
}

} // namespace
