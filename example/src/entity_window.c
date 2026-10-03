/**
 * @file entity_window.c
 * @brief Example: an entity screen shown in a native window.
 *
 * A panel with three buttons and a turned square. Clicking a button
 * toggles its color; only the button's area is redrawn and copied to the
 * window. The loop sleeps until the OS has something to say, so the
 * program uses no CPU while nothing happens.
 *
 * Run with `--frames N` to quit by itself after N rendered frames (used to
 * check it from scripts).
 */

#include <lh/entity/screen.h>
#include <lh/os/window.h>
#include <lh/runtime/allocator.h>

#include <stdlib.h>
#include <string.h>

#define EXAMPLE_WIDTH 640
#define EXAMPLE_HEIGHT 400

static const lh_ui_color_t example_button_off = {70, 90, 160, 255};
static const lh_ui_color_t example_button_on = {255, 122, 24, 255};

struct example
{
    lh_entity_screen_t *screen;
    lh_os_window_t window;
    lh_ui_color_t *pixels;
    lh_bool_t quit;
};

static lh_entity_rect_t *
example_rect(lh_entity_t *parent, lh_float_t x, lh_float_t y, lh_float_t w, lh_float_t h,
             lh_ui_color_t color)
{
    lh_entity_rect_t *rect = lh_entity_cast(lh_entity_create(&lh_entity_rect_class, parent),
                                            &lh_entity_rect_class);
    lh_entity_2d_set_position((lh_entity_2d_t *)rect, lh_math_vec2_make(x, y));
    lh_entity_rect_set_size(rect, lh_math_vec2_make(w, h));
    lh_entity_rect_set_color(rect, color);
    return rect;
}

/* A button: toggles between its two colors on every press. */
static lh_void
example_on_button(lh_entity_event_t *event, lh_ptr user_data)
{
    (void)user_data;
    if (lh_entity_event_get_code(event) != LH_ENTITY_EVENT_POINTER_DOWN)
    {
        return;
    }
    lh_entity_rect_t *button = lh_entity_cast(lh_entity_event_get_current(event),
                                              &lh_entity_rect_class);
    const lh_ui_color_t color = lh_entity_rect_get_color(button);
    lh_entity_rect_set_color(button, color.r == example_button_on.r ? example_button_off
                                                                     : example_button_on);
}

/* Window events: lost pixels are marked for redrawing, the mouse goes to the
   entities, closing ends the loop. */
static lh_void
example_on_window(lh_self_ptr self, lh_os_system_window_handle_t window,
                  const lh_os_system_window_event_t *event)
{
    struct example *example = self;
    (void)window;

    const lh_float_t x = (lh_float_t)lh_os_system_window_event_get_x(event);
    const lh_float_t y = (lh_float_t)lh_os_system_window_event_get_y(event);
    switch (lh_os_system_window_event_get_type(event))
    {
    case lh_os_system_window_event_paint:
        lh_entity_screen_invalidate_area(
            example->screen,
            lh_math_rect_make(lh_os_system_window_event_get_x(event),
                            lh_os_system_window_event_get_y(event),
                            lh_os_system_window_event_get_width(event),
                            lh_os_system_window_event_get_height(event)));
        break;
    case lh_os_system_window_event_pointer_down:
        lh_entity_screen_send_pointer(example->screen, LH_ENTITY_EVENT_POINTER_DOWN,
                                      lh_math_vec2_make(x, y));
        break;
    case lh_os_system_window_event_pointer_up:
        lh_entity_screen_send_pointer(example->screen, LH_ENTITY_EVENT_POINTER_UP,
                                      lh_math_vec2_make(x, y));
        break;
    case lh_os_system_window_event_close:
        example->quit = lh_bool_true;
        break;
    default:
        break;
    }
}

static lh_void
example_build(struct example *example)
{
    lh_entity_t *root = (lh_entity_t *)example->screen;
    lh_entity_rect_set_size((lh_entity_rect_t *)example->screen,
                            lh_math_vec2_make(EXAMPLE_WIDTH, EXAMPLE_HEIGHT));
    lh_entity_rect_set_color((lh_entity_rect_t *)example->screen,
                             lh_ui_color_make(24, 26, 32, 255));

    lh_entity_rect_t *panel =
        example_rect(root, 40, 40, 360, 300, lh_ui_color_make(40, 44, 56, 255));
    for (int i = 0; i < 3; ++i)
    {
        lh_entity_rect_t *button = example_rect((lh_entity_t *)panel, 30, 30 + 80.0f * i, 300, 60,
                                                example_button_off);
        lh_entity_add_handler((lh_entity_t *)button, example_on_button, lh_null);
    }
    /* Reaches past the panel's bottom edge: cut to the panel. */
    example_rect((lh_entity_t *)panel, 30, 270, 300, 60, lh_ui_color_make(90, 160, 90, 255));

    lh_entity_rect_t *square =
        example_rect(root, 520, 120, 80, 80, lh_ui_color_make(200, 60, 80, 255));
    lh_entity_2d_set_angle((lh_entity_2d_t *)square, 0.5f);
}

int
main(int argc, char **argv)
{
    long frames_left = -1;
    if (argc == 3 && strcmp(argv[1], "--frames") == 0)
    {
        frames_left = strtol(argv[2], NULL, 10);
    }

    struct example example;
    memset(&example, 0, sizeof example);
    example.screen = lh_entity_cast(lh_entity_create_root(&lh_entity_screen_class,
                                                          lh_runtime_allocator()),
                                    &lh_entity_screen_class);
    example_build(&example);

    example.pixels = lh_runtime_allocator_alloc(sizeof(lh_ui_color_t) * EXAMPLE_WIDTH *
                                                EXAMPLE_HEIGHT);
    lh_ui_canvas_t canvas;
    lh_ui_canvas_init(&canvas, example.pixels, EXAMPLE_WIDTH, EXAMPLE_HEIGHT, EXAMPLE_WIDTH);

#ifdef _WIN32
    static const wchar_t title[] = L"lh entities";
#else
    static const char title[] = "lh entities";
#endif
    lh_os_window_init(&example.window);
    if (!lh_os_window_open(&example.window, (lh_ptr)title, EXAMPLE_WIDTH, EXAMPLE_HEIGHT))
    {
        return 1;
    }
    lh_os_window_set_handler(example_on_window, &example);
    lh_os_window_show(&example.window);

    while (!example.quit && frames_left != 0)
    {
        const lh_math_rect_t drawn = lh_entity_screen_render(example.screen, &canvas);
        if (!lh_math_rect_is_empty(&drawn))
        {
            lh_os_window_present(&example.window, example.pixels, EXAMPLE_WIDTH, drawn.origin.x,
                                 drawn.origin.y, drawn.size.width, drawn.size.height);
            if (frames_left > 0)
            {
                --frames_left;
            }
        }
        if (frames_left != 0 && !lh_entity_screen_get_dirty_count(example.screen))
        {
            lh_os_window_wait_messages();
        }
        if (lh_os_window_pump_messages())
        {
            break;
        }
    }

    lh_os_window_set_handler(lh_null, lh_null);
    lh_os_window_close(&example.window);
    lh_entity_delete((lh_entity_t *)example.screen);
    lh_runtime_allocator_free(example.pixels);
    return 0;
}
