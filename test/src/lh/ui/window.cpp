/**
 * @file window.cpp
 * @brief Native window composition: input, damage, lifetime and real exposure.
 */
#include <gtest/gtest.h>

#include <lh/ui/window.h>
#include <lh/ui/view/controller.h>
#include <lh/os/app.h>
#include <lh/os/system/window.h>
#include <lh/test/ui/draw_log.h>
#include <lh/expect/death.h>
#include <cstdlib>

namespace
{
using lh_test::rect_of;

bool
window_has_display()
{
#if defined(_WIN32)
    return true;
#else
    const char *name = std::getenv("DISPLAY");
    return name != nullptr && *name != '\0';
#endif
}

void
count_click(lh_ui_button_t *, lh_ptr context)
{
    ++*static_cast<int *>(context);
}

void
feed(lh_ui_window_t *host, lh_os_input_kind_t kind, int x, int y, unsigned id = 0)
{
    lh_os_input_event_t event;
    lh_os_input_event_init_pointer(
        &event, kind, id == 0 ? lh_os_input_device_kind_mouse : lh_os_input_device_kind_touch, id,
        kind == lh_os_input_kind_pointer_move ? lh_os_input_button_none
                                              : lh_os_input_button_primary,
        lh_os_input_coord_from_int(x), lh_os_input_coord_from_int(y), 100);
    lh_os_window_on_native_input(lh_ui_window_get_window_mut(host), &event);
}

class ui_window : public ::testing::Test
{
  protected:
    lh_ui_entity_t root;
    lh_ui_button_t buttons[2];
    lh_ui_style_t background;
    lh_ui_window_t host;
    lh_os_app_t app;

    void
    SetUp() override
    {
        lh_ui_entity_init(&root, rect_of(0, 0, 160, 100));
        lh_os_app_init(&app);
        lh_ui_window_init(&host, &root);
    }

    void
    TearDown() override
    {
        lh_ui_window_deinit(&host);
        EXPECT_EQ(lh_ui_entity_own_view(&root), nullptr);
        EXPECT_FALSE(lh_ui_surface_is_valid(
            lh_os_render_backend_gdi_context_get_surface(lh_ui_window_get_screen(&host))));
        lh_os_app_deinit(&app);
    }

    void
    clear_damage()
    {
        lh_ui_rects_t ignored;
        (void)lh_ui_view_take_damage(lh_ui_window_get_view_mut(&host), &ignored);
        lh_ui_canvas_reset_damage(lh_ui_window_get_canvas_mut(&host));
    }

    bool
    pump_until_painted()
    {
        for (unsigned i = 0; i < 50; ++i)
        {
            if (lh_ui_surface_is_valid(
                    lh_os_render_backend_gdi_context_get_surface(lh_ui_window_get_screen(&host))) &&
                !lh_ui_window_needs_paint(&host))
            {
                return true;
            }
            (void)lh_os_system_window_pump_wait(10);
        }
        return false;
    }
};

TEST_F(ui_window, mounts_root_and_wires_existing_parts)
{
    EXPECT_EQ(lh_ui_view_get_root(lh_ui_window_get_view(&host)), &root);
    EXPECT_EQ(lh_ui_entity_own_view(&root), lh_ui_window_get_view(&host));
    EXPECT_EQ(lh_ui_view_get_canvas(lh_ui_window_get_view(&host)), lh_ui_window_get_canvas(&host));
    EXPECT_EQ(lh_ui_canvas_get_backend(lh_ui_window_get_canvas(&host)), &lh_os_render_backend_gdi);
    EXPECT_EQ(lh_ui_canvas_get_context(lh_ui_window_get_canvas(&host)),
              lh_ui_window_get_screen(&host));
    EXPECT_EQ(lh_os_window_get_controller(lh_ui_window_get_window(&host)),
              lh_ui_window_get_controller(&host));
    EXPECT_EQ(lh_os_controller_get_context(lh_ui_window_get_controller(&host)),
              lh_ui_window_get_view(&host));
    EXPECT_EQ(lh_os_controller_get_handle_cb(lh_ui_window_get_controller(&host)),
              lh_ui_view_controller_handle_event_is_handled);
    EXPECT_EQ(lh_os_controller_get_cancel_cb(lh_ui_window_get_controller(&host)),
              lh_ui_view_controller_cancel);
    EXPECT_EQ(lh_os_window_get_tick_ms(lh_ui_window_get_window(&host)), LH_UI_VIEW_FRAME_MS);
    EXPECT_EQ(lh_ui_window_get_on_update_fn(&host), nullptr);
    EXPECT_EQ(lh_ui_window_get_on_update_context(&host), nullptr);
    EXPECT_FALSE(lh_ui_surface_is_valid(
        lh_os_render_backend_gdi_context_get_surface(lh_ui_window_get_screen(&host))));
}

TEST(ui_window_lifetime, empty_root_and_reinitialization_release_members)
{
    lh_ui_window_t host;
    lh_ui_window_init(&host, nullptr);
    EXPECT_EQ(lh_ui_view_get_root(lh_ui_window_get_view(&host)), nullptr);
    lh_ui_window_update(&host);
    lh_ui_window_deinit(&host);
    EXPECT_EQ(lh_ui_canvas_get_backend(lh_ui_window_get_canvas(&host)), nullptr);
    EXPECT_EQ(lh_ui_view_get_canvas(lh_ui_window_get_view(&host)), nullptr);
    EXPECT_EQ(lh_os_window_get_controller(lh_ui_window_get_window(&host)), nullptr);
    lh_ui_entity_t root;
    lh_ui_entity_init(&root, rect_of(0, 0, 20, 20));
    lh_ui_window_init(&host, &root);
    EXPECT_EQ(lh_ui_entity_own_view(&root), lh_ui_window_get_view(&host));
    lh_ui_window_deinit(&host);
    EXPECT_EQ(lh_ui_entity_own_view(&root), nullptr);
}

TEST_F(ui_window, native_window_input_clicks_button_once_and_rejects_outside_press)
{
    lh_ui_button_t &button = buttons[0];
    lh_ui_button_init(&button, rect_of(10, 10, 40, 30));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&button));
    int clicks = 0;
    lh_ui_button_set_on_click(&button, count_click, &clicks);
    feed(&host, lh_os_input_kind_pointer_down, 20, 20);
    EXPECT_TRUE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&button)));
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 1);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&button)));
    feed(&host, lh_os_input_kind_pointer_down, 9, 20);
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 1);
}

TEST_F(ui_window, dragging_a_standalone_fitting_button_does_not_click)
{
    lh_ui_button_t &button = buttons[0];
    lh_ui_button_init(&button, rect_of(0, 0, 160, 100));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&button));
    int clicks = 0;
    lh_ui_button_set_on_click(&button, count_click, &clicks);
    feed(&host, lh_os_input_kind_pointer_down, 100, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(clicks, 0);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&button)));
}

TEST_F(ui_window, independent_touch_contacts_keep_their_targets)
{
    lh_ui_button_t &first = buttons[0];
    lh_ui_button_init(&first, rect_of(10, 10, 40, 30));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&first));
    int first_clicks = 0;
    lh_ui_button_set_on_click(&first, count_click, &first_clicks);
    lh_ui_button_t &second = buttons[1];
    lh_ui_button_init(&second, rect_of(70, 10, 40, 30));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&second));
    int second_clicks = 0;
    lh_ui_button_set_on_click(&second, count_click, &second_clicks);
    feed(&host, lh_os_input_kind_pointer_down, 20, 20, 1);
    feed(&host, lh_os_input_kind_pointer_down, 80, 20, 2);
    feed(&host, lh_os_input_kind_pointer_up, 80, 20, 2);
    EXPECT_EQ(second_clicks, 1);
    EXPECT_EQ(first_clicks, 0);
    EXPECT_TRUE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&first)));
    feed(&host, lh_os_input_kind_pointer_up, 20, 20, 1);
    EXPECT_EQ(first_clicks, 1);
}

TEST_F(ui_window, resize_and_focus_loss_cancel_without_click)
{
    lh_ui_button_t &button = buttons[0];
    lh_ui_button_init(&button, rect_of(10, 10, 40, 30));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&button));
    int clicks = 0;
    lh_ui_button_set_on_click(&button, count_click, &clicks);
    feed(&host, lh_os_input_kind_pointer_down, 20, 20);
    lh_os_window_on_native_resize(lh_ui_window_get_window_mut(&host), 180, 120, lh_bool_false);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&button)));
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 0);
    feed(&host, lh_os_input_kind_pointer_down, 20, 20);
    feed(&host, lh_os_input_kind_focus_out, 0, 0);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&button)));
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 0);
    feed(&host, lh_os_input_kind_pointer_down, 20, 20);
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 1);
}

TEST(ui_window_lifetime, deinit_cancels_press_and_detaches_borrowed_tree)
{
    lh_ui_button_t root;
    lh_ui_button_init(&root, rect_of(0, 0, 40, 30));
    int clicks = 0;
    lh_ui_button_set_on_click(&root, count_click, &clicks);
    lh_ui_window_t host;
    lh_ui_window_init(&host, lh_ui_button_as_entity(&root));
    feed(&host, lh_os_input_kind_pointer_down, 20, 20);
    ASSERT_TRUE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&root)));
    lh_ui_window_deinit(&host);
    EXPECT_FALSE(lh_ui_entity_is_pressed(lh_ui_button_as_entity(&root)));
    EXPECT_EQ(lh_ui_entity_own_view(lh_ui_button_as_entity(&root)), nullptr);
    EXPECT_EQ(clicks, 0);
}

TEST_F(ui_window, paint_request_includes_view_and_canvas_damage)
{
    clear_damage();
    EXPECT_FALSE(lh_ui_window_needs_paint(&host));
    lh_ui_rect_t damage = rect_of(5, 5, 10, 10);
    lh_ui_view_add_damage(lh_ui_window_get_view_mut(&host), &damage);
    EXPECT_TRUE(lh_ui_window_needs_paint(&host));
    EXPECT_TRUE(lh_ui_window_needs_paint(&host)); // query must not consume it
    clear_damage();
    lh_ui_canvas_add_damage(lh_ui_window_get_canvas_mut(&host), &damage);
    EXPECT_TRUE(lh_ui_window_needs_paint(&host));
    EXPECT_FALSE(lh_ui_view_has_damage(lh_ui_window_get_view(&host)));
    clear_damage();
    EXPECT_FALSE(lh_ui_window_needs_paint(&host));
}

struct update_log
{
    lh_ui_entity_t *root;
    lh_os_window_t *expected_window;
    int calls;
};

void
update_root(lh_os_window_t *window, lh_ptr context)
{
    auto *log = static_cast<update_log *>(context);
    EXPECT_EQ(window, log->expected_window);
    ++log->calls;
    lh_ui_entity_set_rect_v(log->root, rect_of(0, 0, 150, 90));
}

TEST_F(ui_window, update_hook_changes_widgets_and_can_be_disabled)
{
    clear_damage();
    update_log log{&root, lh_ui_window_get_window_mut(&host), 0};
    lh_ui_window_set_on_update(&host, update_root, &log);
    lh_os_window_on_native_tick(lh_ui_window_get_window_mut(&host));
    EXPECT_EQ(log.calls, 1);
    EXPECT_TRUE(lh_ui_window_needs_paint(&host));
    lh_ui_window_set_on_update(&host, nullptr, nullptr);
    clear_damage();
    lh_ui_window_update(&host);
    EXPECT_EQ(log.calls, 1);
    EXPECT_FALSE(lh_ui_window_needs_paint(&host));
}

TEST(ui_window_scrolling, native_drag_records_damage_and_tick_advances_glide)
{
    lh_ui_container_t root;
    lh_ui_container_init(&root, rect_of(0, 0, 100, 60));
    lh_ui_button_t child;
    lh_ui_button_init(&child, rect_of(0, 0, 500, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&root), lh_ui_button_as_entity(&child));
    int clicks = 0;
    lh_ui_button_set_on_click(&child, count_click, &clicks);
    lh_ui_window_t host;
    lh_ui_window_init(&host, lh_ui_container_as_entity(&root));
    lh_ui_view_fit_canvas(lh_ui_window_get_view_mut(&host));
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_move, 20, 20);
    EXPECT_GT(lh_ui_point_get_x(lh_ui_container_get_scroll(&root)), 0);
    EXPECT_TRUE(lh_ui_window_needs_paint(&host));
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(clicks, 0);
    lh_ui_scalar_t before = lh_ui_point_get_x(lh_ui_container_get_scroll(&root));
    lh_ui_window_update(&host);
    EXPECT_GT(lh_ui_point_get_x(lh_ui_container_get_scroll(&root)), before);
    lh_ui_window_deinit(&host);
}

TEST(ui_window_scrolling, a_button_in_a_separate_overlay_never_drags_the_page_below)
{
    lh_ui_entity_t root;
    lh_ui_entity_init(&root, rect_of(0, 0, 100, 60));
    lh_ui_container_t page;
    lh_ui_container_init(&page, rect_of(0, 0, 100, 60));
    lh_ui_entity_add_child(&root, lh_ui_container_as_entity(&page));
    lh_ui_entity_t content;
    lh_ui_entity_init(&content, rect_of(0, 0, 500, 60));
    lh_ui_entity_add_child(lh_ui_container_as_entity(&page), &content);
    // Last sibling is above the page in draw and hit-test order.
    lh_ui_button_t overlay;
    lh_ui_button_init(&overlay, rect_of(0, 0, 100, 60));
    lh_ui_entity_add_child(&root, lh_ui_button_as_entity(&overlay));
    int clicks = 0;
    lh_ui_button_set_on_click(&overlay, count_click, &clicks);
    lh_ui_window_t host;
    lh_ui_window_init(&host, &root);
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 20, 20);
    feed(&host, lh_os_input_kind_pointer_up, 20, 20);
    EXPECT_EQ(lh_ui_point_get_x(lh_ui_container_get_scroll(&page)), 0);
    EXPECT_EQ(clicks, 0);
    feed(&host, lh_os_input_kind_pointer_down, 40, 20);
    feed(&host, lh_os_input_kind_pointer_up, 40, 20);
    EXPECT_EQ(clicks, 1);
    lh_ui_window_deinit(&host);
}

// Event logging verifies that takeover ends a press once, not just its appearance.
struct policy_button
{
    lh_ui_button_t button;
    int presses = 0;
    int releases = 0;
    int clicks = 0;
};

extern const lh_ui_entity_class_t policy_button_class;

void
policy_button_event(const lh_ui_entity_t *entity, const lh_ui_entity_event_t *event)
{
    auto *log = const_cast<policy_button *>(reinterpret_cast<const policy_button *>(entity));
    switch (lh_ui_entity_event_get_code(event))
    {
    case lh_ui_entity_event_press:
        ++log->presses;
        break;
    case lh_ui_entity_event_release:
        ++log->releases;
        break;
    case lh_ui_entity_event_click:
        ++log->clicks;
        break;
    default:
        break;
    }
    lh_ui_entity_class_event_base(&policy_button_class, entity, event);
}

const lh_ui_entity_class_t policy_button_class = {policy_button_event, &lh_ui_button_class};

class ui_scroll_policy : public ::testing::Test
{
  protected:
    lh_ui_container_t page;
    policy_button control;
    lh_ui_entity_t caption;
    lh_ui_window_t host;

    void
    SetUp() override
    {
        lh_ui_container_init(&page, rect_of(0, 0, 100, 60));
        lh_ui_button_init(&control.button, rect_of(0, 0, 500, 60));
        lh_ui_entity_set_class_v(button(), &policy_button_class);
        lh_ui_entity_add_child(lh_ui_container_as_entity(&page), button());
        lh_ui_entity_init(&caption, rect_of(10, 10, 80, 30));
        lh_ui_entity_add_child(button(), &caption);
        lh_ui_window_init(&host, lh_ui_container_as_entity(&page));
        lh_ui_view_fit_canvas(lh_ui_window_get_view_mut(&host));
    }

    void
    TearDown() override
    {
        lh_ui_window_deinit(&host);
    }

    lh_ui_entity_t *
    button()
    {
        return lh_ui_button_as_entity(&control.button);
    }

    lh_ui_scalar_t
    scroll()
    {
        return lh_ui_point_get_x(lh_ui_container_get_scroll(&page));
    }

    void
    wheel()
    {
        lh_os_input_event_t event;
        lh_os_input_event_init(&event, lh_os_input_kind_wheel, lh_os_input_device_kind_mouse, 100);
        lh_os_input_event_set_x(&event, lh_os_input_coord_from_int(80));
        lh_os_input_event_set_y(&event, lh_os_input_coord_from_int(20));
        lh_os_input_event_set_delta(&event, -120, 0);
        lh_os_window_on_native_input(lh_ui_window_get_window_mut(&host), &event);
    }
};

TEST_F(ui_scroll_policy, allowed_drag_releases_button_at_takeover_without_later_click)
{
    EXPECT_TRUE(lh_ui_entity_is_scroll_chain_enabled(button()));
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    EXPECT_EQ(control.presses, 1);
    EXPECT_EQ(control.releases, 0);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    EXPECT_GT(scroll(), 0);
    EXPECT_EQ(control.releases, 1);
    EXPECT_FALSE(lh_ui_entity_is_pressed(button()));
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(control.releases, 1);
    EXPECT_EQ(control.clicks, 0);
}

TEST_F(ui_scroll_policy, blocked_drag_keeps_parent_still_and_regular_tap_still_clicks)
{
    lh_ui_entity_set_scroll_chain(button(), lh_bool_false);
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(scroll(), 0);
    EXPECT_EQ(control.clicks, 0);
    EXPECT_EQ(control.releases, 1);
    feed(&host, lh_os_input_kind_pointer_down, 40, 20);
    feed(&host, lh_os_input_kind_pointer_up, 40, 20);
    EXPECT_EQ(control.clicks, 1);
    EXPECT_EQ(control.releases, 2);
}

TEST_F(ui_scroll_policy, nonclickable_caption_can_stop_the_parent_path)
{
    lh_ui_entity_set_scroll_chain(&caption, lh_bool_false);
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(scroll(), 0);
    EXPECT_EQ(control.clicks, 0);
    feed(&host, lh_os_input_kind_pointer_down, 40, 20);
    feed(&host, lh_os_input_kind_pointer_up, 40, 20);
    EXPECT_EQ(control.clicks, 1);
}

TEST_F(ui_scroll_policy, wheel_obeys_the_same_barrier)
{
    lh_ui_entity_set_scroll_chain(button(), lh_bool_false);
    wheel();
    EXPECT_EQ(scroll(), 0);
    lh_ui_entity_set_scroll_chain(button(), lh_bool_true);
    wheel();
    EXPECT_GT(scroll(), 0);
}

TEST_F(ui_scroll_policy, slight_caption_overflow_does_not_steal_horizontal_page_drag)
{
    lh_ui_entity_set_rect_v(&caption, rect_of(0, 0, 500, 61));
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_GT(scroll(), 0);
    EXPECT_EQ(
        lh_ui_point_get_y(lh_ui_container_get_scroll(lh_ui_button_get_container(&control.button))),
        0);
    EXPECT_EQ(control.clicks, 0);
}

TEST_F(ui_scroll_policy, disabling_own_scroll_blocks_content_and_thumb_input)
{
    lh_ui_container_set_scroll_enabled(&page, lh_bool_false);
    wheel();
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(scroll(), 0);
    lh_ui_scrollbar_t bar;
    lh_ui_scrollbar_init(&bar, rect_of(0, 45, 100, 15), lh_ui_axis_horizontal, &page);
    lh_ui_scrollbar_set_mode(&bar, lh_ui_scrollbar_mode_always);
    lh_ui_entity_add_child(lh_ui_container_as_entity(&page), lh_ui_scrollbar_as_entity(&bar));
    feed(&host, lh_os_input_kind_pointer_down, 5, 50);
    feed(&host, lh_os_input_kind_pointer_move, 80, 50);
    feed(&host, lh_os_input_kind_pointer_up, 80, 50);
    EXPECT_EQ(scroll(), 0);
    feed(&host, lh_os_input_kind_pointer_down, 90, 50);
    feed(&host, lh_os_input_kind_pointer_up, 90, 50);
    EXPECT_EQ(scroll(), 0);
    lh_ui_view_set_focus_v(lh_ui_window_get_view_mut(&host), lh_ui_container_as_entity(&page));
    (void)lh_ui_view_key(lh_ui_window_get_view_mut(&host), lh_os_key_right, lh_bool_true);
    EXPECT_EQ(scroll(), 0);
    lh_ui_view_set_focus_v(lh_ui_window_get_view_mut(&host), nullptr);
    lh_ui_entity_remove_child(lh_ui_container_as_entity(&page), lh_ui_scrollbar_as_entity(&bar));
}

TEST_F(ui_scroll_policy, permission_change_applies_to_the_next_pointer_sequence)
{
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    lh_ui_entity_set_scroll_chain(button(), lh_bool_false);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    EXPECT_GT(scroll(), 0);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    lh_ui_point_t zero;
    lh_ui_point_init(&zero, 0, 0);
    lh_ui_container_set_scroll_v(&page, zero);
    feed(&host, lh_os_input_kind_pointer_down, 80, 20);
    feed(&host, lh_os_input_kind_pointer_move, 50, 20);
    feed(&host, lh_os_input_kind_pointer_up, 50, 20);
    EXPECT_EQ(scroll(), 0);
    EXPECT_EQ(control.clicks, 0);
}

void
solid_style(lh_ui_style_t *style, lh_u32_t hex)
{
    lh_ui_style_init(style);
    lh_ui_color_t color;
    lh_ui_color_init_hex(&color, hex);
    lh_ui_paint_t fill;
    lh_ui_paint_init_color(&fill, &color);
    lh_ui_style_assign_fill(style, &fill);
}

TEST_F(ui_window, real_window_paints_full_exposure_and_releases_surface)
{
    if (!window_has_display())
    {
        GTEST_SKIP() << "no native display";
    }
    solid_style(&background, 0x112233ff);
    lh_ui_entity_set_style_v(&root, &background);
    ASSERT_TRUE(lh_ui_window_try_open(&host, &app, "LH window composition test", 160, 100));
    ASSERT_TRUE(pump_until_painted());
    const lh_ui_surface_t *surface =
        lh_os_render_backend_gdi_context_get_surface(lh_ui_window_get_screen(&host));
    lh_ui_pixmap_t pixmap;
    ASSERT_TRUE(lh_ui_surface_get_pixmap(surface, &pixmap));
    EXPECT_EQ(lh_ui_pixmap_read_word(&pixmap, 0, 0), 0xff112233u);
    EXPECT_EQ(lh_ui_pixmap_read_word(&pixmap, 159, 99), 0xff112233u);
    EXPECT_FALSE(lh_ui_window_needs_paint(&host));
    EXPECT_EQ(lh_os_render_backend_gdi_context_get_hdc(lh_ui_window_get_screen(&host)), nullptr);

    // No widget damage: exposure still has to redraw the complete frame.
    lh_ui_color_t sentinel;
    lh_ui_color_init_hex(&sentinel, 0xff00ffff);
    lh_ui_pixmap_set_pixel(&pixmap, 159, 99, &sentinel);
    lh_os_window_invalidate(lh_ui_window_get_window_mut(&host));
    for (unsigned i = 0; i < 50 && lh_ui_pixmap_read_word(&pixmap, 159, 99) != 0xff112233u; ++i)
    {
        (void)lh_os_system_window_pump_wait(10);
    }
    EXPECT_EQ(lh_ui_pixmap_read_word(&pixmap, 159, 99), 0xff112233u);
    lh_os_window_close(lh_ui_window_get_window_mut(&host));
    EXPECT_EQ(lh_os_app_get_first_window(&app), nullptr);
    EXPECT_TRUE(lh_os_app_is_quit(&app));
}

#if !defined(_WIN32)
TEST(ui_window_failure, missing_display_leaves_composition_reusable_and_tree_detachable)
{
    if (window_has_display())
    {
        GTEST_SKIP() << "run this case separately with DISPLAY empty";
    }
    lh_os_app_t app;
    lh_os_app_init(&app);
    lh_ui_entity_t root;
    lh_ui_entity_init(&root, rect_of(0, 0, 20, 20));
    lh_ui_window_t host;
    lh_ui_window_init(&host, &root);
    EXPECT_FALSE(lh_ui_window_try_open(&host, &app, "Unavailable display", 20, 20));
    EXPECT_EQ(lh_os_app_get_first_window(&app), nullptr);
    EXPECT_EQ(lh_ui_entity_own_view(&root), lh_ui_window_get_view(&host));
    EXPECT_FALSE(lh_ui_window_try_open(&host, &app, "Retry", 20, 20));
    lh_ui_window_deinit(&host);
    EXPECT_EQ(lh_ui_entity_own_view(&root), nullptr);
    lh_os_app_deinit(&app);
}

#    if LH_TEST_EXPECT_DEATH_ENABLED
TEST(UiWindowDeathTest, checked_open_terminates_when_native_creation_fails)
{
    // A fresh process avoids inheriting a cached X connection from another test.
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    LH_EXPECT_CHECK_DEATH({
        (void)setenv("DISPLAY", "", 1);
        lh_os_app_t app;
        lh_os_app_init(&app);
        lh_ui_window_t host;
        lh_ui_window_init(&host, nullptr);
        lh_ui_window_open(&host, &app, "Unavailable display", 20, 20);
    });
}
#    endif
#endif

} // namespace
