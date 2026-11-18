/* example/gx_web_button.c
 *
 * A gx screen in the browser: a button that counts its clicks, a checkbox
 * that lights the background and two radios that colour the button. The tree
 * is a background, a button (a box and its caption), a checkbox (a row, a
 * square, a tick and a caption) and two radios (each a checkbox of the radio
 * class, round by its style, of one group) and a slider that sizes the
 * button (a row, a rail, a fill and a knob) and a text field that names it
 * (a box, a view, a text and a caret), drawn
 * by the gx renderer through the web backend; the browser only plays the
 * frames and sends the pointer back (lh/gx/web.h).
 *
 * Run it, open http://127.0.0.1:8080/ and press the button: it darkens while
 * it is held, and a release over it clicks it. Tap the checkbox, or its
 * caption: the background turns light, and dark again. Choose "Green": the
 * button turns green, and "Blue" is unmarked. Drag the slider: the button
 * grows and shrinks around its centre as the knob follows the pointer. Click
 * the field and type: the arrows, Home, End, Backspace and Delete edit it, and
 * Enter puts its text on the button. Open a second tab: it
 * gets the whole screen, and both follow every change. The host runs on this thread
 * and does not return; stop the program with Ctrl+C.
 */

#include <stdio.h>
#include <stdlib.h>

#include <lh/core/runtime/allocator.h>
#include <lh/core/runtime/terminate.h>
#include <lh/core/memory/std.h>
#include <lh/core/str/format/uint.h>
#include <lh/core/util/str/ptr.h>
#include <lh/gx/box.h>
#include <lh/gx/button.h>
#include <lh/gx/checkbox.h>
#include <lh/gx/field.h>
#include <lh/gx/radio.h>
#include <lh/gx/slider.h>
#include <lh/gl/font.h>
#include <lh/gx/font/baked.h>
#include <lh/gx/style.h>
#include <lh/gx/web.h>
#include <lh/rail/runtime/base.h>
#include <lh/os/net.h>
#include <lh/os/rail/base.h>
#include <lh/os/web/host.h>

/* The example is a host: the core starts with every runtime slot empty, so it
   binds the C library before lh allocates anything. */
static lh_ptr
example_alloc(lh_self_ptr self, lh_usize_t size)
{
    (void)self;
    return malloc(size);
}

static lh_void
example_dealloc(lh_self_ptr self, lh_ptr ptr)
{
    (void)self;
    free(ptr);
}

static lh_void
example_abort(lh_self_ptr self)
{
    (void)self;
    abort();
}

/* The background, and the style the checkbox changes. */
static lh_gx_style_t back_style;
static lh_gx_entity_t back;

/* The button, and the style the radios change. */
static lh_gx_style_t button_style;
static lh_gx_button_t button;

/* The radios: which colour the button is. */
static lh_gx_radio_t blue;
static lh_gx_radio_t green;

/* The caption the button shows after a click: "Clicked N". */
static lh_char_t caption[32] = "Press me";

/* The button's reaction; it runs on the host's thread, where the tree lives. */
static lh_void
on_click(lh_gx_button_t *button, lh_ptr context)
{
    lh_uint_t *clicks = (lh_uint_t *)context;
    static const lh_char_t prefix[] = "Clicked ";
    lh_usize_t n = sizeof(prefix) - 1U;

    *clicks += 1U;
    for (lh_usize_t i = 0U; i < n; ++i)
    {
        caption[i] = prefix[i];
    }
    n += lh_str_ptr_format_uint(*clicks, caption + n, sizeof(caption) - n - 1U);
    caption[n] = '\0';
    lh_gx_button_set_text(button, caption);
}

/* The checkbox's reaction: a light background while it is checked. */
static lh_void
on_light(lh_gx_checkbox_t *checkbox, lh_bool_t checked, lh_ptr context)
{
    (void)checkbox;
    (void)context;
    lh_gx_style_set_fill(&back_style, checked ? 0xFFE8E8ECU : 0xFF2B2B30U);
    lh_gx_entity_invalidate(&back);
}

/* The group's reaction: the button in the colour chosen. */
static lh_void
on_colour(lh_gx_radio_group_t *group, lh_gx_radio_t *radio, lh_ptr context)
{
    (void)group;
    (void)context;
    lh_gx_style_set_fill(&button_style, radio == &green ? 0xFF2E9E5BU : 0xFF3D7BD9U);
    lh_gx_entity_invalidate(lh_gx_button_get_box_mut(&button));
}

/* The slider's reaction: the button @p value wide, centred where it was. */
static lh_void
on_width(lh_gx_slider_t *slider, lh_s32_t value, lh_ptr context)
{
    lh_gx_rect_t place;

    (void)slider;
    (void)context;
    lh_gx_rect_init(&place, 160 - value / 2, 76, value, 48);
    lh_gx_entity_move(lh_gx_button_get_box_mut(&button), place);
    lh_gx_button_center_caption(&button);
}

/* The field's text, and its reaction to Enter: the button takes the text as
   its caption — a copy, for the field goes on editing its own buffer. */
static lh_char_t name[32] = "";

static lh_void
on_name(lh_gx_field_t *field, lh_ptr context)
{
    const lh_usize_t size = lh_str_ptr_len(lh_gx_field_get_text(field));

    (void)context;
    lh_memory_std_copy(caption, lh_gx_field_get_text(field), size);
    caption[size] = '\0';
    lh_gx_button_set_text(&button, caption);
}

int
main(void)
{
    /* Everything the host serves lives as long as the program: the records point
       at each other, and the host never returns. */
    static lh_gx_web_t web;

    lh_memory_allocator_init(lh_runtime_allocator_get_mut(), example_alloc, example_dealloc);
    lh_runtime_terminate_handler_init(lh_runtime_terminate_get_mut(), example_abort, NULL);
    /* The host is a rail loop, and rail runs on what the program gives it: the OS. */
    lh_rail_base_assign(lh_rail_runtime_base_get_mut(), lh_os_rail_base_get());
    if (!lh_os_net_init())
    {
        fprintf(stderr, "the network would not start\n");
        return 1;
    }
    /* The built-in Roboto still lives in gl; it moves with the swap (docs/cleanup.md).
       Without it the button still works; it only has no caption. */
    const lh_font_t *table = lh_gl_font_get_default();
    static lh_gx_font_t font;
    const lh_gx_font_t *face = lh_null;

    if (table != lh_null)
    {
        lh_gx_font_baked_init(&font, table);
        face = &font;
    }

    lh_gx_web_init(&web, 320, 200);
    lh_gx_style_init(&back_style, 0xFF2B2B30U, 0U, 0, 0, 0U, lh_null);
    lh_gx_style_init(&button_style, 0xFF3D7BD9U, 0xFF2A5CA8U, 1, 0, 0xFFFFFFFFU, face);
    static lh_gx_style_t pressed_style;

    lh_gx_style_init(&pressed_style, 0xFF2A5CA8U, 0xFF1D4480U, 1, 0, 0xFFFFFFFFU, face);
    lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 320, 200);
    static lh_uint_t clicks;

    lh_gx_button_init(&button, &button_style, 90, 76, 140, 48, caption, on_click, &clicks);
    lh_gx_button_set_pressed_style(&button, &pressed_style);
    lh_gx_entity_attach(lh_gx_web_get_screen_mut(&web), &back);
    lh_gx_entity_attach(&back, lh_gx_button_get_box_mut(&button));
    static lh_gx_style_t field_style;

    lh_gx_style_init(&field_style, 0xFFFFFFFFU, 0xFF3D7BD9U, 1, 0, 0xFF8A8A96U, face);
    static lh_gx_style_t tick_style;

    lh_gx_style_init(&tick_style, 0xFF3D7BD9U, 0U, 0, 0, 0U, lh_null);
    static lh_gx_checkbox_t light;

    lh_gx_checkbox_init(&light, &field_style, &tick_style, 90, 140, 140, 20, "Light", lh_bool_false,
                        on_light, lh_null);
    lh_gx_entity_attach(&back, lh_gx_checkbox_get_row_mut(&light));
    /* Round radios: a radius of half the square, and of half the mark. */
    static lh_gx_style_t round_style;

    lh_gx_style_init(&round_style, 0xFFFFFFFFU, 0xFF3D7BD9U, 1, 10, 0xFF8A8A96U, face);
    static lh_gx_style_t dot_style;

    lh_gx_style_init(&dot_style, 0xFF3D7BD9U, 0U, 0, 5, 0U, lh_null);
    static lh_gx_radio_group_t colour;

    lh_gx_radio_group_init(&colour, on_colour, lh_null);
    lh_gx_radio_init(&blue, &colour, &round_style, &dot_style, 16, 16, 100, 20, "Blue");
    lh_gx_radio_init(&green, &colour, &round_style, &dot_style, 16, 42, 100, 20, "Green");
    lh_gx_entity_attach(&back, lh_gx_radio_get_row_mut(&blue));
    lh_gx_entity_attach(&back, lh_gx_radio_get_row_mut(&green));
    lh_gx_radio_group_select(&colour, &blue);
    static lh_gx_style_t rail_style;

    lh_gx_style_init(&rail_style, 0xFF5A5A66U, 0U, 0, 2, 0U, lh_null);
    static lh_gx_style_t knob_style;

    lh_gx_style_init(&knob_style, 0xFFFFFFFFU, 0xFF3D7BD9U, 2, 10, 0U, lh_null);
    static lh_gx_slider_t width;

    lh_gx_slider_init(&width, &rail_style, &tick_style, &knob_style, 60, 172, 200, 20, 80, 220, 140,
                      on_width, lh_null);
    lh_gx_entity_attach(&back, lh_gx_slider_get_row_mut(&width));
    static lh_gx_style_t entry_style;

    lh_gx_style_init(&entry_style, 0xFFFFFFFFU, 0xFF8A8A96U, 1, 4, 0xFF202024U, face);
    static lh_gx_style_t entry_focus_style;

    lh_gx_style_init(&entry_focus_style, 0xFFFFFFFFU, 0xFF3D7BD9U, 2, 4, 0xFF202024U, face);
    static lh_gx_style_t caret_style;

    lh_gx_style_init(&caret_style, 0xFF202024U, 0U, 0, 0, 0U, lh_null);
    static lh_gx_field_t entry;

    lh_gx_field_init(&entry, &entry_style, &entry_focus_style, &caret_style, 130, 24, 170, 26, name,
                     sizeof(name), lh_null, on_name, lh_null);
    lh_gx_entity_attach(&back, lh_gx_field_get_box_mut(&entry));
    const lh_net_port_t port = 8080U;

    printf("open http://127.0.0.1:%u/\n", (unsigned)port);
    fflush(stdout);
    if (!lh_os_web_host_run(lh_gx_web_get_app_mut(&web), port))
    {
        fprintf(stderr, "the web host would not start on port %u\n", (unsigned)port);
    }
    lh_gx_web_deinit(&web);
    lh_os_net_deinit();
    return 1;
}
