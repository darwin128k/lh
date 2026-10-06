#include <gtest/gtest.h>

#include <lh/ui/entity/label.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

TEST(entity_label, make_keeps_the_rect_and_the_text_pointer)
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
