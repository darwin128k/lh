#include <gtest/gtest.h>

#include <lh/core/util/addr.h>
#include <lh/expect/death.h>
#include <lh/gl/color.h>
#include <lh/gl/fill.h>
#include <lh/gl/rect/initializer.h>
#include <lh/gl/screen.h>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), 0, 0, width, height);
    return rect;
}

TEST(gl_screen, open_puts_the_layer_in_front_and_back_returns_it)
{
    lh_gl_layer_t root;
    lh_gl_layer_t first;
    lh_gl_layer_t second;

    lh_gl_layer_init(lh_addr_of(root), rect_of(100, 50));
    lh_gl_layer_init(lh_addr_of(first), rect_of(100, 50));
    lh_gl_layer_init(lh_addr_of(second), rect_of(100, 50));
    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), nullptr);

    EXPECT_TRUE(lh_gl_screen_try_open(lh_addr_of(root), lh_addr_of(first)));
    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), lh_addr_of(first));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_gl_layer_get_entity(lh_addr_of(first))), lh_addr_of(root));

    lh_gl_screen_open(lh_addr_of(root), lh_addr_of(second));
    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), lh_addr_of(second));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(root)), 2U);

    EXPECT_EQ(lh_gl_screen_try_back(lh_addr_of(root)), lh_addr_of(second));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_gl_layer_get_entity(lh_addr_of(second))), nullptr);
    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), lh_addr_of(first));

    EXPECT_EQ(lh_gl_screen_back(lh_addr_of(root)), lh_addr_of(first));
    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), nullptr);
    EXPECT_EQ(lh_gl_screen_try_back(lh_addr_of(root)), nullptr);
    EXPECT_TRUE(lh_gl_layer_is_empty(lh_addr_of(root)));

    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(first));
    lh_gl_layer_deinit(lh_addr_of(second));
}

TEST(gl_screen, a_front_widget_is_not_a_screen)
{
    lh_gl_layer_t root;
    lh_gl_layer_t screen;
    lh_gl_color_t color;
    lh_gl_fill_t fill;

    lh_gl_layer_init(lh_addr_of(root), rect_of(100, 50));
    lh_gl_layer_init(lh_addr_of(screen), rect_of(100, 50));
    lh_gl_color_init(lh_addr_of(color), 0, 0, 0, 255);
    lh_gl_fill_init(lh_addr_of(fill), rect_of(10, 10), lh_addr_of(color));
    lh_gl_screen_open(lh_addr_of(root), lh_addr_of(screen));
    lh_gl_layer_add(lh_addr_of(root), lh_gl_fill_get_entity_mut(lh_addr_of(fill)));

    EXPECT_EQ(lh_gl_screen_get_front(lh_addr_of(root)), nullptr);
    EXPECT_EQ(lh_gl_screen_try_back(lh_addr_of(root)), nullptr);
    EXPECT_EQ(lh_gl_entity_get_parent(lh_gl_layer_get_entity(lh_addr_of(screen))),
              lh_addr_of(root));
    EXPECT_EQ(lh_gl_entity_get_parent(lh_gl_fill_get_entity(lh_addr_of(fill))), lh_addr_of(root));

    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(screen));
}

TEST(gl_screen, open_refuses_a_layer_that_already_belongs)
{
    lh_gl_layer_t root;
    lh_gl_layer_t screen;

    lh_gl_layer_init(lh_addr_of(root), rect_of(100, 50));
    lh_gl_layer_init(lh_addr_of(screen), rect_of(100, 50));
    lh_gl_screen_open(lh_addr_of(root), lh_addr_of(screen));
    EXPECT_FALSE(lh_gl_screen_try_open(lh_addr_of(root), lh_addr_of(screen)));
    EXPECT_FALSE(lh_gl_screen_try_open(lh_addr_of(root), lh_addr_of(root)));
    EXPECT_EQ(lh_gl_layer_get_count(lh_addr_of(root)), 1U);

    LH_EXPECT_CHECK_DEATH(lh_gl_screen_open(lh_addr_of(root), lh_addr_of(screen)));
    LH_EXPECT_CHECK_DEATH(lh_gl_screen_back(lh_addr_of(screen)));

    lh_gl_layer_deinit(lh_addr_of(root));
    lh_gl_layer_deinit(lh_addr_of(screen));
}

} /* namespace */
