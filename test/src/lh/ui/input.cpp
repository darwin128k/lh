/**
 * @file input.cpp
 * @brief Tests for `lh/ui/input.h`, and the two text calls it is built on.
 *
 * The tests are about **rules**, not about the calls: a field that steps back one
 * byte over a two-byte character has put the caret inside it, and every assertion
 * here that could have caught that is about the boundary rather than about the
 * number of bytes.
 */

#include <gtest/gtest.h>

#include <lh/test/ui/tiny_font.h>
#include <lh/key.h>
#include <lh/null.h>
#include <lh/ui/entity.h>
#include <lh/ui/font.h>
#include <lh/ui/input.h>
#include <lh/ui/key.h>
#include <lh/ui/label.h>
#include <lh/ui/style.h>
#include <lh/ui/text.h>
#include <lh/util/addr.h>
#include <lh/util/ptr.h>

namespace
{
using lh_test::cap_font;
using lh_test::tiny_font;

/** A two-byte code point: U+041A, the first letter of a Russian word. */
constexpr lh_u32_t k_cyrillic = 0x041AU;

/** A field with a buffer the test owns, and a style that gives it a font. */
struct field
{
    lh_ui_input_t input;
    lh_ui_style_t style;
    lh_char_t buffer[32];
    lh_ui_rect_t rect;
};

void
field_init(field *self, lh_u16_t capacity)
{
    lh_ui_rect_init(lh_addr_of(self->rect), 0, 0, 200, 24);
    lh_ui_input_init(lh_addr_of(self->input), self->rect, self->buffer, capacity);
    lh_ui_style_init(lh_addr_of(self->style));
    lh_ui_style_set_font(lh_addr_of(self->style), tiny_font());
    lh_ui_entity_set_style(lh_ui_input_as_entity(lh_addr_of(self->input)), lh_addr_of(self->style));
}

/** Type @p code as the keyboard would, into a field that has the focus. */
bool
type(lh_ui_input_t *self, lh_u32_t code)
{
    lh_ui_entity_event_t event;
    lh_ui_key_input_t key;

    lh_ui_key_input_init_text(lh_addr_of(key), code);
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_key, lh_addr_of(key));
    return lh_ui_input_on_key(self, lh_addr_of(event)) == lh_bool_true;
}

/** Press @p key on a field that has the focus. */
bool
press_key(lh_ui_input_t *self, lh_key_t key_id)
{
    lh_ui_entity_event_t event;
    lh_ui_key_input_t key;

    lh_ui_key_input_init_key(lh_addr_of(key), key_id, lh_bool_true);
    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_key, lh_addr_of(key));
    return lh_ui_input_on_key(self, lh_addr_of(event)) == lh_bool_true;
}

void
give_focus(lh_ui_input_t *self)
{
    lh_ui_entity_event_t event;

    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_focus, lh_null);
    lh_ui_input_on_focus(self, lh_addr_of(event));
}

/** Put the pointer down at @p at, the way a view would. */
bool
press_at(lh_ui_input_t *self, lh_ui_point_t at)
{
    lh_ui_entity_event_t event;

    lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_press, lh_addr_of(at));
    return lh_ui_input_on_press(self, lh_addr_of(event)) == lh_bool_true;
}

} // namespace

/* в”Ђв”Ђ Lifetime в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

TEST(entity_input, init_keeps_the_buffer_and_starts_empty)
{
    field f;
    field_init(lh_addr_of(f), 16);

    EXPECT_EQ(lh_ui_input_get_text(lh_addr_of(f.input)), f.buffer);
    EXPECT_EQ(f.buffer[0], '\0');
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
    EXPECT_EQ(lh_ui_input_is_focused(lh_addr_of(f.input)), lh_bool_false);
    /* The label the field is draws the same bytes: the text pointer is the buffer,
       which is what makes typing visible without a call to say so. */
    EXPECT_EQ(lh_ui_label_get_text(lh_ui_input_as_label(lh_addr_of(f.input))), f.buffer);
    EXPECT_EQ(lh_ui_entity_get_class(lh_ui_input_as_entity(lh_addr_of(f.input))),
              lh_addr_of(lh_ui_input_class));
}

TEST(entity_input, a_field_is_found_as_a_label_and_a_label_is_not_a_field)
{
    field f;
    lh_ui_label_t label;
    lh_ui_rect_t rect;

    field_init(lh_addr_of(f), 16);
    EXPECT_EQ(lh_ui_entity_as_input(lh_ui_input_as_entity(lh_addr_of(f.input))), lh_addr_of(f.input));

    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 10, 10);
    lh_ui_label_init(lh_addr_of(label), rect, lh_null);
    EXPECT_EQ(lh_ui_entity_as_input(lh_ui_label_as_entity(lh_addr_of(label))), static_cast<lh_ui_input_t *>(lh_null));
}

/* в”Ђв”Ђ Text and editing в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

TEST(entity_input, insert_at_the_end_appends_and_steps_over)
{
    field f;
    field_init(lh_addr_of(f), 16);

    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'A'), lh_bool_true);
    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'B'), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "AB");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
}

TEST(entity_input, insert_in_the_middle_shifts_the_rest_and_leaves_the_caret_after_it)
{
    field f;
    field_init(lh_addr_of(f), 16);

    lh_ui_input_set_text(lh_addr_of(f.input), "AC");
    EXPECT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 1), lh_bool_true);
    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'B'), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "ABC");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
}

TEST(entity_input, a_full_buffer_refuses_the_character_and_writes_nothing)
{
    field f;
    field_init(lh_addr_of(f), 3);

    lh_ui_input_insert(lh_addr_of(f.input), 'A');
    lh_ui_input_insert(lh_addr_of(f.input), 'B');
    lh_ui_input_insert(lh_addr_of(f.input), 'C');
    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'D'), lh_bool_false);
    /* Not one byte of a refused character: a field with no room must not end in the
       start of the character the person just typed. */
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "ABC");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 3);
}

TEST(entity_input, backspace_at_the_start_refuses_and_delete_at_the_end_refuses)
{
    field f;
    field_init(lh_addr_of(f), 16);

    EXPECT_EQ(lh_ui_input_backspace(lh_addr_of(f.input)), lh_bool_false);
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_false);

    lh_ui_input_set_text(lh_addr_of(f.input), "AB");
    /* The caret is at the end after a set_text, so the first delete here is a delete
       at the end and is refused -- the same way the one on an empty field is. Go back
       to the start before asking for the one that is supposed to work. */
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_false);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "AB");
    ASSERT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 0), lh_bool_true);

    /* Forward deletes the character **under** the caret and leaves the caret where it
       is, so the same caret walks the text from "AB" to "B" to "" and then there is
       nothing left to ask about. */
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "B");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_false);
}

TEST(entity_input, backspace_removes_a_whole_character_not_one_byte_of_it)
{
    field f;
    field_init(lh_addr_of(f), 16);

    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), k_cyrillic), lh_bool_true);
    ASSERT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
    EXPECT_EQ(lh_ui_input_backspace(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
}

TEST(entity_input, a_caret_inside_a_character_is_refused_and_the_caret_stays)
{
    field f;
    field_init(lh_addr_of(f), 16);

    lh_ui_input_insert(lh_addr_of(f.input), k_cyrillic);
    ASSERT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
    /* Byte 1 is the second half of the character that starts at byte 0. A caret
       there is not a position in this text. */
    EXPECT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 1), lh_bool_false);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
    EXPECT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 3), lh_bool_false);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
    EXPECT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 0), lh_bool_true);
}

TEST(entity_input, set_text_that_does_not_fit_keeps_whole_characters)
{
    field f;
    lh_char_t text[8];

    /* Four bytes, and a string whose fourth byte is the **lead** byte of a two-byte
       character: a cut that only trimmed trailing continuation bytes would keep the
       first half of that character and call the text three bytes long. */
    field_init(lh_addr_of(f), 3);
    text[0] = 'A';
    text[1] = 'B';
    /* Encoded by the engine rather than spelled out in the test: U+041A is
       `D0 9A`, and writing it as `code >> 8` and `code & 0xFF` gives `04 1A`,
       which is not the character at all. A constant in a test that you remembered is
       not a reference. */
    ASSERT_EQ(lh_ui_text_encode_code(&text[2], 4U, k_cyrillic), 2U);
    text[4] = '\0';

    EXPECT_EQ(lh_ui_input_set_text(lh_addr_of(f.input), text), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "AB");
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 2);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
}

TEST(entity_input, a_three_byte_character_deletes_as_a_whole_and_not_as_two_bytes)
{
    field f;
    /* U+20AC, the euro sign: a three-byte character. Its lead byte is `0xE0`, and
       `0xE0` **as a code point** is below `0x800` -- so a walk over the buffer that
       asked `lh_ui_text_encoded_size` what a code point takes would answer two, cut
       two of the three, and leave the third behind to be read as the next character.
       Every Cyrillic letter agrees with that wrong answer, which is why a suite
       written with Cyrillic in it stays green over it. */
    constexpr lh_u32_t k_three = 0x20ACU;

    field_init(lh_addr_of(f), 16);

    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'A'), lh_bool_true);
    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), k_three), lh_bool_true);
    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'B'), lh_bool_true);
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 5);

    /* 'A' is byte 0, the euro is bytes 1..3, 'B' is byte 4. Two deletes from the
       start: the first takes the 'A', the second has to take all **three** bytes of
       the euro or it leaves one behind, which is the next character as far as anything
       reading this string is concerned. */
    ASSERT_EQ(lh_ui_input_set_caret(lh_addr_of(f.input), 0), lh_bool_true);
    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "\xE2\x82\xAC" "B");
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 4);

    EXPECT_EQ(lh_ui_input_delete_forward(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "B");
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 1);

    /* And the caret never lands inside one either: after 'A' is the second byte of
       the character, and the byte after the whole of it is where the arrow goes. */
    give_focus(lh_addr_of(f.input));
    lh_ui_input_set_text(lh_addr_of(f.input), "A");
    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), k_three), lh_bool_true);
    ASSERT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 4);

    lh_ui_input_home(lh_addr_of(f.input));
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 1);
    /* One step over three bytes: the arrow moves over a character, and a character is
       three bytes here. A step of two would land inside it -- a caret the text can
       still not be typed at. */
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 4);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), false);
}

TEST(entity_input, a_four_byte_character_is_a_character_and_not_a_pair_of_surrogates)
{
    field f;
    /* U+1F600: four bytes, lead `0xF0`. */
    constexpr lh_u32_t k_four = 0x1F600U;

    field_init(lh_addr_of(f), 16);

    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), k_four), lh_bool_true);
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 4);
    /* A surrogate half is not a character, and one of them in a buffer is a string
       that walks off the end of itself. */
    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 0xD83DU), lh_bool_false);
    EXPECT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 0xDFFFU), lh_bool_false);
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 4);

    EXPECT_EQ(lh_ui_input_backspace(lh_addr_of(f.input)), lh_bool_true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");
}

TEST(entity_input, set_text_puts_the_caret_at_the_end_and_clear_puts_it_at_the_start)
{
    field f;
    field_init(lh_addr_of(f), 16);

    lh_ui_input_set_text(lh_addr_of(f.input), "AB");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);
    lh_ui_input_clear(lh_addr_of(f.input));
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
}

/* в”Ђв”Ђ Keys в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

TEST(entity_input, a_typed_character_is_inserted)
{
    field f;
    field_init(lh_addr_of(f), 16);
    give_focus(lh_addr_of(f.input));

    EXPECT_EQ(type(lh_addr_of(f.input), 'A'), true);
    EXPECT_EQ(type(lh_addr_of(f.input), 'B'), true);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "AB");
}

TEST(entity_input, backspace_and_the_arrows_move_over_a_whole_character)
{
    field f;
    field_init(lh_addr_of(f), 16);
    give_focus(lh_addr_of(f.input));

    type(lh_addr_of(f.input), 'A');
    type(lh_addr_of(f.input), k_cyrillic);
    type(lh_addr_of(f.input), 'B');
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 4);

    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_left), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 3); /* before 'B' */
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_left), true);
    /* One step, not two: the character before byte 3 is two bytes long and the arrow
       steps over a character, which is the only unit the text is written in. */
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 1);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 3);

    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_home), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_left), false);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 1);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_end), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 4);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_right), false);
}

TEST(entity_input, a_key_the_field_does_not_own_is_refused)
{
    field f;
    field_init(lh_addr_of(f), 16);
    give_focus(lh_addr_of(f.input));

    /* Enter, Tab and Escape are the form's: what a field does with them is a question
       about what is around it. Answering "yes" here would swallow the key from the
       application that is supposed to act on it. */
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_enter), false);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_tab), false);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_escape), false);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");
}

TEST(entity_input, a_field_is_focusable_because_a_field_nobody_can_reach_is_not_a_field)
{
    field f;
    field_init(lh_addr_of(f), 16);

    /* The walk a press does: find_focusable of whatever the pointer landed on. This is
       the whole of "clicking into a field" -- there is no other path to the focus --
       and it is also the path Tab takes. A field that says no would be a field that
       cannot be clicked into, cannot be tabbed to and never receives a key, with every
       setter in this header working perfectly on a widget nobody can reach. */
    EXPECT_EQ(lh_ui_entity_is_focusable(lh_ui_input_as_entity(lh_addr_of(f.input))), lh_bool_true);
    EXPECT_EQ(lh_ui_entity_find_focusable(lh_ui_input_as_entity(lh_addr_of(f.input))),
              lh_ui_input_as_entity(lh_addr_of(f.input)));
}

TEST(entity_input, keys_before_the_focus_do_nothing)
{
    field f;
    field_init(lh_addr_of(f), 16);

    EXPECT_EQ(type(lh_addr_of(f.input), 'A'), false);
    EXPECT_EQ(press_key(lh_addr_of(f.input), lh_key_backspace), false);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "");

    give_focus(lh_addr_of(f.input));
    EXPECT_EQ(type(lh_addr_of(f.input), 'A'), true);

    {
        lh_ui_entity_event_t event;

        lh_ui_entity_event_init(lh_addr_of(event), lh_ui_entity_event_defocus, lh_null);
        lh_ui_input_on_focus(lh_addr_of(f.input), lh_addr_of(event));
    }
    EXPECT_EQ(lh_ui_input_is_focused(lh_addr_of(f.input)), lh_bool_false);
    EXPECT_EQ(type(lh_addr_of(f.input), 'B'), false);
    EXPECT_STREQ(lh_ui_input_get_text(lh_addr_of(f.input)), "A");
}

/* в”Ђв”Ђ The caret в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

TEST(entity_input, the_caret_sits_where_the_text_before_it_ends)
{
    field f;
    field_init(lh_addr_of(f), 16);
    lh_ui_rect_t caret;

    lh_ui_input_set_text(lh_addr_of(f.input), "AB");
    lh_ui_input_set_caret(lh_addr_of(f.input), 1);
    caret = lh_ui_input_get_caret_rect(lh_addr_of(f.input));

    /* The tiny font gives 'A' an advance of 3 and the field no padding, so the caret
       after one character is three columns from the text's own corner. */
    EXPECT_EQ(lh_ui_rect_get_origin_as_const(lh_addr_of(caret))->x, 3);
    EXPECT_EQ(lh_ui_size_get_width(lh_ui_rect_get_size_as_const(lh_addr_of(caret))), 1);
}

TEST(entity_input, the_caret_of_an_empty_field_still_has_a_height)
{
    field f;
    lh_ui_rect_t with_text;
    lh_ui_rect_t empty;
    lh_ui_style_t style;
    lh_ui_rect_t rect;

    /* The cap font, because the tiny font's line and its ink are both 2 rows and
       cannot tell the two heights apart -- which is the whole reason that font
       cannot be trusted with a rule about height. */
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 200, 24);
    lh_ui_input_init(lh_addr_of(f.input), rect, f.buffer, 16);
    lh_ui_style_init(lh_addr_of(style));
    lh_ui_style_set_font(lh_addr_of(style), cap_font());
    lh_ui_entity_set_style(lh_ui_input_as_entity(lh_addr_of(f.input)), lh_addr_of(style));

    lh_ui_input_set_text(lh_addr_of(f.input), "d");
    with_text = lh_ui_input_get_caret_rect(lh_addr_of(f.input));
    lh_ui_input_clear(lh_addr_of(f.input));
    empty = lh_ui_input_get_caret_rect(lh_addr_of(f.input));

    /* 'd' rises five rows; an empty field has no ink at all and stands on the line box
       of eight. A caret whose height came from the text would be **no caret at all**
       in an empty field, which is where a field starts. */
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(with_text))), 5);
    EXPECT_EQ(lh_ui_size_get_height(lh_ui_rect_get_size_as_const(lh_addr_of(empty))), 8);
}

TEST(entity_input, a_field_with_no_font_has_no_caret_rect)
{
    field f;
    lh_ui_rect_t rect;
    lh_ui_rect_t caret;

    /* No style at all, so no font: there is nothing to measure the text with, and a
       caret at (0, 0) would be a mark in the corner of every field that has not been
       given a look yet. */
    lh_ui_rect_init(lh_addr_of(rect), 0, 0, 200, 24);
    lh_ui_input_init(lh_addr_of(f.input), rect, f.buffer, 16);
    lh_ui_input_set_text(lh_addr_of(f.input), "A");

    caret = lh_ui_input_get_caret_rect(lh_addr_of(f.input));
    EXPECT_EQ(lh_ui_rect_is_empty(lh_addr_of(caret)), lh_bool_true);
}

/* в”Ђв”Ђ The text calls this is built on в”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђв”Ђ */

TEST(entity_input, a_press_puts_the_caret_where_the_pointer_is)
{
    field f;
    lh_ui_point_t at;

    field_init(lh_addr_of(f), 16);
    give_focus(lh_addr_of(f.input));
    ASSERT_EQ(lh_ui_input_set_text(lh_addr_of(f.input), "ABC"), lh_bool_true);

    /* The field is at 0,0 with no padding, and the tiny font gives A, B and C
       advances of 3, 4 and 2. The pointer is in **pixels** and the caret is in
       **bytes**, and this test is where those two are told apart: press at x 4 is
       over the 'B', so the caret goes on the near side of it -- byte 1, because 'B'
       is the second byte of the text and every character in "ABC" is one byte long.
       The first version compared against where the text starts, which is a constant,
       so every press landed on the same byte and a click in a field did nothing. */
    lh_ui_point_init(lh_addr_of(at), 4, 2);
    EXPECT_EQ(press_at(lh_addr_of(f.input), at), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 1);

    /* x 7 is the left edge of the 'C' and x 8 is inside it: both belong before it. */
    lh_ui_point_init(lh_addr_of(at), 7, 2);
    EXPECT_EQ(press_at(lh_addr_of(f.input), at), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 2);

    /* x 9 is past the end of the text: the end of the text, and not past it. */
    lh_ui_point_init(lh_addr_of(at), 190, 2);
    EXPECT_EQ(press_at(lh_addr_of(f.input), at), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 3);
    EXPECT_EQ(lh_ui_input_get_length(lh_addr_of(f.input)), 3);

    /* And a press to the left of the first character is the start. */
    lh_ui_point_init(lh_addr_of(at), 0, 2);
    EXPECT_EQ(press_at(lh_addr_of(f.input), at), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 0);
}

TEST(entity_input, a_press_counts_a_multi_byte_character_as_the_one_step_it_is)
{
    field f;
    lh_ui_point_t at;

    field_init(lh_addr_of(f), 16);
    give_focus(lh_addr_of(f.input));
    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), 'A'), lh_bool_true);
    ASSERT_EQ(lh_ui_input_insert(lh_addr_of(f.input), k_cyrillic), lh_bool_true);

    /* Three bytes of text, and the tiny font has no glyph for the Cyrillic letter --
       so it takes **no room** and the whole text is three columns wide. A press past
       those three columns is the end of the text, which is byte 3, not byte 1: the
       caret is a position in the bytes, and the pixels are only how it was found. */
    lh_ui_point_init(lh_addr_of(at), 100, 2);
    EXPECT_EQ(press_at(lh_addr_of(f.input), at), true);
    EXPECT_EQ(lh_ui_input_get_caret(lh_addr_of(f.input)), 3);
}

TEST(ui_text, a_code_that_is_not_encodable_is_refused)
{
    lh_char_t out[4];

    EXPECT_EQ(lh_ui_text_encoded_size(0), 0U);                    /* the terminator */
    EXPECT_EQ(lh_ui_text_encoded_size(0xD800U), 0U);             /* a surrogate half */
    EXPECT_EQ(lh_ui_text_encoded_size(0xDFFFU), 0U);
    EXPECT_EQ(lh_ui_text_encoded_size(0x110000U), 0U);           /* above the last one */
    EXPECT_EQ(lh_ui_text_encode_code(out, 4U, 0xD800U), 0U);
    /* A code point that does not fit is refused whole, never half of one. */
    EXPECT_EQ(lh_ui_text_encode_code(out, 1U, k_cyrillic), 0U);
    EXPECT_EQ(lh_ui_text_encoded_size(k_cyrillic), 2U);
    EXPECT_EQ(lh_ui_text_encoded_size(0x41U), 1U);
    EXPECT_EQ(lh_ui_text_encoded_size(0x800U), 3U);
    EXPECT_EQ(lh_ui_text_encoded_size(0x10000U), 4U);
}

TEST(ui_text, the_size_of_a_character_is_read_off_its_lead_byte_and_not_off_its_code)
{
    /* The two questions, side by side, because answering one with the other is the
       bug: `0xD0` and `0xE0` are the lead bytes of a two-byte and a three-byte
       character, and **as code points both are below 0x800**, so the wrong function
       says 2 for both. It is right about the Cyrillic one and wrong about the euro. */
    EXPECT_EQ(lh_ui_text_lead_size(0x41U), 1U);   /* 'A' */
    EXPECT_EQ(lh_ui_text_lead_size(0x7FU), 1U);   /* DEL, still one byte */
    EXPECT_EQ(lh_ui_text_lead_size(0xD0U), 2U);   /* U+041A */
    EXPECT_EQ(lh_ui_text_lead_size(0xE0U), 3U);   /* U+20AC */
    EXPECT_EQ(lh_ui_text_lead_size(0xEDU), 3U);   /* U+D000 range, still three */
    EXPECT_EQ(lh_ui_text_lead_size(0xF0U), 4U);   /* U+1F600 */
    EXPECT_EQ(lh_ui_text_lead_size(0xF4U), 4U);

    /* A continuation byte, a stray byte above the range and a zero are one byte:
       the walk is one byte at a time over these, and skipping more would step over a
       character somebody can see. */
    EXPECT_EQ(lh_ui_text_lead_size(0x80U), 1U);
    EXPECT_EQ(lh_ui_text_lead_size(0xBFU), 1U);
    EXPECT_EQ(lh_ui_text_lead_size(0xF8U), 1U);
    EXPECT_EQ(lh_ui_text_lead_size(0x00U), 1U);

    /* And the honest disagreement, spelled out: what the two functions say about the
       three bytes that are not ASCII. */
    EXPECT_EQ(lh_ui_text_encoded_size(0xD0U), 2U); /* agrees by coincidence */
    EXPECT_EQ(lh_ui_text_encoded_size(0xE0U), 2U); /* **wrong**, and the reason */
    EXPECT_EQ(lh_ui_text_lead_size(0xE0U), 3U);    /* the answer */
}

TEST(ui_text, an_encoded_code_reads_back_as_the_same_code)
{
    const lh_u32_t codes[] = {0x41U, 0x7FU, 0x80U, 0x800U, 0xFFFFU, 0x10000U, 0x10FFFFU, k_cyrillic};

    for (const lh_u32_t code : codes)
    {
        lh_char_t out[8];
        const lh_char_t *cursor = out;
        const lh_u32_t written = lh_ui_text_encode_code(out, 8U, code);

        ASSERT_EQ(written, lh_ui_text_encoded_size(code));
        out[written] = '\0';
        EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), code);
        EXPECT_EQ(lh_ui_text_next_code(lh_addr_of(cursor)), 0U);
    }
}