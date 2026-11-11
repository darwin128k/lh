#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <lh/os/loader/list.h>
#include <lh/os/system/name.h>

namespace
{

lh_void
collect(lh_ptr user, const lh_str_view_t *entry)
{
    static_cast<std::vector<std::string> *>(user)->emplace_back(lh_str_view_get_data(entry),
                                                                lh_str_view_get_size(entry));
}

std::string
system_name()
{
    lh_str_view_t view;
    lh_os_system_get_name(&view);
    return std::string(lh_str_view_get_data(&view), lh_str_view_get_size(&view));
}

TEST(os_loader_list, is_skipped_for_blank_and_comment_lines)
{
    const lh_str_view_t blank = lh_str_view_lit("");
    const lh_str_view_t semicolon = lh_str_view_lit("; note");
    const lh_str_view_t hash = lh_str_view_lit("# note");
    const lh_str_view_t entry = lh_str_view_lit("qproto");

    EXPECT_TRUE(lh_os_loader_list_is_skipped(&blank));
    EXPECT_TRUE(lh_os_loader_list_is_skipped(&semicolon));
    EXPECT_TRUE(lh_os_loader_list_is_skipped(&hash));
    EXPECT_FALSE(lh_os_loader_list_is_skipped(&entry));
}

TEST(os_loader_list, try_get_section_reads_a_trimmed_name)
{
    const lh_str_view_t good = lh_str_view_lit("[ linux ]");
    const lh_str_view_t empty = lh_str_view_lit("[  ]");
    const lh_str_view_t nested = lh_str_view_lit("[a]b]");
    const lh_str_view_t open = lh_str_view_lit("[linux");
    lh_str_view_t name = lh_str_view_lit("untouched");

    EXPECT_FALSE(lh_os_loader_list_try_get_section(&empty, &name));
    EXPECT_FALSE(lh_os_loader_list_try_get_section(&nested, &name));
    EXPECT_FALSE(lh_os_loader_list_try_get_section(&open, &name));
    EXPECT_EQ(lh_str_view_get_size(&name), 9U);
    ASSERT_TRUE(lh_os_loader_list_try_get_section(&good, &name));
    const lh_str_view_t want = lh_str_view_lit("linux");
    EXPECT_TRUE(lh_str_view_equals(&name, &want, lh_bool_false));
}

TEST(os_loader_list, is_section_active_for_none_any_modules_and_this_system)
{
    const lh_str_view_t none = lh_str_view_lit("");
    const lh_str_view_t any = lh_str_view_lit("ANY");
    const lh_str_view_t modules = lh_str_view_lit("modules");
    const lh_str_view_t other = lh_str_view_lit("amiga");
    const std::string own = system_name();
    lh_str_view_t mine;
    lh_str_view_init_by_size(&mine, own.data(), own.size());

    EXPECT_TRUE(lh_os_loader_list_is_section_active(&none));
    EXPECT_TRUE(lh_os_loader_list_is_section_active(&any));
    EXPECT_TRUE(lh_os_loader_list_is_section_active(&modules));
    EXPECT_TRUE(lh_os_loader_list_is_section_active(&mine));
    EXPECT_FALSE(lh_os_loader_list_is_section_active(&other));
}

TEST(os_loader_list, try_read_hands_the_entries_for_this_system_in_order)
{
    const std::string text = "\xEF\xBB\xBF; modules\r\n"
                             "  first  \n"
                             "[amiga]\n"
                             "never\n"
                             "[" +
                             system_name() +
                             "]\n"
                             "# skipped\n"
                             "second\n"
                             "[any]\n"
                             "third\n";
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, text.data(), text.size());
    std::vector<std::string> found;
    lh_usize_t bad = 0U;

    ASSERT_TRUE(lh_os_loader_list_try_read(&view, collect, &found, &bad));
    EXPECT_EQ(found, (std::vector<std::string>{"first", "second", "third"}));
    EXPECT_EQ(bad, 0U);
}

TEST(os_loader_list, try_read_stops_at_a_malformed_section)
{
    const std::string text = "first\n\n[broken\nsecond\n";
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, text.data(), text.size());
    std::vector<std::string> found;
    lh_usize_t bad = 0U;

    EXPECT_FALSE(lh_os_loader_list_try_read(&view, collect, &found, &bad));
    EXPECT_EQ(found, (std::vector<std::string>{"first"}));
    EXPECT_EQ(bad, 3U);
}

} // namespace
