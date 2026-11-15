#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/gl/card.h>
#include <lh/gl/input.h>
#include <lh/gl/input/controller.h>
#include <lh/os/controller.h>
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

} // namespace

TEST(gl_card, a_rounded_corner_misses_and_hiding_passes_the_press)
{
    lh_gl_color_t color;
    lh_gl_font_t font;
    lh_gl_layer_t root;
    lh_gl_card_t card;
    lh_gl_input_t input;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_gl_font_init(lh_addr_of(font), lh_null, ' ', 0, 14, 12);
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_card_init(lh_addr_of(card), rect_of(10, 10, 40, 40), lh_addr_of(color), lh_addr_of(font),
                    "Card");
    lh_gl_card_set_radius(lh_addr_of(card), 16);
    lh_gl_layer_add(lh_addr_of(root), lh_gl_card_get_entity_mut(lh_addr_of(card)));
    lh_gl_input_init(lh_addr_of(input));
    lh_gl_point_t corner;

    lh_gl_point_init(lh_addr_of(corner), 10, 10);
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), corner),
              lh_gl_layer_get_entity(lh_addr_of(root)));
    lh_gl_point_t middle;

    lh_gl_point_init(lh_addr_of(middle), 30, 30);
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), middle),
              lh_gl_card_get_entity(lh_addr_of(card)));
    lh_gl_card_set_shown(lh_addr_of(card), lh_bool_false);
    EXPECT_EQ(lh_gl_input_press(lh_addr_of(input), lh_addr_of(root), middle),
              lh_gl_layer_get_entity(lh_addr_of(root)));
    lh_gl_layer_deinit(lh_addr_of(root));
}

TEST(gl_input_controller, a_pointer_down_holds_the_card)
{
    lh_gl_color_t color;
    lh_gl_font_t font;
    lh_gl_layer_t root;
    lh_gl_card_t card;
    lh_gl_input_t input;
    lh_gl_input_binding_t binding;
    lh_os_controller_t controller;
    lh_os_input_event_t down;

    lh_gl_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_gl_font_init(lh_addr_of(font), lh_null, ' ', 0, 14, 12);
    lh_gl_layer_init(lh_addr_of(root), rect_of(0, 0, 100, 100));
    lh_gl_card_init(lh_addr_of(card), rect_of(10, 10, 40, 40), lh_addr_of(color), lh_addr_of(font),
                    "Card");
    lh_gl_layer_add(lh_addr_of(root), lh_gl_card_get_entity_mut(lh_addr_of(card)));
    lh_gl_input_init(lh_addr_of(input));
    lh_gl_input_binding_init(lh_addr_of(binding), lh_addr_of(input), lh_addr_of(root));
    lh_os_controller_init(lh_addr_of(controller), lh_gl_input_controller_handle_event_is_handled,
                          lh_addr_of(binding), lh_null);
    lh_os_controller_set_cancel_cb(lh_addr_of(controller), lh_gl_input_controller_cancel);
    lh_os_input_event_init_pointer(lh_addr_of(down), lh_os_input_kind_pointer_down,
                                   lh_os_input_device_kind_mouse, 0U, lh_os_input_button_primary,
                                   lh_os_input_coord_from_int(30), lh_os_input_coord_from_int(30),
                                   0U);
    EXPECT_TRUE(lh_os_controller_feed(lh_addr_of(controller), lh_addr_of(down)));
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), lh_gl_card_get_entity(lh_addr_of(card)));
    lh_os_controller_cancel(lh_addr_of(controller));
    EXPECT_EQ(lh_gl_input_get_captured(lh_addr_of(input)), nullptr);
    lh_gl_layer_deinit(lh_addr_of(root));
}
