#include <gtest/gtest.h>

#include <lh/fs/path/style.h>

namespace
{

TEST(fs_path_style_is_sep, slash_separates_in_every_style)
{
    EXPECT_TRUE(lh_fs_path_style_is_sep(lh_fs_path_style_posix, '/'));
    EXPECT_TRUE(lh_fs_path_style_is_sep(lh_fs_path_style_windows, '/'));
}

TEST(fs_path_style_is_sep, backslash_separates_only_on_windows)
{
    EXPECT_FALSE(lh_fs_path_style_is_sep(lh_fs_path_style_posix, '\\'));
    EXPECT_TRUE(lh_fs_path_style_is_sep(lh_fs_path_style_windows, '\\'));
}

TEST(fs_path_style_is_sep, other_characters_never_separate)
{
    EXPECT_FALSE(lh_fs_path_style_is_sep(lh_fs_path_style_posix, 'a'));
    EXPECT_FALSE(lh_fs_path_style_is_sep(lh_fs_path_style_windows, ':'));
}

TEST(fs_path_style_get_sep, rendered_separator)
{
    EXPECT_EQ(lh_fs_path_style_get_sep(lh_fs_path_style_posix), '/');
    EXPECT_EQ(lh_fs_path_style_get_sep(lh_fs_path_style_windows), '\\');
}

} // namespace
