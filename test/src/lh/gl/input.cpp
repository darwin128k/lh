#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/input.h>
#include <lh/gl/input/event.h>
#include <lh/gl/layer.h>
#include <lh/os/input/event.h>

namespace
{

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

lh_os_input_event_t
pointer_event(lh_os_input_kind_t kind, lh_u8_t pointer, int x, int y)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(lh_addr_of(event), kind, lh_os_input_device_kind_touch, pointer,
                                   lh_os_input_button_primary, lh_os_input_coord_from_int(x),
                                   lh_os_input_coord_from_int(y), 0U);
    return event;
}

TEST(gl_input, one_pointer_owns_the_hold_and_a_key_does_not)
{
    lh_gl_entity_class_t plain = plain_class();
    lh_gl_layer_t root;
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_entity_t child;
    lh_gl_entity_init(lh_addr_of(child), rect_of(0, 0, 10, 10), lh_addr_of(plain));
    lh_gl_layer_add(lh_addr_of(root), lh_addr_of(child));
    lh_gl_input_t input;
    lh_gl_input_init(lh_addr_of(input));
    lh_os_input_event_t down = pointer_event(lh_os_input_kind_pointer_down, 1U, 1, 1);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(down)),
              lh_addr_of(child));
    EXPECT_EQ(lh_gl_input_get_tracking(lh_addr_of(input)), lh_bool_true);
    EXPECT_EQ(lh_gl_input_get_pointer(lh_addr_of(input)), 1U);
    lh_gl_entity_set_origin(lh_addr_of(child), point_of(50, 50));
    lh_os_input_event_t move = pointer_event(lh_os_input_kind_pointer_move, 1U, 1, 1);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(move)),
              lh_addr_of(child));
    lh_os_input_event_t other = pointer_event(lh_os_input_kind_pointer_down, 2U, 1, 1);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(other)), nullptr);
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), lh_addr_of(child));
    lh_os_input_event_t other_up = pointer_event(lh_os_input_kind_pointer_up, 2U, 1, 1);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(other_up)), nullptr);
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), lh_addr_of(child));
    lh_os_input_event_t key;
    lh_os_input_event_init_key(lh_addr_of(key), lh_os_key_enter, lh_bool_true, lh_bool_false, 0U,
                               0U);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(key)), nullptr);
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), lh_addr_of(child));
    lh_os_input_event_t up = pointer_event(lh_os_input_kind_pointer_up, 1U, 1, 1);
    EXPECT_EQ(lh_gl_input_apply(lh_addr_of(input), lh_addr_of(root), lh_addr_of(up)),
              lh_addr_of(child));
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), nullptr);
    EXPECT_EQ(lh_gl_input_get_tracking(lh_addr_of(input)), lh_bool_false);
    lh_gl_layer_deinit(lh_addr_of(root));
}

} // namespace
