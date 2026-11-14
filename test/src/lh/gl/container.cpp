#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/canvas.h>
#include <lh/gl/container.h>
#include <lh/gl/layer.h>
#include <lh/gl/surface.h>

namespace
{

lh_gl_point_t drawn_origin;
lh_gl_rect_t drawn_clip;
lh_usize_t drawn_count;

lh_void
record_draw(const lh_gl_entity_t *entity, lh_gl_canvas_t *canvas)
{
    (void)entity;
    drawn_origin = *lh_gl_canvas_get_origin(canvas);
    drawn_clip = *lh_gl_canvas_get_clip(canvas);
    ++drawn_count;
}

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_gl_point_t
point_of(lh_gl_scalar_t x, lh_gl_scalar_t y)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), x, y);
    return point;
}

TEST(gl_container, axes_shift_placement_and_leave_rectangles_unchanged)
{
    lh_gl_entity_class_t plain;
    lh_gl_entity_class_init(lh_addr_of(plain), lh_gl_entity_class_kind_entity, record_draw);
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 100, 100));
    lh_gl_entity_t child;
    lh_gl_entity_init(lh_addr_of(child), rect_of(20, 20, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(layer), lh_addr_of(child));
    lh_gl_scroll_axis_t horizontal;
    lh_gl_scroll_axis_init(lh_addr_of(horizontal), 0);
    lh_gl_scroll_axis_t vertical;
    lh_gl_scroll_axis_init(lh_addr_of(vertical), 0);
    lh_gl_container_t container;
    lh_gl_container_init(lh_addr_of(container), lh_addr_of(layer), lh_addr_of(horizontal),
                         lh_addr_of(vertical));
    lh_gl_point_t origin;
    const lh_gl_point_t *window =
        lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(layer))));
    lh_gl_container_content_origin(lh_addr_of(container), window, lh_addr_of(origin));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(origin)), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(origin)), 0);
    EXPECT_EQ(lh_gl_container_find(lh_addr_of(container), point_of(25, 25)), lh_addr_of(child));
    lh_gl_scroll_axis_set_offset(lh_addr_of(horizontal), 20);
    lh_gl_scroll_axis_set_offset(lh_addr_of(vertical), 20);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_addr_of(child)))), 20);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_addr_of(child)))), 20);
    EXPECT_EQ(lh_gl_container_find(lh_addr_of(container), point_of(1, 1)), lh_addr_of(child));
    EXPECT_EQ(lh_gl_container_find(lh_addr_of(container), point_of(25, 25)),
              lh_gl_layer_get_entity(lh_addr_of(layer)));
    EXPECT_EQ(lh_gl_container_find(lh_addr_of(container), point_of(105, 1)), nullptr);
    lh_gl_point_t canvas_origin = point_of(0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), canvas_origin, rect_of(0, 0, 1, 1));
    lh_gl_rect_t clip = rect_of(0, 0, 100, 100);
    drawn_count = 0;
    lh_gl_container_draw(lh_addr_of(container), lh_addr_of(canvas), lh_addr_of(clip));
    EXPECT_EQ(drawn_count, 1U);
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(drawn_origin)), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(drawn_origin)), 0);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_addr_of(drawn_clip))), 0);
    EXPECT_EQ(lh_gl_point_get_y(lh_gl_rect_get_origin(lh_addr_of(drawn_clip))), 0);
    EXPECT_EQ(lh_gl_point_get_x(lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_addr_of(child)))), 20);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_container, one_axis_shifts_only_its_coordinate)
{
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(10, 30, 40, 50));
    lh_gl_scroll_axis_t vertical;
    lh_gl_scroll_axis_init(lh_addr_of(vertical), 8);
    lh_gl_container_t container;
    lh_gl_container_init(lh_addr_of(container), lh_addr_of(layer), lh_null, lh_addr_of(vertical));
    lh_gl_point_t origin;
    lh_gl_container_content_origin(
        lh_addr_of(container),
        lh_gl_rect_get_origin(lh_gl_entity_get_rect(lh_gl_layer_get_entity(lh_addr_of(layer)))),
        lh_addr_of(origin));
    EXPECT_EQ(lh_gl_point_get_x(lh_addr_of(origin)), 10);
    EXPECT_EQ(lh_gl_point_get_y(lh_addr_of(origin)), 22);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

TEST(gl_container, a_null_layer_does_nothing)
{
    lh_gl_container_t container;

    lh_gl_container_init(lh_addr_of(container), lh_null, lh_null, lh_null);
    EXPECT_EQ(lh_gl_container_find(lh_addr_of(container), point_of(0, 0)), nullptr);
    lh_gl_container_invalidate(lh_addr_of(container));
}

TEST(gl_container, invalidate_marks_the_window_and_add_does_not)
{
    lh_gl_layer_t layer;
    lh_gl_layer_init(lh_addr_of(layer), rect_of(0, 0, 80, 40));
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 80, 40);
    lh_gl_surface_t surface;
    lh_gl_surface_init(lh_addr_of(surface), size);
    lh_gl_surface_attach(lh_addr_of(surface), lh_addr_of(layer));
    lh_gl_surface_clear_dirty(lh_addr_of(surface));
    lh_gl_scroll_axis_t vertical;
    lh_gl_scroll_axis_init(lh_addr_of(vertical), 0);
    lh_gl_container_t container;
    lh_gl_container_init(lh_addr_of(container), lh_addr_of(layer), lh_null, lh_addr_of(vertical));
    lh_gl_scroll_axis_add(lh_addr_of(vertical), 12);
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 0U);
    lh_gl_container_invalidate(lh_addr_of(container));
    EXPECT_EQ(lh_gl_surface_get_dirty_count(lh_addr_of(surface)), 1U);
    lh_gl_layer_deinit(lh_addr_of(layer));
}

} // namespace
