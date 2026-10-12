/**
 * @file web.cpp
 * @brief The web sink is a canvas backend, not a second layout engine.
 */

#include <gtest/gtest.h>

#include <lh/ui/canvas.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/color.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/util/addr.h>

#include <fstream>
#include <string>

TEST(canvas_web, records_engine_ops_not_dom)
{
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t fill;
    const char *path = "lh-web-dump.json";

    ASSERT_TRUE(lh_ui_canvas_web_open(lh_addr_of(web), path, 40u, 20u));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), lh_addr_of(web));
    lh_ui_color_init(lh_addr_of(fill), 44, 92, 255, 255);
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(fill));
    lh_ui_rect_init(lh_addr_of(rect), 2, 3, 8, 9);
    lh_ui_color_init(lh_addr_of(fill), 31, 157, 107, 255);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(fill));
    lh_ui_rect_init(lh_addr_of(rect), 10, 4, 12, 12);
    lh_ui_canvas_fill_round_rect(lh_addr_of(canvas), lh_addr_of(rect), (lh_ui_scalar_t)3,
                                 lh_addr_of(fill));
    ASSERT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));

    std::ifstream in(path);
    ASSERT_TRUE(in) << path;
    std::string body((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_NE(body.find("\"kind\":\"lh_ui_canvas\""), std::string::npos);
    EXPECT_NE(body.find("\"op\":\"clear\""), std::string::npos);
    EXPECT_NE(body.find("\"op\":\"fill_rect\""), std::string::npos);
    EXPECT_NE(body.find("\"op\":\"fill_round_rect\""), std::string::npos);
    EXPECT_EQ(body.find("<div"), std::string::npos);
    EXPECT_EQ(body.find("grid-template"), std::string::npos);
}

TEST(canvas_web, memory_buffer_is_the_same_ops_without_a_file)
{
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t fill;
    char buf[1024];

    ASSERT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 8u, 8u));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), lh_addr_of(web));
    lh_ui_color_init(lh_addr_of(fill), 1, 2, 3, 255);
    lh_ui_canvas_clear(lh_addr_of(canvas), lh_addr_of(fill));
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 2, 2);
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(rect), lh_addr_of(fill));
    ASSERT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));
    ASSERT_GT(lh_ui_canvas_web_length(lh_addr_of(web)), 0u);
    EXPECT_NE(std::string(lh_ui_canvas_web_bytes(lh_addr_of(web)),
                          (size_t)lh_ui_canvas_web_length(lh_addr_of(web)))
                  .find("\"op\":\"fill_rect\""),
              std::string::npos);
}
