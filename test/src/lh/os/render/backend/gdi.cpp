/* The GDI backend is the one target that has to widen a buffer on the way out, so
   its default format is a measured decision and not a preference. The whole point
   of these two tests is that the number they pin was nearly lost: RGB565 draws an
   800x600 frame measurably faster (137 750 ns against 94 100 on this machine) and
   presents it far slower (382 400 ns against 839 050, because a 32-bit window DC
   cannot be copied into from 16 bits and GDI expands every pixel). Half the bytes
   on the way in is not half the frame. */

#include <gtest/gtest.h>

#include <lh/compiler/os.h>
#include <lh/null.h>
#include <lh/os/system/surface.h>
#include <lh/ui/canvas.h>
#include <lh/ui/canvas/sw.h>
#include <lh/ui/color.h>
#include <lh/ui/pixmap.h>
#include <lh/ui/point.h>
#include <lh/ui/rect.h>
#include <lh/ui/scalar.h>
#include <lh/ui/size.h>
#include <lh/ui/surface.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

#if LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS

#    include <lh/os/render/backend/gdi.h>

namespace
{

TEST(os_render_backend_gdi, a_fresh_context_draws_into_argb8888)
{
    lh_os_render_backend_gdi_context_t gdi;

    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    EXPECT_EQ(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi)), lh_ui_pixmap_format_argb8888);
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

TEST(os_render_backend_gdi, the_format_is_reachable_before_the_first_frame)
{
    lh_os_render_backend_gdi_context_t gdi;

    /* What an MCU-sized target wants, and the only thing that costs it here is
       that the DIB it asks for is 16-bit. Reaching it is one call. */
    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    lh_os_render_backend_gdi_context_set_format(lh_addr_of(gdi), lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi)), lh_ui_pixmap_format_rgb565);
    EXPECT_EQ(lh_ui_pixmap_format_get_bytes(lh_os_render_backend_gdi_context_get_format(lh_addr_of(gdi))), 2);
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

/* ── The present ──────────────────────────────────────────────────────────
   A backend presents the pixels it drew and nothing else. The canvas clears and
   draws the damage, which in a partial frame is smaller than the area, and a
   DIB holds no promise about the rest: `begin_area` drops it whenever the area
   changes size — between strips it always does — and a fresh one is whatever GDI
   hands back. Blitting all of it put a strip's leftover picture on the screen
   beside whatever the pointer had just moved.

   Both tests count *both* colours, so a frame that presented nothing at all is
   as loud as one that presented everything. */

constexpr lh_u32_t red_hex = 0xFF0000FFu; /* 0xRRGGBBAA, opaque red */
constexpr lh_u32_t blue_hex = 0x0000FFFFu;
constexpr lh_u32_t green_hex = 0x00FF00FFu;

void
fill_surface(lh_ui_surface_t *surface, lh_u32_t hex)
{
    lh_ui_pixmap_t pixmap;
    lh_ui_canvas_sw_t sw;
    lh_ui_color_t color;

    EXPECT_TRUE(lh_ui_surface_get_pixmap(surface, lh_addr_of(pixmap)));
    lh_ui_canvas_sw_init(lh_addr_of(sw));
    lh_ui_canvas_sw_set_pixmap(lh_addr_of(sw), lh_addr_of(pixmap));
    lh_ui_color_init_hex(lh_addr_of(color), hex);
    lh_ui_canvas_sw_clear(lh_addr_of(sw), lh_addr_of(color));
}

lh_u32_t
pixel_at(const lh_ui_pixmap_t *pixmap, int x, int y)
{
    const lh_u32_t *row = reinterpret_cast<const lh_u32_t *>(pixmap->bits + y * pixmap->stride);

    return row[x];
}

/** Frame @p part of @p area onto @p screen, through the canvas, so the clip
 *  bookkeeping under test is the one a view runs. */
void
present_part_of(lh_ui_surface_t *screen, lh_ui_size_t size, const lh_ui_rect_t *area,
                const lh_ui_rect_t *part, lh_u32_t hex)
{
    lh_os_render_backend_gdi_context_t gdi;
    lh_ui_canvas_t canvas;
    lh_ui_point_t zero;
    lh_ui_color_t ink;

    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    lh_os_render_backend_gdi_context_set_hdc(lh_addr_of(gdi), lh_os_system_surface_get_dc(screen->handle));
    lh_ui_canvas_init(lh_addr_of(canvas), &lh_os_render_backend_gdi, lh_addr_of(gdi));
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_color_init_hex(lh_addr_of(ink), hex);
    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));

    lh_ui_canvas_begin_area(lh_addr_of(canvas), area);
    if (!lh_null_eq(part))
    {
        lh_ui_canvas_push(lh_addr_of(canvas), zero, part);
    }
    lh_ui_canvas_fill_rect(lh_addr_of(canvas), part != nullptr ? part : area, lh_addr_of(ink));
    if (!lh_null_eq(part))
    {
        lh_ui_canvas_pop(lh_addr_of(canvas));
    }
    lh_ui_canvas_end(lh_addr_of(canvas));
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

/** Frame two cuts of @p area onto @p screen, through the canvas, the way a
 *  strip carries two independent widgets. Nothing in between is drawn. */
void
present_parts_of(lh_ui_surface_t *screen, lh_ui_size_t size, const lh_ui_rect_t *area,
                 const lh_ui_rect_t *first, const lh_ui_rect_t *second, lh_u32_t hex)
{
    lh_os_render_backend_gdi_context_t gdi;
    lh_ui_canvas_t canvas;
    lh_ui_point_t zero;
    lh_ui_color_t ink;
    const lh_ui_rect_t *parts[2];
    lh_usize_t i;

    lh_os_render_backend_gdi_context_init(lh_addr_of(gdi));
    lh_os_render_backend_gdi_context_set_hdc(lh_addr_of(gdi), lh_os_system_surface_get_dc(screen->handle));
    lh_ui_canvas_init(lh_addr_of(canvas), &lh_os_render_backend_gdi, lh_addr_of(gdi));
    lh_ui_canvas_set_size(lh_addr_of(canvas), size);
    lh_ui_color_init_hex(lh_addr_of(ink), hex);
    lh_ui_point_init(lh_addr_of(zero), lh_ui_scalar(0), lh_ui_scalar(0));

    parts[0] = first;
    parts[1] = second;
    lh_ui_canvas_begin_area(lh_addr_of(canvas), area);
    for (i = 0u; i < 2u; ++i)
    {
        lh_ui_canvas_push(lh_addr_of(canvas), zero, parts[i]);
        lh_ui_canvas_fill_rect(lh_addr_of(canvas), parts[i], lh_addr_of(ink));
        lh_ui_canvas_pop(lh_addr_of(canvas));
    }
    lh_ui_canvas_end(lh_addr_of(canvas));
    lh_os_render_backend_gdi_context_deinit(lh_addr_of(gdi));
}

TEST(os_render_backend_gdi, a_present_carries_only_the_pixels_the_frame_drew)
{
    lh_ui_surface_t screen;
    lh_ui_pixmap_t read_back;
    lh_ui_size_t size;
    lh_ui_rect_t area;
    lh_ui_rect_t part;
    lh_ui_color_t red;
    lh_ui_color_t blue;
    int left_red = 0;
    int inked = 0;

    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(40), lh_ui_scalar(32));
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(40), lh_ui_scalar(32));
    /* Rows 8..17 of 40, the way a scrollbar thumb damages its own band and no
       more. */
    lh_ui_rect_init(lh_addr_of(part), lh_ui_scalar(0), lh_ui_scalar(8), lh_ui_scalar(40), lh_ui_scalar(10));
    lh_ui_color_init_hex(lh_addr_of(red), red_hex);
    lh_ui_color_init_hex(lh_addr_of(blue), blue_hex);

    lh_ui_surface_init(lh_addr_of(screen));
    ASSERT_EQ(lh_ui_surface_set_size(lh_addr_of(screen), size), lh_bool_true);
    fill_surface(lh_addr_of(screen), red_hex);

    present_part_of(lh_addr_of(screen), size, lh_addr_of(area), lh_addr_of(part), blue_hex);

    ASSERT_TRUE(lh_ui_surface_get_pixmap(lh_addr_of(screen), lh_addr_of(read_back)));
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 40; ++x)
        {
            const lh_u32_t argb = pixel_at(lh_addr_of(read_back), x, y);
            if (y >= 8 && y < 18)
            {
                inked += argb == lh_ui_color_get_argb(lh_addr_of(blue)) ? 1 : 0;
            }
            else
            {
                left_red += argb == lh_ui_color_get_argb(lh_addr_of(red)) ? 1 : 0;
            }
        }
    }
    EXPECT_EQ(inked, 40 * 10);
    EXPECT_EQ(left_red, 40 * 22);
}

TEST(os_render_backend_gdi, a_frame_that_was_never_cut_presents_its_whole_surface)
{
    lh_ui_surface_t screen;
    lh_ui_pixmap_t read_back;
    lh_ui_size_t size;
    lh_ui_rect_t area;
    lh_ui_color_t red;
    lh_ui_color_t blue;
    int inked = 0;
    int left_red = 0;

    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(40), lh_ui_scalar(32));
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(40), lh_ui_scalar(32));
    lh_ui_color_init_hex(lh_addr_of(red), red_hex);
    lh_ui_color_init_hex(lh_addr_of(blue), blue_hex);

    lh_ui_surface_init(lh_addr_of(screen));
    ASSERT_EQ(lh_ui_surface_set_size(lh_addr_of(screen), size), lh_bool_true);
    fill_surface(lh_addr_of(screen), red_hex);

    present_part_of(lh_addr_of(screen), size, lh_addr_of(area), lh_null, blue_hex);

    ASSERT_TRUE(lh_ui_surface_get_pixmap(lh_addr_of(screen), lh_addr_of(read_back)));
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 40; ++x)
        {
            const lh_u32_t argb = pixel_at(lh_addr_of(read_back), x, y);
            inked += argb == lh_ui_color_get_argb(lh_addr_of(blue)) ? 1 : 0;
            left_red += argb == lh_ui_color_get_argb(lh_addr_of(red)) ? 1 : 0;
        }
    }
    EXPECT_EQ(inked, 40 * 32);
    EXPECT_EQ(left_red, 0);
}

/* Two widgets in one strip are two cuts, and the picture between them belongs to
   neither. A hull of the two is the cheapest thing to keep — and it is a lie
   about what was drawn: the fourteen columns between the cuts hold bytes of a
   DIB this frame never wrote, so presenting the hull puts a strip's leftover
   picture on the screen in the gap. The demo draws two buttons 6 px apart, which
   is 168 pixels of gap on the very frames a pointer move produces. */
TEST(os_render_backend_gdi, a_present_leaves_the_gap_between_two_cuts_alone)
{
    lh_ui_surface_t screen;
    lh_ui_pixmap_t read_back;
    lh_ui_size_t size;
    lh_ui_rect_t area;
    lh_ui_rect_t left;
    lh_ui_rect_t right;
    lh_ui_color_t green;
    lh_ui_color_t blue;
    int inked = 0;
    int kept = 0;

    lh_ui_size_init(lh_addr_of(size), lh_ui_scalar(40), lh_ui_scalar(32));
    lh_ui_rect_init(lh_addr_of(area), lh_ui_scalar(0), lh_ui_scalar(0), lh_ui_scalar(40), lh_ui_scalar(32));
    lh_ui_rect_init(lh_addr_of(left), lh_ui_scalar(2), lh_ui_scalar(8), lh_ui_scalar(8), lh_ui_scalar(10));
    lh_ui_rect_init(lh_addr_of(right), lh_ui_scalar(24), lh_ui_scalar(8), lh_ui_scalar(8), lh_ui_scalar(10));
    lh_ui_color_init_hex(lh_addr_of(green), green_hex);
    lh_ui_color_init_hex(lh_addr_of(blue), blue_hex);

    lh_ui_surface_init(lh_addr_of(screen));
    ASSERT_EQ(lh_ui_surface_set_size(lh_addr_of(screen), size), lh_bool_true);
    fill_surface(lh_addr_of(screen), green_hex);

    present_parts_of(lh_addr_of(screen), size, lh_addr_of(area), lh_addr_of(left), lh_addr_of(right), blue_hex);

    ASSERT_TRUE(lh_ui_surface_get_pixmap(lh_addr_of(screen), lh_addr_of(read_back)));
    for (int y = 8; y < 18; ++y)
    {
        for (int x = 0; x < 40; ++x)
        {
            const lh_u32_t argb = pixel_at(lh_addr_of(read_back), x, y);
            if ((x >= 2 && x < 10) || (x >= 24 && x < 32))
            {
                inked += argb == lh_ui_color_get_argb(lh_addr_of(blue)) ? 1 : 0;
            }
            else
            {
                kept += argb == lh_ui_color_get_argb(lh_addr_of(green)) ? 1 : 0;
            }
        }
    }
    /* 8x10 twice drawn, and the 24 columns between and beside them still the
       screen's own green: 14 of gap, 2 of margin at the left, 8 at the right. */
    EXPECT_EQ(inked, 2 * 8 * 10);
    EXPECT_EQ(kept, 24 * 10);
}

} // namespace

#endif // LH_COMPILER_OS == LH_COMPILER_OS_WINDOWS