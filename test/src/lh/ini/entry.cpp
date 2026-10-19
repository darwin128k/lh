#include <gtest/gtest.h>

#include <lh/ini/entry.h>

#include <string>

namespace
{

std::string
text(lh_str_view_t v)
{
    return std::string(lh_str_view_get_data(&v), lh_str_view_get_size(&v));
}

TEST(ini_entry_init, is_nothing)
{
    lh_ini_entry_t e;
    lh_ini_entry_init(&e);

    EXPECT_EQ(lh_ini_entry_get_kind(&e), lh_ini_entry_kind_none);
    const lh_str_view_t section = lh_ini_entry_get_section(&e);
    EXPECT_TRUE(lh_str_view_is_empty(&section));
}

TEST(ini_entry_make_section, carries_its_own_name)
{
    const lh_ini_entry_t e = lh_ini_entry_make_section(lh_str_view_lit("server"));

    EXPECT_TRUE(lh_ini_entry_has_kind(&e, lh_ini_entry_kind_section));
    EXPECT_FALSE(lh_ini_entry_has_kind(&e, lh_ini_entry_kind_pair));
    EXPECT_EQ(text(lh_ini_entry_get_section(&e)), "server");
}

TEST(ini_entry_make_pair, holds_key_and_value_as_str_values)
{
    const lh_ini_entry_t e = lh_ini_entry_make_pair(
        lh_str_view_lit("server"), lh_str_view_lit("port"), lh_str_view_lit("27015"));

    EXPECT_TRUE(lh_ini_entry_has_kind(&e, lh_ini_entry_kind_pair));
    EXPECT_EQ(text(lh_ini_entry_get_section(&e)), "server");
    EXPECT_EQ(text(lh_ini_entry_get_key(&e)), "port");
    EXPECT_EQ(text(lh_ini_entry_get_value(&e)), "27015");

    const lh_pair_t *pair = lh_ini_entry_get_pair(&e);
    EXPECT_TRUE(lh_value_has_kind(lh_pair_get_first(pair), lh_value_kind_str));
    EXPECT_TRUE(lh_value_has_kind(lh_pair_get_second(pair), lh_value_kind_str));
}

TEST(ini_entry_assign, copies_everything)
{
    const lh_ini_entry_t src = lh_ini_entry_make_pair(
        lh_str_view_lit("a"), lh_str_view_lit("k"), lh_str_view_lit("v"));
    lh_ini_entry_t dst;
    lh_ini_entry_init(&dst);

    lh_ini_entry_assign(&dst, &src);

    EXPECT_TRUE(lh_ini_entry_has_kind(&dst, lh_ini_entry_kind_pair));
    EXPECT_EQ(text(lh_ini_entry_get_section(&dst)), "a");
    EXPECT_TRUE(lh_pair_equals(lh_ini_entry_get_pair(&dst), lh_ini_entry_get_pair(&src)));
}

} // namespace
