/**
 * @file web.cpp
 * @brief Tests for `lh/gx/backend/web.h`.
 *
 * The web backend writes the canvas's calls as JSON commands. What is checked:
 * a frame is one array and its commands are separated by commas, a shape that
 * lies wholly outside its clip is not written, a mask goes as its pixels row by
 * row without the stride's padding, and a renderer drawing through the backend
 * writes the same calls it would make on any other one.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/web.h>
#include <lh/gx/box.h>
#include <lh/gx/render.h>
#include <lh/gx/style.h>
#include <lh/test/gx/brush.h>

#include <string>

namespace
{

lh_gx_rect_t
rect(lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w, lh_gx_scalar_t h)
{
    lh_gx_rect_t r;

    lh_gx_rect_init(&r, x, y, w, h);
    return r;
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

/* The text, the writer over it and the backend writing through them. */
struct Web
{
    lh_str_t text;
    lh_web_json_writer_t writer;
    lh_gx_backend_web_t web;
    lh_gx_canvas_t canvas;

    Web()
    {
        lh_str_init(&text);
        lh_web_json_writer_init(&writer, &text, 0U);
        lh_gx_backend_web_init(&web, &writer);
        lh_gx_backend_web_canvas_init(&canvas);
    }

    ~Web()
    {
        lh_str_deinit(&text);
    }

    Web(const Web &) = delete;
    Web &
    operator=(const Web &) = delete;

    std::string
    str() const
    {
        const lh_usize_t size = lh_str_get_size(&text);

        return size == 0U ? std::string() : std::string(lh_str_get_data(&text), size);
    }
};

TEST(gx_backend_web, the_table_has_both_slots)
{
    lh_gx_canvas_t table;

    lh_gx_backend_web_canvas_init(&table);
    EXPECT_TRUE(lh_gx_canvas_get_fill_rect(&table) == lh_gx_backend_web_fill_rect);
    EXPECT_TRUE(lh_gx_canvas_get_fill_mask(&table) == lh_gx_backend_web_fill_mask);
    EXPECT_EQ(lh_gx_canvas_is_complete(&table), lh_bool_true);
}

TEST(gx_backend_web, a_frame_is_one_array_of_commands)
{
    Web out;
    const lh_gx_rect_t clip = rect(0, 0, 64, 32);
    const lh_gx_rect_t first = rect(0, 0, 64, 32);
    const lh_gx_rect_t second = rect(-2, 4, 8, 3);

    lh_gx_backend_web_begin(&out.web);
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[]");

    lh_gx_backend_web_begin(&out.web);
    lh_gx_backend_web_fill_rect(&out.web, &clip, &first, lh_test::gx_brush(0xFFFFFFFFU).get());
    lh_gx_backend_web_fill_rect(&out.web, &clip, &second, lh_test::gx_brush(0x80FF0000U).get());
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[]"
                         "[[\"fill_rect\",[0,0,64,32],[0,0,64,32],4294967295],"
                         "[\"fill_rect\",[0,0,64,32],[-2,4,8,3],2164195328]]");
    EXPECT_EQ(lh_gx_backend_web_get_count(&out.web), 2U);
}

TEST(gx_backend_web, a_shape_outside_its_clip_is_not_written)
{
    Web out;
    const lh_gx_rect_t clip = rect(0, 0, 10, 10);
    const lh_gx_rect_t beside = rect(10, 0, 5, 5);
    lh_byte_t pixels[4] = {0xFFU, 0xFFU, 0xFFU, 0xFFU};
    lh_memory_bounds_t bits;

    lh_memory_bounds_init(&bits, pixels, sizeof(pixels));
    lh_gx_pixmap_t mask;
    lh_gx_pixmap_init(&mask, &bits, 2U, 2U, lh_gx_pixmap_format_a8);
    lh_gx_backend_web_begin(&out.web);
    lh_gx_backend_web_fill_rect(&out.web, &clip, &beside, lh_test::gx_brush(0xFFFFFFFFU).get());
    lh_gx_backend_web_fill_mask(&out.web, &clip, &mask, point(-2, 3),
                                lh_test::gx_brush(0xFFFFFFFFU).get());
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[]");
    EXPECT_EQ(lh_gx_backend_web_get_count(&out.web), 0U);
}

TEST(gx_backend_web, a_mask_goes_as_its_pixels_without_the_padding)
{
    Web out;
    const lh_gx_rect_t clip = rect(0, 0, 10, 10);
    /* 3 by 2, rows 4 bytes apart: the fourth byte of each row is padding. */
    lh_byte_t pixels[8] = {0x00U, 0x80U, 0xFFU, 0xEEU, 0x01U, 0x02U, 0x03U, 0xEEU};
    lh_memory_bounds_t bits;

    lh_memory_bounds_init(&bits, pixels, sizeof(pixels));
    lh_gx_pixmap_t mask;
    lh_gx_pixmap_init_stride(&mask, &bits, 3U, 2U, 4U, lh_gx_pixmap_format_a8);
    lh_gx_backend_web_begin(&out.web);
    lh_gx_backend_web_fill_mask(&out.web, &clip, &mask, point(1, 2),
                                lh_test::gx_brush(0xFF000000U).get());
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[[\"fill_mask\",[0,0,10,10],[1,2,3,2],4278190080,\"0080ff010203\"]]");
}

TEST(gx_backend_web, a_renderer_draws_through_it_as_through_any_backend)
{
    Web out;
    lh_gx_render_t render;

    lh_gx_render_init(&render, &out.canvas, &out.web, 16, 8);
    lh_gx_style_t style;
    lh_gx_style_init(&style, 0xFF0000FFU, 0U, 0, 0, 0U, nullptr);
    lh_gx_entity_t card;
    lh_gx_entity_init(&card, &lh_gx_box_class, &style, 2, 2, 4, 3);
    lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &card);
    lh_gx_backend_web_begin(&out.web);
    lh_gx_damage_t drawn;
    lh_gx_render_draw(&render, &drawn);
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[[\"fill_rect\",[2,2,4,3],[2,2,4,3],4278190335]]");

    /* Nothing changed: the next frame is empty. */
    lh_str_clear(&out.text);
    lh_gx_backend_web_begin(&out.web);
    lh_gx_render_draw(&render, &drawn);
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[]");
}

TEST(gx_backend_web, a_gradient_goes_as_its_box_shape_angle_and_stops)
{
    Web out;
    const lh_u32_t stops[2] = {0xFF000000U, 0xFFFFFFFFU};
    const lh_gx_rect_t clip = rect(0, 0, 64, 32);
    const lh_gx_rect_t part = rect(2, 3, 4, 5);
    lh_pixel_gradient_t gradient;
    lh_gx_brush_t brush;

    lh_pixel_gradient_init(&gradient);
    lh_pixel_gradient_init_stops(&gradient, stops, lh_null, 2);
    lh_pixel_gradient_set_linear(&gradient, 135);
    lh_gx_brush_init_gradient(&brush, &gradient, nullptr, rect(1, 2, 30, 20));
    lh_gx_backend_web_begin(&out.web);
    lh_gx_backend_web_fill_rect(&out.web, &clip, &part, &brush);
    lh_pixel_gradient_set_radial(&gradient);
    lh_gx_backend_web_fill_rect(&out.web, &clip, &part, &brush);
    lh_gx_backend_web_end(&out.web);
    EXPECT_EQ(out.str(), "[[\"fill_rect\",[0,0,64,32],[2,3,4,5],"
                         "[\"gradient\",[1,2,30,20],0,135,0,4278190080,255,4294967295]],"
                         "[\"fill_rect\",[0,0,64,32],[2,3,4,5],"
                         "[\"gradient\",[1,2,30,20],1,135,0,4278190080,255,4294967295]]]");
}

} // namespace
