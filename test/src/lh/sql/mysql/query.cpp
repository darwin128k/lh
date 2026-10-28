#include <gtest/gtest.h>

#include <lh/sql/mysql/query.h>

#include <string>
#include <vector>

namespace
{

lh_str_view_t
view(const std::string &s)
{
    lh_str_view_t v;
    if (s.empty())
    {
        lh_str_view_init_empty(&v);
    }
    else
    {
        lh_str_view_init_by_size(&v, s.data(), s.size());
    }
    return v;
}

/* sql with its placeholders replaced by @p literals, or "<fail>". */
std::string
substitute(const std::string &sql, const std::vector<std::string> &literals)
{
    std::vector<lh_str_t> strs(literals.size());
    for (size_t i = 0; i < literals.size(); ++i)
    {
        lh_str_init(&strs[i]);
        lh_str_append_view(&strs[i], view(literals[i]));
    }
    lh_str_t out;
    lh_str_init(&out);
    const bool ok = lh_sql_mysql_query_substitute(view(sql), strs.data(), strs.size(), &out);
    std::string text =
        ok ? std::string(lh_str_get_data(&out) ? lh_str_get_data(&out) : "", lh_str_get_size(&out))
           : "<fail>";
    lh_str_deinit(&out);
    for (auto &s : strs)
    {
        lh_str_deinit(&s);
    }
    return text;
}

lh_usize_t
count(const std::string &sql)
{
    return lh_sql_mysql_query_count_params(view(sql));
}

TEST(sql_mysql_query, replaces_placeholders_in_order)
{
    EXPECT_EQ(substitute("SELECT * FROM t WHERE a = ? AND b = ?", {"1", "'x'"}),
              "SELECT * FROM t WHERE a = 1 AND b = 'x'");
    EXPECT_EQ(substitute("INSERT INTO t VALUES (?,?,?)", {"NULL", "-5", "''"}),
              "INSERT INTO t VALUES (NULL,-5,'')");
    EXPECT_EQ(substitute("SELECT 1", {}), "SELECT 1");
    EXPECT_EQ(substitute("?", {"42"}), "42");
}

TEST(sql_mysql_query, leaves_question_marks_mysql_would_not_read_as_placeholders)
{
    EXPECT_EQ(count("SELECT 'a?b', \"c?\", `d?` FROM t WHERE x = ?"), 1U);
    EXPECT_EQ(count("SELECT 'it''s?', 'esc\\'?' , ?"), 1U);
    EXPECT_EQ(count("SELECT ? -- what?\n, ?"), 2U);
    EXPECT_EQ(count("SELECT ? # why?\n"), 1U);
    EXPECT_EQ(count("SELECT /* is this ? */ ?"), 1U);
    /* "--" without a space after it is not a comment in MySQL: 1--? is 1 - (-?). */
    EXPECT_EQ(count("SELECT 1--?"), 1U);
    EXPECT_EQ(substitute("SELECT 'a?', ? /* ? */", {"7"}), "SELECT 'a?', 7 /* ? */");
}

TEST(sql_mysql_query, an_unbound_placeholder_fails)
{
    EXPECT_EQ(substitute("SELECT ?, ?", {"1"}), "<fail>");
    EXPECT_EQ(substitute("SELECT ?, ?", {"", "2"}), "<fail>");
}

TEST(sql_mysql_query, unterminated_quotes_and_comments_run_to_the_end)
{
    EXPECT_EQ(count("SELECT 'open ?"), 0U);
    EXPECT_EQ(count("SELECT /* open ?"), 0U);
    EXPECT_EQ(count("SELECT `open ?"), 0U);
}

} // namespace
