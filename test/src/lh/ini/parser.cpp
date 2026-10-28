#include <gtest/gtest.h>

#include <lh/ini.h>

#include <string>
#include <vector>

namespace
{

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v), lh_str_view_get_size(&v));
}

/* Each entry as "[section]" or "section.key=value". */
struct parsed
{
    std::vector<std::string> entries;
    lh_ini_error_t error;
    lh_usize_t line;
};

parsed
parse(const std::string &ini)
{
    lh_str_view_t view;
    if (ini.empty())
    {
        lh_str_view_init_empty(&view);
    }
    else
    {
        lh_str_view_init_by_size(&view, ini.data(), ini.size());
    }

    lh_ini_parser_t parser;
    lh_ini_parser_init(&parser, view);

    parsed out;
    lh_ini_entry_t entry;
    while (lh_ini_parser_next(&parser, &entry))
    {
        if (lh_ini_entry_has_kind(&entry, lh_ini_entry_kind_section))
        {
            out.entries.push_back("[" + text(lh_ini_entry_get_section(&entry)) + "]");
        }
        else
        {
            out.entries.push_back(text(lh_ini_entry_get_section(&entry)) + "." +
                                  text(lh_ini_entry_get_key(&entry)) + "=" +
                                  text(lh_ini_entry_get_value(&entry)));
        }
    }
    out.error = lh_ini_parser_get_error(&parser);
    out.line = lh_ini_parser_get_line(&parser);
    return out;
}

using entries_t = std::vector<std::string>;

TEST(ini_parser, reads_sections_and_pairs)
{
    const parsed p = parse("[server]\n"
                           "host = 127.0.0.1\n"
                           "port = 27015\n"
                           "[log]\n"
                           "level = debug\n");

    EXPECT_EQ(p.entries, (entries_t{"[server]", "server.host=127.0.0.1", "server.port=27015",
                                    "[log]", "log.level=debug"}));
    EXPECT_EQ(p.error, lh_ini_error_none);
}

TEST(ini_parser, trims_names_keys_and_values)
{
    const parsed p = parse("  [  server  ]  \n\t key\t=\t value with spaces \t\n");

    EXPECT_EQ(p.entries, (entries_t{"[server]", "server.key=value with spaces"}));
}

TEST(ini_parser, reads_any_line_end)
{
    const parsed p = parse("[a]\r\nx=1\ry=2\nz=3");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.x=1", "a.y=2", "a.z=3"}));
}

TEST(ini_parser, skips_blank_lines_and_comments)
{
    const parsed p = parse("; top comment\n"
                           "\n"
                           "[a]\n"
                           "   # indented comment\n"
                           "   \t  \n"
                           "k = v\n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=v"}));
}

TEST(ini_parser, keeps_inline_semicolon_and_hash_in_value)
{
    const parsed p = parse("[a]\nk = x ; y # z\n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=x ; y # z"}));
}

TEST(ini_parser, splits_at_first_equals)
{
    const parsed p = parse("[a]\nk = b = c\n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=b = c"}));
}

TEST(ini_parser, allows_empty_value)
{
    const parsed p = parse("[a]\nk =\nj=   \n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=", "a.j="}));
}

TEST(ini_parser, skips_utf8_bom)
{
    const parsed p = parse("\xEF\xBB\xBF[a]\nk=v\n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=v"}));
}

TEST(ini_parser, empty_text_has_no_entries)
{
    const parsed p = parse("");

    EXPECT_TRUE(p.entries.empty());
    EXPECT_EQ(p.error, lh_ini_error_none);
    EXPECT_EQ(p.line, 0U);
}

TEST(ini_parser, reports_pair_before_first_section)
{
    const parsed p = parse("; comment\nk = v\n[a]\n");

    EXPECT_TRUE(p.entries.empty());
    EXPECT_EQ(p.error, lh_ini_error_no_section);
    EXPECT_EQ(p.line, 2U);
}

TEST(ini_parser, reports_unclosed_section)
{
    const parsed p = parse("[a]\nk=v\n[b\n");

    EXPECT_EQ(p.entries, (entries_t{"[a]", "a.k=v"}));
    EXPECT_EQ(p.error, lh_ini_error_bad_section);
    EXPECT_EQ(p.line, 3U);
}

TEST(ini_parser, reports_text_after_section)
{
    EXPECT_EQ(parse("[a] ; comment\n").error, lh_ini_error_bad_section);
    EXPECT_EQ(parse("[a]b]\n").error, lh_ini_error_bad_section);
    EXPECT_EQ(parse("[\n").error, lh_ini_error_bad_section);
}

TEST(ini_parser, reports_empty_section)
{
    EXPECT_EQ(parse("[]\n").error, lh_ini_error_empty_section);
    EXPECT_EQ(parse("[   ]\n").error, lh_ini_error_empty_section);
}

TEST(ini_parser, reports_missing_equals)
{
    const parsed p = parse("[a]\njust text\n");

    EXPECT_EQ(p.error, lh_ini_error_missing_equals);
    EXPECT_EQ(p.line, 2U);
}

TEST(ini_parser, reports_empty_key)
{
    EXPECT_EQ(parse("[a]\n= v\n").error, lh_ini_error_empty_key);
    EXPECT_EQ(parse("[a]\n  =\n").error, lh_ini_error_empty_key);
}

TEST(ini_parser, stays_stopped_after_error)
{
    const std::string ini = "[a]\nbad\nk=v\n";
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, ini.data(), ini.size());
    lh_ini_parser_t parser;
    lh_ini_parser_init(&parser, view);
    lh_ini_entry_t entry;

    ASSERT_TRUE(lh_ini_parser_next(&parser, &entry));
    EXPECT_FALSE(lh_ini_parser_next(&parser, &entry));
    EXPECT_TRUE(lh_ini_parser_has_error(&parser));
    EXPECT_FALSE(lh_ini_parser_next(&parser, &entry));
    EXPECT_EQ(lh_ini_parser_get_line(&parser), 2U);
}

TEST(ini_parser, names_section_of_error)
{
    const std::string ini = "[a]\nk=v\n[server]\nbad\n";
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, ini.data(), ini.size());
    lh_ini_parser_t parser;
    lh_ini_parser_init(&parser, view);
    lh_ini_entry_t entry;

    lh_str_view_t section = lh_ini_parser_get_section(&parser);
    EXPECT_TRUE(lh_str_view_is_empty(&section));
    while (lh_ini_parser_next(&parser, &entry))
    {
    }
    ASSERT_TRUE(lh_ini_parser_has_error(&parser));
    EXPECT_EQ(text(lh_ini_parser_get_section(&parser)), "server");
    EXPECT_EQ(lh_ini_parser_get_line(&parser), 4U);
}

TEST(ini_parser, counts_lines_and_tags_entries_with_section)
{
    const std::string ini = "[a]\nk=v\n";
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, ini.data(), ini.size());
    lh_ini_parser_t parser;
    lh_ini_parser_init(&parser, view);
    lh_ini_entry_t entry;

    EXPECT_EQ(lh_ini_parser_get_line(&parser), 0U);
    ASSERT_TRUE(lh_ini_parser_next(&parser, &entry));
    EXPECT_EQ(text(lh_ini_entry_get_section(&entry)), "a");
    EXPECT_EQ(lh_ini_parser_get_line(&parser), 1U);
    ASSERT_TRUE(lh_ini_parser_next(&parser, &entry));
    EXPECT_EQ(text(lh_ini_entry_get_section(&entry)), "a");
    EXPECT_EQ(lh_ini_parser_get_line(&parser), 2U);
}

TEST(ini_parser, line_at_end_counts_trailing_comments)
{
    const parsed p = parse("[a]\nk=v\n; tail\n\n");

    EXPECT_EQ(p.error, lh_ini_error_none);
    EXPECT_EQ(p.line, 4U);
}

TEST(ini_error_get_message, describes_every_error)
{
    EXPECT_STREQ(lh_ini_error_get_message(lh_ini_error_none), "no error");
    EXPECT_STREQ(lh_ini_error_get_message(lh_ini_error_no_section), "pair before the first section");
    EXPECT_STREQ(lh_ini_error_get_message(200U), "unknown error");
}

} // namespace
