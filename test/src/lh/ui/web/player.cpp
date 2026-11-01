/**
 * @file player.cpp
 * @brief The player's accessors: every setter stores what it is given, every
 *        mutable getter hands out the member its const getter reads, and the
 *        application the host serves carries the player itself.
 */

#include <gtest/gtest.h>

#include <lh/config.h>

#if LH_LIBRARY_OPTION_OS

#    include <lh/os/controller.h>
#    include <lh/os/input.h>
#    include <lh/ui/view.h>
#    include <lh/ui/web/player.h>

#    include <cstring>

namespace
{

/* What a player needs to start; it lives as long as the player, for the player keeps
   pointers into it. */
struct player_world
{
    lh_ui_view_t view;
    lh_char_t json[256];
    lh_byte_t page[LH_UI_WEB_PLAYER_PAGE_CAP];
    lh_ui_web_player_t player;
};

/* An input hook that does nothing: with it the player keeps no controller of its own. */
void
ignore_input(lh_ptr ctx, const lh_os_input_event_t *event)
{
    (void)ctx;
    (void)event;
}

/* Start the player of @p world, with @p hooks; true when its document built. */
bool
start(player_world *world, const lh_ui_web_host_hooks_t *hooks)
{
    lh_ui_view_init(lh_addr_of(world->view));
    return lh_ui_web_player_init(lh_addr_of(world->player), lh_addr_of(world->view), 200u, 100u,
                                 world->json, sizeof(world->json), world->page,
                                 sizeof(world->page), hooks) == lh_bool_true;
}

} // namespace

TEST(ui_web_player, init_stores_what_it_is_given)
{
    static player_world world;
    lh_ui_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_ui_web_player_get_view(player), lh_addr_of(world.view));
    EXPECT_EQ(lh_ui_web_player_get_width(player), 200u);
    EXPECT_EQ(lh_ui_web_player_get_height(player), 100u);
    EXPECT_EQ(lh_ui_web_player_get_json(player), world.json);
    EXPECT_EQ(lh_ui_web_player_get_json_cap(player), sizeof(world.json));
    EXPECT_EQ(lh_ui_web_player_get_page(player), world.page);
    EXPECT_EQ(lh_ui_web_player_get_page_cap(player), sizeof(world.page));
    EXPECT_GT(lh_ui_web_player_get_page_len(player), 0u);
    EXPECT_LE(lh_ui_web_player_get_page_len(player), sizeof(world.page));
    EXPECT_EQ(lh_ui_web_player_get_hooks(player), lh_null);
}

/* The handlers get the context back as the player: never the address of a local that held
   a pointer to it. */
TEST(ui_web_player, the_application_carries_the_player_itself)
{
    static player_world world;
    lh_ui_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_ui_web_app_get_ctx(lh_ui_web_player_get_app(player)),
              static_cast<lh_ptr>(player));
}

TEST(ui_web_player, without_hooks_it_routes_to_its_own_controller)
{
    static player_world world;
    lh_ui_web_player_t *player = lh_addr_of(world.player);

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_ui_web_player_get_owns_controller(player), lh_bool_true);
    EXPECT_EQ(lh_ui_web_player_get_routed_controller(player),
              lh_ui_web_player_get_controller_mut(player));
    EXPECT_EQ(lh_ui_web_host_hooks_get_controller(lh_ui_web_player_get_routed(player)),
              lh_ui_web_player_get_controller_mut(player));
}

TEST(ui_web_player, with_an_input_hook_it_keeps_no_controller)
{
    static player_world world;
    lh_ui_web_player_t *player = lh_addr_of(world.player);
    lh_ui_web_host_hooks_t hooks;

    std::memset(lh_addr_of(hooks), 0, sizeof(hooks));
    lh_ui_web_host_hooks_set_input_fn(lh_addr_of(hooks), ignore_input);
    ASSERT_TRUE(start(lh_addr_of(world), lh_addr_of(hooks)));
    EXPECT_EQ(lh_ui_web_player_get_owns_controller(player), lh_bool_false);
    EXPECT_EQ(lh_ui_web_player_get_routed_controller(player), lh_null);
    EXPECT_EQ(lh_ui_web_host_hooks_get_input_fn(lh_ui_web_player_get_routed(player)),
              ignore_input);
    EXPECT_EQ(lh_ui_web_player_get_hooks(player), lh_addr_of(hooks));
}

TEST(ui_web_player, setters)
{
    static player_world world;
    static lh_ui_view_t other_view;
    static lh_char_t other_json[16];
    static lh_byte_t other_page[16];
    lh_ui_web_player_t *player = lh_addr_of(world.player);
    lh_ui_web_host_hooks_t hooks;

    std::memset(lh_addr_of(hooks), 0, sizeof(hooks));
    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    lh_ui_web_player_set_view(player, lh_addr_of(other_view));
    lh_ui_web_player_set_json(player, other_json);
    lh_ui_web_player_set_json_cap(player, sizeof(other_json));
    lh_ui_web_player_set_page(player, other_page);
    lh_ui_web_player_set_page_cap(player, sizeof(other_page));
    lh_ui_web_player_set_page_len(player, 7u);
    lh_ui_web_player_set_hooks(player, lh_addr_of(hooks));
    lh_ui_web_player_set_owns_controller(player, lh_bool_false);
    EXPECT_EQ(lh_ui_web_player_get_view(player), lh_addr_of(other_view));
    EXPECT_EQ(lh_ui_web_player_get_json(player), other_json);
    EXPECT_EQ(lh_ui_web_player_get_json_cap(player), sizeof(other_json));
    EXPECT_EQ(lh_ui_web_player_get_page(player), other_page);
    EXPECT_EQ(lh_ui_web_player_get_page_cap(player), sizeof(other_page));
    EXPECT_EQ(lh_ui_web_player_get_page_len(player), 7u);
    EXPECT_EQ(lh_ui_web_player_get_hooks(player), lh_addr_of(hooks));
    EXPECT_EQ(lh_ui_web_player_get_owns_controller(player), lh_bool_false);
}

TEST(ui_web_player, a_mutable_getter_hands_out_the_member_its_const_getter_reads)
{
    static player_world world;
    lh_ui_web_player_t *player = lh_addr_of(world.player);
    const lh_ui_web_player_t *cplayer = player;

    ASSERT_TRUE(start(lh_addr_of(world), lh_null));
    EXPECT_EQ(lh_ui_web_player_get_routed(cplayer), lh_ui_web_player_get_routed_mut(player));
    EXPECT_EQ(lh_ui_web_player_get_controller(cplayer),
              lh_ui_web_player_get_controller_mut(player));
    EXPECT_EQ(lh_ui_web_player_get_web(cplayer), lh_ui_web_player_get_web_mut(player));
}

#endif /* LH_LIBRARY_OPTION_OS */
