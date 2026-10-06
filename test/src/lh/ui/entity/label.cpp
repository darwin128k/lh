#include <gtest/gtest.h>

#include <lh/ui/canvas.h>
#include <lh/ui/color.h>
#include <lh/ui/entity/label.h>
#include <lh/ui/paint.h>
#include <lh/ui/rect.h>
#include <lh/ui/style.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{
/* Counts only the fill the base class owes: this rect in this color. Other
 * fill_rect calls (e.g. a label painting its text later) are not counted. */
struct base_fill_probe
{
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    int matches;
};

lh_void
probe_fill_rect(lh_ptr context, const lh_ui_rect_t *rect, const lh_ui_color_t *color)
{
    base_fill_probe *probe = lh_ptr_rcast(base_fill_probe, context);
    if (lh_ui_rect_eq(rect, lh_addr_of(probe->rect)) && lh_ui_color_equals(color, lh_addr_of(probe->color)))
    {
        ++probe->matches;
    }
}

const lh_ui_canvas_backend_t g_probe_backend = {nullptr, nullptr, nullptr, probe_fill_rect};
} // namespace

TEST(entity_label, init_keeps_the_rect_and_the_text_pointer)
{
    lh_ui_rect_t rect;

    lh_ui_rect_init(lh_addr_of(rect), 1, 2, 3, 4);
    const lh_char_t *text = "Hi";
    lh_ui_entity_label_t label;
    lh_ui_entity_label_init(lh_addr_of(label), rect, text);
    lh_ui_entity_t *entity = lh_ui_entity_label_as_entity(lh_addr_of(label));
    const lh_ui_rect_t stored = lh_ui_entity_get_rect(entity);

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
    EXPECT_EQ(lh_ui_entity_get_class(entity), lh_addr_of(lh_ui_entity_label_class));
}

TEST(entity_label, set_text_replaces_the_pointer)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_label_init(lh_addr_of(label), rect, "one");
    const lh_char_t *text = "two";

    lh_ui_entity_label_set_text(lh_addr_of(label), text);

    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
}

TEST(entity_label, draw_goes_through_the_embedded_entity)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 1, 1);
    lh_ui_entity_label_init(lh_addr_of(label), rect, "x");
    lh_ui_entity_draw(lh_ui_entity_label_as_entity(lh_addr_of(label)),
                      static_cast<lh_ui_canvas_t *>(nullptr));
}

/* Contract: a derived class keeps the base fill by calling
 * lh_ui_entity_class_event_base. The label class does. */
TEST(entity_label, draw_keeps_the_base_class_fill)
{
    lh_ui_entity_label_t label;
    lh_ui_rect_t rect;
    lh_ui_color_t color;
    lh_ui_paint_t paint;
    lh_ui_style_t style;
    lh_ui_canvas_t canvas;
    base_fill_probe probe{};

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 4, 4);
    lh_ui_color_init(lh_addr_of(color), 1, 2, 3, 255);
    lh_ui_paint_init_color(lh_addr_of(paint), lh_addr_of(color));
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_fill(lh_addr_of(style), lh_addr_of(paint));
    lh_ui_entity_label_init(lh_addr_of(label), rect, "x");
    lh_ui_entity_set_style(lh_ui_entity_label_as_entity(lh_addr_of(label)), lh_addr_of(style));
    probe.rect = rect;
    probe.color = color;
    lh_ui_canvas_init(lh_addr_of(canvas), lh_addr_of(g_probe_backend), lh_addr_of(probe));

    lh_ui_entity_draw(lh_ui_entity_label_as_entity(lh_addr_of(label)), lh_addr_of(canvas));

    EXPECT_EQ(probe.matches, 1);
}
