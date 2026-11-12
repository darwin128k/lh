#include <gtest/gtest.h>

#include <lh/ui/display/backend.h>
#include <lh/ui/entity.h>
#include <lh/ui/style.h>
#include <lh/ui/view.h>

#include <cstring>
#include <vector>

namespace
{

constexpr int k_width = 64;
constexpr int k_height = 40;
constexpr int k_strip = 16;

/* What a panel would show: every present copies the buffer to the rows the
   display says the strip sits at. */
struct panel_record
{
    lh_ui_display_t *display = nullptr;
    lh_u32_t screen[k_width * k_height] = {};
    std::vector<int> strip_rows;
};

lh_void
panel_present(lh_ptr context, const lh_ui_pixmap_t *pixels, const lh_ui_rects_t *drawn)
{
    (void)drawn;
    panel_record *panel = static_cast<panel_record *>(context);
    const int top = static_cast<int>(lh_ui_display_get_strip_y(panel->display));
    panel->strip_rows.push_back(top);
    const int rows = static_cast<int>(lh_ui_pixmap_get_height(pixels));
    for (int y = 0; y < rows && top + y < k_height; ++y)
    {
        std::memcpy(&panel->screen[(top + y) * k_width],
                    lh_ui_pixmap_get_address(pixels, 0, y), sizeof(lh_u32_t) * k_width);
    }
}

struct scene
{
    lh_ui_color_t back_color;
    lh_ui_color_t box_color;
    lh_ui_paint_t back_paint;
    lh_ui_paint_t box_paint;
    lh_ui_style_t back_style;
    lh_ui_style_t box_style;
    lh_ui_entity_t root;
    lh_ui_entity_t box;
};

lh_ui_rect_t
rect_of(int x, int y, int w, int h)
{
    lh_ui_rect_t rect;
    lh_ui_rect_init(&rect, lh_ui_scalar_of(x), lh_ui_scalar_of(y), lh_ui_scalar_of(w), lh_ui_scalar_of(h));
    return rect;
}

/* A rounded box that crosses both strip borders, on a filled background. */
void
build_scene(scene &s)
{
    lh_ui_color_init(&s.back_color, 20, 40, 60, 255);
    lh_ui_color_init(&s.box_color, 230, 120, 30, 255);
    lh_ui_paint_init_color(&s.back_paint, &s.back_color);
    lh_ui_paint_init_color(&s.box_paint, &s.box_color);
    lh_ui_style_init(&s.back_style);
    lh_ui_style_assign_fill(&s.back_style, &s.back_paint);
    lh_ui_style_init(&s.box_style);
    lh_ui_style_assign_fill(&s.box_style, &s.box_paint);
    lh_ui_style_set_radius_v(&s.box_style, lh_ui_scalar(6));
    lh_ui_entity_init(&s.root, rect_of(0, 0, k_width, k_height));
    lh_ui_entity_set_style_v(&s.root, &s.back_style);
    lh_ui_entity_init(&s.box, rect_of(8, 6, 40, 28));
    lh_ui_entity_set_style_v(&s.box, &s.box_style);
    lh_ui_entity_add_child(&s.root, &s.box);
}

void
init_display(lh_ui_display_t *display, lh_u32_t *bits, int rows)
{
    lh_ui_display_init(display);
    lh_ui_size_t panel;
    lh_ui_size_init(&panel, lh_ui_scalar_of(k_width), lh_ui_scalar_of(k_height));
    lh_ui_display_set_panel_v(display, panel);
    lh_ui_display_set_format(display, lh_ui_pixmap_format_argb8888);
    lh_ui_display_set_strip_height(display, lh_ui_scalar_of(rows == k_height ? 0 : rows));
    lh_ui_display_set_buffer(display, reinterpret_cast<lh_byte_ptr>(bits),
                             static_cast<lh_s32_t>(sizeof(lh_u32_t) * k_width));
}

TEST(ui_display_backend, a_stripped_view_presents_each_strip_where_it_belongs)
{
    scene s;
    build_scene(s);

    /* The picture drawn whole, on the plain software backend. */
    static lh_u32_t whole_bits[k_width * k_height];
    lh_ui_display_t whole;
    init_display(&whole, whole_bits, k_height);
    lh_ui_view_t one;
    lh_ui_view_init(&one);
    lh_ui_view_set_canvas(&one, lh_ui_display_get_canvas_mut(&whole));
    lh_ui_view_set_root_v(&one, &s.root);
    lh_ui_view_paint(&one);

    /* The same picture through a 16-row buffer, presented strip by strip. */
    static lh_u32_t strip_bits[k_width * k_strip];
    lh_ui_display_t strips;
    init_display(&strips, strip_bits, k_strip);
    panel_record panel;
    panel.display = &strips;
    lh_ui_present_sink_t sink;
    lh_ui_present_sink_init(&sink, "panel", panel_present, &panel);
    lh_ui_display_set_sink(&strips, 0U, &sink);
    lh_ui_display_backend_bind(&strips);
    lh_ui_view_t many;
    lh_ui_view_init(&many);
    lh_ui_view_set_canvas(&many, lh_ui_display_get_canvas_mut(&strips));
    lh_ui_view_set_root_v(&many, &s.root);
    lh_ui_view_set_strip_height(&many, lh_ui_scalar_of(k_strip));
    lh_ui_view_paint(&many);

    /* 40 rows in 16: 0, 16, and the last one pulled up to a full 16 at row 24. */
    EXPECT_EQ(panel.strip_rows, (std::vector<int>{0, 16, 24}));
    int differ = 0;
    for (int i = 0; i < k_width * k_height; ++i)
    {
        differ += panel.screen[i] != whole_bits[i];
    }
    EXPECT_EQ(differ, 0);
    lh_ui_view_deinit(&one);
    lh_ui_view_deinit(&many);
}

TEST(ui_display_backend, bind_keeps_the_canvas_size)
{
    static lh_u32_t bits[k_width * k_strip];
    lh_ui_display_t display;
    init_display(&display, bits, k_strip);
    lh_ui_display_backend_bind(&display);
    const lh_ui_size_t *size = lh_ui_canvas_get_size(lh_ui_display_get_canvas_mut(&display));
    EXPECT_EQ(lh_ui_size_get_width(size), lh_ui_scalar_of(k_width));
    EXPECT_EQ(lh_ui_size_get_height(size), lh_ui_scalar_of(k_height));
    EXPECT_EQ(lh_ui_canvas_get_backend(lh_ui_display_get_canvas_mut(&display)),
              &lh_ui_display_backend);
}

TEST(ui_display_backend, a_whole_frame_starts_at_row_zero_and_presents_once)
{
    static lh_u32_t bits[k_width * k_height];
    lh_ui_display_t display;
    init_display(&display, bits, k_height);
    panel_record panel;
    panel.display = &display;
    lh_ui_present_sink_t sink;
    lh_ui_present_sink_init(&sink, "panel", panel_present, &panel);
    lh_ui_display_set_sink(&display, 0U, &sink);
    lh_ui_display_set_strip_y(&display, lh_ui_scalar(9));
    lh_ui_display_backend_bind(&display);

    scene s;
    build_scene(s);
    lh_ui_view_t view;
    lh_ui_view_init(&view);
    lh_ui_view_set_canvas(&view, lh_ui_display_get_canvas_mut(&display));
    lh_ui_view_set_root_v(&view, &s.root);
    lh_ui_view_paint(&view);
    EXPECT_EQ(panel.strip_rows, (std::vector<int>{0}));
    lh_ui_view_deinit(&view);
}

} // namespace
