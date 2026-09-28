#include <gtest/gtest.h>

#include <lh/fs/path.h>
#include <lh/str/view.h>

namespace
{

lh_usize_t
segment_count(const char *text, lh_fs_path_style_t style)
{
    lh_fs_path_t path;
    lh_fs_path_init(&path);
    lh_fs_path_set(&path, lh_str_view_make(text), style);
    const lh_usize_t count = lh_fs_path_get_segment_count(&path);
    lh_fs_path_deinit(&path);
    return count;
}

TEST(fs_path_get_segment_count, empty_and_root_only)
{
    EXPECT_EQ(segment_count("", lh_fs_path_style_posix), 0u);
    EXPECT_EQ(segment_count("/", lh_fs_path_style_posix), 0u);
    EXPECT_EQ(segment_count("C:\\", lh_fs_path_style_windows), 0u);
    EXPECT_EQ(segment_count("C:", lh_fs_path_style_windows), 0u);
}

TEST(fs_path_get_segment_count, relative_and_absolute)
{
    EXPECT_EQ(segment_count("a", lh_fs_path_style_posix), 1u);
    EXPECT_EQ(segment_count("a/b/c", lh_fs_path_style_posix), 3u);
    EXPECT_EQ(segment_count("/usr/local/bin", lh_fs_path_style_posix), 3u);
    EXPECT_EQ(segment_count("C:\\Users\\me\\file.txt", lh_fs_path_style_windows), 3u);
}

TEST(fs_path_get_segment_count, repeated_and_trailing_separators_are_normalized_away)
{
    EXPECT_EQ(segment_count("//a///b//", lh_fs_path_style_posix), 2u);
    EXPECT_EQ(segment_count("a\\/\\b\\", lh_fs_path_style_windows), 2u);
    EXPECT_EQ(segment_count("C:\\\\a\\\\b", lh_fs_path_style_windows), 2u);
}

TEST(fs_path_get_segment_count, backslash_is_a_name_character_in_posix_style)
{
    EXPECT_EQ(segment_count("a\\b/c", lh_fs_path_style_posix), 2u);
}

TEST(fs_path_get_segment_count, matches_get_segment)
{
    lh_fs_path_t path;
    lh_fs_path_init(&path);
    lh_fs_path_set(&path, lh_str_view_make("/x/yy/zzz"), lh_fs_path_style_posix);
    ASSERT_EQ(lh_fs_path_get_segment_count(&path), 3u);
    const lh_str_view_t last = lh_fs_path_get_segment(&path, 2);
    EXPECT_EQ(lh_str_view_get_size(&last), 3u);
    lh_fs_path_deinit(&path);
}

TEST(fs_path_get_segment_count, after_join)
{
    lh_fs_path_t dir, name, root, out;
    lh_fs_path_init(&dir);
    lh_fs_path_init(&name);
    lh_fs_path_init(&root);
    lh_fs_path_init(&out);
    lh_fs_path_set(&dir, lh_str_view_make("/a/b"), lh_fs_path_style_posix);
    lh_fs_path_set(&name, lh_str_view_make("c/d"), lh_fs_path_style_posix);
    lh_fs_path_set(&root, lh_str_view_make("/"), lh_fs_path_style_posix);

    ASSERT_TRUE(lh_fs_path_join(&out, &dir, &name));
    EXPECT_EQ(lh_fs_path_get_segment_count(&out), 4u);
    ASSERT_TRUE(lh_fs_path_join(&out, &root, &name));
    EXPECT_EQ(lh_fs_path_get_segment_count(&out), 2u);

    lh_fs_path_deinit(&out);
    lh_fs_path_deinit(&root);
    lh_fs_path_deinit(&name);
    lh_fs_path_deinit(&dir);
}

TEST(fs_path_equals, same_root_and_text)
{
    lh_fs_path_t a;
    lh_fs_path_t b;

    lh_fs_path_init(&a);
    lh_fs_path_init(&b);
    lh_fs_path_set(&a, lh_str_view_make("C:\\Foo\\bar"), lh_fs_path_style_windows);
    lh_fs_path_set(&b, lh_str_view_make("C:/Foo/bar"), lh_fs_path_style_windows);
    EXPECT_TRUE(lh_fs_path_equals(&a, &b, lh_bool_false));

    lh_fs_path_set(&b, lh_str_view_make("c:/foo/bar"), lh_fs_path_style_windows);
    EXPECT_FALSE(lh_fs_path_equals(&a, &b, lh_bool_false));
    EXPECT_TRUE(lh_fs_path_equals(&a, &b, lh_bool_true));

    lh_fs_path_set(&b, lh_str_view_make("/Foo/bar"), lh_fs_path_style_posix);
    EXPECT_FALSE(lh_fs_path_equals(&a, &b, lh_bool_true));

    lh_fs_path_deinit(&b);
    lh_fs_path_deinit(&a);
}

TEST(fs_path_parent, drops_the_last_segment)
{
    lh_fs_path_t path;
    lh_str_t text;

    lh_fs_path_init(&path);
    lh_str_init(&text);

    lh_fs_path_set(&path, lh_str_view_make("C:\\a\\b\\c.txt"), lh_fs_path_style_windows);
    ASSERT_TRUE(lh_fs_path_parent(&path, &path));
    lh_fs_path_to_str(&path, lh_fs_path_style_posix, &text);
    EXPECT_STREQ(lh_str_get_data(&text), "C:/a/b");

    ASSERT_TRUE(lh_fs_path_parent(&path, &path));
    lh_fs_path_to_str(&path, lh_fs_path_style_posix, &text);
    EXPECT_STREQ(lh_str_get_data(&text), "C:/a");

    ASSERT_TRUE(lh_fs_path_parent(&path, &path));
    EXPECT_TRUE(lh_fs_path_is_root(&path));
    EXPECT_FALSE(lh_fs_path_parent(&path, &path));
    EXPECT_TRUE(lh_fs_path_is_root(&path));

    lh_fs_path_set(&path, lh_str_view_make("a"), lh_fs_path_style_posix);
    ASSERT_TRUE(lh_fs_path_parent(&path, &path));
    EXPECT_TRUE(lh_fs_path_is_empty(&path));

    lh_str_deinit(&text);
    lh_fs_path_deinit(&path);
}

} // namespace
