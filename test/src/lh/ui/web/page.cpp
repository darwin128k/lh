/**
 * @file page.cpp
 * @brief The player page: its key table is the enum's, and it sends key-up and moves.
 */

#include <gtest/gtest.h>

#include <lh/os/key.h>
#include <lh/ui/web/page.h>

#include <string>
#include <vector>

namespace
{

/* The player as text, built into as much room as the host gives it (48 KB). */
std::string
page_text()
{
    std::vector<lh_byte_t> mem(48u * 1024u);
    lh_usize_t len = 0u;

    EXPECT_EQ(lh_ui_web_page_build(mem.data(), mem.size(), lh_addr_of(len)), lh_bool_true);
    return std::string(reinterpret_cast<const char *>(mem.data()), len);
}

/* `'<name>':<the enum's number>` -- what the key table has to say for one key. */
std::string
entry(const char *name, lh_os_key_t key)
{
    return std::string("'") + name + "':" + std::to_string(static_cast<int>(key));
}

} // namespace

TEST(web_page, key_table_carries_every_key_by_its_enum_value)
{
    const std::string page = page_text();

    EXPECT_NE(page.find(entry("Tab", lh_os_key_tab)), std::string::npos);
    EXPECT_NE(page.find(entry("Enter", lh_os_key_enter)), std::string::npos);
    EXPECT_NE(page.find(entry("Escape", lh_os_key_escape)), std::string::npos);
    EXPECT_NE(page.find(entry("Backspace", lh_os_key_backspace)), std::string::npos);
    EXPECT_NE(page.find(entry("Delete", lh_os_key_delete)), std::string::npos);
    EXPECT_NE(page.find(entry(" ", lh_os_key_space)), std::string::npos);
    EXPECT_NE(page.find(entry("ArrowLeft", lh_os_key_left)), std::string::npos);
    EXPECT_NE(page.find(entry("ArrowRight", lh_os_key_right)), std::string::npos);
    EXPECT_NE(page.find(entry("ArrowUp", lh_os_key_up)), std::string::npos);
    EXPECT_NE(page.find(entry("ArrowDown", lh_os_key_down)), std::string::npos);
    EXPECT_NE(page.find(entry("Home", lh_os_key_home)), std::string::npos);
    EXPECT_NE(page.find(entry("End", lh_os_key_end)), std::string::npos);
    EXPECT_NE(page.find(entry("PageUp", lh_os_key_page_up)), std::string::npos);
    EXPECT_NE(page.find(entry("PageDown", lh_os_key_page_down)), std::string::npos);
    EXPECT_NE(page.find(entry("Shift", lh_os_key_shift)), std::string::npos);
    EXPECT_NE(page.find(entry("Control", lh_os_key_control)), std::string::npos);
    EXPECT_NE(page.find(entry("Alt", lh_os_key_alt)), std::string::npos);
}

TEST(web_page, keys_go_down_and_up_with_a_direction)
{
    const std::string page = page_text();

    EXPECT_NE(page.find("document.onkeydown"), std::string::npos);
    EXPECT_NE(page.find("document.onkeyup"), std::string::npos);
    EXPECT_NE(page.find("emit('k '+k+' 1')"), std::string::npos);
    EXPECT_NE(page.find("emit('k '+k+' 0')"), std::string::npos);
    /* The HTTP fallback carries the direction too. */
    EXPECT_NE(page.find("'&p='"), std::string::npos);
}

TEST(web_page, a_held_button_sends_moves_and_the_last_before_the_release)
{
    const std::string page = page_text();
    const std::string::size_type up = page.find("window.onmouseup");

    EXPECT_NE(page.find("window.onmousemove"), std::string::npos);
    EXPECT_NE(page.find("emit('m '+"), std::string::npos);
    EXPECT_NE(page.find("/ui/move?x="), std::string::npos);
    ASSERT_NE(up, std::string::npos);
    EXPECT_LT(page.find("flushMove()", up), page.find("emit('r '", up));
}
