#include <gtest/gtest.h>

#include <lh/core/str/template.h>

#include <map>
#include <string>

namespace
{

using values_t = std::map<std::string, std::string>;

lh_bool_t
lookup(lh_ptr user, const lh_str_view_t *name, lh_str_view_t *value)
{
    const auto *values = static_cast<const values_t *>(user);
    const auto found =
        values->find(std::string(lh_str_view_get_data(name), lh_str_view_get_size(name)));

    if (found == values->end())
    {
        return lh_bool_false;
    }
    lh_str_view_init_by_size(value, found->second.data(), found->second.size());
    return lh_bool_true;
}

/* The expansion of `pattern`, or "<fail>". */
std::string
expand(const char *pattern, const values_t &values)
{
    lh_str_view_t view;

    lh_str_view_init(&view, pattern);
    lh_str_t out;
    lh_str_init(&out);

    const lh_bool_t ok =
        lh_str_template_try_expand(&view, lookup, const_cast<values_t *>(&values), &out);
    std::string text = ok ? std::string(lh_str_get_data(&out), lh_str_get_size(&out)) : "<fail>";

    if (!ok)
    {
        EXPECT_EQ(lh_str_get_size(&out), 0U);
    }
    lh_str_deinit(&out);
    return text;
}

bool
is_name(const char *text)
{
    lh_str_view_t view;

    lh_str_view_init(&view, text);
    return lh_str_template_is_name(&view) != lh_bool_false;
}

} // namespace

TEST(lh_str_template, replaces_every_token)
{
    const values_t values = {{"game", "/srv/hl"}, {"mod", "cstrike"}, {"empty", ""}};

    EXPECT_EQ(expand("{game}/{mod}/maps", values), "/srv/hl/cstrike/maps");
    EXPECT_EQ(expand("{game}{mod}", values), "/srv/hlcstrike");
    EXPECT_EQ(expand("a{empty}b", values), "ab");
    EXPECT_EQ(expand("plain", values), "plain");
    EXPECT_EQ(expand("", values), "");
}

TEST(lh_str_template, fails_on_stray_braces_and_unknown_names)
{
    const values_t values = {{"game", "/srv/hl"}};

    EXPECT_EQ(expand("{nope}/x", values), "<fail>");
    EXPECT_EQ(expand("{game", values), "<fail>");
    EXPECT_EQ(expand("game}", values), "<fail>");
    EXPECT_EQ(expand("{game}}", values), "<fail>");
    EXPECT_EQ(expand("{}", values), "<fail>");
}

TEST(lh_str_template, names_are_word_characters)
{
    EXPECT_TRUE(is_name("game"));
    EXPECT_TRUE(is_name("Map_2"));
    EXPECT_FALSE(is_name(""));
    EXPECT_FALSE(is_name("a-b"));
    EXPECT_FALSE(is_name("a b"));
    EXPECT_FALSE(is_name("{a}"));
}
