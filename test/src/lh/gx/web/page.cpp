/**
 * @file page.cpp
 * @brief Tests for `lh/gx/web/page.h`.
 *
 * The page is text for a browser, so what C can check is its shape: one
 * canvas the size of the screen, the player and the transport both on it, the
 * call that starts them on the websocket path the application takes, and the
 * caller's text kept. With the JavaScript lexer built, the transport is also
 * read to its end without an error. The scripts' behaviour is checked in a
 * browser, not here.
 */

#include <gtest/gtest.h>

#include <lh/config.h>
#include <lh/gx/backend/web/player.h>
#include <lh/gx/web/page.h>

#if LH_LIBRARY_OPTION_JS
#    include <lh/core/web/js/lexer.h>
#endif

#include <string>

namespace
{

std::string
text_of(lh_str_view_t view)
{
    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

std::string
page(lh_uint_t width, lh_uint_t height)
{
    lh_str_t out;

    lh_str_init(&out);
    lh_str_append_view(&out, lh_str_view_lit("kept:"));
    lh_gx_web_page_write(&out, width, height);
    std::string text(lh_str_get_data(&out), lh_str_get_size(&out));
    lh_str_deinit(&out);
    return text;
}

TEST(gx_web_page, the_page_is_a_canvas_and_both_scripts)
{
    const std::string text = page(320U, 240U);
    const std::string player = text_of(lh_gx_backend_web_player_get_script());
    const std::string transport = text_of(lh_gx_web_page_get_script());

    EXPECT_EQ(text.rfind("kept:<!doctype html>", 0), 0U) << "appended to what was there";
    EXPECT_NE(text.find("<canvas id=\"lh\" width=\"320\" height=\"240\"></canvas>"),
              std::string::npos);
    const std::size_t at_player = text.find(player);
    const std::size_t at_transport = text.find(transport);
    const std::size_t at_start =
        text.find("lhGxConnect(document.getElementById(\"lh\"), \"" LH_GX_WEB_PAGE_WS_PATH "\");");

    ASSERT_NE(at_player, std::string::npos);
    ASSERT_NE(at_transport, std::string::npos);
    ASSERT_NE(at_start, std::string::npos);
    EXPECT_LT(at_player, at_start);
    EXPECT_LT(at_transport, at_start) << "both scripts are defined before the call";
    EXPECT_EQ(text.substr(text.size() - 24U), "</script></body></html>\n");
}

TEST(gx_web_page, the_transport_sends_the_lines_the_input_reads)
{
    const std::string transport = text_of(lh_gx_web_page_get_script());

    EXPECT_NE(transport.find("function lhGxConnect(canvas, path)"), std::string::npos);
    EXPECT_NE(transport.find("send(\"p \" + "), std::string::npos);
    EXPECT_NE(transport.find("send(\"m \" + "), std::string::npos);
    EXPECT_NE(transport.find("send(\"r \" + "), std::string::npos);
    EXPECT_NE(transport.find("send(\"c\")"), std::string::npos);
    EXPECT_NE(transport.find("lhGxPlay(ctx, JSON.parse(m.data))"), std::string::npos);
    EXPECT_NE(transport.find("document.addEventListener(\"keydown\""), std::string::npos);
    EXPECT_NE(transport.find("send(\"t \" + "), std::string::npos);
    EXPECT_NE(transport.find("send(\"k \" + e.key)"), std::string::npos);
}

#if LH_LIBRARY_OPTION_JS

TEST(gx_web_page, the_transport_lexes_to_its_end)
{
    const std::string text = text_of(lh_gx_web_page_get_script());
    lh_web_js_lexer_t lexer;
    lh_str_ptr message = nullptr;
    lh_usize_t len = 0U;

    lh_web_js_lexer_init(&lexer, text.c_str(), text.size());
    while (lh_web_js_lexer_next(&lexer))
    {
    }
    EXPECT_FALSE(lh_web_js_lexer_get_error(&lexer, &message, &len))
        << std::string(message != nullptr ? message : "", len);
    EXPECT_EQ(lh_web_js_lexer_get_pos(&lexer), text.size());
}

#endif /* LH_LIBRARY_OPTION_JS */

} // namespace
