/**
 * @file web.cpp
 * @brief Tests for `lh/gx/web.h`.
 *
 * A grey background and a white button with a pressed look, on a 40 by 20
 * screen. Without a network: the application answers `GET /` and takes the
 * websocket, the first frame draws the whole screen and the next one nothing,
 * a press line reaches the button and asks for a frame, a release over it
 * clicks, a line that is not a pointer line is ignored. Then through a real
 * host on 127.0.0.1: the page comes back over HTTP, a websocket client gets a
 * frame that stands on its own, and its press and release lines click the
 * button on the host's thread.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/core/web/ws.h>
#    include <lh/gx/box.h>
#    include <lh/gx/button.h>
#    include <lh/gx/class/initializer.h>
#    include <lh/gx/style.h>
#    include <lh/gx/web.h>
#    include <lh/gx/web/page.h>
#    include <lh/os/net.h>
#    include <lh/os/web/host.h>
#    include <lh/test/os/net/loopback.h>
#    include <lh/test/os/net/read_head.h>

#    include <atomic>
#    include <chrono>
#    include <cstring>
#    include <string>
#    include <thread>
#    include <vector>

namespace
{

lh_void
count_click(lh_gx_button_t *, lh_ptr context)
{
    ++*static_cast<std::atomic<int> *>(context);
}

/* The screen of every test. It must not move: the application points into itself. */
struct Screen
{
    lh_gx_web_t web;
    lh_gx_style_t back_style;
    lh_gx_style_t button_style;
    lh_gx_style_t pressed_style;
    lh_gx_entity_t back;
    lh_gx_button_t button;
    std::atomic<int> clicks{0};

    Screen()
    {
        lh_gx_web_init(&web, 40, 20);
        lh_gx_style_init(&back_style, 0xFF404040U, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&button_style, 0xFFFFFFFFU, 0U, 0, 0, 0U, nullptr);
        lh_gx_style_init(&pressed_style, 0xFFC0C0C0U, 0U, 0, 0, 0U, nullptr);
        lh_gx_entity_init(&back, &lh_gx_box_class, &back_style, 0, 0, 40, 20);
        lh_gx_button_init(&button, &button_style, 10, 5, 20, 10, nullptr, count_click, &clicks);
        lh_gx_button_set_pressed_style(&button, &pressed_style);
        lh_gx_entity_attach(lh_gx_web_get_screen_mut(&web), &back);
        lh_gx_entity_attach(&back, lh_gx_button_get_box_mut(&button));
    }

    ~Screen()
    {
        lh_gx_web_deinit(&web);
    }

    Screen(const Screen &) = delete;
    Screen &
    operator=(const Screen &) = delete;

    lh_uint_t
    generation() const
    {
        return lh_os_web_app_get_generation(lh_gx_web_get_app(&web));
    }

    std::string
    frame() const
    {
        const lh_str_t *text = lh_gx_web_get_frame(&web);

        return std::string(lh_str_get_data(text), lh_str_get_size(text));
    }

    bool
    line(const char *text)
    {
        return lh_gx_web_take_line(&web, text, std::strlen(text)) == lh_bool_true;
    }
};

TEST(gx_web, init_serves_the_page_and_takes_the_websocket)
{
    Screen screen;
    const lh_os_web_app_t *app = lh_gx_web_get_app(&screen.web);
    const lh_str_t *page = lh_gx_web_get_page(&screen.web);
    const std::string text(lh_str_get_data(page), lh_str_get_size(page));

    EXPECT_NE(text.find("width=\"40\" height=\"20\""), std::string::npos);
    ASSERT_EQ(lh_os_web_app_get_nroutes(app), 1U);
    EXPECT_EQ(std::string(lh_os_web_app_route_get_path(lh_os_web_app_get_routes(app))), "/");
    EXPECT_EQ(std::string(lh_os_web_app_get_ws_path(app), lh_os_web_app_get_ws_path_len(app)),
              LH_GX_WEB_PAGE_WS_PATH);
    EXPECT_TRUE(lh_os_web_app_get_frame_fn(app) == lh_gx_web_frame);
    EXPECT_TRUE(lh_os_web_app_get_message_fn(app) == lh_gx_web_message);
    EXPECT_TRUE(lh_os_web_app_get_tick_fn(app) == lh_gx_web_tick);
    EXPECT_EQ(lh_os_web_app_get_ctx(app), &screen.web);
    EXPECT_EQ(screen.generation(), 1U) << "a new client is behind it, so it gets a frame";
}

TEST(gx_web, a_restart_draws_the_whole_screen_and_then_nothing_changed)
{
    Screen screen;

    EXPECT_EQ(lh_gx_web_draw(&screen.web, lh_bool_true), 2U) << "the background, the button";
    EXPECT_EQ(screen.frame().rfind("[[\"fill_rect\",[0,0,40,20],[0,0,40,20],4282400832]", 0), 0U);
    EXPECT_EQ(lh_gx_web_draw(&screen.web, lh_bool_false), 0U);
    EXPECT_EQ(screen.frame(), "[]");
}

TEST(gx_web, the_frame_slot_hands_over_a_frame_only_when_there_is_one)
{
    Screen screen;
    const lh_char_t *bytes = "stale";
    lh_usize_t len = 99U;

    lh_gx_web_frame(&screen.web, lh_bool_false, &bytes, &len);
    EXPECT_GT(len, 2U) << "the first frame: the whole tree is new";
    lh_gx_web_frame(&screen.web, lh_bool_false, &bytes, &len);
    EXPECT_EQ(len, 0U);
    EXPECT_TRUE(bytes == nullptr);
    lh_gx_web_frame(&screen.web, lh_bool_true, &bytes, &len);
    EXPECT_EQ(std::string(bytes, len), screen.frame());
    EXPECT_GT(len, 2U);
}

TEST(gx_web, a_press_and_a_release_over_the_button_click_it)
{
    Screen screen;

    lh_gx_web_draw(&screen.web, lh_bool_true);
    ASSERT_TRUE(screen.line("p 20 10"));
    EXPECT_TRUE(lh_gx_button_is_pressed(&screen.button));
    EXPECT_EQ(screen.generation(), 2U) << "the pressed look changed the screen";
    EXPECT_EQ(lh_gx_web_draw(&screen.web, lh_bool_false), 1U) << "only the button";
    EXPECT_NE(screen.frame().find("[10,5,20,10],4290822336]"), std::string::npos);

    ASSERT_TRUE(screen.line("r 21 11"));
    EXPECT_EQ(screen.clicks.load(), 1);
    EXPECT_EQ(screen.generation(), 3U);

    ASSERT_TRUE(screen.line("p 20 10"));
    ASSERT_TRUE(screen.line("r 2 2"));
    EXPECT_EQ(screen.clicks.load(), 1) << "released beside it";
}

TEST(gx_web, a_line_that_is_not_a_pointer_line_is_ignored)
{
    Screen screen;

    lh_gx_web_draw(&screen.web, lh_bool_true);
    EXPECT_FALSE(screen.line("hello"));
    EXPECT_FALSE(screen.line("p 20"));
    EXPECT_FALSE(lh_gx_button_is_pressed(&screen.button));
    EXPECT_EQ(screen.generation(), 1U);
}

/* A field of the test: it takes the pointer and the keys, and writes down
   the kinds it heard. */
lh_void
note_kind(lh_gx_entity_t *entity, const lh_gx_event_t *event)
{
    static_cast<std::vector<lh_gx_event_kind_t> *>(lh_gx_entity_get_content(entity))
        ->push_back(lh_gx_event_get_kind(event));
}

const lh_gx_class_t field_class =
    lh_gx_class_initializer(lh_null, lh_null, lh_null, note_kind, note_kind, lh_null);

TEST(gx_web, a_press_focuses_what_takes_keys_and_the_key_lines_go_to_it)
{
    Screen screen;
    std::vector<lh_gx_event_kind_t> heard;
    lh_gx_entity_t field;

    lh_gx_entity_init(&field, &field_class, nullptr, 32, 2, 6, 6);
    lh_gx_entity_set_content(&field, &heard);
    lh_gx_entity_attach(&screen.back, &field);
    EXPECT_TRUE(screen.line("t 97")) << "a key line with no focus is read, and goes nowhere";
    EXPECT_TRUE(heard.empty());

    ASSERT_TRUE(screen.line("p 34 4"));
    ASSERT_TRUE(screen.line("r 34 4"));
    EXPECT_EQ(lh_gx_focus_get_focused(lh_gx_web_get_focus(&screen.web)), &field);
    ASSERT_TRUE(screen.line("t 97"));
    ASSERT_TRUE(screen.line("k Backspace"));
    EXPECT_FALSE(screen.line("k F5"));
    ASSERT_TRUE(screen.line("p 20 10"));
    ASSERT_TRUE(screen.line("r 20 10"));
    EXPECT_TRUE(lh_gx_focus_get_focused(lh_gx_web_get_focus(&screen.web)) == nullptr)
        << "the button takes no keys: the press there takes the focus away";
    EXPECT_EQ(heard, (std::vector<lh_gx_event_kind_t>{
                         lh_gx_event_kind_press, lh_gx_event_kind_focus, lh_gx_event_kind_release,
                         lh_gx_event_kind_text, lh_gx_event_kind_key, lh_gx_event_kind_blur}));
    EXPECT_EQ(screen.clicks.load(), 1);
    lh_gx_entity_detach(&field);
}

TEST(gx_web, a_tick_asks_for_a_frame_only_when_something_changed)
{
    Screen screen;

    lh_gx_web_draw(&screen.web, lh_bool_true);
    lh_gx_web_tick(&screen.web);
    EXPECT_EQ(screen.generation(), 1U);
    lh_gx_entity_set_shown(lh_gx_button_get_box_mut(&screen.button), lh_bool_false);
    EXPECT_TRUE(lh_gx_web_bump_if_changed(&screen.web));
    lh_gx_web_tick(&screen.web);
    EXPECT_EQ(screen.generation(), 3U) << "the damage is still there until a frame is drawn";
    lh_gx_web_draw(&screen.web, lh_bool_false);
    EXPECT_FALSE(lh_gx_web_bump_if_changed(&screen.web));
}

/* ── through a real host ─────────────────────────────────────────────────── */

/* Wait at most a second for @p sock to have something to read. */
bool
readable(lh_os_net_socket_t *sock)
{
    lh_os_net_socket_t *const read[] = {sock};

    return lh_os_net_socket_wait(read, 1U, nullptr, 0U, 1000U) > 0;
}

/* The payload of the next websocket frame from the host; empty when none came. */
std::string
read_frame(lh_os_net_socket_t *sock)
{
    std::string got;
    char chunk[4096];

    while (readable(sock))
    {
        const lh_ssize_t n = lh_os_net_socket_recv(sock, chunk, sizeof(chunk));

        if (n <= 0)
        {
            break;
        }
        got.append(chunk, static_cast<std::size_t>(n));
        lh_uint_t opcode = 0U;
        lh_bool_t fin = lh_bool_false;
        const lh_byte_t *payload = nullptr;
        lh_usize_t size = 0U;
        const lh_byte_t *mask = nullptr;
        const lh_usize_t used = lh_web_ws_read(reinterpret_cast<lh_byte_t *>(&got[0]), got.size(),
                                               &opcode, &fin, &payload, &size, &mask);

        if (used != 0U)
        {
            return std::string(reinterpret_cast<const char *>(payload), size);
        }
    }
    return std::string();
}

bool
send_line(lh_os_net_socket_t *sock, const char *text)
{
    unsigned char frame[64];
    const lh_usize_t n = lh_web_ws_write(frame, sizeof(frame), LH_WEB_WS_TEXT, lh_bool_true, text,
                                         std::strlen(text));

    return n != 0U && lh_os_net_socket_send_all(sock, frame, n);
}

TEST(gx_web, a_browser_gets_the_page_a_frame_and_clicks_the_button)
{
    ASSERT_TRUE(lh_os_net_init());
    Screen screen;
    const lh_net_port_t port = lh_test::free_port();

    if (port == 0U)
    {
        lh_os_net_deinit();
        GTEST_SKIP() << "no free port on loopback";
    }
    lh_os_web_host_t *host = lh_os_web_host_try_create(lh_gx_web_get_app_mut(&screen.web), port);

    if (host == nullptr)
    {
        lh_os_net_deinit();
        GTEST_SKIP() << "the web host would not listen on loopback";
    }
    ASSERT_TRUE(lh_os_web_host_try_start(host));

    lh_os_net_socket_t http;
    ASSERT_TRUE(lh_test::connect_loopback(&http, port));
    static char get[] = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&http, get, sizeof(get) - 1U));
    const std::string head = lh_test::read_head(&http);
    lh_os_net_socket_close(&http);
    EXPECT_NE(head.find(" 200 "), std::string::npos);
    EXPECT_NE(head.find("text/html"), std::string::npos);

    lh_os_net_socket_t ws;
    ASSERT_TRUE(lh_test::connect_loopback(&ws, port));
    static char upgrade[] = "GET /ws HTTP/1.1\r\nHost: 127.0.0.1\r\n"
                            "Upgrade: websocket\r\nConnection: Upgrade\r\n"
                            "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                            "Sec-WebSocket-Version: 13\r\n\r\n";
    ASSERT_TRUE(lh_os_net_socket_send_all(&ws, upgrade, sizeof(upgrade) - 1U));
    EXPECT_NE(lh_test::read_head(&ws).find(" 101 "), std::string::npos);
    const std::string first = read_frame(&ws);
    EXPECT_EQ(first.rfind("[[\"fill_rect\",[0,0,40,20],[0,0,40,20],4282400832]", 0), 0U)
        << "a new client gets the whole screen: " << first;

    ASSERT_TRUE(send_line(&ws, "p 20 10"));
    ASSERT_TRUE(send_line(&ws, "r 20 10"));
    for (int i = 0; i < 200 && screen.clicks.load() == 0; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_EQ(screen.clicks.load(), 1);

    lh_os_net_socket_close(&ws);
    lh_os_web_host_destroy(host);
    lh_os_net_deinit();
}

} // namespace

#endif /* LH_LIBRARY_OPTION_OS */
