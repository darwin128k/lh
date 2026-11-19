/**
 * @file present.cpp
 * @brief Tests for `lh/gx/present.h` and `lh/gx/input.h`.
 *
 * The present is a table with one slot; a frame (::lh_gx_present_draw) draws
 * what changed and shows exactly the rectangles drawn, and shows nothing when
 * nothing changed. The input brick sends a press to the pointer and moves the
 * focus, and a key to the focus.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/class/initializer.h>
#include <lh/gx/input.h>
#include <lh/gx/present.h>

#include <vector>

namespace
{

/* What a target saw: every rectangle shown, one call at a time. */
struct Seen
{
    int calls = 0;
    std::vector<lh_gx_rect_t> rects;
    const lh_gx_pixmap_t *pixels = nullptr;
};

lh_void
remember(lh_ptr context, const lh_gx_pixmap_t *pixels, lh_gx_point_t, const lh_gx_damage_t *drawn)
{
    auto *seen = static_cast<Seen *>(context);

    seen->calls++;
    seen->pixels = pixels;
    for (lh_u32_t i = 0U; i < lh_gx_damage_get_count(drawn); ++i)
    {
        seen->rects.push_back(*lh_gx_damage_get_rect(drawn, i));
    }
}

struct Screen
{
    lh_u32_t words[20 * 10] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_present_t present;
    lh_gx_style_t style;
    lh_gx_entity_t card;
    Seen seen;

    Screen()
    {
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 20U, 10U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 20, 10);
        lh_gx_present_init(&present);
        lh_gx_present_set_show(&present, remember);
        lh_gx_style_init(&style, 0xFFFF0000U, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&card, &lh_gx_box_class, &style, 2, 2, 4, 3);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &card);
    }

    bool
    frame()
    {
        return lh_gx_present_draw(&present, &seen, &render, &pixmap) == lh_bool_true;
    }
};

TEST(gx_present, a_new_table_is_empty_and_needs_show)
{
    lh_gx_present_t present;

    lh_gx_present_init(&present);
    EXPECT_TRUE(lh_gx_present_get_show(&present) == nullptr);
    EXPECT_FALSE(lh_gx_present_is_complete(&present));
    lh_gx_present_set_show(&present, remember);
    EXPECT_TRUE(lh_gx_present_is_complete(&present));
}

TEST(gx_present, a_frame_shows_what_it_drew_and_nothing_else)
{
    Screen screen;

    ASSERT_TRUE(screen.frame());
    EXPECT_EQ(screen.seen.calls, 1);
    EXPECT_EQ(screen.seen.pixels, &screen.pixmap);
    EXPECT_EQ(screen.words[2 * 20 + 2], 0xFFFF0000U) << "drawn before it was shown";

    EXPECT_FALSE(screen.frame()) << "nothing changed: nothing shown";
    EXPECT_EQ(screen.seen.calls, 1);

    lh_gx_entity_move(&screen.card, lh_gx_rect_t{});
    screen.seen.rects.clear();
    lh_gx_rect_t moved;

    lh_gx_rect_init(&moved, 10, 5, 4, 3);
    lh_gx_entity_move(&screen.card, moved);
    ASSERT_TRUE(screen.frame());
    lh_u64_t area = 0U;

    for (const lh_gx_rect_t &r : screen.seen.rects)
    {
        area += static_cast<lh_u64_t>(lh_gx_size_get_width(lh_gx_rect_get_size(&r)) *
                                      lh_gx_size_get_height(lh_gx_rect_get_size(&r)));
    }
    EXPECT_EQ(area, 2U * 4U * 3U) << "where the card was and where it is";
}

/* A field-like entity: it takes the pointer and the keys, and writes down the kinds. */
lh_void
note(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    static_cast<std::vector<lh_gx_event_kind_t> *>(lh_gx_entity_get_content(entity))
        ->push_back(lh_gx_event_get_kind(event));
}

const lh_gx_class_t field_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, note, note, lh_null);

TEST(gx_input, a_press_goes_to_the_pointer_and_the_focus_and_a_key_to_the_focus)
{
    std::vector<lh_gx_event_kind_t> heard;
    lh_gx_entity_t screen;

    lh_gx_entity_init(&screen, nullptr, nullptr, 0, 0, 20, 10);
    lh_gx_entity_t field;
    lh_gx_entity_init(&field, &field_class, nullptr, 0, 0, 10, 10);
    lh_gx_entity_set_content(&field, &heard);
    lh_gx_entity_attach(&screen, &field);
    lh_gx_pointer_t pointer;
    lh_gx_pointer_init(&pointer);
    lh_gx_focus_t focus;
    lh_gx_focus_init(&focus);

    lh_gx_point_t at;
    lh_gx_point_init(&at, 2, 2);
    lh_gx_event_t event;
    lh_gx_event_init(&event, lh_gx_event_kind_press, at);
    lh_gx_input_take(&pointer, &focus, &screen, &event);
    lh_gx_event_init_text(&event, 0x61U);
    lh_gx_input_take(&pointer, &focus, &screen, &event);
    lh_gx_event_init_kind(&event, lh_gx_event_kind_blur);
    lh_gx_input_take(&pointer, &focus, &screen, &event);
    EXPECT_EQ(lh_gx_focus_get_focused(&focus), &field);
    EXPECT_EQ(heard, (std::vector<lh_gx_event_kind_t>{
                         lh_gx_event_kind_press, lh_gx_event_kind_focus, lh_gx_event_kind_text}))
        << "a blur is the focus's own, not a source's";
}

TEST(gx_event, a_text_code_is_a_character_that_types)
{
    EXPECT_TRUE(lh_gx_event_is_text_code(0x61U));
    EXPECT_TRUE(lh_gx_event_is_text_code(0x1F600U));
    EXPECT_FALSE(lh_gx_event_is_text_code(0x0DU)) << "Enter is a key";
    EXPECT_FALSE(lh_gx_event_is_text_code(0x08U)) << "Backspace is a key";
    EXPECT_FALSE(lh_gx_event_is_text_code(0x7FU));
    EXPECT_FALSE(lh_gx_event_is_text_code(0xDC00U));
    EXPECT_FALSE(lh_gx_event_is_text_code(0x110000U));
}

} // namespace
