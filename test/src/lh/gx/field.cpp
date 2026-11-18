/**
 * @file field.cpp
 * @brief Tests for `lh/gx/field.h`.
 *
 * A field at (2,2) 30 by 8 on a black 40 by 12 screen, drawn with the tiny
 * font ("A" 3 wide, "Ж" 2 wide, a line 3 high): its view is 22 wide from x 6,
 * the text at row 5 on the screen. Typing goes into the application's buffer
 * by whole characters and stops at its capacity; the keys move and delete by
 * whole characters; a press puts the cursor at the nearest boundary; a long
 * text scrolls to keep the cursor in the view; the focus shows the caret.
 */

#include <gtest/gtest.h>

#include <lh/gx/backend/sw.h>
#include <lh/gx/box.h>
#include <lh/gx/field.h>
#include <lh/gx/focus.h>
#include <lh/gx/pointer.h>
#include <lh/gx/render.h>
#include <lh/test/gx/tiny_font.h>

#include <cstring>
#include <string>

namespace
{

constexpr lh_gx_color_t black = 0xFF000000U;
constexpr lh_gx_color_t white = 0xFFFFFFFFU;
constexpr lh_gx_color_t pale = 0xFFEEEEEEU;
constexpr lh_gx_color_t red = 0xFFFF0000U;
constexpr lh_gx_color_t blue = 0xFF0000FFU;

lh_void
count(lh_gx_field_t *, lh_ptr context)
{
    ++*static_cast<int *>(context);
}

lh_gx_point_t
point(lh_gx_scalar_t x, lh_gx_scalar_t y)
{
    lh_gx_point_t at;

    lh_gx_point_init(&at, x, y);
    return at;
}

bool
place_is(const lh_gx_entity_t *entity, lh_gx_scalar_t x, lh_gx_scalar_t y, lh_gx_scalar_t w,
         lh_gx_scalar_t h)
{
    lh_gx_rect_t want;

    lh_gx_rect_init(&want, x, y, w, h);
    return lh_gx_rect_eq(lh_gx_entity_get_place(entity), &want) == lh_bool_true;
}

struct Screen
{
    lh_test::gx_tiny_font tiny;
    lh_u32_t words[40 * 12] = {};
    lh_memory_bounds_t bits;
    lh_gx_pixmap_t pixmap;
    lh_gx_backend_sw_t sw;
    lh_gx_canvas_t canvas;
    lh_gx_render_t render;
    lh_gx_style_t back_style;
    lh_gx_style_t field_style;
    lh_gx_style_t focus_style;
    lh_gx_style_t caret_style;
    lh_gx_entity_t back;
    lh_gx_field_t field;
    lh_gx_pointer_t pointer;
    lh_gx_focus_t focus;
    lh_char_t buffer[16];
    int changes = 0;
    int submits = 0;

    explicit Screen(const char *text, lh_usize_t capacity = 16U)
    {
        std::strcpy(buffer, text);
        lh_memory_bounds_init(&bits, words, sizeof(words));
        lh_gx_pixmap_init(&pixmap, &bits, 40U, 12U, lh_gx_pixmap_format_argb8888);
        lh_gx_backend_sw_init(&sw, &pixmap, 0, 0);
        lh_gx_backend_sw_canvas_init(&canvas);
        lh_gx_render_init(&render, &canvas, &sw, 40, 12);
        lh_gx_style_init(&back_style, black, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&field_style, white, 0U, 0, 0, red, &tiny.font);
        lh_gx_style_init(&focus_style, pale, 0U, 0, 0, red, &tiny.font);
        lh_gx_style_init(&caret_style, blue, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 12);
        lh_gx_field_init(&field, &field_style, &focus_style, &caret_style, 2, 2, 30, 8, buffer,
                         capacity, count, count, &changes);
        lh_gx_field_set_on_submit(&field, count);
        lh_gx_field_set_context(&field, &changes);
        lh_gx_entity_attach(lh_gx_render_get_screen_mut(&render), &back);
        lh_gx_entity_attach(&back, lh_gx_field_get_box_mut(&field));
        lh_gx_pointer_init(&pointer);
        lh_gx_focus_init(&focus);
        draw();
    }

    lh_void
    draw()
    {
        lh_gx_damage_t drawn;

        lh_gx_render_draw(&render, &drawn);
    }

    lh_u32_t
    at(int x, int y) const
    {
        return words[y * 40 + x];
    }

    lh_void
    press(lh_gx_scalar_t x)
    {
        const lh_gx_point_t where = point(x, 5);

        lh_gx_pointer_press(&pointer, lh_gx_render_get_screen_mut(&render), &where);
        lh_gx_focus_press(&focus, lh_gx_pointer_get_captured(&pointer));
        lh_gx_pointer_release(&pointer, &where);
    }

    lh_void
    type(lh_u32_t code)
    {
        lh_gx_event_t event;

        lh_gx_event_init_text(&event, code);
        lh_gx_focus_take(&focus, &event);
    }

    lh_void
    key(lh_gx_key_t key)
    {
        lh_gx_event_t event;

        lh_gx_event_init_key(&event, key);
        lh_gx_focus_take(&focus, &event);
    }

    std::string
    text() const
    {
        return std::string(lh_gx_field_get_text(&field));
    }

    lh_usize_t
    cursor() const
    {
        return lh_gx_field_get_cursor(&field);
    }
};

TEST(gx_field, the_view_the_text_and_the_caret_are_laid_out)
{
    Screen screen("AA");

    EXPECT_EQ(lh_gx_field_get_length(&screen.field), 2U);
    EXPECT_EQ(screen.cursor(), 2U) << "at the end of what the buffer held";
    EXPECT_TRUE(place_is(lh_gx_field_get_view(&screen.field), 4, 0, 22, 8));
    EXPECT_TRUE(place_is(lh_gx_field_get_text_entity(&screen.field), 0, 3, 6, 3));
    EXPECT_TRUE(place_is(lh_gx_field_get_caret(&screen.field), 6, 3, 1, 3));
    EXPECT_FALSE(lh_gx_entity_is_visible(lh_gx_field_get_caret(&screen.field)));
    EXPECT_EQ(screen.at(6, 5), red) << "the first A";
    EXPECT_EQ(screen.at(3, 5), white) << "the padding";
}

TEST(gx_field, the_focus_shows_the_caret_and_the_focus_look)
{
    Screen screen("AA");

    screen.press(20);
    EXPECT_TRUE(lh_gx_field_is_focused(&screen.field));
    EXPECT_EQ(screen.cursor(), 2U) << "right of the text: its end";
    screen.draw();
    EXPECT_EQ(screen.at(12, 5), blue) << "the caret after the second A";
    EXPECT_EQ(screen.at(20, 5), pale) << "the focus look";
    lh_gx_focus_move(&screen.focus, nullptr);
    screen.draw();
    EXPECT_FALSE(lh_gx_field_is_focused(&screen.field));
    EXPECT_EQ(screen.at(12, 5), white);
}

TEST(gx_field, typing_inserts_at_the_cursor_and_calls_the_reaction)
{
    Screen screen("AA");

    screen.press(10); /* inside the second A, nearer its start */
    EXPECT_EQ(screen.cursor(), 1U);
    screen.type(0x416U);
    EXPECT_EQ(screen.text(), "A\xD0\x96"
                             "A");
    EXPECT_EQ(screen.cursor(), 3U) << "past both bytes";
    EXPECT_EQ(screen.changes, 1);
    screen.key(lh_gx_key_left);
    EXPECT_EQ(screen.cursor(), 1U) << "back over the whole character";
    screen.key(lh_gx_key_right);
    EXPECT_EQ(screen.cursor(), 3U);
    screen.key(lh_gx_key_backspace);
    EXPECT_EQ(screen.text(), "AA");
    EXPECT_EQ(screen.cursor(), 1U);
    screen.key(lh_gx_key_delete);
    EXPECT_EQ(screen.text(), "A");
    screen.key(lh_gx_key_delete);
    EXPECT_EQ(screen.text(), "A") << "nothing after the cursor";
    EXPECT_EQ(screen.changes, 3) << "a deletion that deletes nothing calls nothing";
    screen.key(lh_gx_key_home);
    EXPECT_EQ(screen.cursor(), 0U);
    screen.key(lh_gx_key_backspace);
    screen.key(lh_gx_key_end);
    EXPECT_EQ(screen.cursor(), 1U);
    screen.key(lh_gx_key_enter);
    EXPECT_EQ(screen.changes, 4) << "Enter calls on_submit, given the same context here";
}

TEST(gx_field, a_character_that_does_not_fit_is_refused_whole)
{
    Screen screen("AA", 4U);

    lh_gx_focus_move(&screen.focus, lh_gx_field_get_box_mut(&screen.field));
    screen.type(0x416U);
    EXPECT_EQ(screen.text(), "AA") << "two bytes, one left";
    screen.type(0x41U);
    EXPECT_EQ(screen.text(), "AAA");
    screen.type(0x41U);
    EXPECT_EQ(screen.text(), "AAA") << "full";
    EXPECT_EQ(screen.changes, 1);
    EXPECT_FALSE(lh_gx_field_insert(&screen.field, 0xD800U)) << "not a character";
}

TEST(gx_field, a_long_text_scrolls_to_keep_the_cursor_in_the_view)
{
    Screen screen("AAAAAAAAAA");

    /* 30 pixels of text in a 22-pixel view, the cursor at the end: 30 - 22 + 1. */
    EXPECT_EQ(lh_gx_field_get_scroll(&screen.field), 9);
    EXPECT_TRUE(place_is(lh_gx_field_get_text_entity(&screen.field), -9, 3, 30, 3));
    screen.draw();
    EXPECT_EQ(screen.at(4, 5), white) << "the text is cut to the view, not drawn on the padding";
    lh_gx_field_place_cursor(&screen.field, 0U);
    EXPECT_EQ(lh_gx_field_get_scroll(&screen.field), 0);
    lh_gx_field_set_text(&screen.field, "A");
    EXPECT_EQ(lh_gx_field_get_scroll(&screen.field), 0) << "a text that fits does not scroll";
}

TEST(gx_field, the_program_sets_whole_characters_only_and_calls_nothing)
{
    Screen screen("", 4U);

    lh_gx_field_set_text(&screen.field, "AA\xD0\x96\xD0\x96");
    EXPECT_EQ(screen.text(), "AA") << "three bytes would cut the Zhe";
    EXPECT_EQ(screen.cursor(), 2U);
    EXPECT_EQ(screen.changes, 0);
    EXPECT_EQ(lh_gx_field_from_entity(lh_gx_field_get_box_mut(&screen.field)), &screen.field);
}

} // namespace
