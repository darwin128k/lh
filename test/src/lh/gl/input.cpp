#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/input.h>
#include <lh/gl/layer.h>

namespace
{

lh_gl_rect_t
rect_of(lh_math_scalar_t x, lh_math_scalar_t y, lh_math_scalar_t width, lh_math_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

lh_gl_point_t
point_of(lh_math_scalar_t x, lh_math_scalar_t y)
{
    lh_gl_point_t point;

    lh_gl_point_init(lh_addr_of(point), x, y);
    return point;
}

lh_gl_entity_class_t
plain_class()
{
    lh_gl_entity_class_t cls;

    lh_gl_entity_class_init(lh_addr_of(cls), lh_gl_entity_class_kind_entity, lh_null);
    return cls;
}

TEST(gl_input, init_holds_nothing)
{
    lh_gl_input_t input;

    lh_gl_input_init(lh_addr_of(input));
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), nullptr);
    EXPECT_EQ(lh_gl_input_move(lh_addr_of(input)), nullptr);
    EXPECT_EQ(lh_gl_input_release(lh_addr_of(input)), nullptr);
}

TEST(gl_input, press_holds_through_a_move_after_the_entity_leaves_the_point)
{
    lh_gl_entity_class_t plain = plain_class();
    lh_gl_layer_t root;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_entity_t child;
    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_input_t input;
    lh_gl_input_init(lh_addr_of(input));
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(1, 1)),
              lh_addr_of(child));
    lh_gl_entity_set_origin(lh_addr_of(child), point_of(50, 50));
    EXPECT_EQ(lh_gl_layer_find_entity(lh_addr_of(root), point_of(1, 1)),
              lh_gl_layer_get_entity(lh_addr_of(root)));
    EXPECT_EQ(lh_gl_input_move(lh_addr_of(input)), lh_addr_of(child));
    EXPECT_EQ(lh_gl_input_release(lh_addr_of(input)), lh_addr_of(child));
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), nullptr);
    lh_gl_layer_deinit(lh_addr_of(root));
}

TEST(gl_input, a_miss_clears_and_a_second_press_replaces)
{
    lh_gl_entity_class_t plain = plain_class();
    lh_gl_layer_t root;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_entity_t back;
    lh_gl_entity_init(lh_addr_of(back), rect_of(0, 0, 10, 10), lh_addr_of(plain));
    lh_gl_entity_t front;
    lh_gl_entity_init(lh_addr_of(front), rect_of(0, 0, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(back));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(front));
    lh_gl_input_t input;
    lh_gl_input_init(lh_addr_of(input));
    lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(1, 1));
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(200, 200)), nullptr);
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), nullptr);
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(1, 1)),
              lh_addr_of(front));
    lh_gl_layer_deinit(lh_addr_of(root));
}

TEST(gl_input, press_returns_the_deepest_entity)
{
    lh_gl_entity_class_t plain = plain_class();
    lh_gl_layer_t root;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_layer_t nested;
    lh_gl_layer_init(lh_addr_of(nested), rect_of(10, 10, 40, 40));
    lh_gl_entity_t leaf;
    lh_gl_entity_init(lh_addr_of(leaf), rect_of(5, 5, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(root), lh_gl_layer_get_entity_mut(lh_addr_of(nested)));
    lh_gl_layer_add(lh_addr_of(nested), lh_addr_of(leaf));
    lh_gl_input_t input;
    lh_gl_input_init(lh_addr_of(input));
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(16, 16)),
              lh_addr_of(leaf));
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), point_of(12, 12)),
              lh_gl_layer_get_entity(lh_addr_of(nested)));
    lh_gl_layer_deinit(lh_addr_of(nested));
    lh_gl_layer_deinit(lh_addr_of(root));
}

} // namespace
