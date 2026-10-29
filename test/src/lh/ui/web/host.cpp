/**
 * @file host.cpp
 * @brief The web host's input lines: keys down and up, the old key line, pointer moves,
 *        the wheel.
 *
 * The host is a loop that does not return, so one runs on a thread of its own for the whole
 * test program and the tests talk to it the way the player does: WebSocket lines, and the
 * HTTP fallback. What it did is read back through its hooks -- `input` logs every key and
 * hands every event on to the view, `controller` counts what reaches it after `input` and
 * how often the host updates it, and `tick`, which runs on the host's thread once a pass,
 * copies the scroll the view is at.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/test/ui/draw_log.h>

#    include <lh/os/controller.h>
#    include <lh/os/input.h>
#    include <lh/os/net.h>
#    include <lh/os/net/socket.h>
#    include <lh/os/tick.h>
#    include <lh/ui/container.h>
#    include <lh/ui/scrollbar.h>
#    include <lh/ui/view.h>
#    include <lh/ui/web/host.h>
#    include <lh/ui/web/ws.h>

#    include <atomic>
#    include <chrono>
#    include <cstring>
#    include <string>
#    include <thread>

namespace
{

constexpr int key_log_max = 32;
constexpr lh_u64_t wait_ms = 5000u;

/* root 200x200 > box (0,0) 100x100 over content 100x400, and its vertical bar
   (100,0) 10x200: thumb 50 of a 200 track, scroll 0..300. */
struct host_world
{
    lh_ui_entity_t root;
    lh_ui_container_t box;
    lh_ui_entity_t content;
    lh_ui_scrollbar_t down;
    lh_ui_view_t view;
    lh_ui_web_host_hooks_t hooks;
    char json[256u * 1024u];
    lh_net_port_t port;
    std::atomic<int> started;
    std::atomic<int> failed;
    std::atomic<int> keys;
    int key[key_log_max];
    int pressed[key_log_max];
    std::atomic<int> ticks;
    std::atomic<int> scroll_y;
    lh_os_controller_t controller;
    std::atomic<int> controlled_presses;
    std::atomic<int> controlled_clicks;
    std::atomic<int> controller_updates;
};

host_world s_world;

lh_void
world_tick(lh_ptr ctx)
{
    auto *world = static_cast<host_world *>(ctx);
    lh_ui_point_t scroll;

    lh_ui_container_get_clamped_scroll(lh_addr_of(world->box), lh_addr_of(scroll));
    world->scroll_y.store(static_cast<int>(lh_ui_point_get_y(lh_addr_of(scroll))));
    world->ticks.fetch_add(1);
}

/* Every line is one event: a key is logged, and everything goes on to the view, as an
   application's own sink would pass it. */
lh_void
world_input(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *world = static_cast<host_world *>(self);
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);

    if (kind == lh_os_input_kind_key_down || kind == lh_os_input_kind_key_up)
    {
        const int at = world->keys.load();

        if (at < key_log_max)
        {
            world->key[at] = static_cast<int>(lh_os_input_event_get_key(event));
            world->pressed[at] = kind == lh_os_input_kind_key_down ? 1 : 0;
        }
        world->keys.store(at + 1);
    }
    (void)lh_ui_view_input(lh_addr_of(world->view), event);
}

/* The controller's handler: presses and the recognizer's clicks, counted. */
lh_bool_t
world_control(lh_self_ptr self, const lh_os_input_event_ptr event)
{
    auto *world = static_cast<host_world *>(self);
    const lh_os_input_kind_t kind = lh_os_input_event_get_kind(event);

    if (kind == lh_os_input_kind_pointer_down)
    {
        world->controlled_presses.fetch_add(1);
    }
    if (kind == lh_os_input_kind_click)
    {
        world->controlled_clicks.fetch_add(1);
    }
    return lh_bool_false;
}

lh_void
world_update(lh_self_ptr self, lh_tick_t /*now*/)
{
    static_cast<host_world *>(self)->controller_updates.fetch_add(1);
}

/* A port nothing listens on right now: bind to 0, read what was given, let it go. */
lh_net_port_t
free_port()
{
    lh_os_net_socket_t probe;
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;
    lh_net_port_t port = 0u;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), 0);
    lh_os_net_socket_init(lh_addr_of(probe));
    if (lh_os_net_socket_open(lh_addr_of(probe), lh_os_system_net_socket_type_tcp) &&
        lh_os_net_socket_bind(lh_addr_of(probe), lh_addr_of(addr)) &&
        lh_os_net_socket_get_local_addr(lh_addr_of(probe), lh_addr_of(addr)))
    {
        port = lh_net_ip4_socket_addr_get_port(lh_addr_of(addr));
    }
    lh_os_net_socket_close(lh_addr_of(probe));
    return port;
}

bool
connect_to_host(lh_os_net_socket_t *sock)
{
    lh_net_ip4_t ip;
    lh_net_ip4_socket_addr_t addr;

    lh_net_ip4_init(lh_addr_of(ip), 127, 0, 0, 1);
    lh_net_ip4_socket_addr_init(lh_addr_of(addr), lh_addr_of(ip), s_world.port);
    lh_os_net_socket_init(sock);
    if (!lh_os_net_socket_open(sock, lh_os_system_net_socket_type_tcp))
    {
        return false;
    }
    if (!lh_os_net_socket_connect(sock, lh_addr_of(addr)))
    {
        lh_os_net_socket_close(sock);
        return false;
    }
    return true;
}

/* The host's own thread: it returns only when the host would not listen. */
void
host_thread()
{
    (void)lh_ui_web_host_run(lh_addr_of(s_world.view), 200u, 200u, s_world.json,
                             sizeof(s_world.json), s_world.port, lh_addr_of(s_world.hooks));
    s_world.failed.store(1);
}

/* Start the one host of this program, once; false when it would not listen. */
bool
host_up()
{
    static bool tried = false;
    static bool up = false;

    if (tried)
    {
        return up;
    }
    tried = true;
    if (!lh_os_net_init())
    {
        return false;
    }
    lh_ui_entity_init(lh_addr_of(s_world.root), lh_test::rect_of(0, 0, 200, 200));
    lh_ui_container_init(lh_addr_of(s_world.box), lh_test::rect_of(0, 0, 100, 100));
    lh_ui_entity_init(lh_addr_of(s_world.content), lh_test::rect_of(0, 0, 100, 400));
    lh_ui_scrollbar_init(lh_addr_of(s_world.down), lh_test::rect_of(100, 0, 10, 200),
                         lh_ui_axis_vertical, lh_addr_of(s_world.box));
    lh_ui_entity_add_child(lh_ui_container_as_entity(lh_addr_of(s_world.box)),
                           lh_addr_of(s_world.content));
    lh_ui_entity_add_child(lh_addr_of(s_world.root),
                           lh_ui_container_as_entity(lh_addr_of(s_world.box)));
    lh_ui_entity_add_child(lh_addr_of(s_world.root),
                           lh_ui_scrollbar_as_entity(lh_addr_of(s_world.down)));
    lh_ui_view_init(lh_addr_of(s_world.view));
    lh_ui_view_set_root_v(lh_addr_of(s_world.view), lh_addr_of(s_world.root));
    std::memset(lh_addr_of(s_world.hooks), 0, sizeof(s_world.hooks));
    s_world.hooks.ctx = lh_addr_of(s_world);
    s_world.hooks.tick = world_tick;
    s_world.hooks.input = world_input;
    lh_os_controller_init(lh_addr_of(s_world.controller), world_control, lh_addr_of(s_world),
                          lh_null);
    lh_os_controller_set_update_cb(lh_addr_of(s_world.controller), world_update);
    s_world.hooks.controller = lh_addr_of(s_world.controller);
    s_world.port = free_port();
    if (s_world.port == 0u)
    {
        return false;
    }
    /* Never joined: the host returns only when it could not listen, and the process
       ending is what stops it otherwise. */
    std::thread(host_thread).detach();
    /* Up once it answers a connection and has run a pass. */
    const lh_u64_t t0 = lh_os_tick_ms();
    while (lh_os_tick_ms() - t0 < wait_ms && s_world.failed.load() == 0)
    {
        lh_os_net_socket_t probe;

        if (s_world.ticks.load() > 0 && connect_to_host(lh_addr_of(probe)))
        {
            lh_os_net_socket_close(lh_addr_of(probe));
            up = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return up;
}

#    define LH_TEST_NEEDS_HOST()                                                                   \
        if (!host_up())                                                                            \
        GTEST_SKIP() << "the web host would not listen on loopback"

bool
send_all(lh_os_net_socket_t *sock, const void *bytes, lh_usize_t len)
{
    const char *p = static_cast<const char *>(bytes);

    while (len > 0u)
    {
        const lh_ssize_t n = lh_os_net_socket_send(sock, const_cast<char *>(p), len);

        if (n <= 0)
        {
            return false;
        }
        p += n;
        len -= static_cast<lh_usize_t>(n);
    }
    return true;
}

/* Everything up to the blank line that ends an HTTP head, or up to the peer closing. */
std::string
read_head(lh_os_net_socket_t *sock)
{
    std::string head;
    char byte;

    while (head.find("\r\n\r\n") == std::string::npos &&
           lh_os_net_socket_recv(sock, lh_addr_of(byte), 1u) == 1)
    {
        head += byte;
    }
    return head;
}

/* One HTTP fallback request; its status line. The host answers and closes. */
std::string
http_get(const std::string &path)
{
    lh_os_net_socket_t sock;
    const std::string req = "GET " + path + " HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    std::string head;

    if (!connect_to_host(lh_addr_of(sock)))
    {
        return head;
    }
    if (send_all(lh_addr_of(sock), req.data(), req.size()))
    {
        head = read_head(lh_addr_of(sock));
    }
    lh_os_net_socket_close(lh_addr_of(sock));
    return head.substr(0, head.find("\r\n"));
}

/* A WebSocket the way the player opens one. Frames the host pushes back are not read. */
struct ws_client
{
    lh_os_net_socket_t sock;
    bool open;

    ws_client() : open(false)
    {
        static const char upgrade[] = "GET /ws HTTP/1.1\r\nHost: 127.0.0.1\r\n"
                                      "Upgrade: websocket\r\nConnection: Upgrade\r\n"
                                      "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                                      "Sec-WebSocket-Version: 13\r\n\r\n";

        if (!connect_to_host(lh_addr_of(sock)))
        {
            return;
        }
        open = send_all(lh_addr_of(sock), upgrade, sizeof(upgrade) - 1u) &&
               read_head(lh_addr_of(sock)).find(" 101 ") != std::string::npos;
    }

    ~ws_client()
    {
        lh_os_net_socket_close(lh_addr_of(sock));
    }

    bool
    line(const char *text)
    {
        unsigned char frame[64];
        const lh_usize_t n = lh_ui_web_ws_write(frame, sizeof(frame), LH_UI_WEB_WS_TEXT,
                                                lh_bool_true, text, std::strlen(text));

        return n != 0u && send_all(lh_addr_of(sock), frame, n);
    }
};

/* Wait until @p done holds or the time is up; whether it held. */
template <typename F>
bool
wait_for(F done)
{
    const lh_u64_t t0 = lh_os_tick_ms();

    while (!done())
    {
        if (lh_os_tick_ms() - t0 >= wait_ms)
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return true;
}

/* Wait for @p count more keys than @p base; whether they came. */
bool
wait_keys(int base, int count)
{
    return wait_for([&] { return s_world.keys.load() >= base + count; });
}

/* Wait for two passes of the host past now, so its tick has seen every input before. */
void
wait_passes()
{
    const int t = s_world.ticks.load();

    (void)wait_for([&] { return s_world.ticks.load() >= t + 2; });
}

} // namespace

TEST(web_host, ws_key_lines_carry_the_direction_and_the_old_line_is_a_key_down)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);
    const int base = s_world.keys.load();

    ASSERT_TRUE(ws.line("k 6 1"));
    ASSERT_TRUE(ws.line("k 6 0"));
    ASSERT_TRUE(ws.line("k 3"));
    ASSERT_TRUE(wait_keys(base, 3));
    ASSERT_LE(base + 3, key_log_max);
    EXPECT_EQ(s_world.key[base], static_cast<int>(lh_os_key_space));
    EXPECT_EQ(s_world.pressed[base], 1);
    EXPECT_EQ(s_world.key[base + 1], static_cast<int>(lh_os_key_space));
    EXPECT_EQ(s_world.pressed[base + 1], 0);
    EXPECT_EQ(s_world.key[base + 2], static_cast<int>(lh_os_key_escape));
    EXPECT_EQ(s_world.pressed[base + 2], 1);
}

TEST(web_host, http_key_takes_p_and_without_it_is_a_key_down)
{
    LH_TEST_NEEDS_HOST();
    const int base = s_world.keys.load();

    EXPECT_NE(http_get("/ui/key?k=5&p=0").find(" 200 "), std::string::npos);
    ASSERT_TRUE(wait_keys(base, 1));
    EXPECT_NE(http_get("/ui/key?k=5").find(" 200 "), std::string::npos);
    ASSERT_TRUE(wait_keys(base, 2));
    ASSERT_LE(base + 2, key_log_max);
    EXPECT_EQ(s_world.key[base], static_cast<int>(lh_os_key_delete));
    EXPECT_EQ(s_world.pressed[base], 0);
    EXPECT_EQ(s_world.key[base + 1], static_cast<int>(lh_os_key_delete));
    EXPECT_EQ(s_world.pressed[base + 1], 1);
}

/* Thumb 50 of a 200 track over scroll 0..300: dragging its top from 0 to 75 is scroll 150,
   and back to 0 is scroll 0 -- once as WebSocket lines, once as HTTP requests. */
TEST(web_host, a_move_while_held_drags_the_thumb_and_one_not_held_does_nothing)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);
    const int base = s_world.keys.load();

    wait_passes();
    ASSERT_EQ(s_world.scroll_y.load(), 0);
    ASSERT_TRUE(ws.line("p 105 10"));
    ASSERT_TRUE(ws.line("m 105 85"));
    ASSERT_TRUE(ws.line("r 105 85"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 150; }));

    /* Nothing held: a move is no drag. The key after it is the mark that it was read. */
    ASSERT_TRUE(ws.line("m 105 10"));
    ASSERT_TRUE(ws.line("k 0 1"));
    ASSERT_TRUE(wait_keys(base, 1));
    wait_passes();
    EXPECT_EQ(s_world.scroll_y.load(), 150);

    EXPECT_NE(http_get("/ui/press?x=105&y=85").find(" 200 "), std::string::npos);
    EXPECT_NE(http_get("/ui/move?x=105&y=10").find(" 200 "), std::string::npos);
    EXPECT_NE(http_get("/ui/release?x=105&y=10").find(" 200 "), std::string::npos);
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 0; }));
}

/* A `w` line is a notch, positive up: one down scrolls the view one step, one up brings it
   back -- the same direction a desktop wheel scrolls. The step is the host machine's
   (lh_os_input_get_wheel_step): the view asks the OS layer, not the browser. */
TEST(web_host, a_wheel_line_scrolls_one_step_per_notch)
{
    LH_TEST_NEEDS_HOST();
    ws_client ws;
    ASSERT_TRUE(ws.open);

    wait_passes();
    ASSERT_EQ(s_world.scroll_y.load(), 0);
    ASSERT_TRUE(ws.line("w 50 50 0 -1"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == lh_os_input_get_wheel_step(); }));
    ASSERT_TRUE(ws.line("w 50 50 0 1"));
    EXPECT_TRUE(wait_for([] { return s_world.scroll_y.load() == 0; }));
}

/* The hooks' controller takes every event after `input` and is updated at every pass:
   a click over HTTP is a press to it, and the recognizer's click after the release. */
TEST(web_host, the_controller_gets_the_events_after_input_and_is_updated_every_pass)
{
    LH_TEST_NEEDS_HOST();
    const int presses = s_world.controlled_presses.load();
    const int clicks = s_world.controlled_clicks.load();
    const int updates = s_world.controller_updates.load();

    EXPECT_NE(http_get("/ui/click?x=150&y=150").find(" 200 "), std::string::npos);
    EXPECT_TRUE(wait_for([&] { return s_world.controlled_clicks.load() == clicks + 1; }));
    EXPECT_EQ(s_world.controlled_presses.load(), presses + 1);
    wait_passes();
    EXPECT_GE(s_world.controller_updates.load(), updates + 2);
}

#endif /* LH_LIBRARY_OPTION_OS */
