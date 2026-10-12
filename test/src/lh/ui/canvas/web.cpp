/**
 * @file web.cpp
 * @brief The web sink is a canvas backend, not a second layout engine.
 */

#include <gtest/gtest.h>

#include <lh/ui/canvas.h>
#include <lh/ui/canvas/web.h>
#include <lh/ui/color.h>
#include <lh/null.h>
#include <lh/ui/mask.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/shadow.h>
#include <lh/util/addr.h>

#include <cstring>
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

TEST(canvas_web, shadow_is_one_op_not_a_pixel_each)
{
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t fill;
    lh_ui_shadow_t shadow;
    char buf[2048];

    ASSERT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 80u, 80u));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), lh_addr_of(web));
    lh_ui_color_init(lh_addr_of(fill), 0, 0, 0, 40);
    lh_ui_shadow_init(lh_addr_of(shadow));
    lh_ui_shadow_set_color(lh_addr_of(shadow), fill);
    lh_ui_shadow_set_spread(lh_addr_of(shadow), (lh_ui_scalar_t)8);
    lh_ui_rect_init(lh_addr_of(rect), 10, 10, 40, 40);
    ASSERT_TRUE(lh_ui_canvas_shadow(lh_addr_of(canvas), lh_addr_of(rect), (lh_ui_scalar_t)8,
                                   lh_addr_of(shadow)));
    ASSERT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));
    {
        const std::string body(lh_ui_canvas_web_bytes(lh_addr_of(web)),
                               (size_t)lh_ui_canvas_web_length(lh_addr_of(web)));
        EXPECT_NE(body.find("\"op\":\"shadow\""), std::string::npos);
        EXPECT_EQ(body.find("\"op\":\"fill_rect\""), std::string::npos);
        EXPECT_EQ(web.ops, 1u);
    }
}

TEST(canvas_web, glyph_cover_is_interned)
{
    lh_ui_canvas_web_t web;
    lh_ui_point_t origin;
    lh_ui_color_t fill;
    lh_ui_mask_t mask;
    const lh_byte_t bits[] = {0xA0};
    char buf[2048];

    ASSERT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 16u, 16u));
    lh_ui_mask_init(lh_addr_of(mask), bits, 3, 1, 1, 1);
    lh_ui_color_init(lh_addr_of(fill), 1, 2, 3, 255);
    lh_ui_point_init(lh_addr_of(origin), 0, 0);
    ASSERT_TRUE(lh_ui_canvas_backend_web.fill_mask(lh_addr_of(web), lh_addr_of(origin), lh_addr_of(mask),
                                                   lh_addr_of(fill)));
    lh_ui_point_init(lh_addr_of(origin), 4, 0);
    ASSERT_TRUE(lh_ui_canvas_backend_web.fill_mask(lh_addr_of(web), lh_addr_of(origin), lh_addr_of(mask),
                                                   lh_addr_of(fill)));
    ASSERT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));
    {
        const std::string body(lh_ui_canvas_web_bytes(lh_addr_of(web)),
                               (size_t)lh_ui_canvas_web_length(lh_addr_of(web)));
        size_t first = body.find("\"cover\":");
        size_t second = (first == std::string::npos) ? std::string::npos : body.find("\"cover\":", first + 1);
        EXPECT_NE(body.find("\"m\":"), std::string::npos);
        EXPECT_NE(first, std::string::npos);
        EXPECT_EQ(second, std::string::npos);
        EXPECT_EQ(web.ops, 2u);
    }
}

namespace
{

std::string
one_glyph_frame(lh_ui_canvas_web_glyphs_t *glyphs, const lh_ui_mask_t *mask)
{
    lh_ui_canvas_web_t web;
    lh_ui_point_t origin;
    lh_ui_color_t fill;
    char buf[2048];

    EXPECT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 16u, 16u));
    lh_ui_canvas_web_set_glyphs(lh_addr_of(web), glyphs);
    lh_ui_color_init(lh_addr_of(fill), 1, 2, 3, 255);
    lh_ui_point_init(lh_addr_of(origin), 0, 0);
    EXPECT_TRUE(lh_ui_canvas_backend_web.fill_mask(lh_addr_of(web), lh_addr_of(origin), mask, lh_addr_of(fill)));
    EXPECT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));
    return std::string(lh_ui_canvas_web_bytes(lh_addr_of(web)), (size_t)lh_ui_canvas_web_length(lh_addr_of(web)));
}

} // namespace

/* A stream of frames to one reader sends a glyph's coverage once; a frame on its
   own always carries it, and a reset makes the next frame carry it again. */
TEST(canvas_web, a_kept_glyph_set_sends_a_cover_once_per_reader)
{
    lh_ui_canvas_web_glyphs_t glyphs;
    lh_ui_mask_t mask;
    const lh_byte_t bits[] = {0x5C};

    lh_ui_mask_init(lh_addr_of(mask), bits, 4, 1, 1, 1);
    lh_ui_canvas_web_glyphs_reset(lh_addr_of(glyphs));
    EXPECT_NE(one_glyph_frame(lh_addr_of(glyphs), lh_addr_of(mask)).find("\"cover\":"), std::string::npos);
    const std::string again = one_glyph_frame(lh_addr_of(glyphs), lh_addr_of(mask));
    EXPECT_EQ(again.find("\"cover\":"), std::string::npos);
    EXPECT_NE(again.find("\"m\":"), std::string::npos);
    EXPECT_NE(one_glyph_frame(lh_null, lh_addr_of(mask)).find("\"cover\":"), std::string::npos);
    lh_ui_canvas_web_glyphs_reset(lh_addr_of(glyphs));
    EXPECT_NE(one_glyph_frame(lh_addr_of(glyphs), lh_addr_of(mask)).find("\"cover\":"), std::string::npos);
}

/* The numbers are written by hand now, not by printf: negatives and zero included. */
TEST(canvas_web, numbers_are_written_as_json_numbers)
{
    lh_ui_canvas_web_t web;
    lh_ui_canvas_t canvas;
    lh_ui_rect_t rect;
    lh_ui_color_t fill;
    char buf[512];

    ASSERT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 980u, 0u));
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(lh_ui_canvas_backend_web), lh_addr_of(web));
    lh_ui_color_init(lh_addr_of(fill), 0, 10, 255, 128);
    lh_ui_rect_init(lh_addr_of(rect), -12, 0, 1234567, 9);
    lh_ui_canvas_backend_web.fill_rect(lh_addr_of(web), lh_addr_of(rect), lh_addr_of(fill));
    ASSERT_TRUE(lh_ui_canvas_web_close(lh_addr_of(web)));
    EXPECT_EQ(std::string(lh_ui_canvas_web_bytes(lh_addr_of(web)), (size_t)lh_ui_canvas_web_length(lh_addr_of(web))),
              "{\"kind\":\"lh_ui_canvas\",\"width\":980,\"height\":0,\"ops\":[{\"op\":\"fill_rect\",\"rect\":[-12,0,"
              "1234567,9],\"color\":[0,10,255,128]}]}");
    EXPECT_EQ(std::strlen(lh_ui_canvas_web_bytes(lh_addr_of(web))), (size_t)lh_ui_canvas_web_length(lh_addr_of(web)));
}

/* A buffer one byte too small fails instead of writing past its end. */
TEST(canvas_web, a_full_buffer_fails_the_frame)
{
    lh_ui_canvas_web_t web;
    lh_ui_rect_t rect;
    lh_ui_color_t fill;
    char buf[80];

    ASSERT_TRUE(lh_ui_canvas_web_open_mem(lh_addr_of(web), buf, sizeof(buf), 8u, 8u));
    lh_ui_color_init(lh_addr_of(fill), 1, 2, 3, 255);
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 2, 2);
    lh_ui_canvas_backend_web.fill_rect(lh_addr_of(web), lh_addr_of(rect), lh_addr_of(fill));
    EXPECT_FALSE(lh_ui_canvas_web_close(lh_addr_of(web)));
    EXPECT_LT(lh_ui_canvas_web_length(lh_addr_of(web)), sizeof(buf));
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
