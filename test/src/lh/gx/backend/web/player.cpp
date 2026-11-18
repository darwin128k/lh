/**
 * @file player.cpp
 * @brief Tests for `lh/gx/backend/web/player.h`.
 *
 * The player is JavaScript, so it is not run here; what is checked is what C
 * can see. The script is read to its end by the library's own JavaScript
 * lexer without an error, it defines the entry the header promises, and it
 * knows every command the web backend writes — the two sides use the same
 * names.
 */

#include <gtest/gtest.h>

#include <lh/core/web/js/lexer.h>
#include <lh/gx/backend/web/player.h>

#include <string>

namespace
{

std::string
script()
{
    const lh_str_view_t view = lh_gx_backend_web_player_get_script();

    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

TEST(gx_backend_web_player, the_script_lexes_to_its_end)
{
    const std::string text = script();
    lh_web_js_lexer_t lexer;
    lh_str_ptr message = nullptr;
    lh_usize_t len = 0U;

    ASSERT_FALSE(text.empty());
    lh_web_js_lexer_init(&lexer, text.c_str(), text.size());
    while (lh_web_js_lexer_next(&lexer))
    {
    }
    EXPECT_FALSE(lh_web_js_lexer_get_error(&lexer, &message, &len))
        << std::string(message != nullptr ? message : "", len);
    EXPECT_EQ(lh_web_js_lexer_get_pos(&lexer), text.size());
}

TEST(gx_backend_web_player, the_script_knows_every_command_of_the_backend)
{
    const std::string text = script();

    EXPECT_NE(text.find("function lhGxPlay(ctx, frame)"), std::string::npos);
    EXPECT_NE(text.find("\"fill_rect\""), std::string::npos);
    EXPECT_NE(text.find("\"fill_mask\""), std::string::npos);
    EXPECT_EQ(text.find('\0'), std::string::npos) << "the size does not count the NUL";
}

} // namespace
