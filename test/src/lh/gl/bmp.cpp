#include <gtest/gtest.h>

#include <lh/core/bit/endian.h>
#include <lh/core/memory/std.h>
#include <lh/core/util/addr.h>
#include <lh/expect/death.h>
#include <lh/gl/bmp.h>
#include <lh/gl/canvas.h>

#include <cstdio>
#include <vector>

namespace
{

lh_gl_rect_t
rect_of(lh_gl_scalar_t x, lh_gl_scalar_t y, lh_gl_scalar_t width, lh_gl_scalar_t height)
{
    lh_gl_rect_t rect;

    lh_gl_rect_init(lh_addr_of(rect), x, y, width, height);
    return rect;
}

TEST(gl_bmp, stores_bottom_up_bgra)
{
    lh_gl_color_t pixels[2];
    lh_gl_color_init(lh_addr_of(pixels[0]), 10, 20, 30, 40);
    lh_gl_color_init(lh_addr_of(pixels[1]), 50, 60, 70, 80);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 1, 2);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), pixels, size);
    EXPECT_EQ(lh_gl_bmp_byte_count(lh_addr_of(pixmap)), 54U + 8U);
    lh_byte_t bytes[62];
    lh_memory_std_set(bytes, 0, sizeof bytes);
    EXPECT_TRUE(lh_gl_bmp_try_store(lh_addr_of(pixmap), bytes, sizeof bytes));
    EXPECT_EQ(bytes[0], 'B');
    EXPECT_EQ(bytes[1], 'M');
    EXPECT_EQ(lh_bit_unpack_le32(bytes + 2), 62U);
    EXPECT_EQ(lh_bit_unpack_le32(bytes + 18), 1U);
    EXPECT_EQ(lh_bit_unpack_le32(bytes + 22), 2U);
    EXPECT_EQ(bytes[54], 70);
    EXPECT_EQ(bytes[55], 60);
    EXPECT_EQ(bytes[56], 50);
    EXPECT_EQ(bytes[57], 80);
    EXPECT_EQ(bytes[58], 30);
    EXPECT_EQ(bytes[59], 20);
    EXPECT_EQ(bytes[60], 10);
    EXPECT_EQ(bytes[61], 40);
    lh_gl_size_init(lh_addr_of(size), 0, 2);
    lh_gl_pixmap_set_size(lh_addr_of(pixmap), size);
    EXPECT_EQ(lh_gl_bmp_byte_count(lh_addr_of(pixmap)), 0U);
    EXPECT_FALSE(lh_gl_bmp_try_store(lh_addr_of(pixmap), bytes, sizeof bytes));
    lh_gl_size_init(lh_addr_of(size), 1, 2);
    lh_gl_pixmap_set_size(lh_addr_of(pixmap), size);
    lh_gl_pixmap_set_pixels(lh_addr_of(pixmap), lh_null);
    EXPECT_FALSE(lh_gl_bmp_try_store(lh_addr_of(pixmap), bytes, sizeof bytes));
    lh_gl_pixmap_set_pixels(lh_addr_of(pixmap), pixels);
    EXPECT_FALSE(lh_gl_bmp_try_store(lh_addr_of(pixmap), lh_null, sizeof bytes));
    EXPECT_FALSE(lh_gl_bmp_try_store(lh_addr_of(pixmap), bytes, 54U));
}

TEST(gl_bmp, store_checks_a_short_buffer)
{
    lh_gl_color_t pixel;
    lh_gl_color_init(lh_addr_of(pixel), 1, 2, 3, 4);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 1, 1);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), lh_addr_of(pixel), size);
    lh_byte_t byte = 0;
    LH_EXPECT_CHECK_DEATH(lh_gl_bmp_store(lh_addr_of(pixmap), lh_addr_of(byte), 1U));
}

lh_gl_color_t look_pixels[480 * 320];

TEST(gl_bmp, writes_three_bars_a_viewer_can_open)
{
    lh_memory_std_set(look_pixels, 0, sizeof look_pixels);
    lh_gl_size_t size;
    lh_gl_size_init(lh_addr_of(size), 480, 320);
    lh_gl_pixmap_t pixmap;
    lh_gl_pixmap_init(lh_addr_of(pixmap), look_pixels, size);
    lh_gl_point_t origin;
    lh_gl_point_init(lh_addr_of(origin), 0, 0);
    lh_gl_canvas_t canvas;
    lh_gl_canvas_init(lh_addr_of(canvas), origin, rect_of(0, 0, 480, 320));
    lh_gl_canvas_set_target(lh_addr_of(canvas), lh_addr_of(pixmap));
    lh_gl_canvas_set_backend(lh_addr_of(canvas), lh_gl_pixmap_get_canvas_backend());
    lh_gl_color_t color;
    lh_gl_color_init(lh_addr_of(color), 32, 36, 40, 255);
    lh_gl_rect_t full = rect_of(0, 0, 480, 320);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(full), lh_addr_of(color));
    lh_gl_color_init(lh_addr_of(color), 220, 64, 48, 255);
    lh_gl_rect_t left = rect_of(40, 40, 120, 240);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(left), lh_addr_of(color));
    lh_gl_color_init(lh_addr_of(color), 48, 140, 220, 255);
    lh_gl_rect_t middle = rect_of(180, 40, 120, 240);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(middle), lh_addr_of(color));
    lh_gl_color_init(lh_addr_of(color), 48, 180, 96, 255);
    lh_gl_rect_t right = rect_of(320, 40, 120, 240);
    lh_gl_canvas_fill_rect(lh_addr_of(canvas), lh_addr_of(right), lh_addr_of(color));
    lh_usize_t count = lh_gl_bmp_byte_count(lh_addr_of(pixmap));
    std::vector<lh_byte_t> bytes(count);
    lh_gl_bmp_store(lh_addr_of(pixmap), bytes.data(), count);
    std::FILE *file = std::fopen("H:/Projects/lh/build/gl-look.bmp", "wb");
    ASSERT_NE(file, nullptr);
    EXPECT_EQ(std::fwrite(bytes.data(), 1U, count, file), count);
    std::fclose(file);
}

} // namespace
