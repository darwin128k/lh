#include <gtest/gtest.h>

#include <lh/null.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/ui/surface.h>
#include <lh/util/addr.h>

namespace
{

TEST(surface, starts_empty_and_argb8888)
{
    lh_ui_surface_t surface;

    lh_ui_surface_init(lh_addr_of(surface));
    EXPECT_FALSE(lh_ui_surface_is_valid(lh_addr_of(surface)));
    EXPECT_EQ(lh_ui_surface_get_format(lh_addr_of(surface)), lh_ui_pixmap_format_argb8888);
    lh_ui_surface_deinit(lh_addr_of(surface));
}

/* The format is what says how wide a pixel is, so the pixmap has to answer in the
   format asked for and not in the one the buffer was born with. */
TEST(surface, the_format_decides_the_pixmap_under_it)
{
    lh_ui_surface_t surface;
    lh_ui_size_t size;
    lh_ui_pixmap_t pixmap;

    lh_ui_surface_init(lh_addr_of(surface));
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(64), lh_ui_scalar(8));

    lh_ui_surface_set_format(lh_addr_of(surface), lh_ui_pixmap_format_rgb565);
    ASSERT_TRUE(lh_ui_surface_set_size(lh_addr_of(surface), size));
    ASSERT_TRUE(lh_ui_surface_is_valid(lh_addr_of(surface)));
    ASSERT_TRUE(lh_ui_surface_get_pixmap(lh_addr_of(surface), lh_addr_of(pixmap)));
    EXPECT_EQ(lh_ui_pixmap_get_format(lh_addr_of(pixmap)), lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_ui_pixmap_get_pixel_bytes(lh_addr_of(pixmap)), 2);
    /* 16 bits is half the stride, and every row of the buffer is that half. */
    EXPECT_EQ(lh_ui_pixmap_get_row(lh_addr_of(pixmap), 1) - lh_ui_pixmap_get_row(lh_addr_of(pixmap), 0),
              64 * 2);
    EXPECT_EQ(lh_ui_pixmap_get_width(lh_addr_of(pixmap)), 64);
    EXPECT_EQ(lh_ui_pixmap_get_height(lh_addr_of(pixmap)), 8);
    lh_ui_surface_deinit(lh_addr_of(surface));

    lh_ui_surface_init(lh_addr_of(surface));
    ASSERT_TRUE(lh_ui_surface_set_size(lh_addr_of(surface), size));
    ASSERT_TRUE(lh_ui_surface_get_pixmap(lh_addr_of(surface), lh_addr_of(pixmap)));
    EXPECT_EQ(lh_ui_pixmap_get_format(lh_addr_of(pixmap)), lh_ui_pixmap_format_argb8888);
    EXPECT_EQ(lh_ui_pixmap_get_row(lh_addr_of(pixmap), 1) - lh_ui_pixmap_get_row(lh_addr_of(pixmap), 0),
              64 * 4);
    lh_ui_surface_deinit(lh_addr_of(surface));
}

/* A swapped word order is for a controller on the way out, not for a buffer this
   side draws into: over native-order words every pixel would read as a different
   colour. Asking for it here is the caller's mistake, not a quiet one. */
TEST(surface, a_swapped_format_is_not_something_to_draw_into)
{
    lh_ui_surface_t surface;
    lh_ui_size_t size;

    lh_ui_surface_init(lh_addr_of(surface));
    EXPECT_DEATH(lh_ui_surface_set_format(lh_addr_of(surface), lh_ui_pixmap_format_rgb565_swapped), "");
    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(8), lh_ui_scalar(8));
    (void)lh_ui_surface_set_size(lh_addr_of(surface), size);
    /* And a live buffer keeps its format: a frame drawn in two is not a frame. */
    EXPECT_DEATH(lh_ui_surface_set_format(lh_addr_of(surface), lh_ui_pixmap_format_rgb565), "");
    lh_ui_surface_deinit(lh_addr_of(surface));
}

} // namespace