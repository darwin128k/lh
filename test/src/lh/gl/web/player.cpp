/**
 * @file player.cpp
 * @brief The player's accessors: every setter stores what it is given, every
 *        mutable getter hands out the member its const getter reads, and the
 *        application the host serves carries the player itself.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/gl/view.h>
#    include <lh/gl/view/sink.h>
#    include <lh/gl/web/player.h>

#    include <cstring>

namespace
{

/* What a player needs to start; it lives as long as the player, for the player keeps
   pointers into it. */
struct player_world
{
    lh_gl_view_t view;
    lh_char_t json[256];
    lh_byte_t page[LH_GL_WEB_PLAYER_PAGE_CAP];
    lh_gl_web_player_t player;
};

/* An input hook that does nothing: with it the player puts no recognizer of its own in
   front. */
void
ignore_input(lh_ptr ctx, const lh_input_event_t *event)
{
    (void)ctx;
    (void)event;
}

/* Start the player of @p world, with @p hooks; true when its document built. */
bool
start(player_world *world, const lh_os_web_host_hooks_t *hooks)
{
    lh_gl_view_init(lh_addr_of(world->view));
    return lh_gl_web_player_init(lh_addr_of(world->player), lh_addr_of(world->view), 200u, 100u,
                                 world->json, sizeof(world->json), world->page, sizeof(world->page),
                                 hooks) == lh_bool_true;
}

} // namespace

TEST(ui_web_player, init_stores_what_it_is_given)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_gl_web_player_get_view(player), lh_addr_of(world.view));
    EXPECT_EQ(lh_gl_web_player_get_width(player), 200u);
    EXPECT_EQ(lh_gl_web_player_get_height(player), 100u);
    EXPECT_EQ(lh_gl_web_player_get_json(player), world.json);
    EXPECT_EQ(lh_gl_web_player_get_json_cap(player), sizeof(world.json));
    EXPECT_EQ(lh_gl_web_player_get_page(player), world.page);
    EXPECT_EQ(lh_gl_web_player_get_page_cap(player), sizeof(world.page));
    EXPECT_GT(lh_gl_web_player_get_page_len(player), 0u);
    EXPECT_LE(lh_gl_web_player_get_page_len(player), sizeof(world.page));
    EXPECT_EQ(lh_gl_web_player_get_hooks(player), lh_null);
}

/* The handlers get the context back as the player: never the address of a local that held
   a pointer to it. */
TEST(ui_web_player, the_application_carries_the_player_itself)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_os_web_app_get_ctx(lh_gl_web_player_get_app(player)), static_cast<lh_ptr>(player));
}

TEST(ui_web_player, without_hooks_its_own_recognizer_stands_in_front_of_the_view)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_gl_web_player_get_routed_recognizer(player),
              lh_gl_web_player_get_recognizer_mut(player));
    EXPECT_EQ(lh_gl_web_player_get_sink(player), lh_gl_view_sink);
    EXPECT_EQ(lh_gl_web_player_get_sink_context(player), static_cast<lh_ptr>(&world.view));
}

TEST(ui_web_player, with_an_input_hook_alone_it_sends_raw_events_to_the_hook)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    lh_os_web_host_hooks_t hooks;
    int ctx = 0;

    std::memset(lh_addr_of(hooks), 0, sizeof(hooks));
    lh_os_web_host_hooks_set_ctx(lh_addr_of(hooks), &ctx);
    lh_os_web_host_hooks_set_input_fn(lh_addr_of(hooks), ignore_input);
    ASSERT_TRUE(start(lh_addr_of(world), lh_addr_of(hooks)));
    EXPECT_EQ(lh_gl_web_player_get_routed_recognizer(player), lh_null);
    EXPECT_EQ(lh_gl_web_player_get_sink(player), ignore_input);
    EXPECT_EQ(lh_gl_web_player_get_sink_context(player), static_cast<lh_ptr>(&ctx));
    EXPECT_EQ(lh_gl_web_player_get_hooks(player), lh_addr_of(hooks));
}

TEST(ui_web_player, a_recognizer_in_the_hooks_stands_in_front_of_the_view)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    lh_os_web_host_hooks_t hooks;
    static lh_input_recognizer_t theirs;

    std::memset(lh_addr_of(hooks), 0, sizeof(hooks));
    lh_input_recognizer_init(&theirs, lh_null);
    lh_os_web_host_hooks_set_recognizer(lh_addr_of(hooks), &theirs);
    ASSERT_TRUE(start(lh_addr_of(world), lh_addr_of(hooks)));
    EXPECT_EQ(lh_gl_web_player_get_routed_recognizer(player), &theirs);
    EXPECT_EQ(lh_gl_web_player_get_sink(player), lh_gl_view_sink);
}

/* A move over nothing changes nothing, so it asks for no frame; a press that lands on
   nothing either. The answer is the view's damage. */
TEST(ui_web_player, input_answers_whether_the_view_has_something_to_draw)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    lh_gl_rects_t damage;

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    lh_gl_view_take_damage(lh_addr_of(world.view), &damage);
    EXPECT_FALSE(lh_gl_web_player_pointer(player, lh_input_kind_pointer_move, 10, 10));
    lh_gl_rect_t changed;
    lh_gl_rect_init(&changed, 0, 0, 10, 10);
    lh_gl_view_add_damage(lh_addr_of(world.view), &changed);
    EXPECT_TRUE(lh_gl_web_player_pointer(player, lh_input_kind_pointer_move, 10, 10));
}

TEST(ui_web_player, setters)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    static lh_gl_view_t other_view;
    static lh_char_t other_json[16] = {};
    static lh_byte_t other_page[16] = {};
    lh_os_web_host_hooks_t hooks;

    std::memset(lh_addr_of(hooks), 0, sizeof(hooks));
    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    lh_gl_web_player_set_view(player, lh_addr_of(other_view));
    lh_gl_web_player_set_json(player, other_json);
    lh_gl_web_player_set_json_cap(player, sizeof(other_json));
    lh_gl_web_player_set_page(player, other_page);
    lh_gl_web_player_set_page_cap(player, sizeof(other_page));
    lh_gl_web_player_set_page_len(player, 7u);
    lh_gl_web_player_set_hooks(player, lh_addr_of(hooks));
    EXPECT_EQ(lh_gl_web_player_get_view(player), lh_addr_of(other_view));
    EXPECT_EQ(lh_gl_web_player_get_json(player), other_json);
    EXPECT_EQ(lh_gl_web_player_get_json_cap(player), sizeof(other_json));
    EXPECT_EQ(lh_gl_web_player_get_page(player), other_page);
    EXPECT_EQ(lh_gl_web_player_get_page_cap(player), sizeof(other_page));
    EXPECT_EQ(lh_gl_web_player_get_page_len(player), 7u);
    EXPECT_EQ(lh_gl_web_player_get_hooks(player), lh_addr_of(hooks));
}

TEST(ui_web_player, a_mutable_getter_hands_out_the_member_its_const_getter_reads)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    const lh_gl_web_player_t *cplayer = player;

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_gl_web_player_get_routed(cplayer), lh_gl_web_player_get_routed_mut(player));
    EXPECT_EQ(lh_gl_web_player_get_recognizer(cplayer),
              lh_gl_web_player_get_recognizer_mut(player));
    EXPECT_EQ(lh_gl_web_player_get_web(cplayer), lh_gl_web_player_get_web_mut(player));
}

TEST(ui_web_player, a_restart_forgets_the_glyphs_the_clients_held)
{
    static player_world world;
    lh_gl_web_player_t *player = lh_addr_of(world.player);
    const lh_char_t *bytes = lh_null;
    lh_usize_t len = 0u;

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    lh_byte_t *sent = lh_gl_canvas_web_glyphs_get_sent_mut(lh_gl_web_player_get_glyphs_mut(player));
    EXPECT_EQ(sent[LH_GL_CANVAS_WEB_GLYPHS_MAX - 1u], 0u) << "a new player has sent nothing";

    /* The view draws no text: the mark set by hand is the only one. */
    sent[LH_GL_CANVAS_WEB_GLYPHS_MAX - 1u] = 1u;
    lh_gl_web_player_frame(player, lh_bool_false, &bytes, &len);
    EXPECT_EQ(sent[LH_GL_CANVAS_WEB_GLYPHS_MAX - 1u], 1u) << "the clients still hold it";
    lh_gl_web_player_frame(player, lh_bool_true, &bytes, &len);
    EXPECT_EQ(sent[LH_GL_CANVAS_WEB_GLYPHS_MAX - 1u], 0u) << "a new client holds nothing";
}

#endif /* LH_LIBRARY_OPTION_OS */
