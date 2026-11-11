#include <gtest/gtest.h>

#include <lh/core/http/char.h>
#include <lh/core/http/target.h>

#include <string>

namespace
{

struct copied
{
    std::string path;
    lh_http_target_error_t error;
};

copied
copy(const std::string &target, lh_usize_t capacity = 64)
{
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, target.data(), target.size());
    std::string out(capacity, 'x');
    lh_http_target_error_t error = 99;
    const lh_usize_t n = lh_http_target_try_copy_path(&view, &out[0], capacity, &error);
    EXPECT_EQ(out[n], '\0');
    return {out.substr(0, n), error};
}

lh_http_target_error_t
path_error(const std::string &path)
{
    lh_str_view_t view;
    lh_str_view_init_by_size(&view, path.data(), path.size());
    return lh_http_target_get_path_error(&view);
}

TEST(http_target_try_copy_path, decodes_once_and_drops_query_and_fragment)
{
    EXPECT_EQ(copy("/maps/de_dust2.bsp").path, "maps/de_dust2.bsp");
    EXPECT_EQ(copy("/maps/a%20b.bsp?x=1").path, "maps/a b.bsp");
    EXPECT_EQ(copy("/sound/x.wav#top").path, "sound/x.wav");
    EXPECT_EQ(copy("/a%2541").path, "a%41");
    EXPECT_EQ(copy("/maps/x.bsp").error, lh_http_target_error_none);
}

TEST(http_target_try_copy_path, refuses_what_the_decode_refuses)
{
    EXPECT_EQ(copy("").error, lh_http_target_error_bad_target);
    EXPECT_EQ(copy("/").error, lh_http_target_error_bad_target);
    EXPECT_EQ(copy("maps/x.bsp").error, lh_http_target_error_bad_target);
    EXPECT_EQ(copy("/maps/x.bsp%").error, lh_http_target_error_bad_encoding);
    EXPECT_EQ(copy("/maps/x%zz").error, lh_http_target_error_bad_encoding);
    EXPECT_EQ(copy("/maps%2fx.bsp").error, lh_http_target_error_encoded_separator);
    EXPECT_EQ(copy("/maps%00x").error, lh_http_target_error_encoded_separator);
    EXPECT_EQ(copy("/maps/a%5cb.bsp").error, lh_http_target_error_bad_char);
    EXPECT_EQ(copy("/maps/a%3ab").error, lh_http_target_error_bad_char);
    EXPECT_EQ(copy("/maps/a%7fb").error, lh_http_target_error_bad_char);
    EXPECT_EQ(copy("/abcdef", 6).error, lh_http_target_error_too_long);
    EXPECT_EQ(copy("/abcde", 6).path, "abcde");
}

TEST(http_target_try_copy_path, refuses_unsafe_components)
{
    EXPECT_EQ(copy("/maps//x.bsp").error, lh_http_target_error_empty_component);
    EXPECT_EQ(copy("/maps/").error, lh_http_target_error_empty_component);
    EXPECT_EQ(copy("/maps/.x.bsp").error, lh_http_target_error_hidden_or_dot);
    EXPECT_EQ(copy("/maps/../x").error, lh_http_target_error_hidden_or_dot);
    EXPECT_EQ(copy("/maps./x").error, lh_http_target_error_trailing_dot_or_space);
    EXPECT_EQ(copy("/maps%20/x").error, lh_http_target_error_trailing_dot_or_space);
    EXPECT_EQ(copy("/MAPS~1/x").error, lh_http_target_error_short_name);
    EXPECT_EQ(copy("/maps~x/y").error, lh_http_target_error_none);
}

TEST(http_target_get_path_error, checks_each_component)
{
    EXPECT_EQ(path_error("a/b/c.txt"), lh_http_target_error_none);
    EXPECT_EQ(path_error(""), lh_http_target_error_empty_component);
    EXPECT_EQ(path_error("a/.b"), lh_http_target_error_hidden_or_dot);
    EXPECT_EQ(path_error("a /b"), lh_http_target_error_trailing_dot_or_space);
    EXPECT_EQ(path_error("a/b~9"), lh_http_target_error_short_name);
}

TEST(http_target_error_get_name, names_every_error)
{
    EXPECT_STREQ(lh_http_target_error_get_name(lh_http_target_error_none), "none");
    EXPECT_STREQ(lh_http_target_error_get_name(lh_http_target_error_bad_encoding), "bad_encoding");
    EXPECT_STREQ(lh_http_target_error_get_name(lh_http_target_error_too_long), "uri_too_long");
    EXPECT_STREQ(lh_http_target_error_get_name(lh_http_target_error_trailing_dot_or_space),
                 "trailing_dot_or_space");
    EXPECT_STREQ(lh_http_target_error_get_name(200), "unknown");
}

TEST(http_is_name_char, refuses_controls_del_and_windows_reserved)
{
    EXPECT_TRUE(lh_http_is_name_char('a'));
    EXPECT_TRUE(lh_http_is_name_char(' '));
    EXPECT_TRUE(lh_http_is_name_char(static_cast<lh_char_t>(0xE9)));
    EXPECT_FALSE(lh_http_is_name_char('\0'));
    EXPECT_FALSE(lh_http_is_name_char('\x1f'));
    EXPECT_FALSE(lh_http_is_name_char('\x7f'));
    for (char c : std::string("\\:*?\"<>|"))
    {
        EXPECT_FALSE(lh_http_is_name_char(c)) << c;
    }
}

} // namespace
