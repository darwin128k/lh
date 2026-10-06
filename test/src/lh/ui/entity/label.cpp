#include <gtest/gtest.h>

#include <lh/ui/entity/label.h>
#include <lh/ui/rect.h>
#include <lh/util/addr.h>

TEST(entity_label, make_keeps_the_rect_and_the_text_pointer)
{
    const lh_ui_rect_t rect = lh_ui_rect_make(1, 2, 3, 4);
    const lh_char_t *text = "Hi";
    lh_ui_entity_label_t label;
    lh_ui_entity_label_init(lh_addr_of(label), rect, text);
    const lh_ui_rect_t stored = lh_ui_entity_label_get_rect(lh_addr_of(label));

    EXPECT_EQ(lh_ui_rect_eq(lh_addr_of(rect), lh_addr_of(stored)), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
    EXPECT_EQ(lh_ui_entity_get_class(lh_addr_of(label.entity)), lh_addr_of(lh_ui_entity_label_class));
}

TEST(entity_label, set_text_replaces_the_pointer)
{
    lh_ui_entity_label_t label;
    lh_ui_entity_label_init(lh_addr_of(label), lh_ui_rect_make(0, 0, 1, 1), "one");
    const lh_char_t *text = "two";

    lh_ui_entity_label_set_text(lh_addr_of(label), text);

    EXPECT_EQ(lh_ui_entity_label_get_text(lh_addr_of(label)), text);
}
