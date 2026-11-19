/**
 * @file window.cpp
 * @brief Tests for `lh/gx/window.h` and `lh/gx/window/input.h`.
 *
 * The system's input is translated to gx events: the primary button's press,
 * move and release at whole pixels, the control keys, the characters that
 * type — and nothing else. The window itself runs on a fake window backend
 * that writes down what it is asked to repaint, over a real system surface:
 * opening makes the surface and the first frame draws it all and asks for all
 * of it; a frame with nothing changed asks for nothing; an input that changes
 * the screen is drawn at once; a resize makes a new surface.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW

#    include <lh/gx/box.h>
#    include <lh/gx/class/initializer.h>
#    include <lh/gx/window.h>
#    include <lh/gx/window/input.h>
#    include <lh/os/app.h>
#    include <lh/os/window/backend.h>

#    include <vector>

namespace
{

lh_os_input_event_t
pointer(lh_os_input_kind_t kind, lh_os_input_button_t button, int x, int y)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_pointer(&event, kind, lh_os_input_device_kind_mouse, 0U, button,
                                   lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y),
                                   0U);
    return event;
}

TEST(gx_window_input, the_primary_button_is_the_pointer_at_whole_pixels)
{
    lh_os_input_event_t event =
        pointer(lh_os_input_kind_pointer_down, lh_os_input_button_primary, 10, 5);

    lh_gx_event_t out;
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_kind(&out), lh_gx_event_kind_press);
    EXPECT_EQ(lh_gx_point_get_x(lh_gx_event_get_at(&out)), 10) << "Q24.8 to pixels";
    EXPECT_EQ(lh_gx_point_get_y(lh_gx_event_get_at(&out)), 5);
    event = pointer(lh_os_input_kind_pointer_up, lh_os_input_button_primary, 11, 6);
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_kind(&out), lh_gx_event_kind_release);
    event = pointer(lh_os_input_kind_pointer_down, lh_os_input_button_secondary, 1, 1);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out)) << "another button";
    event = pointer(lh_os_input_kind_pointer_move, lh_os_input_button_none, 1, 1);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out)) << "a move with nothing held";
    lh_os_input_event_set_buttons(&event, lh_os_input_button_to_mask(lh_os_input_button_primary));
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_kind(&out), lh_gx_event_kind_move);
    event = pointer(lh_os_input_kind_pointer_cancel, lh_os_input_button_none, 0, 0);
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_kind(&out), lh_gx_event_kind_cancel);
}

TEST(gx_window_input, a_control_key_is_a_key_and_a_character_that_types_is_text)
{
    lh_os_input_event_t event;

    lh_os_input_event_init_key(&event, lh_os_key_backspace, lh_bool_true, lh_bool_false, 0U, 0U);
    lh_gx_event_t out;
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_key(&out), lh_gx_key_backspace);
    lh_os_input_event_init_key(&event, lh_os_key_backspace, lh_bool_false, lh_bool_false, 0U, 0U);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out)) << "a key going up";
    lh_os_input_event_init_key(&event, lh_os_key_shift, lh_bool_true, lh_bool_false, 0U, 0U);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out)) << "no control uses Shift";
    lh_os_input_event_init_text(&event, 0x42FU, 0U);
    ASSERT_TRUE(lh_gx_window_input_try_get_event(&event, &out));
    EXPECT_EQ(lh_gx_event_get_code(&out), 0x42FU);
    lh_os_input_event_init_text(&event, 0x0DU, 0U);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out)) << "Enter as a character";
    lh_os_input_event_init_wheel(&event, 0, 0, 0, 120, 0U);
    EXPECT_FALSE(lh_gx_window_input_try_get_event(&event, &out));
}

/* A window backend that opens nothing and writes down what it is asked to repaint. */
struct Fake
{
    lh_os_window_backend_t table;
    std::vector<lh_math_rect_t> asked;
};

lh_bool_t
fake_open(lh_ptr context, lh_os_window_t *window, const lh_char_t *, int width, int height,
          lh_os_window_t *)
{
    lh_os_window_set_native(window, context);
    lh_os_window_on_native_resize(window, width, height, lh_bool_false);
    return lh_bool_true;
}

lh_void
fake_close(lh_ptr, lh_os_window_t *window)
{
    lh_os_window_on_native_destroy(window);
}

lh_void
fake_invalidate(lh_ptr, lh_os_window_t *)
{
}

lh_void
fake_invalidate_rect(lh_ptr context, lh_os_window_t *, int left, int top, int right, int bottom)
{
    lh_math_rect_t r;

    lh_math_rect_init(&r, left, top, right - left, bottom - top);
    static_cast<Fake *>(context)->asked.push_back(r);
}

lh_tick_t
fake_now(lh_ptr)
{
    return 0U;
}

lh_void
fake_set_timer(lh_ptr, lh_os_window_t *, lh_u32_t)
{
}

/* A card that takes the pointer and turns green when pressed. */
lh_gx_style_t red_style;
lh_gx_style_t green_style;

lh_void
turn_green(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    if (lh_gx_event_get_kind(event) == lh_gx_event_kind_press)
    {
        lh_gx_entity_set_style(entity, &green_style);
        lh_gx_entity_invalidate(entity);
    }
}

const lh_gx_class_t card_class = lh_gx_class_initializer(lh_gx_box_paint, lh_gx_box_covers, lh_null,
                                                         turn_green, lh_null, lh_null, lh_null);

struct Scene
{
    Fake fake;
    lh_os_app_t app;
    lh_gx_window_t window;
    lh_gx_entity_t card;

    Scene()
    {
        lh_os_window_backend_init(&fake.table);
        lh_os_window_backend_set_open(&fake.table, fake_open);
        lh_os_window_backend_set_close(&fake.table, fake_close);
        lh_os_window_backend_set_invalidate(&fake.table, fake_invalidate);
        lh_os_window_backend_set_invalidate_rect(&fake.table, fake_invalidate_rect);
        lh_os_window_backend_set_now(&fake.table, fake_now);
        lh_os_window_backend_set_set_timer(&fake.table, fake_set_timer);
        lh_os_app_init(&app);
        lh_os_app_install_window_backend(&app, &fake.table, &fake);
        lh_gx_style_init(&red_style, 0xFFFF0000U, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&green_style, 0xFF00FF00U, 0U, 0, 0, 0U, nullptr);
        lh_gx_window_init(&window);
        lh_gx_entity_init(&card, &card_class, &red_style, 0, 0, 40, 30);
        lh_gx_entity_attach(lh_gx_window_get_screen_mut(&window), &card);
    }

    ~Scene()
    {
        lh_gx_window_deinit(&window);
        lh_os_app_deinit(&app);
    }

    Scene(const Scene &) = delete;
    Scene &
    operator=(const Scene &) = delete;

    lh_u32_t
    at(int x, int y) const
    {
        const auto *words = static_cast<const lh_u32_t *>(
            lh_os_system_surface_get_pixels(lh_gx_window_get_surface(&window)));
        return words[y * lh_os_system_surface_get_width(lh_gx_window_get_surface(&window)) + x];
    }
};

TEST(gx_window, a_frame_draws_into_the_surface_and_asks_for_what_it_drew)
{
    Scene scene;

    ASSERT_TRUE(lh_gx_window_try_open(&scene.window, &scene.app, "test", 40, 30));
    ASSERT_NE(lh_gx_window_get_surface(&scene.window), LH_OS_SYSTEM_SURFACE_HANDLE_INVALID);
    scene.fake.asked.clear();
    ASSERT_TRUE(lh_gx_window_update(&scene.window));
    EXPECT_EQ(scene.at(5, 5), 0xFFFF0000U);
    ASSERT_FALSE(scene.fake.asked.empty());
    lh_math_rect_t whole;

    lh_math_rect_init(&whole, 0, 0, 40, 30);
    EXPECT_TRUE(lh_math_rect_eq(&scene.fake.asked[0], &whole)) << "the whole new surface";
    scene.fake.asked.clear();
    EXPECT_FALSE(lh_gx_window_update(&scene.window));
    EXPECT_TRUE(scene.fake.asked.empty()) << "nothing changed: nothing asked";
}

TEST(gx_window, an_input_that_changes_the_screen_is_drawn_at_once)
{
    Scene scene;

    ASSERT_TRUE(lh_gx_window_try_open(&scene.window, &scene.app, "test", 40, 30));
    lh_gx_window_update(&scene.window);
    scene.fake.asked.clear();
    const lh_os_input_event_t press =
        pointer(lh_os_input_kind_pointer_down, lh_os_input_button_primary, 10, 10);

    ASSERT_TRUE(lh_gx_window_take_input(&scene.window, &press));
    EXPECT_EQ(scene.at(5, 5), 0xFF00FF00U) << "drawn without waiting for a tick";
    EXPECT_FALSE(scene.fake.asked.empty());
    const lh_os_input_event_t wheel = []
    {
        lh_os_input_event_t e;
        lh_os_input_event_init_wheel(&e, 0, 0, 0, 120, 0U);
        return e;
    }();

    EXPECT_FALSE(lh_gx_window_take_input(&scene.window, &wheel));
}

TEST(gx_window, a_resize_makes_a_new_surface_the_new_size)
{
    Scene scene;

    ASSERT_TRUE(lh_gx_window_try_open(&scene.window, &scene.app, "test", 40, 30));
    lh_gx_window_update(&scene.window);
    lh_os_window_on_native_resize(lh_gx_window_get_window_mut(&scene.window), 60, 50,
                                  lh_bool_false);
    EXPECT_EQ(lh_os_system_surface_get_width(lh_gx_window_get_surface(&scene.window)), 60);
    EXPECT_EQ(lh_os_system_surface_get_height(lh_gx_window_get_surface(&scene.window)), 50);
    EXPECT_EQ(scene.at(5, 5), 0xFFFF0000U) << "the screen drawn again into the new surface";
    lh_gx_rect_t screen;

    lh_gx_rect_init(&screen, 0, 0, 60, 50);
    EXPECT_TRUE(
        lh_gx_rect_eq(lh_gx_entity_get_place(lh_gx_window_get_screen_mut(&scene.window)), &screen));
}

} // namespace

#endif /* LH_LIBRARY_OPTION_OS && LH_LIBRARY_OPTION_OS_WINDOW */
